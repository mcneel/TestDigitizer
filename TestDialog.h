#pragma once

#include "Resource.h"

class CTestDialog : public CRhinoDialog
{
	DECLARE_DYNAMIC(CTestDialog)

public:
	CTestDialog(CWnd* pParent);
	virtual ~CTestDialog();

	enum { IDD = IDD_TEST_DIALOG };
  CListBox m_list;
  CButton m_ok;
  HANDLE m_hDevice;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnSelChangeListBox();
  afx_msg void OnDblClkListBox();
	DECLARE_MESSAGE_MAP()

private:
  void FillListBox();
  bool GetFriendlyDeviceName(const wchar_t* pszDeviceName, ON_wString& friendly_name);
};
