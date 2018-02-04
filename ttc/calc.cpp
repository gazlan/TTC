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

#include "float.h"
#include "math.h"

#include "..\shared\buffer_circular.h"
#include "..\shared\hash_sha160.h"
#include "..\shared\prng_mersenne_twister.h"
#include "..\shared\hash_hsieh.h"
#include "..\shared\math_gamma.h"
#include "..\shared\tray.h"

#include "calc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#define WM_ICON_NOTIFY        (WM_APP + 10)

#define ERR_PREFIX            "**  ERR:  "

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

/* ******************************************************************** **
** @@ pVerbs
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

static CALC_OPERATOR pVerbs[]  =
{
   {'(',  "(",       1, 0  }, 
   {'+',  "+",       1, 2  }, 
   {'-',  "-",       1, 3  }, 
   {'%',  "%",       1, 4  }, 
   {'*',  "*",       1, 4  },
   {'/',  "/",       1, 5  }, 
   {'\\', "\\",      1, 5  },    // Calculates the floating-point remainder
   {'^',  "^",       1, 6  }, 
   {')',  ")",       1, 99 }, 
   {'g',  "G(",      2, 0  }, 
   {'H',  "SH(",     3, 0  }, 
   {'h',  "CH(",     3, 0  }, 
   {'L',  "LN(",     3, 0  }, 
   {'G',  "LG(",     3, 0  }, 
   {'S',  "SIN(",    4, 0  }, 
   {'C',  "COS(",    4, 0  }, 
   {'A',  "ABS(",    4, 0  }, 
   {'E',  "EXP(",    4, 0  }, 
   {'t',  "TAN(",    4, 0  }, 
   {'I',  "ASIN(",   5, 0  }, 
   {'O',  "ACOS(",   5, 0  }, 
   {'l',  "LOG2(",   5, 0  }, 
   {'s',  "SQRT(",   5, 0  }, 
   {'T',  "ATAN(",   5, 0  }, 
   { 0,   NULL,      0, 0  }
};
                  
const double Pi = 3.141592653589793238462643383279502884197;
const double e  = 2.718281828459045235360287471352662497757;

static char       op_stack [MAX_PATH];    // Operator stack
static double     arg_stack[MAX_PATH];    // Argument stack
static char       token    [MAX_PATH];    // Token buffer

static int        op_sptr  = 0;           // op_stack pointer
static int        arg_sptr = 0;           // arg_stack pointer
static int        parens   = 0;           // Nesting level
static int        state    = 0;           // 0 = Awaiting expression, 1 = Awaiting operator

/* ******************************************************************** **
** @@ CCalc::CCalc()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

CCalc::CCalc(CWnd* pParent /*=NULL*/)
:  CDialog(CCalc::IDD, pParent)
{
   //{{AFX_DATA_INIT(CCalc)
   //}}AFX_DATA_INIT

   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   _hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

   _pTray = NULL;
   _bExit = false;

   _sInput = _T("");

   _iSelected = -1;

   _iCalcError = 0;

   _dwHistoryIndex = 0;

   memset(_pCalcHistory,0,(MAX_PATH + 1) * MAX_HISTORY_DEPTH);
}

/* ******************************************************************** **
** @@ CCalc::DoDataExchange()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CCalc)
   DDX_Control(pDX, IDC_LIST, m_List);
   DDX_Control(pDX, IDC_EDT_INPUT, m_Input);
   //}}AFX_DATA_MAP
}

/* ******************************************************************** **
** @@ CCalc::MESSAGE_MAP()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CCalc, CDialog)
   //{{AFX_MSG_MAP(CCalc)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
   ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
   ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, OnGetdispinfoList)
   ON_BN_CLICKED(IDC_BTN_PERCENT, OnBtnPercent)
   ON_BN_CLICKED(IDC_BTN_DIVIDE, OnBtnDivide)
   ON_BN_CLICKED(IDC_BTN_INV_DIV, OnBtnInvDiv)
   ON_BN_CLICKED(IDC_BTN_MULTIPLATE, OnBtnMultiplate)
   ON_BN_CLICKED(IDC_BTN_MINUS, OnBtnMinus)
   ON_BN_CLICKED(IDC_BTN_7, OnBtn7)
   ON_BN_CLICKED(IDC_BTN_8, OnBtn8)
   ON_BN_CLICKED(IDC_BTN_9, OnBtn9)
   ON_BN_CLICKED(IDC_BTN_PLUS, OnBtnPlus)
   ON_BN_CLICKED(IDC_BTN_4, OnBtn4)
   ON_BN_CLICKED(IDC_BTN_5, OnBtn5)
   ON_BN_CLICKED(IDC_BTN_6, OnBtn6)
   ON_BN_CLICKED(IDC_BTN_1, OnBtn1)
   ON_BN_CLICKED(IDC_BTN_2, OnBtn2)
   ON_BN_CLICKED(IDC_BTN_3, OnBtn3)
   ON_BN_CLICKED(IDC_BTN_0, OnBtn0)
   ON_BN_CLICKED(IDC_BTN_DOT, OnBtnDot)
   ON_BN_CLICKED(IDC_BTN_LEFT, OnBtnLeft)
   ON_BN_CLICKED(IDC_BTN_RIGHT, OnBtnRight)
   ON_BN_CLICKED(IDC_BTN_RESET, OnBtnReset)
   ON_BN_CLICKED(IDC_BTN_POWER, OnBtnPower)
   ON_BN_CLICKED(IDC_BTN_SQRT, OnBtnSqrt)
   ON_BN_CLICKED(IDC_BTN_SIN, OnBtnSin)
   ON_BN_CLICKED(IDC_BTN_COS, OnBtnCos)
   ON_BN_CLICKED(IDC_BTN_SH, OnBtnSh)
   ON_BN_CLICKED(IDC_BTN_CH, OnBtnCh)
   ON_BN_CLICKED(IDC_BTN_LN, OnBtnLn)
   ON_BN_CLICKED(IDC_BTN_LOG10, OnBtnLog10)
   ON_BN_CLICKED(IDC_BTN_LOG2, OnBtnLog2)
   ON_BN_CLICKED(IDC_BTN_EXP, OnBtnExp)
   ON_BN_CLICKED(IDC_BTN_ABS, OnBtnAbs)
   ON_BN_CLICKED(IDC_BTN_ATAN, OnBtnAtan)
   ON_BN_CLICKED(IDC_BTN_PI, OnBtnPi)
   ON_BN_CLICKED(IDC_BTN_E, OnBtnE)
   ON_BN_CLICKED(IDC_BTN_GAMMA, OnBtnGamma)
   ON_COMMAND(IDM_COPY, OnCopySingle)
   ON_COMMAND(IDM_CLEAR, OnClear)
   ON_COMMAND(IDM_COPY_ALL, OnCopyAll)
   ON_COMMAND(IDM_COPY_SELECTED, OnCopySelected)
   ON_COMMAND(IDM_REPEAT, OnRepeat)
   ON_BN_CLICKED(IDC_BTN_ARCSIN, OnBtnArcsin)
   ON_BN_CLICKED(IDC_BTN_ARCCOS, OnBtnArccos)
   ON_BN_CLICKED(IDC_BTN_TAN, OnBtnTan)
   ON_BN_CLICKED(IDC_BTN_RAND, OnBtnRand)
   ON_WM_CLOSE()
   ON_WM_DESTROY()
   ON_MESSAGE(WM_ICON_NOTIFY,OnTrayNotification)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CCalc::OnInitDialog()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BOOL CCalc::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   CenterWindow();

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(_hIcon,TRUE);   // Set big icon
   SetIcon(_hIcon,FALSE);  // Set small icon

   _pTray = new CSystemTray;

   if (_pTray)
   {
      _pTray->Create(this,WM_ICON_NOTIFY,"Tray Tape Calculator",_hIcon,IDR_POPUP_MENU,FALSE);
      _pTray->MaximiseFromTray(this);
   }

   CMenu*   pSysMenu = GetSystemMenu(FALSE);

   if (pSysMenu)
   {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING,IDM_EXIT,"E&xit\tX");
   }

   _iCalcError = 0;  // Reset

   // Reset Again !
   _History.Resize(MAX_HISTORY_DEPTH);

   _dwHistoryIndex = 0;

   memset(_pCalcHistory,0,(MAX_PATH + 1) * MAX_HISTORY_DEPTH);

   // Virtual List !
   m_List.InsertColumn(0,"Input",LVCFMT_LEFT,400,0);
   
   DWORD    dwStyle = m_List.GetExtendedStyle();
   
   dwStyle |= LVS_OWNERDATA | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_FLATSB | LVS_EX_GRIDLINES;
   
   m_List.SetExtendedStyle(dwStyle);

   Update();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/* ******************************************************************** **
** @@ CCalc::OnPaint()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CCalc::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);

      CRect rect;
      GetClientRect(&rect);
      
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x,y,_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

/* ******************************************************************** **
** @@ CCalc::OnQueryDragIcon()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCalc::OnQueryDragIcon()
{
   return (HCURSOR)_hIcon;
}

/* ******************************************************************** **
** @@ CCalc::OnSysCommand()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CCalc::OnSysCommand(UINT nID,LPARAM lParam)
{
   switch (nID & 0xFFF0)
   {  
      case SC_CLOSE:
      {
         if (_pTray)
         {
            _pTray->MinimiseToTray(this);
         }

         break;
      }
      case IDM_EXIT:
      {
         _bExit = true;
         PostMessage(WM_CLOSE);
         break;
      }
      default:
      {
         CDialog::OnSysCommand(nID,lParam);
      }
   }
}

/* ******************************************************************** **
** @@ CCalc::PreTranslateMessage()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

BOOL CCalc::PreTranslateMessage(MSG* pMsg) 
{  
   if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
   {  
      // Prevent <ESC> exit !
      return TRUE;
   }

   return CDialog::PreTranslateMessage(pMsg);
}

/* ******************************************************************** **
** @@ CCalc::OnDestroy()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CCalc::OnDestroy() 
{
   CDialog::OnDestroy();

   if (_pTray)
   {
      _pTray->RemoveIcon();  

      delete _pTray;
      _pTray = NULL;
   }
}

/* ******************************************************************** **
** @@ CCalc::OnTrayNotification()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

LRESULT CCalc::OnTrayNotification(WPARAM wParam,LPARAM lParam)
{
   //Return quickly if its not for this tray icon
   if (wParam != IDR_POPUP_MENU)
   {
      return 0;
   }

   if (IsWindowVisible())
   {
      return 0;
   }
   
   // Clicking with right button brings up a context menu
   if (LOWORD(lParam) == WM_LBUTTONUP)
   {
      if (_pTray)
      {
         _pTray->MaximiseFromTray(this);
      }

      return 0;
   }
   
   return 0;
}

/* ******************************************************************** **
** @@ CCalc::OnClose()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CCalc::OnClose() 
{
   if (!IsWindowVisible())
   {
      return;
   }

   if (!_bExit)
   {
      if (_pTray)
      {
         _pTray->MinimiseToTray(this);
      }
   }
   else
   {
      CDialog::OnClose();   
   }
}

/* ******************************************************************** **
** @@ CCalc::OnBtn0()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtn0() 
{
   AppendInput("0");
}

/* ******************************************************************** **
** @@ CCalc::OnBtn1()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtn1() 
{
   AppendInput("1");
}

/* ******************************************************************** **
** @@ CCalc::OnBtn2()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtn2() 
{
   AppendInput("2");
}

/* ******************************************************************** **
** @@ CCalc::OnBtn3()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtn3() 
{
   AppendInput("3");
}

/* ******************************************************************** **
** @@ CCalc::OnBtn4()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtn4() 
{
   AppendInput("4");
}

/* ******************************************************************** **
** @@ CCalc::OnBtn5()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtn5() 
{
   AppendInput("5");
}

/* ******************************************************************** **
** @@ CCalc::OnBtn6()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtn6() 
{
   AppendInput("6");
}

/* ******************************************************************** **
** @@ CCalc::OnBtn7()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtn7() 
{
   AppendInput("7");
}

/* ******************************************************************** **
** @@ CCalc::OnBtn8()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtn8() 
{
   AppendInput("8");
}

/* ******************************************************************** **
** @@ CCalc::OnBtn9()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtn9() 
{
   AppendInput("9");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnPercent()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnPercent() 
{
   AppendInput("%");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnDivide()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnDivide() 
{
   AppendInput("/");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnInvDiv()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnInvDiv() 
{
   AppendInput("\\");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnMultiplate()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnMultiplate() 
{
   AppendInput("*");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnMinus()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnMinus() 
{
   AppendInput("-");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnPlus()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnPlus() 
{
   AppendInput("+");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnDot()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnDot() 
{
   AppendInput(".");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnLeft()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnLeft() 
{
   AppendInput("(");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnRight()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnRight() 
{
   AppendInput(")");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnPower()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnPower() 
{
   AppendInput("^");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnSqrt()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnSqrt() 
{
   AppendInput("sqrt");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnSin()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnSin() 
{
   AppendInput("sin");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnCos()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnCos() 
{
   AppendInput("cos");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnArcsin()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnArcsin() 
{
   AppendInput("asin");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnArccos()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnArccos() 
{
   AppendInput("acos");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnTan()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnTan() 
{
   AppendInput("tan");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnAtan()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnAtan() 
{
   AppendInput("atan");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnSh()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnSh() 
{
   AppendInput("sh");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnCh()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnCh() 
{
   AppendInput("ch");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnLn()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnLn() 
{
   AppendInput("ln");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnLog10()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnLog10() 
{
   AppendInput("lg");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnLog2()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnLog2() 
{
   AppendInput("log2");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnExp()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnExp() 
{
   AppendInput("exp");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnAbs()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnAbs() 
{
   AppendInput("abs");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnPi()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnPi() 
{
   AppendInput("Pi");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnE()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnE() 
{
   AppendInput("e");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnRand()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnRand() 
{
   AppendInput("rand");
}

/* ******************************************************************** **
** @@ CCalc::OnBtnGamma()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnGamma() 
{
   AppendInput("G");
}

/* ******************************************************************** **
** @@ CCalc::PushLine()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::PushLine(const char* const pszLine)
{
   strncpy(_pCalcHistory[_dwHistoryIndex],pszLine,MAX_PATH);
   _pCalcHistory[_dwHistoryIndex][MAX_PATH] = 0; // ASCIIZ

   _History.Push(_dwHistoryIndex);

   ++_dwHistoryIndex;
   _dwHistoryIndex %= MAX_HISTORY_DEPTH;
}

/* ******************************************************************** **
** @@ CCalc::AppendInput()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::AppendInput(const char *const pszText)
{
   // If nStartChar is –1, any current selection is removed.
   m_Input.SetSel(-1,0);

   // If there is no current selection, the replacement text is inserted at the current cursor location.
   m_Input.ReplaceSel(pszText);
}

/* ******************************************************************** **
** @@ CCalc::OnBtnReset()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnBtnReset() 
{
   _sInput = _T("");

   m_Input.SetWindowText((LPCTSTR)_sInput);
   
   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CCalc::OnOK()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnOK() 
{
   _iCalcError = 0;  // Reset

   m_Input.GetWindowText(_sInput);

   _sInput.Replace(',','.');

   if (_sInput.Find(ERR_PREFIX) == -1)
   {
      PushLine((LPCTSTR)_sInput);
   }

   _sInput.Format("%.15f",Evaluate(_sInput));
                       
   if (_iCalcError < 0)
   {
      _sInput = ERR_PREFIX;

      switch (_iCalcError)
      {
         case -1:
         {
            _sInput += "Invalid float literal.";
            break;
         }
         case -2:
         {
            _sInput += "Invalid operator or function.";
            break;
         }
         case -3:
         {
            _sInput += "Invalid parenthesis expression.";
            break;
         }
         case -4:
         {
            _sInput += "Invalid operation.";
            break;
         }
         case -5:
         {
            _sInput += "Operation stack error.";
            break;
         }
         case -6:
         {
            _sInput += "Invalid function argument.";
            break;
         }
         case -7:
         {
            _sInput += "Invalid arithmetical expression.";
            break;
         }
      }
   }

   m_Input.SetWindowText((LPCTSTR)_sInput);

   Update();

   // Don't close !  
   // CDialog::OnOK();
}

/* ******************************************************************** **
** @@ CCalc::Update()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void CCalc::Update()
{
   DWORD    dwCnt = _History.Count();

   m_List.SetItemCount(dwCnt);
   m_List.RedrawItems(0,(int)dwCnt - 1);

   m_List.EnsureVisible((int)dwCnt - 1,FALSE);
}

/* ******************************************************************** **
** @@ CCalc::OnClickList()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   *pResult = 0;

   NM_LISTVIEW*      pNMListView = (NM_LISTVIEW*)pNMHDR;
      
   _iSelected = pNMListView->iItem;
}

/* ******************************************************************** **
** @@ CCalc::OnDblclkList()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   *pResult = 0;

   NM_LISTVIEW*      pNMListView = (NM_LISTVIEW*)pNMHDR;
      
   _iSelected = pNMListView->iItem;

   DWORD    dwIndex = _History.Get(_iSelected);

   _sInput = _pCalcHistory[dwIndex];

   m_Input.SetWindowText((LPCTSTR)_sInput);
}

/* ******************************************************************** **
** @@ CCalc::OnRclickList()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

bool CCalc::IsAllSelected()
{
   return _History.Count() == m_List.GetSelectedCount();
}

/* ******************************************************************** **
** @@ CCalc::OnRclickList()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   *pResult = 0;

   NM_LISTVIEW*      pNMListView = (NM_LISTVIEW*)pNMHDR;
      
   _iSelected = pNMListView->iItem;

   if (!_History.Count())
   {
      return;
   }

   CMenu    Menu;
   
   VERIFY(Menu.LoadMenu(IDR_CALC));
   
   CMenu*   pPopup = Menu.GetSubMenu(0);
   
   ASSERT(pPopup != NULL);

   ASSERT(pPopup != NULL);

   DWORD    dwSelCnt = m_List.GetSelectedCount();

   pPopup->EnableMenuItem(IDM_COPY_ALL,MF_BYCOMMAND | _History.Count()  ?  MF_ENABLED  :  MF_GRAYED);
   
   switch (dwSelCnt)
   {
      case 0:
      {
         pPopup->EnableMenuItem(IDM_REPEAT,       MF_BYCOMMAND | MF_GRAYED);
         pPopup->EnableMenuItem(IDM_COPY,         MF_BYCOMMAND | MF_GRAYED);
         pPopup->EnableMenuItem(IDM_COPY_SELECTED,MF_BYCOMMAND | MF_GRAYED);

         pPopup->EnableMenuItem(IDM_CLEAR,        MF_BYCOMMAND | MF_ENABLED);
         break;
      }
      case 1:
      {
         pPopup->EnableMenuItem(IDM_REPEAT,       MF_BYCOMMAND | MF_ENABLED);
         pPopup->EnableMenuItem(IDM_COPY,         MF_BYCOMMAND | MF_ENABLED);
         pPopup->EnableMenuItem(IDM_CLEAR,        MF_BYCOMMAND | MF_ENABLED);

         pPopup->EnableMenuItem(IDM_COPY_SELECTED,MF_BYCOMMAND | MF_GRAYED);
         break;
      }
      default:
      {

         pPopup->EnableMenuItem(IDM_REPEAT,       MF_BYCOMMAND | MF_GRAYED);
         pPopup->EnableMenuItem(IDM_COPY,         MF_BYCOMMAND | MF_GRAYED);

         pPopup->EnableMenuItem(IDM_COPY_SELECTED,MF_BYCOMMAND | IsAllSelected()  ?  MF_GRAYED  :  MF_ENABLED);

         pPopup->EnableMenuItem(IDM_CLEAR,        MF_BYCOMMAND | MF_ENABLED);
         break;
      }
   }

   CPoint Point;
  
   if (GetCursorPos(&Point))
   {
      pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,Point.x,Point.y,this);
   }
}

/* ******************************************************************** **
** @@ CCalc::OnRepeat()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnRepeat() 
{
   if (_iSelected < 0)
   {
      return;
   }
   
   DWORD    dwIndex = _History.Get(_iSelected);

   _sInput = _pCalcHistory[dwIndex];

   m_Input.SetWindowText((LPCTSTR)_sInput);
}

/* ******************************************************************** **
** @@ CCalc::OnCopySingle()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnCopySingle() 
{
   if (_iSelected < 0)
   {
      MessageBox("No entry selected!","Nothing to do...",MB_OK | MB_ICONSTOP);
      return;         
   }

   DWORD    dwIndex = _History.Get(_iSelected);

   const char*    pStr = _pCalcHistory[dwIndex];

      if (pStr && *pStr)
      {
         CopyData((BYTE*)pStr,strlen(pStr) + 1);   // ASCIIZ
      }
}

/* ******************************************************************** **
** @@ CCalc::OnCopySelected()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnCopySelected() 
{
   DWORD    dwCnt = m_List.GetSelectedCount();

   if (!dwCnt)
   {
      MessageBox("No entry selected!","Nothing to do...",MB_OK | MB_ICONSTOP);
      return;         
   }

   POSITION    Pos = m_List.GetFirstSelectedItemPosition();

   if (!Pos)
   {
      MessageBox("Can't get first selected entry!","Nothing to do...",MB_OK | MB_ICONSTOP);
      return;         
   }
   
   CMemFile    MemFile;

   for (DWORD ii = 0; ii < dwCnt; ++ii)
   {
      int      iIndex = m_List.GetNextSelectedItem(Pos);

      const char*    pStr = _pCalcHistory[iIndex];

      if (pStr && *pStr)
      {
         MemFile.Write(pStr,strlen(pStr));
         MemFile.Write("\r\n",2);
      }
   }

   BYTE     byZero = 0;
   
   MemFile.Write(&byZero,sizeof(BYTE));   // ASCIIZ

   DWORD    dwSize = MemFile.GetLength();

   BYTE*    pBuf = MemFile.Detach();

   CopyData(pBuf,dwSize);

   free(pBuf);
   pBuf = NULL;

   MemFile.Close();
}

/* ******************************************************************** **
** @@ CCalc::OnCopyAll()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnCopyAll() 
{
   DWORD    dwCnt = _History.Count();

   if (!dwCnt)
   {
      return;
   }

   CMemFile    MemFile;

   for (DWORD ii = 0; ii < dwCnt; ++ii)
   {
      const char*    pStr = _pCalcHistory[ii];

      if (pStr && *pStr)
      {
         MemFile.Write(pStr,strlen(pStr));
         MemFile.Write("\r\n",2);
      }
   }

   BYTE     byZero = 0;
   
   MemFile.Write(&byZero,sizeof(BYTE));   // ASCIIZ

   DWORD    dwSize = MemFile.GetLength();

   BYTE*    pBuf = MemFile.Detach();

   CopyData(pBuf,dwSize);

   free(pBuf);
   pBuf = NULL;

   MemFile.Close();
}

/* ******************************************************************** **
** @@ CCalc::CopyData()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

bool CCalc::CopyData(const BYTE* const pBuf,DWORD dwSize)
{
   if (!OpenClipboard())
   { 
      return FALSE; 
   }

   EmptyClipboard(); 

   // Allocate a global memory object for the text. 
   HGLOBAL     hMem = GlobalAlloc(GMEM_MOVEABLE,dwSize); 

   if (!hMem) 
   { 
      CloseClipboard(); 
      return false; 
   } 

   // Lock the handle and copy the text to the buffer. 
   LPVOID      pMem = GlobalLock(hMem); 

   memcpy(pMem,pBuf,dwSize); 
   
   GlobalUnlock(hMem); 

   // Place the handle on the clipboard. 
   SetClipboardData(CF_TEXT,hMem); 

   CloseClipboard(); 
   GlobalFree(hMem);

   return true;
}

/* ******************************************************************** **
** @@ CCalc::OnClear()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnClear() 
{
   _History.Reset();
   _History.Resize(MAX_HISTORY_DEPTH);

   Update();
}

/* ******************************************************************** **
** @@ CCalc::OnGetdispinfoList()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CCalc::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   *pResult = 0;

   NMLVDISPINFO*     pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

   LV_ITEM*    pItem = &(pDispInfo)->item;

   int      iIndex = pItem->iItem;

   if (pItem->mask & LVIF_TEXT)
   {
      DWORD    dwIndex = _History.Get(iIndex);

      if (pItem && !pItem->iSubItem)
      {
         strcpy(pItem->pszText,_pCalcHistory[dwIndex]);
      }
   }
}

/* ******************************************************************** **
** @@ CCalc::PRNG()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

double CCalc::PRNG()
{
   // Generate PRNG Seed
   GUID     _GUID = GUID_NULL;

   ::CoCreateGuid(&_GUID);

   MTRand      _PRNG;

   // Compress to applicable size & Initialize
   DWORD    dwSeed = SuperFastHash((BYTE*)&_GUID,sizeof(GUID));    
   
   _PRNG.seed(dwSeed);

   // Generate double random in (0,1)
   return _PRNG.randDblExc(); // 
}

/* ******************************************************************** **
** @@ CCalc::Gamma()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

double CCalc::Gamma(double fArg)
{
   if (fabs(fArg) > 170.0)
   {
      MessageBox("Argument out of range [0..170].\n\nCalculation aborted!","Euler Gamma function",MB_OK | MB_ICONSTOP);
      return 0.0;
   }

   return ::Gamma(fArg);
}

/* ******************************************************************** **
** @@ CCalc::Log2()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

double CCalc::Log2(double fArg)
{
   return log(fArg) / log(2.0);
}

/* ******************************************************************** **
** @@ CCalc::DoOperation()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Evaluate stacked arguments and operands
** ******************************************************************** */

int CCalc::DoOperation()
{
   double   arg1 = 0.0;
   double   arg2 = 0.0;

   int      op = 0;

   if (PopOperation(&op) == -1)
   {
      return -1;
   }

   PopArgument(&arg1);
   PopArgument(&arg2);
   
   switch (op)
   {  
      case '+':
      {
         PushArgument(arg2 + arg1);
         break;
      }
      case '-':
      {
         PushArgument(arg2 - arg1);
         break;
      }
      case '%':
      {
         PushArgument(arg2 * arg1 / 100.0);
         break;
      }
      case '*':
      {
         PushArgument(arg2 * arg1);
         break;
      }
      case '/':
      {
         if (fabs(arg1) < FLT_EPSILON)
         {
            return -1;
         }

         PushArgument(arg2 / arg1);
         break;
      }
      case '\\':  // Calculates the floating-point remainder
      {
         if (fabs(arg1) < FLT_EPSILON)
         {
            return -1;
         }

         PushArgument(fmod(arg2,arg1));
         break;
      }
      case '^':
      {
         PushArgument(pow(arg2,arg1));
         break;
      }
      case 't':   // TAN
      {
         ++arg_sptr; // Function()
         PushArgument(tan(arg1));
         break;
      }
      case 'S':   // SIN
      {
         ++arg_sptr; // Function()
         PushArgument(sin(arg1));
         break;
      }
      case 's':   // SQRT
      {
         ++arg_sptr; // Function()
         PushArgument(sqrt(arg1));
         break;
      }
      case 'C':   // COS
      {
         ++arg_sptr; // Function()
         PushArgument(cos(arg1));
         break;
      }
      case 'A':   // ABS
      {
         ++arg_sptr; // Function()
         PushArgument(fabs(arg1));
         break;
      }
      case 'L':   // LN
      {
         if (fabs(arg1) > FLT_EPSILON)
         {
            ++arg_sptr; // Function()
            PushArgument(log(arg1));
            break;
         }
         else
         {
            return -1;
         }
      }
      case 'E':   // EXP
      {
         ++arg_sptr; // Function()
         PushArgument(exp(arg1));
         break;
      }
      case 'H':   // SH
      {
         ++arg_sptr; // Function()
         PushArgument(sinh(arg1));
         break;
      }
      case 'h':   // CH
      {
         ++arg_sptr; // Function()
         PushArgument(cosh(arg1));
         break;
      }
      case 'g':   // Gamma
      {
         ++arg_sptr; // Function()
         PushArgument(Gamma(arg1));
         break;
      }
      case 'G':   // LG
      {
         ++arg_sptr; // Function()
         PushArgument(log10(arg1));
         break;
      }
      case 'I':   // ARCSIN
      {
         ++arg_sptr; // Function()
         PushArgument(asin(arg1));
         break;
      }
      case 'O':   // ARCCOS
      {
         ++arg_sptr; // Function()
         PushArgument(acos(arg1));
         break;
      }
      case 'l':   // LOG2
      {
         ++arg_sptr; // Function()
         PushArgument(Log2(arg1));
         break;
      }
      case 'T':   // ATAN
      {
         ++arg_sptr; // Function()
         PushArgument(atan(arg1));
         break;
      }
      case '(':
      {
         arg_sptr += 2;
         break;
      }
      default:
      {
         return -1;
      }
   }

   if (arg_sptr < 1)
   {
      return -1;
   }

   return op;
}

/* ******************************************************************** **
** @@ CCalc::DoParenthesis()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Evaluate one level
** ******************************************************************** */

int CCalc::DoParenthesis()
{
   int   op = 0;

   if (parens-- < 1)
   {
      return -1;
   }

   do
   {
      op = DoOperation();

      if (op < 0)
      {
         break;
      }
   }
   while (GetPrecedence((char)op));

   return op;
}

/* ******************************************************************** **
** @@ CCalc::PushOperation()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Stack operations
** ******************************************************************** */

void CCalc::PushOperation(char op)
{
   if (!GetPrecedence(op))
   {
      ++parens;
   }
  
   op_stack[op_sptr++] = op;
}

/* ******************************************************************** **
** @@ CCalc::PushArgument()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CCalc::PushArgument(double arg)
{
   arg_stack[arg_sptr++] = arg;
}

/* ******************************************************************** **
** @@ CCalc::PopArgument()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

int CCalc::PopArgument(double* arg)
{  
   *arg = arg_stack[--arg_sptr];
   
   return (arg_sptr < 0)  ?  -1  :  0;
}

/* ******************************************************************** **
** @@ CCalc::PopOperation()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

int CCalc::PopOperation(int* op)
{
   if (!op_sptr)
   {
      return -1;
   }

   *op = op_stack[--op_sptr];

   return 0;
}

/* ******************************************************************** **
** @@ CCalc::GetExpression()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Get an expression
** ******************************************************************** */

char* CCalc::GetExpression(char* str)
{
   char*    ptr  = str; 
   char*    tptr = token;
  
   CALC_OPERATOR*    op = NULL;

   if (!strncmp(str,"PI",2))
   {
      return strcpy(token,"PI");
   }

   while (*ptr)
   {
      op = GetOperator(ptr);

      if (op)
      {
         if ('-' == *ptr)
         {
            if (str != ptr && 'E' != ptr[-1])
            {
               break;
            }

            if (str == ptr && !isdigit(ptr[1]) && '.' != ptr[1])
            {
               PushArgument(0.0);
               strcpy(token,op->tag);
               return token;
            }
         }
         else if (str == ptr)
         {
            strcpy(token,op->tag);
            return token;
         }
         else
         {
            break;
         }
      }

      *tptr++ = *ptr++;
   }

   *tptr = NULL;

   return token;
}

/* ******************************************************************** **
** @@ CCalc::GetOperator()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Get an operator
** ******************************************************************** */

CALC_OPERATOR* CCalc::GetOperator(char* str)
{
   for (CALC_OPERATOR* pCO = pVerbs; pCO->token; ++pCO)
   {
      if (!strncmp(str,pCO->tag,pCO->taglen))
      {
         return pCO;
      }
   }

   return NULL;
}

/* ******************************************************************** **
** @@ CCalc::GetPrecedence()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Get precedence of a token
** ******************************************************************** */

int CCalc::GetPrecedence(char token)
{
   for (CALC_OPERATOR* pCO = pVerbs; pCO->token; ++pCO)
   {
      if (pCO->token == token)
      {
         return pCO->precedence;
      }
   }

   return 0;
}

/* ******************************************************************** **
** @@ CCalc::GetStackTopPrecedence()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Get precedence of TOS token
** ******************************************************************** */

int CCalc::GetStackTopPrecedence()
{
   if (!op_sptr)
   {
      return 0;
   }

   return GetPrecedence(op_stack[op_sptr - 1]);
}

/* ******************************************************************** **
** @@ CCalc::Evaluate()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

// Basically, EVAL.C converts infix notation to postfix notation. If you're
// not familiar with these terms, infix notation is standard human-readable
// equations such as you write in a C program. Postfix notation is most familiar
// as the "reverse Polish" notation used in Hewlett Packard calculators and in
// the Forth language. Internally, all languages work by converting to postfix
// evaluation. Only Forth makes it obvious to the programmer.
// 
// EVAL.C performs this conversion by maintaining 2 stacks, an operand stack
// and an operator stack. As it scans the input, each time it comes across a
// numerical value, it pushes it onto the operand stack. Whenever it comes
// across an operator, it pushes it onto the operator stack. Once the input
// expression has been scanned, it evaluates it by popping operands off the
// operand stack and applying the operators to them.
// 
// For example the simple expression "2+3-7" would push the values 2, 3, and 7
// onto the operand stack and the operators "+" and "-" onto the operator stack.
// When evaluating the expression, it would first pop 3 and 7 off the operand
// stack and then pop the "-" operator off the operator stack and apply it. This
// would leave a value of -4 on the stack. Next the value of 2 would be popped
// from the operand stack and the remaining operator off of the operator stack.
// Applying the "+" operator to the values 2 and -4 would leave the result of
// the evaluation, -2, on the stack to be returned.
// 
// The only complication of this in EVAL.C is that instead of raw operators
// (which would all have to be 1 character long), I use operator tokens which
// allow multicharacter operators and precedence specification. What I push on
// the operator stack is still a single character token, but its the operator
// token which is defined in the 'verbs' array of valid tokens. Multicharacter
// tokens are always assumed to include any leading parentheses. For example, in
// the expression "SQRT(37)", the token is "SQRT(".
// 
// Using parentheses forces evaluation to be performed out of the normal
// sequence. I use the same sort of mechanism to implement precedence rules.
// Unary negation is yet another feature which takes some explicit exception
// processing to process. Other than these exceptions, it's pretty
// straightforward stuff.
//                                                                       
//   EVAL.C - A simple mathematical expression evaluator in C            
//                                                                       
//   operators supported: Operator               Precedence              
//                                                                       
//                          (                     Lowest                 
//                          )                     Highest                
//                          +   (addition)        Low                    
//                          -   (subtraction)     Low                    
//                          *   (multiplication)  Medium                 
//                          /   (division)        Medium                 
//                          \   (modulus)         High                   
//                          ^   (exponentiation)  High                   
//                          sin(                  Lowest                 
//                          cos(                  Lowest                 
//                          atan(                 Lowest                 
//                          abs(                  Lowest                 
//                          sqrt(                 Lowest                 
//                          ln(                   Lowest                 
//                          exp(                  Lowest                 
//                                                                       
//   constants supported: pi                                             
//                                                                       
//   Original Copyright 1991-93 by Robert B. Stout as part of            
//   the MicroFirm Function Library (MFL)                                
//                                                                       
//   The user is granted a free limited license to use this source file  
//   to create royalty-free programs, subject to the terms of the        
//   license restrictions specified in the LICENSE.MFL file.             

double CCalc::Evaluate(const CString& rExpr)
{
   char     pszExpr[MAX_PATH + 1];

   double      arg     = 0.0;
   double      fResult = 0.0;

   int         ercode  = 0;

   char*             str    = NULL; 
   char*             endptr = NULL;
   CALC_OPERATOR*    op     = NULL;

   CString     sExpr = rExpr;

   sExpr.MakeUpper();

   sExpr.Remove(0x08);
   sExpr.Remove(0x09);
   sExpr.Remove(0x0A);
   sExpr.Remove(0x0B);
   sExpr.Remove(0x0C);
   sExpr.Remove(0x0D);
   sExpr.Remove(0x20);

   memset(pszExpr,0,sizeof(pszExpr));

   strncpy(pszExpr,(LPCTSTR)sExpr,MAX_PATH);

   char*    ptr = pszExpr; 

   // Reset
   state    = 0;
   op_sptr  = 0;
   arg_sptr = 0; 
   parens   = 0;

   while (*ptr)
   {
      switch (state)
      {
         case 0:
         {
            str = GetExpression(ptr);

            if (str)
            {
               op = GetOperator(str);

               if (op && (strlen(str) == op->taglen))
               {
                  PushOperation(op->token);
                  ptr += op->taglen;
                  break;
               }

               if (!strcmp(str,"-"))
               {
                  PushOperation(*str);
                  ++ptr;
                  break;
               }

               if (!strcmp(str,"PI"))
               {
                  PushArgument(Pi);
               }
               else if (!strcmp(str,"E"))
               {
                  PushArgument(e);
               }
               else if (!strcmp(str,"RAND"))
               {
                  PushArgument(PRNG());
               }
               else
               {
                  arg = strtod(str,&endptr);

                  if ((fabs(arg) < FLT_EPSILON) && !strchr(str,'0'))
                  {
                     // Error !
                     _iCalcError = -1;
                     return 0.0;
                  }

                  PushArgument(arg);
               }

               ptr += strlen(str);
            }
            else
            {
               // Error !
               _iCalcError = -2;
               return 0.0;
            }

            state = 1;
            break;
         }
         case 1:
         {
            op = GetOperator(ptr);

            if (op)
            {
               if (*ptr == ')')
               {
                  ercode = DoParenthesis();

                  if (ercode < 0)
                  {
                     // Error !
                     _iCalcError = -3;
                     return 0.0;
                  }
               }
               else
               {
                  while (op_sptr && op->precedence <= GetStackTopPrecedence())
                  {
                     DoOperation();
                  }

                  PushOperation(op->token);
                  state = 0;
               }

               ptr += op->taglen;
            }
            else
            {
               // Error !
               _iCalcError = -4;
               return 0.0;
            }

            break;
         }
      }
   }

   while (arg_sptr > 1)
   {
      ercode = DoOperation();

      if (ercode < 0)
      {
         // Error !
         _iCalcError = -5;
         return 0.0;
      }
   }

   if (!op_sptr)
   {
      if (PopArgument(&fResult) < 0)
      {
         // Error !
         _iCalcError = -6;
         return 0.0;
      }

      return fResult;
   }

   // Error !
   _iCalcError = -7;
   return 0.0;
}

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
