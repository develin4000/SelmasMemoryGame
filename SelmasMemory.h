/*
->==================================================<-
->= Selmas Memory Game - Â© Copyright 2015 OnyxSoft =<-
->==================================================<-
->= Version  : 0.1                                 =<-
->= File     : SelmasMemory.h                      =<-
->= Author   : Stefan Blixth                       =<-
->= Compiled : 2015-05-02                          =<-
->==================================================<-
*/

#ifndef SELMASMEMORYMUI_H_
#define SELMASMEMORYMUI_H_

/* Standard... */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* System... */
#if !defined (__amigaos4__) || !defined(__AROS__)
 #include <clib/alib_protos.h>
#endif

#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/exec.h>
//#include <proto/asl.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/gadtools.h>
#include <libraries/mui.h>
#include <libraries/iffparse.h> /* for MAKE_ID */
#include <intuition/intuitionbase.h>

#include <mui/Rawimage_mcc.h>
#include <mui/Aboutbox_mcc.h>

#include "SDI_hook.h"
#include "SDI_stdarg.h"

#define MAX_CARDS 32 // 48 // 8*6 cards...


#define VYEARS    "2015"
#define AUTHOR    "Stefan Blixth & Selma Blixth"
#define EMAIL     "stefan@onyxsoft.se"
#define URL       "www.onyxsoft.se"
#define COMPANY   "OnyxSoft"
#define COPYRIGHT "© " VYEARS " " AUTHOR ", " COMPANY

#define VERSION   "0"
#define REVISION  "1"

#ifndef __AMIGADATE__
 #define __AMIGADATE__   "02.05.15"
#endif

#define DATE            __AMIGADATE__

#define NAME            "Selmas Memory Game"
#define VERS            "Selmas Memory Game "VERSION"."REVISION

#if defined (__MORPHOS__)
   #define VERSSHORT       VERSION"."REVISION" [MorphOS, PowerPC]"
#elif defined (__AROS__)
   #define VERSSHORT       VERSION"."REVISION" [AROS, x86]"
#elif defined (__amigaos4__)
   #define VERSSHORT       VERSION"."REVISION" [AmigaOS, PowerPC]"
#else
   #define VERSSHORT       VERSION"."REVISION" [AmigaOS, 68k]"
#endif


#define VSTRING         VERS" ("DATE") © "VYEARS " " AUTHOR ", " COMPANY
#define VERSTAG         "$VER:" VSTRING
#define VERSTAG_MUI     "$VER: "VERS " ("DATE")"
#define VERSTAG_SCREEN  VERS " ("DATE") "
#define ABOUT           "Selmas Memory Game is a MUI-based memory game.\n\nAuthor : "AUTHOR " ("EMAIL")\nVersion : "VERSSHORT"\n\n\33c"URL"\n\n\33cÂ©Copyright "VYEARS " " COMPANY


static const char version[] = VERSTAG;

static char *credits =
   "\33b%p\33n\n"           // Programming:
   "\tStefan Blixth\n"
   "\n"
   "\33b%g\33n\n"           // Graphics:
   "\tSelma Blixth\n"
   "\n"
   "\33b%W\33n\n"           // Web support:
   "\twww.onyxsoft.se";
#ifndef MAKE_ID
 #define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif


#if defined (__VBCC__) || defined(__AROS__) || defined(__amigaos4__)
 #define UTILITYBASE_TYPE   struct UtilityBase
#else
 #define UTILITYBASE_TYPE   struct Library
#endif

struct GfxBase       *GfxBase       = NULL;
struct Library       *IconBase      = NULL;
struct Library       *MUIMasterBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;

#ifdef __amigaos4__
struct IntuitionIFace   *IIntuition = NULL;
struct GraphicsIFace    *IGraphics  = NULL;
struct IconIFace        *IIcon      = NULL;
struct MUIMasterIFace   *IMUIMaster = NULL;
struct UtilityIFace     *IUtility   = NULL;
#endif

LONG __stack = 16384;

struct CardData
{
   WORD   position;
   BOOL   shown;
   BOOL   paired;
};

struct Data
{
   //BOOL   active;
   ULONG  refrate;

   char cardcntr;
   char current;
   char previous;
   char pairstep;
   ULONG numofcards;
   struct CardData card_data[MAX_CARDS];
};

// Enums for objects
enum
{
   ID_OBJ_SIZE = 1,
   ID_OBJ_LEVEL,
   ID_OBJ_TIMER,
   ID_OBJ_SCORE,
   ID_BUTTON_USE,
   ID_BUTTON_CANCEL,
   ID_BUTTON_PREFS,
   ID_BUTTON_NEWGAME,
   ID_BUTTON_ABOUTGAME
};

#define CARD_NONE 0
#define CARD_PAIR 2

static char *boardsize[]= { "Tiny", "Medium", "Big", NULL };
//static char *gametimer[]= { "None", "Counter", "Deadline - Slow", "Deadline - Quick", NULL };

#define SERIALNUMBER        (1)
#define TAGBASE_DEVELIN     (TAG_USER | (SERIALNUMBER<<16))
#define MUIM_TriggerRedraw  (TAGBASE_DEVELIN | 0x0001)
#define MUIM_TriggerTime    (TAGBASE_DEVELIN | 0x0002)
#define MUIM_SelectCard     (TAGBASE_DEVELIN | 0x0003)
#define MUIM_NewGame        (TAGBASE_DEVELIN | 0x0004)
#define MUIM_NewBoard       (TAGBASE_DEVELIN | 0x0005)

struct MUIP_SelectCard      { ULONG MethodID; ULONG Val; };


/* IO macros */
#define IO_SIGBIT(req)  ((LONG)(((struct IORequest *)req)->io_Message.mn_ReplyPort->mp_SigBit))
#define IO_SIGMASK(req) ((LONG)(1L<<IO_SIGBIT(req)))

/* Global variables... */
static Object *app        = NULL;  // MUI-Application object
static Object *board_obj  = NULL;  // MUI-MCC object
static Object *win_main   = NULL;  // MUI-Window object
static Object *win_prefs  = NULL;  // MUI-Window object
static Object *grp_main   = NULL;
static Object *card_img[MAX_CARDS];

Object *grp_cardboard     = NULL;
Object *but_prefs         = NULL;
Object *but_newgame       = NULL;
Object *but_aboutgame     = NULL;
Object *but_use           = NULL;
Object *but_cancel        = NULL;
Object *cyc_boardsize     = NULL;
//Object *cyc_gametimer     = NULL;
Object *gau_timer         = NULL;
LONG temps                = 0;
LONG cardnumstore         = 0;
LONG tmpsize              = 0;
LONG tmptimer             = 0;

APTR aboutbox;
APTR mi_about;

struct MUI_CustomClass *board_mcc = NULL;

#endif /* SELMASMEMORYMUI_H_ */

