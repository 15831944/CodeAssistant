#pragma once

#include "DocumentDlg.h"

// ���7����ǩ
#define MAXTABPAGE 1000


// CTabSheet

class CTabSheet : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabSheet)

public:
	CTabSheet();
	virtual ~CTabSheet();

	//CDialog* m_dlgWnd[MAXTABPAGE]; //����Ǵ�ŶԻ���ָ���ָ������
	CDocumentDlg* m_dlgWnd[MAXTABPAGE]; //����Ǵ�ŶԻ���ָ���ָ������
	int m_curTabNumber; //��¼��ǰ�û�����˼�����ǩҳ
	int m_selTabID;  //��ǰ�û�����ı�ǩҳ��ID

	// �ĵ�����
	vector<CDocumentDlg*> m_DocumentTab;

	//ͨ���������,���Խ�һ���Ի���ָ������ӵı�ǩҳ��������,insWnd�Ǵ����ķ�ģʽ�Ի����ָ��,wndID�ǶԻ����ID,pageText�Ǳ�ǩҳ�ı���
	void CreateTabPage(CWnd *insWnd, int wndID,CString pageText);

	void SetPageTitle( int index, TCHAR *caption );

protected:
	DECLARE_MESSAGE_MAP()

public:
	//��ӿؼ��ĵ���¼��Ĵ���,�������õ���ǰ����ı�ǩҳ��ID,Ȼ����˱�ǩҳ��صĶԻ�����ʾ,���������ؼ���
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
