;    Algorithm for evaluating a postfix expresdion:
;    1. Create a stack.
;    2. Scan the input starting from left.
;    3. If input is a hex number, push it to stack.
;    4. If input is operator, pop last two from stack and evaluate with operator.
;
;    Source: https://www.geeksforgeeks.org/stack-set-4-evaluation-postfix-expresdion/

;    Authors: Esad Yusuf Atik, Orkun Mahir Kılıç


code segment ; initialize all registers, di is used to determine input's type
    mov ax, 0h
    mov bx, 0h
    mov cx, 0h
    mov dx, 0h
    mov di, 0h   
    mov si, 0h
    jmp readInput

; <-- INPUT -->
readInput:
    ; input reading interrupt
    mov ah, 01h  
    int 21h     
    ; end of input reading (al is now input)
    cmp al, 20h ; if operator / input is ended -> ' '
        je pushToStack
    cmp al, 2Bh ; if it is '+'
        je addLastTwo
    cmp al, 2Fh ; if it is '/'
        je divideLastTwo
    cmp al, 2Ah ; if it is '*'
        je multiplyLastTwo
    cmp al, 26h ; if it is '&'
        je andLastTwo
    cmp al, 7Ch ; if it is '|'
        je orLastTwo
    cmp al, 5Eh ; if it is '^'
        je xorLastTwo
    cmp al, 0Dh ; if input is ended completely -> '\n'(enter)
        je output
    mov di, 01h ; this is like else (input is not one of the operators)
    cmp al, 3ah ; 3ah is the first ascii char after numbers
        jg inputToNumber ; jg because if it is bigger than 3ah, it is a number
    sub al, '0'  ; convert ascii to mathematical number
    jmp multiDigitNumber ; number is like '123'

inputToNumber: ; this part is from PS
    sub al, "A"
    add al, 10d
    jmp multiDigitNumber

multiDigitNumber: ; this part is from PS
    mov cx, 0h
    mov cl, al ; cl is the first digit
    mov ax, 10h ; ax = 10h because mul uses ax
    mul bx ; if there is first digit, multiply it with 10 ; else it results in 0
    add cx, ax ; add multipication to cx
    mov bx, cx ; move to bx as it is default input source
    jmp readInput
; <-- INPUT -->

; <-- STACK -->
pushToStack:
    cmp di, 0h ; if input is operator, go to reading
        je readInput
    push bx ; push input to stack as algorithm requires
    mov bx, 0h ; make bx 0 to continue reading
    jmp readInput
; <-- STACK -->

; <-- OPERATIONS -->
addLastTwo:
    pop ax
    pop bx
    add ax, bx
    push ax ; push the result
    mov di, 0h
    mov bx, 0h ; clear the registers
    jmp readInput

divideLastTwo:
    pop bx 
    pop ax 
    mov dx, 0h
    div bx ; div -> al(ax) = ax / bx
    push ax ; push the result
    mov di, 0h
    mov bx, 0h ; clear the registers
    jmp readInput

multiplyLastTwo:
    pop ax 
    pop bx
    mov dx, 0h
    mul bx ; mul -> ax = al(ax) * bx
    push ax ; push the result
    mov di, 0h
    mov bx, 0h ; clear the registers
    jmp readInput

andLastTwo:
    pop ax 
    pop bx 
    and ax, bx
    push ax ; push the result
    mov di, 0h
    mov bx, 0h ; clear the registers
    jmp readInput

orLastTwo:
    pop ax 
    pop bx 
    or ax, bx
    push ax ; push the result
    mov di, 0h
    mov bx, 0h ; clear the registers
    jmp readInput

xorLastTwo:
    pop ax 
    pop bx 
    xor ax, bx
    push ax ; push the result
    mov di, 0h
    mov bx, 0h ; clear the registers
    jmp readInput
; <-- OPERATIONS -->

; <-- OUTPUT -->
output: ; this is also from PS
    ; output interrupt
    mov ah, 02h
    mov dl, 10d ; \n
    int 21h
    ; end of output interrupt
    pop ax ; start popping the results
    mov bx, 10h ; div uses bx
    mov cx, 0h
    mov dx, 0h ; clear the registers
    div bx ; remainder is in dx now, it is the last digit
    push dx ; push to output later
    add cx, 1h ; cx is used for counting the digits in the stack
    mov dx, 0h ; clear the remainder (todo:: think about removing)
    div bx ; next digit is in remainder now
    push dx
    add cx, 1h ; count the digits
    mov dx, 0h
    div bx ; next digit is in remainder now
    push dx
    add cx, 1h ; count the digits
    push ax ; this must be the last digit, push it
    add cx, 1h ; count the digits
    jmp outputFinish

outputFinish:
    mov ah, 02h ; start outputting
    pop dx ; this is the first digit
    cmp dl, 9h ; if it is not a number
        jg hexToOutput
    add dl, '0'

outputContinue:
    int 21h ; start interrupt
    sub cx, 1h ; number of digit is decreased
    jnz outputFinish ; if there are still digits, go to outputContinue
    int 20h ; end interrupt

hexToOutput: ; hex to ascii; this is from PS
    sub dl, 10d
    add dl, "A"
    jmp outputContinue
; <-- OUTPUT -->

code ends