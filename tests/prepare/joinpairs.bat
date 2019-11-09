@echo on
SETLOCAL EnableDelayedExpansion

SET count=0
SET next=0
SET par=EVEN
SET fname=
SET fnamenext=

FOR /F "tokens=*" %%G IN ('dir /b D:\GitHub\manytaskexpriment\tests\files\*') DO (
    SET fname=file
    SET fnamenext=file
    SET /A next = count + 1	
	
	IF !count! LSS 10000 SET fname=!fname!0
	IF !count! LSS 1000 SET fname=!fname!0
    IF !count! LSS 100 SET fname=!fname!0
	IF !count! LSS 10 SET fname=!fname!0
 	
	IF !next! LSS 10000 SET fnamenext=!fnamenext!0
	IF !next! LSS 1000 SET fnamenext=!fnamenext!0
    IF !next! LSS 100 SET fnamenext=!fnamenext!0
	IF !next! LSS 10 SET fnamenext=!fnamenext!0
	
	SET fname=!fname!!count!.txt
	SET fnamenext=!fnamenext!!next!.txt
	
	IF /I !par!==EVEN COPY  .\files\!fname! + .\files\!fnamenext! .\files\!fname!.par
    IF /I !par!==EVEN ERASE .\files\!fname! /Q
    IF /I !par!==EVEN ERASE .\files\!fnamenext! /Q	
	
	SET /A count = count + 1
	
	IF /I !par!==EVEN (
	 SET par=ODD
	) ELSE (
	 SET par=EVEN
	) 
)
