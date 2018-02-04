#if !defined(AFX_CALC_H__CAF953B3_08A7_4F88_9A39_450DBAA0CC03__INCLUDED_)
#define AFX_CALC_H__CAF953B3_08A7_4F88_9A39_450DBAA0CC03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_HISTORY_DEPTH                    (128)

/* ******************************************************************** **
** @@ struct CALC_OPERATOR
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

struct CALC_OPERATOR
{
   char              token;
   char*             tag;
   size_t            taglen;
   int               precedence;
};

/* ******************************************************************** **
** @@ class CCalc : public CDialog
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

class CCalc : public CDialog
{
   private:

      CSystemTray*               _pTray;
      bool                       _bExit;
      CircularBuffer             _History;
      int                        _iSelected;
      CString                    _sInput;
      char                       _pCalcHistory[MAX_HISTORY_DEPTH][MAX_PATH + 1];
      DWORD                      _dwHistoryIndex;
      int                        _iCalcError;
      HICON                      _hIcon;

   // Construction
   public:

      CCalc(CWnd* pParent = NULL);   // standard constructor

   // Dialog Data
      //{{AFX_DATA(CCalc)
      enum { IDD = IDD_CALC };
      CListCtrl   m_List;
      CEdit m_Input;
   //}}AFX_DATA


   // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(CCalc)
   public:

      virtual BOOL PreTranslateMessage(MSG* pMsg);

   protected:

      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   // Implementation
   protected:

      // Generated message map functions
      //{{AFX_MSG(CCalc)
      virtual BOOL OnInitDialog();
      afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
      afx_msg void OnPaint();
      afx_msg HCURSOR OnQueryDragIcon();
      afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
      afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
      afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
      afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
      afx_msg void OnBtnPercent();
      afx_msg void OnBtnDivide();
      afx_msg void OnBtnInvDiv();
      afx_msg void OnBtnMultiplate();
      afx_msg void OnBtnMinus();
      afx_msg void OnBtn7();
      afx_msg void OnBtn8();
      afx_msg void OnBtn9();
      afx_msg void OnBtnPlus();
      afx_msg void OnBtn4();
      afx_msg void OnBtn5();
      afx_msg void OnBtn6();
      afx_msg void OnBtn1();
      afx_msg void OnBtn2();
      afx_msg void OnBtn3();
      afx_msg void OnBtn0();
      afx_msg void OnBtnDot();
      virtual void OnOK();
      afx_msg void OnBtnLeft();
      afx_msg void OnBtnRight();
      afx_msg void OnBtnReset();
      afx_msg void OnBtnPower();
      afx_msg void OnBtnSqrt();
      afx_msg void OnBtnSin();
      afx_msg void OnBtnCos();
      afx_msg void OnBtnArcsin();
      afx_msg void OnBtnArccos();
      afx_msg void OnBtnTan();
      afx_msg void OnBtnAtan();
      afx_msg void OnBtnSh();
      afx_msg void OnBtnCh();
      afx_msg void OnBtnLn();
      afx_msg void OnBtnLog10();
      afx_msg void OnBtnLog2();
      afx_msg void OnBtnExp();
      afx_msg void OnBtnAbs();
      afx_msg void OnBtnPi();
      afx_msg void OnBtnE();
      afx_msg void OnBtnRand();
      afx_msg void OnBtnGamma();
      afx_msg void OnCopySingle();
      afx_msg void OnCopySelected();
      afx_msg void OnCopyAll();
      afx_msg void OnClear();
      afx_msg void OnRepeat();
      afx_msg void OnDestroy();
      afx_msg void OnClose();
      afx_msg LRESULT OnTrayNotification(WPARAM wParam,LPARAM lParam);
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

   private:

      BOOL     SetTrayIcon(HWND hwnd,UINT uID,HICON hicon,LPSTR lpszTip);
      BOOL     ResetTrayIcon(HWND hwnd, UINT uID);
      void     TrayEvent(WPARAM wParam,LPARAM lParam);
      void     Restore();
      bool     IsAllSelected();
      void     AppendInput(const char* const pszText);
      void     PushLine(const char* const pszLine);
      void     Update();
      bool     CopyData(const BYTE* const pBuf,DWORD dwSize);

      // Calculator
      double            PRNG();
      double            Gamma(double);
      double            Log2(double);
      int               DoOperation();
      int               DoParenthesis();
      void              PushOperation(char);
      void              PushArgument(double);
      int               PopArgument(double*);
      int               PopOperation(int*);
      char*             GetExpression(char*);
      CALC_OPERATOR*    GetOperator(char*);
      int               GetPrecedence(char);
      int               GetStackTopPrecedence();
      double            Evaluate(const CString& rExpr);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
