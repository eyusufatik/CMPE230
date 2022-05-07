;    Algorithm for evaluating a postfix expresdion:
;    1. Create a stack.
;    2. Scan the input starting from left.
;    3. If input is a hex number, push it to stack.
;    4. If input is operator, pop last two from stack and evaluate with operator.
;
;    Source: https://www.geeksforgeeks.org/stack-set-4-evaluation-postfix-expresdion/

;    Authors: Esad Yusuf Atik, Orkun Mahir Kılıç

; ORDER OF LABELS (It is important as a86 gives error if you jump > 128)
; code segment -> initialization
; stack operation
; operator operations
; input operations
; output operations


code segment ; initialize all registers, di is used to determine input's type
    mov ax, 0H
    mov bx, 0H
    mov cx, 0H
    mov dx, 0H
    mov di, 0H   
    mov si, 0H
    jmp readInput

; <-- STACK -->
pushToStack:
    cmp di, 0H ; if input is operator, go to reading
        je readInput
    push bx ; push input to stack as algorithm requires
    mov bx, 0H ; make bx 0 to continue reading
    jmp readInput
; <-- STACK -->

; <-- OPERATIONS -->
addLastTwo:
    pop ax
    pop bx
    add ax, bx
    push ax ; push the result
    mov di, 0H
    mov bx, 0H ; clear the registers
    jmp readInput

divideLastTwo:
    pop bx 
    pop ax 
    mov dx, 0H
    div bx ; div -> al(ax) = ax / bx
    push ax ; push the result
    mov di, 0H
    mov bx, 0H ; clear the registers
    jmp readInput

multiplyLastTwo:
    pop ax 
    pop bx
    mov dx, 0H
    mul bx ; mul -> ax = al(ax) * bx
    push ax ; push the result
    mov di, 0H
    mov bx, 0H ; clear the registers
    jmp readInput

andLastTwo:
    pop ax 
    pop bx 
    and ax, bx
    push ax ; push the result
    mov di, 0H
    mov bx, 0H ; clear the registers
    jmp readInput

orLastTwo:
    pop ax 
    pop bx 
    or ax, bx
    push ax ; push the result
    mov di, 0H
    mov bx, 0H ; clear the registers
    jmp readInput

xorLastTwo:
    pop ax 
    pop bx 
    xor ax, bx
    push ax ; push the result
    mov di, 0H
    mov bx, 0H ; clear the registers
    jmp readInput
; <-- OPERATIONS -->

; <-- INPUT -->
readInput:
    ; input reading interrupt
    mov ah, 01H  
    int 21H     
    ; end of input reading (al is now input)
    cmp al, 20H ; if operator / input is ended -> ' '
        je pushToStack
    cmp al, 2BH ; if it is '+'
        je addLastTwo
    cmp al, 2FH ; if it is '/'
        je divideLastTwo
    cmp al, 2AH ; if it is '*'
        je multiplyLastTwo
    cmp al, 26H ; if it is '&'
        je andLastTwo
    cmp al, 7CH ; if it is '|'
        je orLastTwo
    cmp al, 5EH ; if it is '^'
        je xorLastTwo
    cmp al, 0DH ; if input is ended completely -> '\n'(enter)
        je output
    mov di, 01H ; this is like else (input is not one of the operators)
    cmp al, 3aH ; 3ah is the first ascii char after numbers
        jg inputToNumber ; jg because if it is bigger than 3ah, it is a number
    sub al, '0'  ; convert ascii to mathematical number
    jmp multiDigitNumber ; number is like '123'

inputToNumber: ; this part is from PS
    sub al, "A"
    add al, 10D
    jmp multiDigitNumber

multiDigitNumber: ; this part is from PS
    mov cx, 0H
    mov cl, al ; cl is the first digit
    mov ax, 10H ; ax = 10h because mul uses ax
    mul bx ; if there is first digit, multiply it with 10 ; else it results in 0
    add cx, ax ; add multipication to cx
    mov bx, cx ; move to bx as it is default input source
    jmp readInput
; <-- INPUT -->


; <-- OUTPUT -->
output: ; this is also from PS
    ; output interrupt
    mov ah, 02H
    mov dl, 10D ; \n
    int 21H
    ; end of output interrupt
    pop ax ; start popping the results
    mov bx, 10H ; div uses bx
    mov si, 0H
    mov dx, 0H ; clear the registers
    div bx ; remainder is in dx now, it is the last digit
    push dx ; push to output later
    add si, 1H ; cx is used for counting the digits in the stack
    mov dx, 0H ; clear the remainder (todo:: think about removing)
    div bx ; next digit is in remainder now
    push dx
    add si, 1H ; count the digits
    mov dx, 0H
    div bx ; next digit is in remainder now
    push dx
    add si, 1H ; count the digits
    push ax ; this must be the last digit, push it
    add si, 1H ; count the digits
    jmp outputFinish

outputFinish:
    mov ah, 02H ; start outputting
    pop dx ; this is the first digit
    cmp dl, 9H ; if it is not a number
        jg hexToOutput
    add dl, '0'

outputContinue:
    int 21H ; start interrupt
    sub si, 1H ; number of digit is decreased
    jnz outputFinish ; if there are still digits, go to outputContinue
    int 20H ; end interrupt

hexToOutput: ; hex to ascii; this is from PS
    sub dl, 10D
    add dl, "A"
    jmp outputContinue
; <-- OUTPUT -->

code ends