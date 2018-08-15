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
	// 下面这个可选，去掉RichEdit的双字体设置
	// 因为说实话，我不知道怎么分别设置中英文字体，如果哪位同学知道请告诉
	// 我一下
	DWORD fontStyle = SendMessage(EM_GETLANGOPTIONS);
	fontStyle &= ~IMF_DUALFONT;
	SendMessage(EM_SETLANGOPTIONS, 0, fontStyle);

	// 修改为背景透明，刷避免闪烁，我们自己绘制白色背景 （导致bug）
	//ModifyStyleEx(0, WS_EX_TRANSPARENT);

	// 这个接口指针使用非常频繁，存下来提高效率
	m_pRichEditOle = GetIRichEditOle();

	// 启动定时器，用于更新动画
	SetTimer(FRM_TIMER_ID, MIN_FRM_DELAY, NULL);

	// 设置 ENM_CHANGE (响应Change事件)
	//EM_SETEVENTMASK
	long lMask = GetEventMask();
	lMask |= ENM_CHANGE;
	lMask &= ~ENM_PROTECTED;
	SetEventMask(lMask);

	// 格式
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;

	cf.dwEffects&=~CFE_BOLD;      //设置粗体，取消用cf.dwEffects&=~CFE_BOLD;
	cf.dwEffects&=~CFE_ITALIC;    //设置斜体，取消用cf.dwEffects&=~CFE_ITALIC;
	cf.dwEffects&=~CFE_UNDERLINE; //设置斜体，取消用cf.dwEffects&=~CFE_UNDERLINE;
	cf.crTextColor = RGB(0,0,0);  //设置颜色
	cf.yHeight = 14 * 14;         //设置高度
	//strcpy_s(cf.szFaceName, 1024 ,_T("宋体"));//设置字体
	SetDefaultCharFormat(cf);

	::SendMessage(m_hWnd,EM_SETLANGOPTIONS, 0, 0);

	//超链接
	DWORD mask =::SendMessage(m_hWnd,EM_GETEVENTMASK, 0, 0);  
    mask = mask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS |ENM_KEYEVENTS;  
    ::SendMessage(m_hWnd,EM_SETEVENTMASK, 0, mask);  
    ::SendMessage(m_hWnd,EM_AUTOURLDETECT, true, 0);

	// 自动换行
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
	// 停止定时器，这是必要的
	KillTimer(FRM_TIMER_ID);
	// 清空内容，目的是删除所有插入的COleImage对象
	SetWindowText(_T(""));
	// 但是别忘了，RichEdit支持多级撤消，对象虽然删除了，但是撤消缓存
	// 中还保留有对象的引用，清空撤消缓存才能真正的释放这些对象
	EmptyUndoBuffer();

	// 上面的工作必须在父类OnDestroy前调用，否则窗口已经销毁，清理没用
	CRichEditCtrl::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

BOOL COleRichEditCtrl::OnEraseBkgnd(CDC* pDC)
{
	// 绘制背景
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


// 解决复制问题
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
	// TODO:  控件将不发送此通知，除非您重写
	// CRichEditCtrl::OnInitDialog() 函数，以将 EM_SETEVENTMASK 消息发送
	// 到该控件，同时将 ENM_PROTECTED标志“或”运算到 lParam 掩码中。

	// TODO:  在此添加控件通知处理程序代码
	ENPROTECTED* pEP = (ENPROTECTED*)pNMHDR;
	switch (pEP->msg) 
	{
	case WM_KEYDOWN://按键，判断pEP->wParam
	case WM_COPY ://复制
	case WM_PASTE://粘贴
	case WM_CUT://剪切
	case EM_SETCHARFORMAT:
	default:
		break;
	};

	*pResult = 0;
}


// 解决不响应Tab
//UINT COleRichEditCtrl::OnGetDlgCode()
//{
//	return DLGC_WANTTAB; 
//}


//
// 查找第一个字符索引大于或等于cpMin的对象在所有对象中的索引
//
int COleRichEditCtrl::FindFirstObject(int cpMin, int nObjectCount)
{
	// 标准的二分查找算法，不用解释了
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

	// 只不过最后没找到时不是返回-1，而是返回low，此时low必然大于或等于high
	// 刚好满足条件
	return low;
}


//
// 定时处理函数，更新动画
//
void COleRichEditCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// 定时器ID必须为我们所设置的定时器ID，不要以为调用SetTimer这里收到的就一定时我们
	// 设置的定时器，别忘了父类还可能设置，这一点不用怀疑，我测试到过！
	// 当写入很多行文本，滚动条出现后，再在最后插入动画，然后向上滚动到底部，再继续向下
	// 滚动，虽然滚动条不动了，但是动画却动得特别快，显然收到了父类的定时器
	// 因此这个定时器ID最好设置一个不常用的值，这里随便设置了一个，为1
	if (nIDEvent == FRM_TIMER_ID) {

		// 得到对象总数，大于0时才需要刷新
		int nObjectCount = m_pRichEditOle->GetObjectCount();
		if (nObjectCount > 0) {
			CRect rc;
			GetRect(rc); // 得到可输入的矩形区域
			// 分别使用左上角和右下角的点得到最小和最大字符索引
			// 即可见区域的最小和最大字符索引
			int cpMin = CharFromPos(rc.TopLeft());
			int cpMax = CharFromPos(rc.BottomRight());

			// 使用二分查找算法找到第一个字符索引大于或等于cpMin的对象索引
			int iFirst = FindFirstObject(cpMin, nObjectCount);
			REOBJECT reo = {0};
			reo.cbStruct = sizeof(REOBJECT);

			// 从第一个索引开始遍历对象更换帧
			for (int i = iFirst; i < nObjectCount; i++) {
				if (m_pRichEditOle->GetObject(i, &reo, REO_GETOBJ_POLEOBJ) == S_OK) {
					reo.poleobj->Release();
					// 当前对象的字符索引大于最大字符索引，说明对象不在可见区域，停止遍历
					if (reo.cp > cpMax) {
						break;
					}

					// 是COleImage对象时才能更新
					if (InlineIsEqualGUID(reo.clsid, CLSID_OleImage)) {
						// 更换帧
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
// 插入图像
//
HRESULT COleRichEditCtrl::InsertImage(LPCTSTR lpszPathName)
{
	// 全部使用智能指针
	CComPtr<IStorage> spStorage;
	CComPtr<ILockBytes> spLockBytes;
	CComPtr<IOleClientSite> spOleClientSite;	
	CComPtr<COleImage> spOleImage;
	CComPtr<IOleObject> spOleObject;
	CLSID clsid;
	REOBJECT reobject;
	HRESULT hr = E_FAIL;

	do {

		// 创建LockBytes
		hr = CreateILockBytesOnHGlobal(NULL, TRUE, &spLockBytes);
		if (hr != S_OK) {
			break;
		}

		ASSERT(spLockBytes != NULL);

		// 创建Storage
		hr = StgCreateDocfileOnILockBytes(spLockBytes,
			STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &spStorage);
		if (hr != S_OK) {
			break;
		}

		// 获取ClientSite
		hr = m_pRichEditOle->GetClientSite(&spOleClientSite);
		if (hr != S_OK) {
			break;
		}

		// 创建COleImage实例
		hr = CoCreateInstance(CLSID_OleImage, NULL, CLSCTX_INPROC, IID_IOleImage, (LPVOID*) &spOleImage);
		if (hr != S_OK) {
			break;
		}

		// 加载图像
		hr = spOleImage->LoadFromFile(_bstr_t(lpszPathName), this, 400);
		if (hr != S_OK) {
			break;
		}

		// 获取IOleObject接口
		hr = spOleImage->QueryInterface(IID_IOleObject, (LPVOID *) &spOleObject);
		if (hr != S_OK) {
			break;
		}

		// 获取IOleObject的用户CLSID
		hr = spOleObject->GetUserClassID(&clsid);
		if (hr != S_OK) {
			break;
		}

		// 填充OLE对象属性
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

		// 插入OLE对象
		hr = m_pRichEditOle->InsertObject(&reobject);
		if (hr != S_OK) {
			break;
		}

		// 通知OLE容器保证OLE对象被正确引用
		hr = OleSetContainedObject(spOleObject, TRUE);

	} while (0);

	return hr;
}
