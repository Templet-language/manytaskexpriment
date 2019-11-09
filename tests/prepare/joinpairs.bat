@echo on
SETLOCAL EnableDelayedExpansion

SET count=0
SET next=0
SET par=EVEN
SET fname=
SET fnamenext=

FOR /F "tokens=*" %%G IN ('dir /b E:\*') DO (
    SET fname=file
    SET fnamenext=file
    SET /A next = count + 1	
	
	rem IF !count! LSS 10000 SET fname=!fname!0
	rem IF !count! LSS 1000 SET fname=!fname!0
    IF !count! LSS 100 SET fname=!fname!0
	IF !count! LSS 10 SET fname=!fname!0
 	
	rem IF !next! LSS 10000 SET fnamenext=!fnamenext!0
	rem IF !next! LSS 1000 SET fnamenext=!fnamenext!0
    IF !next! LSS 100 SET fnamenext=!fnamenext!0
	IF !next! LSS 10 SET fnamenext=!fnamenext!0
	
	SET fname=!fname!!count!.v00
	SET fnamenext=!fnamenext!!next!.v00
	
	IF /I !par!==EVEN COPY  E:\!fname! + E:\!fnamenext! E:\!fname!.par
    IF /I !par!==EVEN ERASE E:\!fname! /Q
    IF /I !par!==EVEN ERASE E:\!fnamenext! /Q	
	
	SET /A count = count + 1
	
	IF /I !par!==EVEN (
	 SET par=ODD
	) ELSE (
	 SET par=EVEN
	) 
)