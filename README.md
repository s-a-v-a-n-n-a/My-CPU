# My-CPU
That is my realisarion of softprocessor based on stack and array of four registers: RAX, RBX, RCX and RDX. The last are nulified before tha start of the processing.
You can write your instructions and run it with "My_processor.cpp".
How will it work:
- PUSH 1   ; pushes 1 into the stack
- PUSH RAX ; or other register - pushes the value from the register to the stack
- ADD      ; pops two last values from the stack, add one to the other and pushes their sum back
- ADD RAX  ; or other register - pops the last value from the stack and adds it to register written (RAX in this case)
- SUB      ; pops two last values from the stack, substructs the later one from the earlier one and pushes the result back to the stack

To be continued
