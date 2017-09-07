/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: PanelCB */
#define  PANEL_OPEN_BIN_FILE              2       /* control type: command, callback function: OpenBinFilePanelCallback */
#define  PANEL_COM1_SEND_DATA             3       /* control type: command, callback function: SetSendFlgCallback */
#define  PANEL_SETPARA_2                  4       /* control type: command, callback function: OpenSetModelPanelCallback */
#define  PANEL_SETPARA                    5       /* control type: command, callback function: OpenSetParaPanelCallback */
#define  PANEL_OPENCOM1                   6       /* control type: command, callback function: OpenCom1Callback */
#define  PANEL_QUIT_2                     7       /* control type: command, callback function: ClearTextBoxCallback */
#define  PANEL_QUIT                       8       /* control type: command, callback function: Quit */
#define  PANEL_TIMER                      9       /* control type: timer, callback function: SysTickCallback */
#define  PANEL_INFOTEXTBOX                10      /* control type: textBox, callback function: (none) */
#define  PANEL_DECORATION_2               11      /* control type: deco, callback function: (none) */
#define  PANEL_SOURCEADDR                 12      /* control type: numeric, callback function: (none) */
#define  PANEL_FRAMCODE                   13      /* control type: numeric, callback function: (none) */
#define  PANEL_DESTADDR                   14      /* control type: numeric, callback function: (none) */
#define  PANEL_DELAYMS                    15      /* control type: numeric, callback function: (none) */
#define  PANEL_FRAMNUM                    16      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_2                  17      /* control type: textMsg, callback function: (none) */
#define  PANEL_COM1SENDTEXTBOX            18      /* control type: string, callback function: (none) */
#define  PANEL_RECVFMT                    19      /* control type: radioButton, callback function: (none) */
#define  PANEL_SENDFMT                    20      /* control type: radioButton, callback function: (none) */
#define  PANEL_TEXTMSG_3                  21      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_4                  22      /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION                 23      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG                    24      /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION_3               25      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK ClearTextBoxCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenBinFilePanelCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenCom1Callback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenSetModelPanelCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenSetParaPanelCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Quit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetSendFlgCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SysTickCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
