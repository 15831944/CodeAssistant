#pragma once
#include "sortlistctrl.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "olericheditctrl.h"
#include "SettingDlg.h"
#include "../capture/ScreenToolDlg.h"

#include "TabSheet.h"
#include "DocumentDlg.h"

#include "AssistantDlg.h"
#include "capture/common.h"
#include <atlimage.h>

// CMainDlg 对话框

class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // 标准构造方法
	virtual ~CMainDlg();

	CSortListCtrl m_List;
	CComboBox m_Class;
	CComboBox m_Type;
	CToolTipCtrl m_toolTips;

	int TabCount;
	CTabSheet m_Tab;
	CDocumentDlg* m_pDocument[1000];

	CString CurClass, CurType;

	void static DeleteDirectory(CString Directory);
	void Refresh(), OnHelp();
	void OnEditFunction();

	void Split(CString source, CString divKey, CStringArray &dest);
	void Complete();
	void OnError();

	void OnCheck();
	void OnSaveCode();
	void OnNewCode();
	void OnNewLabel();
	void OnSetLabel();

	void OnCopy(), OnPaste(), OnCut();
	void OnOpen(), OnUndo(), OnRedo();
	void SetPic(), OnImage(), OnScreenCapture(), SavePic();
	void SaveImage();
	void OnLeft(), OnRight(), OnCenter();
	int  static CountFile(CString DirPath);
	void OnParagraph(), OnNone(), OnSymbol(), OnNumber();
	void OnLowerCase(), OnUpperCase(), OnLowerRome(), OnUpperRome();

	bool Compress  (const char* scrfilename, const char* desfilename);
	bool Uncompress(const char* scrfilename, const char* desfilename);

	// 编码助理
	HCURSOR m_hCursor;
	CAssistantDlg * m_pAssisDlg;

	//定义当前窗口句柄
	HWND m_curWnd;

	//定义前一个窗口句柄
	HWND m_preWnd;
	RECT m_rectOfWnd;
	HBITMAP m_rectOfWndBmp;
	BOOL m_bSnap;

	// 退出标识
	BOOL IsExit;

	void SnapWindow(POINT point);
	void DrawRectInWnd(LPRECT pRect);
	void OnCodeAssistant();
	void ClearScreen();
	void OnChangeClassType(int Class, int Type);

// 对话框数据
	enum { IDD = IDD_MAIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 图标
	HICON m_hIcon;

	// 设置对话框
	CSettingDlg * m_Setting;

	// 操作类型
	int Type;

	// 线程对象
	CWinThread * m_hOperate, * m_hUpDate;

	CString UpDateInfo, Error, Msg, FilePath, FileName, FileClass, FileType;
	BOOL IsNew, IsEdit, IsSave, IsCancel;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// 子窗口消息
	afx_msg LRESULT OnMessageChild(WPARAM wParam, LPARAM lParam);

	// 工作者线程
	static UINT Operate(LPVOID pParam);
	static UINT UpDate (LPVOID pParam);

	// 各个控件消息映射
	afx_msg void OnSave();
	afx_msg void OnDelete();
	afx_msg void OnOK();
	afx_msg void OnFont();
	afx_msg void OnDirectory();
	afx_msg void OnRemove();

	afx_msg void OnClickCodeList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRClickCodeList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkCodeList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemchangedCodeList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnKillfocusClassCombo();
	afx_msg void OnKillfocusTypeCombo();

	afx_msg void OnDropdownClassCombo();
	afx_msg void OnDropdownTypeCombo();

	afx_msg void OnEdit();
	afx_msg void OnClearFormat();
	afx_msg void OnSynchronization();
	afx_msg void OnSetting();
	afx_msg void OnManager();
	afx_msg void OnNew();

	virtual void OnCancel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnIcon();
};
