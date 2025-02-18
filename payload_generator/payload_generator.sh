#!/bin/bash

# Compile shellcode.c into an executable binary
gcc -o shellcode shellcode.c

# Compile the .s file into an executable binary
nasm -o decryptor.o -f elf64 -g decryptor.s
ld decryptor.o -o asm_binary

# Run the shellcode binary with the asm_binary as an argument
./shellcode asm_binary

# Clean up the directory by removing binaries and object files
rm -f shellcode asm_binary *.o
