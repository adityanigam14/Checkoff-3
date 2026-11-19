# Stack Buffer Overflows in C

This is my hands-on exploration of stack buffer overflows for COMP 211's Checkoff 3,
done by writing small C programs, printing memory addresses, and testing
predictions about how the stack is laid out.

## Overview

A stack buffer overflow happens when a program writes more data into a
fixed-size local array than it can hold, and C does nothing to stop it.
The extra bytes then land in whatever memory sits next to the array, which can
silently change other variables, crash the program, or in the worst case
let an attacker control where the program jumps to.

I explored this by writing two small demos, deliberately overflowing them,
and testing the compiler's and OS's defenses against my own predictions
rather than just accepting what I was told.

## What I explored

- How local variables are laid out next to each other on the stack, and
  how `strcpy` and `scanf("%s")` write past a buffer's end with no
  bounds checking
- Using a crafted input to flip an unrelated variable (`is_admin`) from 0
  to nonzero without ever setting it directly
- Using the same technique to bypass a password check (`login_demo.c`)
  without the password ever matching
- Comparing the same bug on the stack vs. the heap (`malloc`'d buffers),
  and why heap corruption is caught later and less reliably than stack
  corruption
- Fixing the bug with `snprintf`, and verifying the fix with the same
  overflow input that broke the original
- Stack canaries: how GCC's stack protector catches the corruption, and
  why it reorders local variables when it's enabled

## Key discoveries

- **Variable layout isn't fixed by declaration order.** It depends on
  the compiler, platform, and whether the stack protector is on. My own
  gap between `buf` and `is_admin` (20 bytes) didn't match predictions
  made on a different machine (28 bytes), and the compiler flipped the
  order of the two variables once I compiled with the default (protected)
  flags.
- **A prediction about crashing turned out wrong on my machine.** 100
  characters was predicted to segfault (based on overwriting the return
  address), but on my build it ran to completion normally, because the
  return address wasn't in the overflow's path. The bug was still there,
  it just didn't happen to crash anything.
- **The heap's "protection" is a side effect, not a security feature.**
  `malloc`/`free` check chunk metadata for their own bookkeeping reasons,
  not to catch overflows. If a corrupted chunk is never freed (e.g. the
  program just exits), nothing ever notices.
- **A one-line fix closes the bug completely.** Swapping `strcpy` for
  `snprintf(buf, sizeof buf, ...)` kept `is_admin` at 0 even with 100
  characters of input, because it respects the buffer's actual size.

## Code examples

All programs were compiled with `gcc` on `gcc -O0` (see individual
commands below; `-fno-stack-protector` is used only to show the
*unprotected* behavior, and is dropped to show the stack canary).

| File | What it shows | How to run |
|---|---|---|
| `stackbufferoverflows.c` | Basic overflow: a long input flips `is_admin` | `gcc -O0 -fno-stack-protector -o demo stackbufferoverflows.c`<br>`./demo $(python3 -c "print('A'*32)")` |
| `stackbufferoverflows_fixed.c` | Same program, fixed with `snprintf` | `gcc -O0 -fno-stack-protector -o demo_fixed stackbufferoverflows_fixed.c`<br>`./demo_fixed $(python3 -c "print('A'*100)")` |
| `login_demo.c` | Overflow bypasses a password check | `gcc -O0 -fno-stack-protector -o login_demo login_demo.c`<br>`python3 -c "print('A'*24)" | ./login_demo` |
| `heap_demo.c` | Same bug on a `malloc`'d buffer | `gcc -O0 -o heap_demo heap_demo.c`<br>`./heap_demo $(python3 -c "print('A'*36)")` |
| `stackbufferoverflows.c` (default flags) | Stack canary catching the overflow | `gcc -O0 -o demo_protected stackbufferoverflows.c`<br>`./demo_protected $(python3 -c "print('A'*100)")` |
