###############################################################################
# Makefile for project 
###############################################################################

## General options
PROJECT = 
MCU = 80C31
COMPILER = "Keil for 8051"
TARGET = Debug
SHELL = C:\Windows\system32\cmd.exe

## Tools general options
CCFLAGS=ROM(SMALL) BROWSE DEBUG CODE OBJECTEXTEND PREPRINT

## Processing Tools
CC=C51.exe
LD=BL51.exe
ASM=A51.exe
#  D:/我的坚果云/教学区/宁夏大学教学/单片机原理及应用/个人练习/80C31/Debug/Debug.OMF: TOOL=LD
#     D:/我的坚果云/教学区/宁夏大学教学/单片机原理及应用/个人练习/80C31/Debug/main.OBJ: TOOL=CC
#        D:/我的坚果云/教学区/宁夏大学教学/单片机原理及应用/个人练习/main.c

# Build tree:
all:	Debug

Debug:	Debug.OMF

Debug.OMF:	main.OBJ 
	"..\..\..\..\..\..\..\Program Files (x86)\Labcenter Electronics\Proteus 8 Professional\Tools\MAKE\RunTool.exe" --good-exits=0,1 --executable=$(LD) "main.OBJ" TO "Debug.OMF" $(LDFLAGS) 

main.OBJ:	../../main.c 
	"..\..\..\..\..\..\..\Program Files (x86)\Labcenter Electronics\Proteus 8 Professional\Tools\MAKE\RunTool.exe" --good-exits=0,1 --executable=$(CC) "..\..\main.c" $(CCFLAGS)  OBJECT("main.OBJ")

# tidy - delete all temporary files which are not involved in the target generation
tidy:
	rm -rf Debug.M51
	rm -rf ./main.lst
	rm -rf ./main.I

# cleanup - delete all generated files
clean:	tidy
	rm -rf Debug.OMF
	rm -rf main.OBJ
