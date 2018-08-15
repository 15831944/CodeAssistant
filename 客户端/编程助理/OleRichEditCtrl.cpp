// OleRichEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "OleRichEditCtrl.h"
#include "OleImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COleRichEditCtrl

COleRichEditCtrl::COleRichEditCtrl()
{
	m_bCallbackSet = FALSE;
	m_pRichEditOle = NULL;
}

COleRichEditCtrl::~COleRichEditCtrl()
{
	// IExRichEditOleCallback class is a reference-counted class  
	// which deletes itself and for which delete should not be called

	delete m_pIRichEditOleCallback;
}


BEGIN_MESSAGE_MAP(COleRichEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(COleRichEditCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	//ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(EN_PROTECTED, &COleRichEditCtrl::OnEnProtected)
END_MESSAGE_MAP()


int COleRichEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
 	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
 		return -1;
 	
	// m_pIRichEditOleCallback should have been created in PreSubclassWindow

 	ASSERT( m_pIRichEditOleCallback != NULL );	

	// set the IExRichEditOleCallback pointer if it wasn't set 
	// successfully in PreSubclassWindow

	if ( !m_bCallbackSet )
	{
		SetOLECallback( m_pIRichEditOleCallback );
	}
 	
 	return 0;
}

void COleRichEditCtrl::PreSubclassWindow() 
{
	// ���������ѡ��ȥ��RichEdit��˫��������
	// ��Ϊ˵ʵ�����Ҳ�֪����ô�ֱ�������Ӣ�����壬�����λͬѧ֪�������
	// ��һ��
	DWORD fontStyle = SendMessage(EM_GETLANGOPTIONS);
	fontStyle &= ~IMF_DUALFONT;
	SendMessage(EM_SETLANGOPTIONS, 0, fontStyle);

	// �޸�Ϊ����͸����ˢ������˸�������Լ����ư�ɫ���� ������bug��
	//ModifyStyleEx(0, WS_EX_TRANSPARENT);

	// ����ӿ�ָ��ʹ�÷ǳ�Ƶ�������������Ч��
	m_pRichEditOle = GetIRichEditOle();

	// ������ʱ�������ڸ��¶���
	SetTimer(FRM_TIMER_ID, MIN_FRM_DELAY, NULL);

	// ���� ENM_CHANGE (��ӦChange�¼�)
	//EM_SETEVENTMASK
	long lMask = GetEventMask();
	lMask |= ENM_CHANGE;
	lMask &= ~ENM_PROTECTED;
	SetEventMask(lMask);

	// ��ʽ
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;

	cf.dwEffects&=~CFE_BOLD;      //���ô��壬ȡ����cf.dwEffects&=~CFE_BOLD;
	cf.dwEffects&=~CFE_ITALIC;    //����б�壬ȡ����cf.dwEffects&=~CFE_ITALIC;
	cf.dwEffects&=~CFE_UNDERLINE; //����б�壬ȡ����cf.dwEffects&=~CFE_UNDERLINE;
	cf.crTextColor = RGB(0,0,0);  //������ɫ
	cf.yHeight = 14 * 14;         //���ø߶�
	//strcpy_s(cf.szFaceName, 1024 ,_T("����"));//��������
	SetDefaultCharFormat(cf);

	::SendMessage(m_hWnd,EM_SETLANGOPTIONS, 0, 0);

	//������
	DWORD mask =::SendMessage(m_hWnd,EM_GETEVENTMASK, 0, 0);  
    mask = mask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS |ENM_KEYEVENTS;  
    ::SendMessage(m_hWnd,EM_SETEVENTMASK, 0, mask);  
    ::SendMessage(m_hWnd,EM_AUTOURLDETECT, true, 0);

	// �Զ�����
	if(GetPrivateProfileInt(_T("Setting"), _T("Line"), 0, _T("./Setting.ini")) == 1)
		SetTargetDevice(NULL,0);

	// base class first
	CRichEditCtrl::PreSubclassWindow();	

	m_pIRichEditOleCallback = NULL;
	m_pIRichEditOleCallback = new IExRichEditOleCallback;
	ASSERT( m_pIRichEditOleCallback != NULL );

	m_bCallbackSet = SetOLECallback( m_pIRichEditOleCallback );
}

void COleRichEditCtrl::OnDestroy()
{
	// ֹͣ��ʱ�������Ǳ�Ҫ��
	KillTimer(FRM_TIMER_ID);
	// ������ݣ�Ŀ����ɾ�����в����COleImage����
	SetWindowText(_T(""));
	// ���Ǳ����ˣ�RichEdit֧�ֶ༶������������Ȼɾ���ˣ����ǳ�������
	// �л������ж�������ã���ճ�����������������ͷ���Щ����
	EmptyUndoBuffer();

	// ����Ĺ��������ڸ���OnDestroyǰ���ã����򴰿��Ѿ����٣�����û��
	CRichEditCtrl::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
}

BOOL COleRichEditCtrl::OnEraseBkgnd(CDC* pDC)
{
	// ���Ʊ���
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, RGB(255, 255, 255));
	return TRUE;
}


long COleRichEditCtrl::StreamInFromResource(int iRes, LPCTSTR sType)
{
	HINSTANCE hInst = AfxGetInstanceHandle();
	HRSRC hRsrc = ::FindResource(hInst,
		MAKEINTRESOURCE(iRes), sType);
	
	DWORD len = SizeofResource(hInst, hRsrc); 
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc); 
	ASSERT(lpRsrc); 
 
	CMemFile mfile;
	mfile.Attach(lpRsrc, len); 

	EDITSTREAM es;
	es.pfnCallback = readFunction;
	es.dwError = 0;
	es.dwCookie = (DWORD) &mfile;

	return StreamIn( SF_RTF, es );
}

long COleRichEditCtrl::StreamInFromResource(CString strRes, LPCTSTR sType)
{
	/*HINSTANCE hInst = AfxGetInstanceHandle();
	HRSRC hRsrc = ::FindResource(hInst,
		MAKEINTRESOURCE(iRes), sType);*/

	//DWORD len = wcslen(strRes);/*SizeofResource(hInst, hRsrc); */
	DWORD len = strlen(strRes);/*SizeofResource(hInst, hRsrc); */

	BYTE* lpRsrc = (BYTE*)strRes.GetBuffer(0);/*LoadResource(hInst, hRsrc)*/ 
	ASSERT(lpRsrc); 

	//CMemFile mfile;
	//mfile.Attach(lpRsrc, len); 
	CFile file;
	if (!file.Open(strRes, CFile::modeReadWrite))
		return 0;

	EDITSTREAM es;
	es.pfnCallback = readFunction;
	es.dwError = 0;
	es.dwCookie = (DWORD)(LONGLONG)(&file);;

	long lresult=StreamIn( SF_RTF, es );
	file.Close();
	return lresult;
}

long COleRichEditCtrl::StreamOutToFile(CString strDesFile, LPCTSTR sType)
{
	/*HINSTANCE hInst = AfxGetInstanceHandle();
	HRSRC hRsrc = ::FindResource(hInst,
	MAKEINTRESOURCE(iRes), sType);*/

	//DWORD len = wcslen(strDesFile);/*SizeofResource(hInst, hRsrc); */
	DWORD len = strlen(strDesFile);/*SizeofResource(hInst, hRsrc); */
	
	BYTE* lpRsrc = (BYTE*)strDesFile.GetBuffer(0);/*LoadResource(hInst, hRsrc)*/ 
	ASSERT(lpRsrc); 

	//CMemFile mfile;
	//mfile.Attach(lpRsrc, len); 
	CFile file;
	if (!file.Open(strDesFile, CFile::modeReadWrite|CFile::modeCreate))
		return 0;

	EDITSTREAM es;
	es.pfnCallback = writeFunction;
	es.dwError = 0;
	es.dwCookie = (DWORD)(LONGLONG)(&file);;

	long lresult=StreamOut( SF_RTF, es );
	file.Close();
	return lresult;
}

/* static */
DWORD CALLBACK COleRichEditCtrl::readFunction(DWORD dwCookie,
		 LPBYTE lpBuf,			// the buffer to fill
		 LONG nCount,			// number of bytes to read
		 LONG* nRead)			// number of bytes actually read
{
	CFile* fp = (CFile *)dwCookie;
	*nRead = fp->Read(lpBuf,nCount);
	return 0;
}

DWORD CALLBACK COleRichEditCtrl::writeFunction(DWORD dwCookie,
											  LPBYTE lpBuf,			// the buffer to fill
											  LONG nCount,			// number of bytes to read
											  LONG* nWrite)			// number of bytes actually read
{
	CFile* fp = (CFile *)dwCookie;
	fp->Write(lpBuf,nCount);
	*nWrite = (LONG)fp->GetLength();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

COleRichEditCtrl::IExRichEditOleCallback::IExRichEditOleCallback()
{
	pStorage = NULL;
	m_iNumStorages = 0;
	m_dwRef = 0;

	// set up OLE storage

	HRESULT hResult = ::StgCreateDocfile(NULL,
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE /*| STGM_DELETEONRELEASE */|STGM_CREATE ,
		0, &pStorage );

	if ( pStorage == NULL ||
		hResult != S_OK )
	{
		AfxThrowOleException( hResult );
	}
}

COleRichEditCtrl::IExRichEditOleCallback::~IExRichEditOleCallback()
{
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetNewStorage(LPSTORAGE* lplpstg)
{
	m_iNumStorages++;
	WCHAR tName[50];
	swprintf_s(tName, L"REOLEStorage%d", m_iNumStorages);

	HRESULT hResult = pStorage->CreateStorage(tName, 
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
		0, 0, lplpstg );

	if (hResult != S_OK )
	{
		::AfxThrowOleException( hResult );
	}

	return hResult;
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;
	
	if ( iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback )
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}



ULONG STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::AddRef()
{
	return ++m_dwRef;
}



ULONG STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::Release()
{
	if ( --m_dwRef == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
	LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
	DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	return S_OK;
}


// �����������
HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	//IRichEditOle* pThis = ((IRichEditOle*)((BYTE*)this - offsetof(IRichEditOle, m_IRichEditOleCallbackEx)));

	//return pThis->GetIRichEditOle()->GetClipboardData(lpchrg, reco, lplpdataobj);
	//return S_OK;

	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
	HMENU FAR *lphmenu)
{
	return S_OK;
}


void COleRichEditCtrl::OnEnProtected(NMHDR *pNMHDR, LRESULT *pResult)
{
	ENPROTECTED *pEnProtected = reinterpret_cast<ENPROTECTED *>(pNMHDR);
	// TODO:  �ؼ��������ʹ�֪ͨ����������д
	// CRichEditCtrl::OnInitDialog() �������Խ� EM_SETEVENTMASK ��Ϣ����
	// ���ÿؼ���ͬʱ�� ENM_PROTECTED��־�������㵽 lParam �����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ENPROTECTED* pEP = (ENPROTECTED*)pNMHDR;
	switch (pEP->msg) 
	{
	case WM_KEYDOWN://�������ж�pEP->wParam
	case WM_COPY ://����
	case WM_PASTE://ճ��
	case WM_CUT://����
	case EM_SETCHARFORMAT:
	default:
		break;
	};

	*pResult = 0;
}


// �������ӦTab
//UINT COleRichEditCtrl::OnGetDlgCode()
//{
//	return DLGC_WANTTAB; 
//}


//
// ���ҵ�һ���ַ��������ڻ����cpMin�Ķ��������ж����е�����
//
int COleRichEditCtrl::FindFirstObject(int cpMin, int nObjectCount)
{
	// ��׼�Ķ��ֲ����㷨�����ý�����
	int low = 0;
	int high = nObjectCount - 1;
	REOBJECT reoMid = {0};
	reoMid.cbStruct = sizeof(REOBJECT);
	while (low <= high) {
		int mid = (low + high) >> 1;
		if (m_pRichEditOle->GetObject(mid, &reoMid, REO_GETOBJ_POLEOBJ) != S_OK) {
			return -1;
		}
		reoMid.poleobj->Release();
		if (reoMid.cp == cpMin) {
			return mid;
		} else if (reoMid.cp < cpMin) {
			low = mid + 1;
		} else {
			high = mid - 1;
		}
	}

	// ֻ�������û�ҵ�ʱ���Ƿ���-1�����Ƿ���low����ʱlow��Ȼ���ڻ����high
	// �պ���������
	return low;
}


//
// ��ʱ�����������¶���
//
void COleRichEditCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	// ��ʱ��ID����Ϊ���������õĶ�ʱ��ID����Ҫ��Ϊ����SetTimer�����յ��ľ�һ��ʱ����
	// ���õĶ�ʱ���������˸��໹�������ã���һ�㲻�û��ɣ��Ҳ��Ե�����
	// ��д��ܶ����ı������������ֺ����������붯����Ȼ�����Ϲ������ײ����ټ�������
	// ��������Ȼ�����������ˣ����Ƕ���ȴ�����ر�죬��Ȼ�յ��˸���Ķ�ʱ��
	// ��������ʱ��ID�������һ�������õ�ֵ���������������һ����Ϊ1
	if (nIDEvent == FRM_TIMER_ID) {

		// �õ���������������0ʱ����Ҫˢ��
		int nObjectCount = m_pRichEditOle->GetObjectCount();
		if (nObjectCount > 0) {
			CRect rc;
			GetRect(rc); // �õ�������ľ�������
			// �ֱ�ʹ�����ϽǺ����½ǵĵ�õ���С������ַ�����
			// ���ɼ��������С������ַ�����
			int cpMin = CharFromPos(rc.TopLeft());
			int cpMax = CharFromPos(rc.BottomRight());

			// ʹ�ö��ֲ����㷨�ҵ���һ���ַ��������ڻ����cpMin�Ķ�������
			int iFirst = FindFirstObject(cpMin, nObjectCount);
			REOBJECT reo = {0};
			reo.cbStruct = sizeof(REOBJECT);

			// �ӵ�һ��������ʼ�����������֡
			for (int i = iFirst; i < nObjectCount; i++) {
				if (m_pRichEditOle->GetObject(i, &reo, REO_GETOBJ_POLEOBJ) == S_OK) {
					reo.poleobj->Release();
					// ��ǰ������ַ�������������ַ�������˵�������ڿɼ�����ֹͣ����
					if (reo.cp > cpMax) {
						break;
					}

					// ��COleImage����ʱ���ܸ���
					if (InlineIsEqualGUID(reo.clsid, CLSID_OleImage)) {
						// ����֡
						COleImage *pOleImage = COleImage::FromOleObject(reo.poleobj);
						pOleImage->ChangeFrame();
					}
				}
			}
		}
	} else {
		CRichEditCtrl::OnTimer(nIDEvent);
	}
}


//
// ����ͼ��
//
HRESULT COleRichEditCtrl::InsertImage(LPCTSTR lpszPathName)
{
	// ȫ��ʹ������ָ��
	CComPtr<IStorage> spStorage;
	CComPtr<ILockBytes> spLockBytes;
	CComPtr<IOleClientSite> spOleClientSite;	
	CComPtr<COleImage> spOleImage;
	CComPtr<IOleObject> spOleObject;
	CLSID clsid;
	REOBJECT reobject;
	HRESULT hr = E_FAIL;

	do {

		// ����LockBytes
		hr = CreateILockBytesOnHGlobal(NULL, TRUE, &spLockBytes);
		if (hr != S_OK) {
			break;
		}

		ASSERT(spLockBytes != NULL);

		// ����Storage
		hr = StgCreateDocfileOnILockBytes(spLockBytes,
			STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &spStorage);
		if (hr != S_OK) {
			break;
		}

		// ��ȡClientSite
		hr = m_pRichEditOle->GetClientSite(&spOleClientSite);
		if (hr != S_OK) {
			break;
		}

		// ����COleImageʵ��
		hr = CoCreateInstance(CLSID_OleImage, NULL, CLSCTX_INPROC, IID_IOleImage, (LPVOID*) &spOleImage);
		if (hr != S_OK) {
			break;
		}

		// ����ͼ��
		hr = spOleImage->LoadFromFile(_bstr_t(lpszPathName), this, 400);
		if (hr != S_OK) {
			break;
		}

		// ��ȡIOleObject�ӿ�
		hr = spOleImage->QueryInterface(IID_IOleObject, (LPVOID *) &spOleObject);
		if (hr != S_OK) {
			break;
		}

		// ��ȡIOleObject���û�CLSID
		hr = spOleObject->GetUserClassID(&clsid);
		if (hr != S_OK) {
			break;
		}

		// ���OLE��������
		ZeroMemory(&reobject, sizeof(REOBJECT));		
		reobject.cbStruct	= sizeof(REOBJECT);
		reobject.clsid		= clsid;
		reobject.cp			= REO_CP_SELECTION;
		reobject.dvaspect	= DVASPECT_CONTENT;
		reobject.dwFlags	= REO_BELOWBASELINE;
		reobject.poleobj	= spOleObject;
		reobject.polesite	= spOleClientSite;
		reobject.pstg		= spStorage;
		SIZEL sizel = {0};
		reobject.sizel = sizel;

		// ����OLE����
		hr = m_pRichEditOle->InsertObject(&reobject);
		if (hr != S_OK) {
			break;
		}

		// ֪ͨOLE������֤OLE������ȷ����
		hr = OleSetContainedObject(spOleObject, TRUE);

	} while (0);

	return hr;
}
