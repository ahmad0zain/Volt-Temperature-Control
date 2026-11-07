#!/usr/bin/env python3
"""Host protocol/calculation tests; no physical LCD or AVR instructions run."""
from pathlib import Path
import os
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
FLAGS = ['-std=c99', '-Wall', '-Wextra', '-Werror', '-Wshadow', '-Wconversion',
         '-pedantic', '-fsanitize=address,undefined']
INCLUDES = ['-I' + str(ROOT / name) for name in ('common', 'boards/volt_board', 'mcal/atmega32', 'mcal/atmega32/gpio', 'mcal/atmega32/delay', 'mcal/atmega32/interrupt', 'hal/lcd')]
ENV = dict(os.environ, ASAN_OPTIONS='detect_leaks=0')

def run(args, **kwargs):
    return subprocess.run(args, check=True, env=ENV, **kwargs)

with tempfile.TemporaryDirectory(prefix='volt_lcd_tests_') as tmp:
    temp = Path(tmp)
    lcd_test = temp / 'lcd_test'
    run(['gcc', *FLAGS, *INCLUDES, str(ROOT / 'hal/lcd/lcd.c'),
         str(ROOT / 'tests/test_lcd.c'), '-o', str(lcd_test)])
    run([str(lcd_test)])

    for clock in (1000000, 1000001, 7372800, 8000000, 16000000):
        exe = temp / ('delay_' + str(clock))
        run(['gcc', *FLAGS, *INCLUDES, '-DDELAY_UNIT_TEST', '-DF_CPU=' + str(clock) + 'UL',
             str(ROOT / 'mcal/atmega32/delay/delay.c'), str(ROOT / 'tests/test_delay.c'), '-o', str(exe)])
        run([str(exe)])

    for clock in (0, 999999, 16000001):
        result = subprocess.run(['gcc', *INCLUDES, '-DF_CPU=' + str(clock) + 'UL',
            '-x', 'c', '-fsyntax-only', '-'], input='#include "Fcpu.h"\n',
            text=True, capture_output=True)
        assert result.returncode != 0 and 'Volt delay supports' in result.stderr
    print('PASS Fcpu.h: invalid clock values rejected; build-wide overrides accepted.')

    config = (ROOT / 'boards/volt_board/lcd_config.h').read_text()
    variants = (
        config.replace('#define LCD_RS_PIN  GPIO_PIN_0', '#define LCD_RS_PIN  GPIO_PIN_1'),
        config.replace('#define LCD_RS_PORT GPIO_PORT_B', '#define LCD_RS_PORT ((GPIO_Port_t)4)'),
        config.replace('#define LCD_RS_PORT GPIO_PORT_B', '#define LCD_RS_PORT ((GPIO_Port_t)-1)'),
        config.replace('#define LCD_RS_PIN  GPIO_PIN_0', '#define LCD_RS_PIN  ((GPIO_Pin_t)8)'),
        config.replace('#define LCD_RS_PIN  GPIO_PIN_0', '#define LCD_RS_PIN  ((GPIO_Pin_t)-1)'),
    )
    for i, variant in enumerate(variants):
        folder = temp / ('bad_config_' + str(i))
        folder.mkdir()
        (folder / 'lcd_config.h').write_text(variant)
        (folder / 'lcd.c').write_text((ROOT / 'hal/lcd/lcd.c').read_text())
        exe = folder / 'test'
        run(['gcc', *FLAGS, *INCLUDES, str(folder / 'lcd.c'),
             str(ROOT / 'tests/test_bad_config.c'), '-o', str(exe)])
        run([str(exe)])
    print('PASS configuration: duplicate and invalid pin/port choices rejected before GPIO writes.')
    run(['gcc', *FLAGS, *INCLUDES, '-fsyntax-only', str(ROOT / 'app/main.c')])
    print('PASS example: C99 syntax with strict warnings.')

print('Host verification complete. AVR compilation and hardware timing are separate checks.')
