@echo on
SETLOCAL EnableDelayedExpansion

SET count=0
SET fname=

FOR /F "tokens=*" %%G IN ('dir /b E:\*') DO (
	SET fname=file
	
	rem IF !count! LSS 10000 SET fname=!fname!0
	rem IF !count! LSS 1000 SET fname=!fname!0
    IF !count! LSS 100 SET fname=!fname!0
	IF !count! LSS 10 SET fname=!fname!0
 	
	REN "E:\%%G" !fname!!count!.v00
	
	SET /A count = count + 1 
)