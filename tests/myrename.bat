@echo off
SETLOCAL EnableDelayedExpansion

SET count=0
SET fname=

FOR /F "tokens=*" %%G IN ('dir /b D:\everest\*.txt') DO (
	SET fname=file
	
	IF !count! LSS 10000 SET fname=!fname!0
	IF !count! LSS 1000 SET fname=!fname!0
        IF !count! LSS 100 SET fname=!fname!0
	IF !count! LSS 10 SET fname=!fname!0
 	
	REN "%%G" !fname!!count!.txt
	
	SET /A count = count + 1 
)
