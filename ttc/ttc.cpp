/* ******************************************************************** **
** @@ Tray Type Calculator
** @  Copyrt :
** @  Author :
** @  Modify : 
** @  Update :
** @  Dscr   :
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include "resource.h"

#include "..\shared\buffer_circular.h"
#include "..\shared\tray.h"

#include "calc.h"
#include "ttc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
#endif

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

CTTCApp     theApp;  // The one and only CTTCApp object

/* ******************************************************************** **
** @@ MESSAGE_MAP()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CTTCApp, CWinApp)
   //{{AFX_MSG_MAP(CTTCApp)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG
   ON_COMMAND(ID_HELP,CWinApp::OnHelp)
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CTTCApp::CTTCApp()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

CTTCApp::CTTCApp()
{
}

/* ******************************************************************** **
** @@ CTTCApp::InitInstance()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BOOL CTTCApp::InitInstance()
{
   // Only One Instance of this App is available !
   _hMutext = CreateMutex(NULL,TRUE,"TrayTypeCalculatorMutext");

   if (!_hMutext)
   {
      return FALSE;
   }

   if (GetLastError() == ERROR_ALREADY_EXISTS)
   {
      return FALSE;
   }

   #ifdef _AFXDLL
   Enable3dControls();        // Call this when using MFC in a shared DLL
   #else
   Enable3dControlsStatic();  // Call this when linking to MFC statically
   #endif

   if (::CoInitialize(NULL) != S_OK)
   {
      // OLE Error !
      return FALSE;
   }

   InitCommonControls();

   CCalc     dlg;

   dlg.DoModal();

   return FALSE;
}

/* ******************************************************************** **
** @@ CTTCApp::ExitInstance
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

int CTTCApp::ExitInstance()
{
   CloseHandle(_hMutext);
   _hMutext = INVALID_HANDLE_VALUE;

   ::CoUninitialize();

   return CWinApp::ExitInstance();
}

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
