
CC = cl
CFLAGS = /nologo /utf-8 /std:c17 /Wall /Oi /GS- /Gs999999999 /Fe"find-techniques"
LFLAGS = /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /OPT:REF /STACK:0x100000,0x100000 /NODEFAULTLIB Kernel32.lib Ole32.lib Uuid.lib Shlwapi.lib
MERGEFLAGS = /MERGE:.rdata=. /MERGE:.pdata=. /MERGE:.text=. /SECTION:.,ER,ALIGN:16 
RELEASE = /Os /O2
DEBUG = /Od /Zi /MD


all: main.obj
.PHONY: all

main.obj: main.c main.h utils.h
	@$(CC) main.c $(CFLAGS) $(RELEASE) /link $(LFLAGS) $(MERGEFLAGS)


clean: 
	@del /f main.obj main.exe
