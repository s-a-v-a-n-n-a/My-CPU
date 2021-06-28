# My-CPU
**DOCUMENTATION**
================
That is my realisarion of softprocessor based on stack and array of four registers: RAX, RBX, RCX and RDX. 

**DESCRIPTION OF THE STRUCTURE**
--------------------------------
There are 3 separately working parts: Assembler, Processor and Disassembler. 
Assembler makes "executable"(.xex) file from the "original code" (.xax).
Processor executes .xex file.
Disassembler makes AASM code from binary (.xex) code.

**FUNCTIONAL**
--------------
You can write your instructions and run it with "My_processor.cpp".
How will it work:
- HLT        ; Stops the program running
- PUSH 1     ; pushes 1 into the stack
- PUSH RAX   ; or other register - pushes the value from the register to the stack
- PUSH [RAX] ; or other register - pushes the value from the index of memory storaged in this register to the stack
- ADD        ; pops two last values from the stack, add one to the other and pushes their sum back
- SUB        ; pops two last values from the stack, substructs the later one from the earlier one and pushes the result back to the stack
- MUL        ; pops two last values from the stack, multiplies one to the other and pushes the result back to the stack
- MUL        ; pops two last values from the stack, multiplies one to the other and pushes the result back to the stack
- OUT        ; pops the last value from stack and prints it out
- SIN        ; pops the last value from stack and pushes its sinus back to the stack
- COS        ; pops the last value from stack and pushes its cosinus back to the stack
- POP        ; the same as OUT
- POP RAX    ; or other register - pops the last value from the stack and pushes it to the register
- POP [RAX]  ; or other register - pushes the value from stack to the index of memory, that storages in this register
- SQRT       ; pops the last value from stack and pushes its square root back to the stack
- IN         ; reads from the cmd the value and pushes it to the stack
- DUMP       ; outputs the current state of stack to the log_file

To be continued

**NOTES**
---------
The array of four registers is nulified before tha start of the processing.
