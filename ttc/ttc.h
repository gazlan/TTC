/* ******************************************************************** **
** @@ Tray Type Calculator
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Dscr   :
** ******************************************************************** */

#ifndef  _TTC_H_
#define  _TTC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
   #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"      // main symbols

/* ******************************************************************** **
** @@                   internal #defines
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
** @@                   Class CTTCApp
** ******************************************************************** */

class CTTCApp : public CWinApp
{
   private:

      HANDLE      _hMutext;

   public:

      CTTCApp();

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(CTTCApp)
	public:
      virtual BOOL InitInstance();
      virtual int  ExitInstance();
	//}}AFX_VIRTUAL

      // Implementation

      //{{AFX_MSG(CTTCApp)
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
};

/* ******************************************************************** **
** @@                   Prototypes
** ******************************************************************** */

#endif 

/* ******************************************************************** **
**                End of File
** ******************************************************************** */
