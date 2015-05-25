/*
->==================================================<-
->= Selmas Memory Game - © Copyright 2015 OnyxSoft =<-
->==================================================<-
->= Version  : 0.1                                 =<-
->= File     : SelmasMemory.c                      =<-
->= Author   : Stefan Blixth                       =<-
->= Compiled : 2015-05-02                          =<-
->==================================================<-
*/

#include "SelmasMemory.h"
#include "SelmasMemory_gfx.h"
#include "debug.h"

/*=----------------------------- Patches and Macros()-------------------------*
 * Patching some incompatible functions and adds some useful macros           *
 *----------------------------------------------------------------------------*/
#ifndef __MORPHOS__
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  Object *rc;
  VA_LIST args;

  VA_START(args, obj);
  rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, VA_ARG(args, IPTR), NULL);
  VA_END(args);

  return rc;
}
#endif

#ifdef __amigaos4__
 #define GETIFACE(x)    (I ## x = (struct x ## IFace *) GetInterface((struct Library *) x ## Base, "main", 1L, NULL))
 #define DROPIFACE(x)    DropInterface((struct Interface *) I ## x);
#else
 #define GETIFACE(x)
 #define DROPIFACE(x)
#endif

#ifdef __AROS__
 #define MUI_Requester(a,b,c,d,e) MUI_Request(a, b, 0, c, d, e, 0)
 #ifndef MUIV_Frame_Window
   #define MUIV_Frame_Window  18
 #endif
#else
 #define MUI_Requester(a,b,c,d,e) MUI_Request(a, b, 0, c, d, e)
#endif

/*=*/

/*=----------------------------- render_new() --------------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
static ULONG render_new(struct IClass *cl, Object *obj, struct opSet *msg)
{
   struct Data *data;
   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);

   obj = DoSuperNew(cl, obj,
                    Child, grp_main = GroupObject, NoFrame,
                       Child, grp_cardboard = GroupObject, MUIA_Group_Columns, 8, End,
                    End, TAG_MORE, msg->ops_AttrList);

   if (!obj)
   {
      return(0);
   }
   
   data = (struct Data *)INST_DATA(cl, obj);

   //data->active   = FALSE;
   data->cardcntr = 0;
   data->current  = 0;
   data->previous = 0;
   data->pairstep = 0;

   return((ULONG)obj);
}
/*=*/

/*=----------------------------- render_dispose() ----------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
static ULONG render_dispose(struct IClass *cl, Object *obj, Msg msg)
{
   //struct Data *data = (struct Data *)INST_DATA(cl, obj);
   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);
   
   return(DoSuperMethodA(cl, obj, msg));
}
/*=*/

/*=----------------------------- render_select() -----------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
static ULONG render_select(struct Data *data, struct MUIP_SelectCard *msg)
{
   LONG gaugeval=0, maxgauge=0;
   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);

   if (data->card_data[msg->Val].shown == TRUE)
      return 0;
   else
   {
      data->cardcntr++;
      
      if (data->cardcntr <= CARD_PAIR)
      {
         data->previous = data->current;
         data->current  = msg->Val;
         set(card_img[data->current], MUIA_Rawimage_Data, cards[data->card_data[data->current].position]);
         data->card_data[data->current].shown = TRUE;
         
         if (data->cardcntr == CARD_PAIR)
         {
            if (data->card_data[data->current].position == data->card_data[data->previous].position)
            {
               data->card_data[data->current].paired = TRUE;
               data->card_data[data->previous].paired = TRUE;
               
               get(gau_timer, MUIA_Gauge_Max, &maxgauge);
               get(gau_timer, MUIA_Gauge_Current, &gaugeval);
               set(gau_timer, MUIA_Gauge_Current, gaugeval+1);
               
               if ((gaugeval+1) >= maxgauge)
                  MUI_Requester(NULL, NULL, "Game finished!", "Continue", "Congratulations!\n\nYou found each and one of the pairs!");
            }
         }
         
      }
      else // Check the selected pair...
      {
         if (data->card_data[data->current].position != data->card_data[data->previous].position)
         {
            if (data->card_data[data->current].paired == FALSE)
            {
               set(card_img[data->current], MUIA_Rawimage_Data, cards[CARD_NONE]);
               data->card_data[data->current].shown = FALSE; 
               data->current = 0;
            }
         
            if (data->card_data[data->previous].paired == FALSE)
            {
               set(card_img[data->previous], MUIA_Rawimage_Data, cards[CARD_NONE]);
               data->card_data[data->previous].shown = FALSE; 
               data->previous = 0;
            }
         }
         
         data->cardcntr = 0;

         data->cardcntr++;
         data->previous  = data->current;
         data->current = msg->Val;
         set(card_img[data->current], MUIA_Rawimage_Data, cards[data->card_data[data->current].position]);
         data->card_data[data->current].shown = TRUE;
      }
   }
   return 0;
}
/*=*/

/*=----------------------------- render_newgame() ----------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
static ULONG render_newgame(struct Data *data)
{
   //struct Data *data = (struct Data *)INST_DATA(cl, obj);
   LONG cardcnt;
   int xcnt, xnum, xval, cardrand;
   char paired;
   
   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);
   
   for (cardcnt = 0; cardcnt < data->numofcards; cardcnt++)
   {
      data->card_data[cardcnt].position = 0;
      data->card_data[cardcnt].paired = FALSE;
      data->card_data[cardcnt].shown = FALSE; 
      set(card_img[cardcnt], MUIA_Rawimage_Data, cards[0]);
   }
   
   //data->active = FALSE;
   data->previous  = 0;
   data->current = 0;
   data->cardcntr = 0;

   // Randomize the used card on the board...
   cardrand = data->numofcards/2;

   srand(time(NULL));
   for (xcnt=0; xcnt < data->numofcards; xcnt++)
   {
      do
      {
         xval = (rand()%cardrand)+1;
         paired = 0;
         
         for (xnum=0; xnum < xcnt; xnum++)
         {
            if (data->card_data[xnum].position == xval)
            {
               paired++;   
            }
        }

        if (paired < 2) 
           data->card_data[xnum].position = xval;

     } while(paired >= 2);
     
   }
   
   set(gau_timer, MUIA_Gauge_Current, 0);
   return 0;
}
/*=*/

/*=----------------------------- render_newboard() ---------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
static ULONG render_newboard(struct IClass *cl, Object *obj, Msg msg) //struct Data *data)
{
   LONG size = 0, colsize = 3, cardnum = 6, cardcnt;

   struct Data *data = (struct Data *)INST_DATA(cl, obj);
   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);


   get(cyc_boardsize, MUIA_Cycle_Active, &size);

   if (size == 1)      // Medium
   {
      colsize = 6;
      cardnum = 18;
   }
   else if (size == 2) // Big
   {
      colsize = 8;
      cardnum = 32;
   }
   else                // Tiny
   {
      colsize = 3;
      cardnum = 6;
   }
   
   if (cardnumstore)
   {
      if(DoMethod(grp_cardboard, MUIM_Group_InitChange))
      {
         for (cardcnt = 0; cardcnt < cardnumstore; cardcnt++)
            DoMethod(grp_cardboard, OM_REMMEMBER, card_img[cardcnt]);
            
         DoMethod(grp_cardboard, MUIM_Group_ExitChange);
      }
   }


   if(DoMethod(grp_main, MUIM_Group_InitChange)) 
   { 
      DoMethod(grp_main, OM_REMMEMBER, grp_cardboard);
      grp_cardboard = GroupObject, MUIA_Group_Columns, colsize, End;
      DoMethod(grp_main, OM_ADDMEMBER, grp_cardboard); 
      DoMethod(grp_main, MUIM_Group_ExitChange); 
   }


   if(DoMethod(grp_cardboard, MUIM_Group_InitChange))
   {
      cardnumstore = cardnum;

      for (cardcnt = 0; cardcnt < cardnum; cardcnt++)
      {
         Child, card_img[cardcnt] = RawimageObject,
            MUIA_DoubleBuffer, 0,
            MUIA_Frame, MUIV_Frame_Button,
            MUIA_InputMode, MUIV_InputMode_RelVerify,
            MUIA_Rawimage_Data, cards[0],
         End,

         DoMethod(grp_cardboard, OM_ADDMEMBER, card_img[cardcnt]); 

         // Add check and do action if the object failes to be added ?
      }

      DoMethod(grp_cardboard, MUIM_Group_ExitChange); 
   }
   
   data->numofcards = cardnumstore;
   
   for (cardcnt = 0; cardcnt < cardnum; cardcnt++)
      DoMethod(card_img[cardcnt], MUIM_Notify, MUIA_Pressed, TRUE, obj, 2, MUIM_SelectCard, cardcnt);

   data->pairstep = cardnum/2;

   set(gau_timer, MUIA_Gauge_Max, data->pairstep);
   set(gau_timer, MUIA_Gauge_Current, 0);
   
   //return obj;
   return 0;
}


/*=----------------------------- DISPATCHER ----------------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
DISPATCHER(Render)
{
#ifdef EXTDEBUG
   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);
#endif

   struct Data *data = (struct Data *)INST_DATA(cl, obj);

   switch (msg->MethodID)
   {
      case OM_NEW             : return render_new       (cl, obj, (APTR)msg); break;
      case OM_DISPOSE         : return render_dispose   (cl, obj, (APTR)msg); break;
      case MUIM_SelectCard    : return render_select    (data, (APTR)msg); break;
      case MUIM_NewGame       : return render_newgame   (data); break;
      case MUIM_NewBoard      : return render_newboard  (cl, obj, (APTR)msg); break;
   }

   return DoSuperMethodA(cl, obj, msg);
}
/*=*/

BOOL init(void)
{
   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);
   
   if (!(IconBase = (struct Library *)OpenLibrary("icon.library", 37L))) return FALSE;
   if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 37L))) return FALSE;
   if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37L))) return FALSE;
   if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN))) return FALSE;
   if (!(UtilityBase = (UTILITYBASE_TYPE *)OpenLibrary("utility.library", 39))) return FALSE;

   GETIFACE(Icon);
#ifdef __amigaos4__
   if (!(IGraphics = (struct GraphicsIFace*)GetInterface((struct Library*)GfxBase,"main",1,NULL))) return FALSE; //GETIFACE(Graphics);
#endif
   GETIFACE(Intuition);
   GETIFACE(MUIMaster);
   GETIFACE(Utility);

   if (!(board_mcc = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), ENTRY(Render)))) return FALSE;

   return TRUE;
}

void cleanup(void)
{
   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);

   DROPIFACE(Icon);
   DROPIFACE(Graphics);
   DROPIFACE(Intuition);
   DROPIFACE(MUIMaster);
   DROPIFACE(Utility);
   
   if (app) MUI_DisposeObject(app);
   if (board_mcc) MUI_DeleteCustomClass(board_mcc);
   if (UtilityBase) CloseLibrary((struct Library *) UtilityBase);
   if (MUIMasterBase) CloseLibrary(MUIMasterBase);
   if (IntuitionBase) CloseLibrary((struct Library *) IntuitionBase);
   if (IconBase) CloseLibrary((struct Library *) IconBase);
   if (GfxBase) CloseLibrary((struct Library *) GfxBase);
}
/*=*/

/*=----------------------------- opengui() -----------------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
BOOL opengui(void)
{
   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);

   app = ApplicationObject, MUIA_Application_Title           , NAME,
                            MUIA_Application_Version         , VERSTAG_MUI,
                            MUIA_Application_Copyright       , COPYRIGHT,
                            MUIA_Application_Author          , AUTHOR,
                            MUIA_Application_Description     , "Selmas Memory Game",
                            MUIA_Application_Base            , "SELMASMEMORYGAME",
                            MUIA_Application_UseCommodities  , TRUE,

                            SubWindow, aboutbox = AboutboxObject,
                               MUIA_Window_ID,         MAKE_ID('A','B','O','U'),
                               MUIA_Aboutbox_Credits,  credits,
                            End,  // SubWindow
                        
                            SubWindow, win_main = WindowObject,
                               MUIA_Frame,                MUIV_Frame_None,
                               MUIA_Window_Borderless,    FALSE,
                               MUIA_Window_CloseGadget,   TRUE,
                               MUIA_Window_DepthGadget,   TRUE,
                               MUIA_Window_SizeGadget,    TRUE,
                               MUIA_Window_DragBar,       TRUE,
                               MUIA_Window_ID,            MAKE_ID('M','A','I','N'),
                               MUIA_Window_Title,         NAME,
                               MUIA_Window_ScreenTitle,   VERSTAG_SCREEN,
                        
                               WindowContents, VGroup,
                                  MUIA_Frame,       MUIV_Frame_None,
                                  MUIA_InnerLeft,   0,
                                  MUIA_InnerRight,  0,
                                  MUIA_InnerTop,    0,
                                  MUIA_InnerBottom, 0,

                                  Child, HGroup,

                                     Child, board_obj = NewObject(board_mcc->mcc_Class, NULL, NoFrame, TAG_DONE),

                                     Child, GroupObject, NoFrame,
                                  
                                        Child, but_newgame = TextObject, ButtonFrame,
                                           MUIA_Background, MUII_ButtonBack,
                                           MUIA_Weight, 0,
                                           MUIA_Text_PreParse, "\33c",
                                           MUIA_Text_Contents, "New Game",
                                           MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        End,
                                        Child, but_aboutgame = TextObject, ButtonFrame,
                                           MUIA_Background, MUII_ButtonBack,
                                           MUIA_Weight, 0,
                                           MUIA_Text_PreParse, "\33c",
                                           MUIA_Text_Contents, "About Game",
                                           MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        End,
                                        Child, but_prefs = TextObject, ButtonFrame,
                                           MUIA_Background, MUII_ButtonBack,
                                           MUIA_Weight, 0,
                                           MUIA_Text_PreParse, "\33c",
                                           MUIA_Text_Contents, "Settings",
                                           MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        End,
                                        
                                       Child, gau_timer = GaugeObject, GaugeFrame,
                                          MUIA_Gauge_Max, 32,
                                          MUIA_Gauge_Current, 0,
                                          MUIA_VertWeight, 100,
                                          MUIA_HorizWeight, 0,
                                          MUIA_Gauge_Horiz, FALSE,
                                       End,
                                     End,
                                  End,
                               End,
                            End, // SubWindow
                            
                            SubWindow, win_prefs = WindowObject,
                               MUIA_Frame,                MUIV_Frame_Window,
                               MUIA_Window_Borderless,    FALSE,
                               MUIA_Window_CloseGadget,   TRUE,
                               MUIA_Window_DepthGadget,   TRUE,
                               MUIA_Window_SizeGadget,    TRUE,
                               MUIA_Window_DragBar,       TRUE,
                               MUIA_Window_ID,            MAKE_ID('P','R','F','S'),
                               MUIA_Window_Title,         "Settings",
                               MUIA_Window_ScreenTitle,   VERSTAG_SCREEN,
                        
                               WindowContents, VGroup,
                                  MUIA_Frame,       MUIV_Frame_Window,
                                  
                                  Child, GroupObject, MUIA_Group_Columns, 2,
                                     Child, FreeLabel("Board size :"),
                                     Child, cyc_boardsize = CycleObject,
                                        MUIA_Cycle_Entries, boardsize,
                                        MUIA_ObjectID,      ID_OBJ_SIZE,
                                        MUIA_UserData,      ID_OBJ_SIZE,
                                     End,
//
//                                     Child, FreeLabel("Game timer :"),
//                                     Child, cyc_gametimer = CycleObject,
//                                        MUIA_Cycle_Entries, gametimer,
//                                        MUIA_ObjectID,      ID_OBJ_TIMER,
//                                        MUIA_UserData,      ID_OBJ_TIMER,
//                                     End,
                                  End,
 
                                  Child, GroupObject, MUIA_Group_Columns, 2,
                                     Child, but_cancel = TextObject, ButtonFrame,
                                        MUIA_Background, MUII_ButtonBack,
                                        MUIA_Text_PreParse, "\33c",
                                        MUIA_Text_Contents, "Cancel",
                                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                                     End,

                                     Child, but_use = TextObject, ButtonFrame,
                                        MUIA_Background, MUII_ButtonBack,
                                        MUIA_Text_PreParse, "\33c",
                                        MUIA_Text_Contents, "Use",
                                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                                     End,
                                  End,
                               End,
                            End, // SubWindow
                            
                         End;
                         
   if (!app)
   {
      puts("Could not create application!\n");
      return FALSE;
   }
   
   DoMethod(win_main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
   
   DoMethod(but_newgame, MUIM_Notify, MUIA_Pressed, TRUE, board_obj, 1, MUIM_NewGame);
      
   DoMethod(but_prefs, MUIM_Notify, MUIA_Pressed, FALSE, win_prefs, 3, MUIM_Set, MUIA_Window_Open, TRUE);
   DoMethod(but_cancel, MUIM_Notify, MUIA_Pressed, FALSE, win_prefs, 3, MUIM_Set, MUIA_Window_Open, FALSE);
   
   DoMethod(but_use, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_BUTTON_USE);
   DoMethod(win_prefs, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, win_prefs, 3, MUIM_Set, MUIA_Window_Open, FALSE);
   
   DoMethod(but_prefs, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_BUTTON_PREFS);
   DoMethod(but_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_BUTTON_CANCEL);
   DoMethod(but_aboutgame, MUIM_Notify, MUIA_Pressed, FALSE, aboutbox, 3, MUIM_Set, MUIA_Window_Open, TRUE);

   DoMethod(app, MUIM_Application_Load, MUIV_Application_Load_ENV);  // Was ENVARC
   
   DoMethod(board_obj, MUIM_NewBoard);
   DoMethod(board_obj, MUIM_NewGame);
   set(win_main, MUIA_Window_Open, TRUE);
   
   return TRUE;
}
/*=*/

/*=----------------------------- handler() -----------------------------------*
 *                                                                            *
 *----------------------------------------------------------------------------*/
void handler(void)
{
   BOOL going     = TRUE;
   ULONG muisig   = 0;

   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);
   
   while (going)
   {
      switch (DoMethod(app, MUIM_Application_NewInput, &muisig))
      {
         case MUIV_Application_ReturnID_Quit :
            DoMethod(app, MUIM_Application_Save, MUIV_Application_Save_ENV);
            DoMethod(app, MUIM_Application_Save, MUIV_Application_Save_ENVARC);

            going = FALSE;
            break;

         case ID_BUTTON_USE :
            set(win_prefs, MUIA_Window_Open, FALSE);
            DoMethod(board_obj, MUIM_NewBoard);
            DoMethod(board_obj, MUIM_NewGame);
            break;

         case ID_BUTTON_PREFS :
            get(cyc_boardsize, MUIA_Cycle_Active, &tmpsize);
            //get(cyc_gametimer, MUIA_Cycle_Active, &tmptimer);
            break;
         
         case ID_BUTTON_CANCEL :
            set(cyc_boardsize, MUIA_Cycle_Active, tmpsize);
            //set(cyc_gametimer, MUIA_Cycle_Active, tmptimer);
            break;
            
         default:
            break;
      }

      if (going && muisig)
         Wait(muisig);
   }
}
/*=*/


int main(void)
{
   debug_print("%s : %s (%d)\n", __FILE__ , __func__, __LINE__);

   if (init())
   {
      if (opengui())
      {
         handler();
      }
   }
   
   cleanup();

   return 0;
}

