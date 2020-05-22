#include "stdafx.h"
#include "TestDialog.h"

IMPLEMENT_DYNAMIC(CTestDialog, CRhinoDialog)

CTestDialog::CTestDialog(CWnd* pParent)
	: CRhinoDialog(IDD_TEST_DIALOG, pParent)
  , m_hDevice(0)
{
}

CTestDialog::~CTestDialog()
{
}

void CTestDialog::DoDataExchange(CDataExchange* pDX)
{
	CRhinoDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_list);
  DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CTestDialog, CRhinoDialog)
  ON_LBN_SELCHANGE(IDC_LIST, OnSelChangeListBox)
  ON_LBN_DBLCLK(IDC_LIST, OnDblClkListBox)
END_MESSAGE_MAP()

BOOL CTestDialog::OnInitDialog()
{
  EnableSaveDialogPositionAndSize(TRUE, L"TestDialog");

  CRhinoDialog::OnInitDialog();

  FillListBox();

  OnSelChangeListBox();

  return TRUE;
}

void CTestDialog::OnOK()
{
  int nIndex = m_list.GetCurSel();
  if (nIndex != LB_ERR)
  {
    m_hDevice = (HANDLE)m_list.GetItemData(nIndex);
  }

  CRhinoDialog::OnOK();
}

void CTestDialog::OnSelChangeListBox()
{
  int nIndex = m_list.GetCurSel();
  m_ok.EnableWindow(nIndex == LB_ERR ? FALSE : TRUE);
}

void CTestDialog::OnDblClkListBox()
{
  CTestDialog::OnOK();
}

void CTestDialog::FillListBox()
{
  // Get number of input devices
  UINT nDevices = 0;
  UINT nResult = GetRawInputDeviceList(nullptr, &nDevices, sizeof(RAWINPUTDEVICELIST));
  if (nResult < 0 || nDevices == 0)
    return;

  // Fill device list buffer
  PRAWINPUTDEVICELIST pRawInputDeviceList = (PRAWINPUTDEVICELIST)onmalloc(sizeof(RAWINPUTDEVICELIST) * nDevices);
  nResult = GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST));
  if (nResult < 0)
  {
    onfree(pRawInputDeviceList); // Dont' leak...
    return;
  }

  // Loop through device list
  for (UINT i = 0; i < nDevices; i++)
  {
    UINT cbSize = sizeof(RID_DEVICE_INFO);

    RID_DEVICE_INFO rdiDeviceInfo;
    memset(&rdiDeviceInfo, 0, cbSize);
    rdiDeviceInfo.cbSize = cbSize;

    // Skip anthing that isn't a mosue with at least 1 button
    nResult = GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &rdiDeviceInfo, &cbSize);
    if (nResult < 0 || rdiDeviceInfo.dwType != RIM_TYPEMOUSE || 0 == rdiDeviceInfo.mouse.dwNumberOfButtons)
      continue;

    cbSize = 0;
    nResult = GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, nullptr, &cbSize);
    if (nResult < 0 || cbSize == 0)
      continue;

    ON_wString device_name;
    device_name.SetLength(cbSize + 1);
    nResult = GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, device_name.Array(), &cbSize);
    if (nResult < 0 || device_name.IsEmpty())
      continue;

    ON_wString friendly_name;
    if (GetFriendlyDeviceName(static_cast<const wchar_t*>(device_name), friendly_name))
    {
      ON_wString string;
      string.Format(
        L"%s (%d %s)",
        static_cast<const wchar_t*>(friendly_name),
        rdiDeviceInfo.mouse.dwNumberOfButtons,
        rdiDeviceInfo.mouse.dwNumberOfButtons == 1 ? L"Button" : L"Buttons"
      );

      int index = m_list.AddString(static_cast<const wchar_t*>(string));
      if (index != LB_ERR)
        m_list.SetItemData(index, (DWORD_PTR)pRawInputDeviceList[i].hDevice);
    }
  }

  onfree(pRawInputDeviceList); // Dont' leak...
}

static int SplitString(const wchar_t* pszString, const wchar_t* pszDelimiters, ON_ClassArray<ON_wString>& tokens)
{
  if (nullptr == pszString || 0 == pszString[0] || nullptr == pszDelimiters || 0 == pszDelimiters[0])
    return 0;

  ON_wString string = pszString;
  const wchar_t* pszToken = nullptr;
  wchar_t* ptr = nullptr;

  for (pszToken = wcstok_s(string.Array(), pszDelimiters, &ptr); nullptr != pszToken; pszToken = wcstok_s(nullptr, pszDelimiters, &ptr))
    tokens.Append(ON_wString(pszToken));

  return tokens.Count();
}


bool CTestDialog::GetFriendlyDeviceName(const wchar_t* pszDeviceName, ON_wString& friendly_name)
{
  if (pszDeviceName == nullptr || pszDeviceName[0] == 0)
    return false;

  ON_wString device_name(pszDeviceName);
  device_name.TrimLeft(L"\\\\?\\"); // Remove the \\?\

  ON_ClassArray<ON_wString> tokens;
  SplitString(static_cast<const wchar_t*>(device_name), L"#", tokens);
  if (tokens.Count() != 4)
    return false;

  ON_wString key;
  key.Format(L"System\\CurrentControlSet\\Enum\\%s\\%s\\%s",
    tokens[0].Array(),
    tokens[1].Array(),
    tokens[2].Array()
  );

  CRegKey reg;
  if (reg.Open(HKEY_LOCAL_MACHINE, key, KEY_READ) == ERROR_SUCCESS)
  {
    DWORD dwLen = 2048;
    ON_wString value;
    value.SetLength(dwLen);
    if (reg.QueryStringValue(L"DeviceDesc", value.Array(), &dwLen) == ERROR_SUCCESS)
    {
      if (!value.IsEmpty())
      {
        tokens.Destroy();
        SplitString(static_cast<const wchar_t*>(value), L";", tokens);
        if (tokens.Count() > 1)
        {
          friendly_name = tokens[tokens.Count() - 1];
          return true;
        }
      }
    }
  }

  return false;
}
