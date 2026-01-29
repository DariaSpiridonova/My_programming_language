PUSH 0
PUSH 0 
POPR DI  
POPM [DI]
PUSH 0
PUSH 1 
POPR DI  
POPM [DI]
; ****************************************
PUSH 0
PUSH 2 
POPR DI  
POPM [DI]
; ****************************************
PUSH 0
PUSH 3 
POPR DI  
POPM [DI]
; ****************************************
PUSH 0
PUSH 4 
POPR DI  
POPM [DI]
; ****************************************
PUSH 0
PUSH 5 
POPR DI  
POPM [DI]
; ****************************************
PUSH 8
PUSH 6 
POPR DI  
POPM [DI]
; ****************************************
PUSH -1
PUSH 7 
POPR DI  
POPM [DI]
; ****************************************
IN
PUSH 0 
POPR DI  
POPM [DI]
; ****************************************
IN
PUSH 1 
POPR DI  
POPM [DI]
; ****************************************
IN
PUSH 2 
POPR DI  
POPM [DI]
; ****************************************
PUSH 0
PUSH 8 
POPR DI  
POPM [DI]
; ****************************************
JMP :0  

:200    ; definition of the "isLinear" function 
PUSH 9 
POPR DI  
POPM [DI]
PUSH 10 
POPR DI  
POPM [DI]
PUSH 11 
POPR DI  
POPM [DI]
:1 ; entering the "if" 
PUSH 11  
POPR SI   
PUSHM [SI]
PUSH 0
JE :2
POPR   BP
POPR   BP
PUSH 0
JMP :3
:2
PUSH 1
:3
PUSH 0 
JNE :4  

POPR   BP
POPR   BP
JMP :5  

:4  

; $$$$$$$$$$$$$$$$$$$$$$$$$

:8 ; entering the "if" 
PUSH 10  
POPR SI   
PUSHM [SI]
PUSH 0
JE :9
POPR   BP
POPR   BP
PUSH 0
JMP :10
:9
PUSH 1
:10
PUSH 0 
JNE :11  

POPR   BP
POPR   BP
JMP :12  

:11  

; $$$$$$$$$$$$$$$$$$$$$$$$$

:14 ; entering the "if" 
PUSH 9  
POPR SI   
PUSHM [SI]
PUSH 0
JE :15
POPR   BP
POPR   BP
PUSH 0
JMP :16
:15
PUSH 1
:16
PUSH 0 
JNE :17  

POPR   BP
POPR   BP
JMP :18  

:17  

PUSH 6  
POPR SI   
PUSHM [SI]
OUT
JMP :7 ; cond_stat_mark from if
:18
; entering the "else" 
PUSH 7  
POPR SI   
PUSHM [SI]
OUT
:7 ; exiting an if-else statement
JMP :6 ; cond_stat_mark from if
:12
; entering the "else" 
PUSH 0
PUSH 1
SUB
PUSH 9  
POPR SI   
PUSHM [SI]
MUL
PUSH 10  
POPR SI   
PUSHM [SI]
DIV
OUT
:6 ; exiting an if-else statement
PUSH 1
PUSH 8 
POPR DI  
POPM [DI]
; ****************************************
:5
RET  ; end of the definition of the "isLinear" function

:0 

; ****************************************
JMP :21  

:201    ; definition of the "calculateDiscriminant" function 
PUSH 9 
POPR DI  
POPM [DI]
PUSH 10 
POPR DI  
POPM [DI]
PUSH 11 
POPR DI  
POPM [DI]
PUSH 10  
POPR SI   
PUSHM [SI]
PUSH 10  
POPR SI   
PUSHM [SI]
MUL
PUSH 4
PUSH 11  
POPR SI   
PUSHM [SI]
MUL
PUSH 9  
POPR SI   
PUSHM [SI]
MUL
SUB
PUSH 3 
POPR DI  
POPM [DI]
RET  ; end of the definition of the "calculateDiscriminant" function

:21 

; ****************************************
JMP :22  

:202    ; definition of the "calculateRoots" function 
PUSH 12 
POPR DI  
POPM [DI]
PUSH 10 
POPR DI  
POPM [DI]
PUSH 11 
POPR DI  
POPM [DI]
; $$$$$$$$$$$$$$$$$$$$$$$$$

:25 ; entering the "if" 
PUSH 3  
POPR SI   
PUSHM [SI]
PUSH 0
JAE :26
POPR   BP
POPR   BP
PUSH 0
JMP :27
:26
PUSH 1
:27
PUSH 0 
JNE :28  

POPR   BP
POPR   BP
JMP :29  

:28  

PUSH 0
PUSH 1
SUB
PUSH 1  
POPR SI   
PUSHM [SI]
MUL
PUSH 3  
POPR SI   
PUSHM [SI]
PUSH 1
PUSH 2
DIV
DEG
ADD
PUSH 2
PUSH 0  
POPR SI   
PUSHM [SI]
MUL
DIV
PUSH 4 
POPR DI  
POPM [DI]
PUSH 0
PUSH 1
SUB
PUSH 1  
POPR SI   
PUSHM [SI]
MUL
PUSH 3  
POPR SI   
PUSHM [SI]
PUSH 1
PUSH 2
DIV
DEG
SUB
PUSH 2
PUSH 0  
POPR SI   
PUSHM [SI]
MUL
DIV
PUSH 5 
POPR DI  
POPM [DI]
; ****************************************
JMP :23 ; cond_stat_mark from if
:29
; entering the "else" 
PUSH -1
PUSH 4 
POPR DI  
POPM [DI]
PUSH -1
PUSH 5 
POPR DI  
POPM [DI]
; ****************************************
:23 ; exiting an if-else statement
RET  ; end of the definition of the "calculateRoots" function

:22 

; ****************************************
JMP :31  

:203    ; definition of the "printRoots" function 
PUSH 13 
POPR DI  
POPM [DI]
PUSH 14 
POPR DI  
POPM [DI]
PUSH 14  
POPR SI   
PUSHM [SI]
OUT
PUSH 13  
POPR SI   
PUSHM [SI]
OUT
; ****************************************
RET  ; end of the definition of the "printRoots" function

:31 

; ****************************************
PUSH 0  
POPR SI   
PUSHM [SI]
PUSH 1  
POPR SI   
PUSHM [SI]
PUSH 2  
POPR SI   
PUSHM [SI]
CALL :200    ; calling the "isLinear" function 
; ****************************************
:32 ; entering the "if" 
PUSH 8  
POPR SI   
PUSHM [SI]
PUSH 0
JE :33
POPR   BP
POPR   BP
PUSH 0
JMP :34
:33
PUSH 1
:34
PUSH 0 
JNE :35  

POPR   BP
POPR   BP
JMP :36  

:35  

PUSH 0  
POPR SI   
PUSHM [SI]
PUSH 1  
POPR SI   
PUSHM [SI]
PUSH 2  
POPR SI   
PUSHM [SI]
CALL :201    ; calling the "calculateDiscriminant" function 
PUSH 0  
POPR SI   
PUSHM [SI]
PUSH 1  
POPR SI   
PUSHM [SI]
PUSH 3  
POPR SI   
PUSHM [SI]
CALL :202    ; calling the "calculateRoots" function 
; ****************************************
PUSH 4  
POPR SI   
PUSHM [SI]
PUSH 5  
POPR SI   
PUSHM [SI]
CALL :203    ; calling the "printRoots" function 
; ****************************************
:36
; ****************************************
HLT
