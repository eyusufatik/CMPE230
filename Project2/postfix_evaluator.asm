;    Algorithm for evaluating a postfix expresdion:
;    1. Create a stack.
;    2. Scan the input starting from left.
;    3. If input is a hex number, push it to stack.
;    4. If input is operator, pop last two from stack and evaluate with operator.
;
;    Source: https://www.geeksforgeeks.org/stack-set-4-evaluation-postfix-expresdion/

;    Authors: Esad Yusuf Atik, Orkun Mahir Kılıç

code segment
    mov ax, 0h
    mov bx, 0h
    mov cx, 0h
    mov dx, 0h
    mov di, 0h ; if di = 0, last input was an operation. Otherwise last input was a number.    
    jmp readInput ; jumps to readInput

; <-- STACK -->
pushToStack:
    cmp di, 0h ; if operator -> then read input
        je readInput
    push bx ; push input to stack
    mov bx, 0h ; clear bx
    jmp readInput
; <-- STACK -->

; <-- OPERATIONS -->
addLastTwo:
    pop ax ; take the top of stack
    pop bx ; take the top of stack
    add ax, bx ; add them
    push ax ; push it to stack
    mov di, 0h
    mov bx, 0h
    jmp readInput

multiplyLastTwo:
    pop ax ; take the top of stack
    pop bx ; take the top of stack
    mov dx, 0h ; make dx zero, just in case
    mul bx ; multiply them
    push ax ; push back to stack
    mov dx, 0h ; make dx zero, just in case
    mov di, 0h
    mov bx, 0h
    jmp readInput

divideLastTwo:
    pop bx ; take the top of stack
    pop ax ; take the top of stack
    mov dx, 0h ; make dx zero, just in case
    div bx ; divide ax by bx
    push ax ; push the result to the stack
    mov dx, 0h ; make dx zero, just in case
    mov di, 0h
    mov bx, 0h
    jmp readInput

xorLastTwo:
    pop ax ; take the top of stack
    pop bx ; take the top of stack
    xor ax, bx ; xor operation
    push ax ; push the result in stack
    mov di, 0h
    mov bx, 0h
    jmp readInput

andLastTwo:
    pop ax ; take the top of stack
    pop bx ; take the top of stack
    and ax, bx ; and operation
    push ax ; push the result in stack
    mov di, 0h
    mov bx, 0h
    jmp readInput

orLastTwo:
    pop ax ; take the top of stack
    pop bx ; take the top of stack
    or ax, bx ; or operation
    push ax ; push the result in stack
    mov di, 0h
    mov bx, 0h
    jmp readInput
; <-- OPERATIONS -->

; <-- INPUT -->
readInput:
    ; input reading interrupt
    mov ah, 01h  
    int 21h     
    ; end of input reading (al is now input)
    cmp al, 20h  ; if ' '
        je pushToStack
    cmp al, 2Bh  ; if +
        je addLastTwo
    cmp al, 2Ah  ; if *
        je multiplyLastTwo
    cmp al, 2Fh  ; if /
        je divideLastTwo
    cmp al, 5Eh  ; if ^
        je xorLastTwo
    cmp al, 26h  ; if &
        je andLastTwo
    cmp al, 7Ch  ; if |
        je orLastTwo
    cmp al, 0Dh  ; if enter
        je output
    mov di, 01h  ; this is like else (input is not one of the operators)
    cmp al, 3ah  ; 3ah is the first ascii char after numbers
        jg inputToNumber
    sub al, '0'  ; ascii to value
    jmp multiDigitNumber

inputToNumber: ; letter to number
    sub al, "A"
    add al, 10d
    jmp multiDigitNumber

multiDigitNumber: ; this part is from PS
    mov cx, 0h
    mov cl, al  ; put the input in cl
    mov ax, 10h
    mul bx     ; if the given input is digit of a large number, multiply the first digits with 10h, if it is not the result will be 0
    add cx, ax  ; then add the input to the result of multiplyLastTwo
    mov bx, cx  ; move the input to bx
    jmp readInput   ; back to readInput
; <-- INPUT -->

; <-- OUTPUT -->
output: ; this is also from PS
    ; output interrupt
    mov ah, 02h ; for outputing
    mov dl, 10d ; new line
    int 21h ; output new line
    ; end of output interrupt
    pop ax ; pop top of the stack
    mov bx, 10h ; for dividing
    mov cx, 0h
    mov dx, 0h
    div bx ; divide the number to 10d in order to take the last digit of it seperately
    push dx ; push the remainder -last digit of num- to stack
    add cx, 1h ; c++, to know number of digits that will be in stack ; inc
    cmp ax, 0h ; if the quotient is 0, there is nothing more to divide, jump to finish
        je outputFinish
    mov dx, 0h ; make it 0, just in case
    div bx ; same process again, if the quotient is not 0
    push dx
    add cx, 1h ; inc
    cmp ax, 0h
        je outputFinish
    mov dx, 0h
    div bx ; same process again, if the quotient is not 0
    push dx
    add cx, 1h ; inc
    cmp ax, 0h
        je outputFinish
    push ax ; if there is a last digit, push it to stack
    add cx, 1h ; inc
    jmp outputFinish

outputFinish:
    mov ah, 02h ; to output
    pop dx ; pop the top of the stack, first digit of the result
    cmp dl, 9h ; if it is a letter
        jg hexToOutput
    add dl, '0'

outputContinue:
    int 21h ; output it
    sub cx, 1h ; c-- , c is the number of digits in stack
    jnz outputFinish ; if c is not zero, same process again
    int 20h ; quit

hexToOutput: ; if the output is a letter convert it to ascii
    sub dl, 10d
    add dl, "A"
    jmp outputContinue
; <-- OUTPUT -->

code ends