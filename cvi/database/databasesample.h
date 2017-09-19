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

#define  SETM_PANEL                       1
#define  SETM_PANEL_TABLE                 2       /* control type: table, callback function: tableCB */
#define  SETM_PANEL_CMD_MODIFY            3       /* control type: command, callback function: modifydata */
#define  SETM_PANEL_CMD_INSERT            4       /* control type: command, callback function: insertdata */
#define  SETM_PANEL_RECV_OIL              5       /* control type: command, callback function: RecvOilCallBack */
#define  SETM_PANEL_CMD_READDATA          6       /* control type: command, callback function: readdata */
#define  SETM_PANEL_QUITBUTTON            7       /* control type: command, callback function: DataBaseQuitCallback */
#define  SETM_PANEL_DECORATION_2          8       /* control type: deco, callback function: (none) */
#define  SETM_PANEL_TEXTMSG_2             9       /* control type: textMsg, callback function: (none) */
#define  SETM_PANEL_TEXTMSG_3             10      /* control type: textMsg, callback function: (none) */
#define  SETM_PANEL_DECORATION_3          11      /* control type: deco, callback function: (none) */
#define  SETM_PANEL_MODEL_NUM             12      /* control type: ring, callback function: (none) */
#define  SETM_PANEL_TEXTMSG               13      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK DataBaseQuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK insertdata(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK modifydata(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK readdata(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RecvOilCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK tableCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
