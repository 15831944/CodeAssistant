// ScreenToolDlg.h : ͷ�ļ�
//

#pragma once
#include "imagebutton.h"
#include "FullScreenWnd.h"

#define IDHOTKEYSTOP 123
#define IDHOTKEYSAVE 124
#define IDHOTKEYCWS 125
#define IDHOTKEYCRS 126
#define IDHOTKEYCFS 127
#define IDHOTKEYCFWS 128
#define IDHOTKEYSC 129
#define IDHOTKEYSM 130
#define WM_TRAY_MSG WM_USER + 100

enum CLIPTYPE {RECTCLIP, WINDOWCLIP, SCREENCLIP, CURRENTCLIP};

// CScreenToolDlg �Ի���
class CScreenToolDlg : public CDialog
{
// ����
public:
	CScreenToolDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CScreenToolDlg();

	// �˳���ʶ
	BOOL IsExit;

// �Ի�������
	enum { IDD = IDD_SCREENTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	//ȫ����ͼ
	void ClipFullScreenToFile();

	//��Ļ���������ͼ
	void ClipRectOfScreen();

	//��Ļ���ڽ�ͼ
	void ClipWindowOfScreen();

	//��ȡ��ǰǰ������
	void ClipForegroundWindow();

	//��Ӧ�ó������õ�����
	BOOL SetTray(BOOL bAdd);

	// ���پ��δ���
	void OnDestoryScreenWnd();

	//�˳�
	void OnExit();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	CImageButton m_imageCutBtn;
	CImageButton m_imageColorBtn;
	CImageButton m_imageMagnifierBtn;
	CImageButton m_imageDropCutBtn;
	CImageButton m_imageHelpBtn;
	CImageList m_images;

	//����һ��ȫ���Ľ�ͼ����
	CFullScreenWnd m_fullScreenWnd;
	CMenu m_cutPopupMenu;
	CMenu m_trayPopupMenu;
	LPCTSTR m_pszClassName;
	CLIPTYPE m_clipType;
	BOOL m_bTray;

public:
	afx_msg void OnBnClickedDropcutBtn();
	afx_msg void OnRectcut();
	afx_msg void OnWindowcut();
	afx_msg void OnScreencut();
	afx_msg void OnCurrentcut();
	afx_msg void OnBnClickedNewcutBtn();
	afx_msg void OnBnClickedPickcolorBtn();
	afx_msg void OnBnClickedMagnifierBtn();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnBnClickedHelpBtn();
	afx_msg LRESULT OnTrayCallBackMsg(WPARAM wparam, LPARAM lparam);
	afx_msg void OnShowMenu();
	afx_msg void OnHelpMenu();
	afx_msg void OnExitMenu();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
