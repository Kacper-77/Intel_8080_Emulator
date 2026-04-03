# Intel 8080 Emulator in C

A cycle-accurate emulator for the Intel 8080 CPU, built from scratch to explore the foundations of processor architecture and low-level execution. This project includes full instruction decoding, stack and interrupt handling, and a suite of test programs with visual logs.

# The following test ROMs have been successfully passed:
- **CPUTEST**
- **8080EXM**
- **TST8080**

# 🧠 Human Side — Motivation & Highlights
## 🎯 Why I Built This Emulator
I wanted to understand what a processor really is — not just conceptually, but practically. The Intel 8080 felt like the perfect place to start: it’s one of the earliest widely adopted CPUs, and it laid the groundwork for modern computing. By emulating it, I could explore where it all began — how instructions are executed, how memory is accessed, and how control flow is managed at the hardware level.

This project was my way of demystifying the CPU. I wanted to see how each opcode translates into behavior, how flags are affected, and how the stack and interrupts interact with execution. It wasn’t just about building something — it was about understanding the essence of computation.

## 🧱 The Hardest Part
The most challenging aspect was stitching everything together into a coherent, modular, and scalable system. Writing individual components like instruction handlers or memory access was straightforward — but designing an architecture where everything communicates cleanly and can be extended easily took serious planning and iteration.

I wanted the emulator to feel like a real machine, but also like a clean codebase. That meant abstracting complexity without losing fidelity, and making sure every part — from the CPU core to the decoder — could grow as needed.

## 💡 My Favorite Part
The instruction decoder. I built a system that maps opcodes directly to instruction handlers, so adding new instructions was as simple as writing a few lines of code. It turned what could have been a tedious process into something elegant and satisfying. Watching the CPU execute new instructions just moments after adding them to the table was incredibly rewarding.

# ⚙️ Technical Side — Architecture, Execution & Demo
## 🧩 What It Is
This is a cycle-accurate emulator for the Intel 8080 CPU, written entirely in C. It simulates the behavior of the processor at the instruction level, including:
- Full instruction decoding and execution
- Register and flag updates
- Stack operations (PUSH, POP, CALL, RET)
- Interrupt handling (EI, DI, RST, TRAP)
- Cycle tracking per instruction
- Memory access and program loading
- Trap detection and logging

The goal was to build a minimal yet complete emulator that behaves like the real 8080 — not just in terms of output, but in how it gets there.

# 🔍 Why the Intel 8080?
The Intel 8080 is one of the earliest general-purpose microprocessors, and it laid the foundation for the x86 architecture. Emulating it is like stepping into the roots of modern computing. It’s simple enough to implement without external dependencies, but rich enough to teach:
- Instruction decoding
- Flag logic
- Memory addressing
- Control flow
- Interrupt architecture

It’s also historically significant — powering systems like the Altair 8800 and early CP/M machines.

# 🛠️ How It Works (Brief Overview)
## 🧠 CPU Core
- Registers: A, B, C, D, E, H, L, SP, PC
- Flags: Zero, Sign, Parity, Carry, Aux Carry
- Cycle counter: updated per instruction
- Instruction decoder: maps opcodes to handler functions

## 📦 Memory
- 64KB flat memory space
- Program loaded into memory at address 0x0000
- Interrupt and trap handlers mapped to fixed vectors

## 🔁 Execution Loop
Fetch -> Decode -> Execute

Each instruction updates registers, flags, memory, and cycles

Special instructions (HLT, CALL, RET, JMP, etc.) alter control flow

Interrupts and traps can also be triggered

## 🧩 Decoder Design
The decoder is built as a lookup table that maps each opcode to its corresponding handler. This makes the architecture modular and scalable — adding a new instruction is as simple as writing a handler and registering it.


# 🚀 How to Run
## 🧱 Requirements:
- C compiler
- Terminal or shell environment

## 💻 Run:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

```bash
./emulator8080
```
