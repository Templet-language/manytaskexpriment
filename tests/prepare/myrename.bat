@echo on
SETLOCAL EnableDelayedExpansion

SET count=0
SET fname=

FOR /F "tokens=*" %%G IN ('dir /b D:\GitHub\manytaskexpriment\tests\files\*') DO (
	SET fname=file
	
	IF !count! LSS 10000 SET fname=!fname!0
	IF !count! LSS 1000 SET fname=!fname!0
    IF !count! LSS 100 SET fname=!fname!0
	IF !count! LSS 10 SET fname=!fname!0
 	
	REN .\files\"%%G" !fname!!count!.txt
	
	SET /A count = count + 1 
)
