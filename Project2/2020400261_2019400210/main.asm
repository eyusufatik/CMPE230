;    Authors: Esad Yusuf Atik, Orkun Mahir Kılıç

;    Algorithm for evaluating a postfix expression:
;    1. Create a stack.
;    2. Scan the input starting from left.
;    3. If input is a hex number, push it to stack.
;    4. If input is operator, pop last two from stack and evaluate with operator.
;
;    Source: https://www.geeksforgeeks.org/stack-set-4-evaluation-postfix-expression/

; ORDER OF LABELS (It is important as a86 gives error if you jump > 128)
; code segment -> initialization
; stack operation
; operator operations
; input operations
; output operations


code segment ; initialize all registers, di is used to determine input's type, si is used to count output digits
    mov ax, 0H
    mov bx, 0H
    mov cx, 0H
    mov dx, 0H
    mov di, 0H
    mov si, 4H ; number of digits to output
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
    jmp goBackToReadInput

divideLastTwo:
    pop bx 
    pop ax 
    mov dx, 0H
    div bx ; div -> al(ax) = ax / bx
    push ax ; push the result
    jmp goBackToReadInput

multiplyLastTwo:
    pop ax 
    pop bx
    mov dx, 0H
    mul bx ; mul -> ax = al(ax) * bx
    push ax ; push the result
    jmp goBackToReadInput

andLastTwo:
    pop ax 
    pop bx 
    and ax, bx
    push ax ; push the result
    jmp goBackToReadInput

orLastTwo:
    pop ax 
    pop bx 
    or ax, bx
    push ax ; push the result
    jmp goBackToReadInput

xorLastTwo:
    pop ax 
    pop bx 
    xor ax, bx
    push ax ; push the result
    jmp goBackToReadInput
; <-- OPERATIONS -->

; <-- INPUT -->
goBackToReadInput:
    mov di, 0H
    mov bx, 0H ; clear the registers
    jmp readInput

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
        je prepareOutput
    mov di, 01H ; this is like else (input is not one of the operators)
    cmp al, 3AH ; 3ah is the first ascii char after numbers
        jg inputToNumber ; jg because if it is bigger than 3ah, it is a number
    sub al, 30H  ; convert ascii to mathematical number
    jmp multiDigitNumber ; number is like '123'

inputToNumber: ; this part is from PS
    sub al, 37H ; subtract 37H to get the number
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
prepareOutput: ; this is also from PS
    ; output interrupt
    mov ah, 02H
    mov dl, 10D ; \n
    int 21H
    ; end of output interrupt
    pop ax ; start popping the results
    mov bx, 10H ; div uses bx
    mov dx, 0H ; clear dx
    div bx ; remainder is in dx now, it is the last digit
    push dx ; push to output later
    mov dx, 0H ; clear dx
    div bx ; next digit is in remainder now
    push dx
    mov dx, 0H ; clear dx
    div bx ; next digit is in remainder now
    push dx
    push ax ; this must be the last digit, push
    jmp output

output:
    mov ah, 02H ; start outputting
    pop dx ; this is the first digit
    cmp dl, 9H ; if it is not a number
        jg hexToOutput
    add dl, 30H

outputContinue:
    int 21H ; start interrupt
    sub si, 1H ; number of digit is decreased
    jnz output ; if there are still digits, go to outputContinue
    int 20H ; end interrupt

hexToOutput: ; this is from PS
    add dl, 37H ; add 37H to get the number
    jmp outputContinue
; <-- OUTPUT -->

code ends
