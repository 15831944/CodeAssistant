#pragma once


// CAutoCombox

class CAutoCombox : public CComboBox
{
	DECLARE_DYNAMIC(CAutoCombox)

public:
	CAutoCombox();
	virtual ~CAutoCombox();

	// manipulating listbox items  
    int AddString(LPCTSTR lpszString);  
    int DeleteString(UINT nIndex);  
    int InsertString(int nIndex, LPCTSTR lpszString);  
    void ResetContent();  

	void OnMatch();
  
    //set state  
    void SetFlag(UINT nFlag)  
    {m_nFlag = nFlag;}  

private:  
    int Dir(UINT attr, LPCTSTR lpszWildCard)  
    {ASSERT(FALSE);}//forbidden  
  
protected:  
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);  
    void AutoSelect();  
    void AutoMatchAndSel();  
  
    DECLARE_MESSAGE_MAP()  
  
private:  
    CEdit* m_pEdit; //edit control  
    UINT m_nFlag;   //some flag  
                    //bit 0: 0 is show all, 1 is remove not matching, if no maching, show all.  
    CStringArray m_strArr;  
};
