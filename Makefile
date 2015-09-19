#->==================================================<-
#->= Selmas Memory Game - © Copyright 2015 OnyxSoft =<-
#->==================================================<-
#->= Version  : 0.2                                 =<-
#->= File     : Makefile                            =<-
#->= Author   : Stefan Blixth                       =<-
#->= Compiled : 2015-09-16                          =<-
#->==================================================<-

#
# Application specific stuff
#
APPNAME				= SelmasMemory

APP_MORPHOS			= $(APPNAME)_MorphOS
APP_MORPHOS_DB		= $(APP_MORPHOS)_db
APP_AROS				= $(APPNAME)_AROS
APP_AMIGAOS3		= $(APPNAME)_AmigaOS3
APP_AMIGAOS4		= $(APPNAME)_AmigaOS4
APP_AMIGAOS4_DB	= $(APP_AMIGAOS4)_db


#
# Compiler tools assign
#
CC_MORPHOS			= ppc-morphos-gcc
CC_AMIGAOS3			= vc
CC_AMIGAOS4			= ppc-amigaos-gcc
CC_AROS				= i386-aros-gcc
STRIP_MORPHOS		= ppc-morphos-strip
STRIP_AMIGAOS3		=
STRIP_AMIGAOS4		= ppc-amigaos-strip
STRIP_AROS			=



#
# Relative paths
#
OBJDIR				= obj
EXEDIR				= release


#
# Platform specific compiler and linker flags 
#
CFLG_MOS			= -noixemul -D__AMIGADATE__=\"$(shell date "+%d.%m.%y")\"
LFLG_MOS			=
OPTS_MOS			= -O2
OBJS_MOS			= $(APPNAME)_morphos.o

CFLG_MOS_DB		= -noixemul -Wall -D__AMIGADATE__=\"$(shell date "+%d.%m.%y")\" -DUSEDEBUG -g
LFLG_MOS_DB		= -ldebug -traditional-format
OPTS_MOS_DB		= 
OBJS_MOS_DB		= $(APPNAME)_morphos_db.o

CFLG_OS3		= +aos68k -O2 -c99 -DNO_INLINE_STDARG
LFLG_OS3		= +aos68k -lamiga
OPTS_OS3		= -O2
OBJS_OS3		= $(APPNAME)_amigaos3.o

CFLG_OS4			= -D__USE_BASETYPE__ -D__USE_INLINE__ -D__USE_OLD_TIMEVAL__
LFLG_OS4			= 
OPTS_OS4			= -O2
OBJS_OS4			= $(APPNAME)_amigaos4.o

CFLG_OS4_DB		=  -D__USE_BASETYPE__ -D__USE_INLINE__ -D__USE_OLD_TIMEVAL__ -DUSEDEBUG -g
LFLG_OS4_DB		= 
OPTS_OS4_DB		= 
OBJS_OS4_DB		= $(APPNAME)_amigaos4_db.o

CFLG_AROS		= -D__AMIGADATE__=\"$(shell date "+%d.%m.%y")\"
LFLG_AROS		= -lmui
OPTS_AROS		= -O2
OBJS_AROS		= $(APPNAME)_aros.o


#
# Source & include files
#
SOURCE_APP		= $(APPNAME).c

.PHONY:	clean usage


usage:
	@echo ""
	@echo "  Application - $(APPNAME)"
	@echo " +-------------------------------------------------------------------+"
	@echo " | clean       - Deletes all files in the obj & release-directories. |"
	@echo " | morphos_db  - Compiles a debug-enabled binary for MorphOS.        |"
	@echo " | amigaos4_db - Compiles a debug-enabled binary for AmigaOS4.       |"
	@echo " +-------------------------------------------------------------------+"
	@echo " | morphos     - Compiles a binary for MorphOS.                      |"
	@echo " | amigaos3    - Compiles a binary for AmigaOS 3.x                   |"
	@echo " | amigaos4    - Compiles a binary for AmigaOS 4.x                   |"
	@echo " | aros        - Compiles a binary for AROS.                         |"
	@echo " +-------------------------------------------------------------------+"
	@echo ""


clean:
	@echo "Cleaning up..."
	@echo ""
	rm -f $(OBJDIR)/*
	rm -f $(EXEDIR)/*
	@echo ""
	@echo "Done."


morphos_db: $(APP_MORPHOS_DB)
	@echo ""
	@echo "Debug-enabled MorphOS binary sucessfully built..."
	@echo ""

$(APP_MORPHOS_DB):	$(SOURCE_APP)
	$(CC_MORPHOS) $(SOURCE_APP) $(OPTS_MOS_DB) $(CFLG_MOS_DB) $(LFLG_MOS_DB) -o $(EXEDIR)/$(APP_MORPHOS_DB)


morphos: $(APP_MORPHOS)
	@echo ""
	@echo "MorphOS binary sucessfully built..."
	@echo ""

$(APP_MORPHOS):	$(SOURCE_APP)
	$(CC_MORPHOS) $(SOURCE_APP) $(OPTS_MOS) $(CFLG_MOS) $(LFLG_MOS) -o $(EXEDIR)/$(APP_MORPHOS)
	$(STRIP_MORPHOS) $(EXEDIR)/$(APP_MORPHOS)


amigaos3:	$(APP_AMIGAOS3)
	@echo ""
	@echo "AmigaOS3 binary sucessfully built..."
	@echo ""

$(APP_AMIGAOS3):	$(OBJS_OS3) 
	$(CC_AMIGAOS3) $(LFLG_OS3) -o $(EXEDIR)/$(APP_AMIGAOS3) $(OBJDIR)/$(OBJS_OS3)

$(OBJS_OS3):	$(SOURCE_APP)
	@echo ""
	@echo "Compiling a binary for AmigaOS3..."
	@echo ""
	$(CC_AMIGAOS3) $(CFLG_OS3) -c $(SOURCE_APP) -o $(OBJDIR)/$(OBJS_OS3)


amigaos4: $(APP_AMIGAOS4)
	@echo ""
	@echo "AmigaOS4 binary sucessfully built..."
	@echo ""

$(APP_AMIGAOS4):  $(SOURCE_APP)
	$(CC_AMIGAOS4)  $(SOURCE_APP) $(OPTS_OS4) $(CFLG_OS4) $(LFLG_OS4) -o $(EXEDIR)/$(APP_AMIGAOS4)


amigaos4_db: $(APP_AMIGAOS4_DB)
	@echo ""
	@echo "Debug-enabled AmigaOS4 binary sucessfully built..."
	@echo ""


$(APP_AMIGAOS4_DB):  $(SOURCE_APP)
	$(CC_AMIGAOS4) $(SOURCE_APP) $(OPTS_OS4_DB) $(CFLG_OS4_DB) $(LFLG_OS4_DB) -o $(EXEDIR)/$(APP_AMIGAOS4_DB)


aros: $(APP_AROS)
	@echo ""
	@echo "AROS binary sucessfully built..."
	@echo ""

$(APP_AROS):	$(SOURCE_APP)
	$(CC_AROS) $(SOURCE_APP) $(OPTS_AROS) $(CFLG_AROS) $(LFLG_AROS) -o $(EXEDIR)/$(APP_AROS)
