# Validation — 2026-11-07

This report applies to the current assembled folder layout.

| Check | Result |
|---|---|
| Recovered source integrity | Current source bytes compared with materialization hashes before packaging |
| LCD protocol mocks | Passed startup order, nibble ordering, RS/EN timing requests, cursor positions, text bounds, invalid inputs and error recovery |
| Delay arithmetic mocks | Passed all 65536 microsecond argument values at 1000000, 1000001, 7372800, 8000000 and 16000000 Hz; millisecond boundaries also passed |
| Board configuration checks | Passed invalid clock compile guards and rejection of duplicate/out-of-range port/pin mappings |
| Host C99 checks | Five production C files parsed, and every production header resolved independently, with target macros enabled |
| Build command checks | GNU Make dry runs passed for the default target and for ATmega32A with an 8 MHz override |
| Sanitizers | AddressSanitizer and UndefinedBehaviorSanitizer used by the restored host test runner |
| Archive integrity | ZIP CRC and individual file byte comparisons passed |

Host C checks with __AVR__ macros are not an AVR cross-compilation. They do not
validate AVR register constraints, assembly execution or elapsed hardware time.
An AVR-GCC toolchain is not installed in this environment; no ELF/HEX build is
claimed. Proteus and physical hardware tests have not been run.

The newly added Gie.c definitions have been C-parsed; their AVR instructions
must be compiled and checked with the target toolchain. The saved GPIO source
was not functionally changed when this folder bundle was assembled.
