#include "stdafx.h"
#include "TestRawInputWindow.h"
#include "TestDigitizerPlugIn.h"
#include "resource.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

IMPLEMENT_DYNAMIC(CTestRawInputWindow, CWnd)

CTestRawInputWindow* CTestRawInputWindow::m_pWnd = nullptr;

CTestRawInputWindow::CTestRawInputWindow()
  : m_hDevice(0)
  , m_hCursor(0)
  , m_hOldCursor(0)
{
}

CTestRawInputWindow::CTestRawInputWindow(HANDLE hDevice)
  : m_hDevice(hDevice)
  , m_hCursor(0)
  , m_hOldCursor(0)
{
}

CTestRawInputWindow::~CTestRawInputWindow()
{
}

BEGIN_MESSAGE_MAP(CTestRawInputWindow, CWnd)
  ON_MESSAGE(WM_INPUT, OnRawInput)
END_MESSAGE_MAP()

bool CTestRawInputWindow::CreateWnd(HANDLE hDevice)
{
  if (CTestRawInputWindow::m_pWnd && ::IsWindow(CTestRawInputWindow::m_pWnd->GetSafeHwnd()))
    return true;

  if (CTestRawInputWindow::m_pWnd)
  {
    delete CTestRawInputWindow::m_pWnd;
    CTestRawInputWindow::m_pWnd = nullptr;
  }

  CTestRawInputWindow::m_pWnd = new CTestRawInputWindow(hDevice);

  BOOL rc = CTestRawInputWindow::m_pWnd->CreateEx(
    0,
    AfxRegisterWndClass(0),
    L"Message-Only Window",
    WS_VISIBLE,
    0, 0, 0, 0,
    HWND_MESSAGE,
    0
  );

  if (!rc)
  {
    delete CTestRawInputWindow::m_pWnd;
    CTestRawInputWindow::m_pWnd = nullptr;
    return false;
  }

  CTestRawInputWindow::m_pWnd->LoadCursors();
  CTestRawInputWindow::m_pWnd->RegisterDevices();

  return true;
}

void CTestRawInputWindow::DestroyWnd()
{
  if (CTestRawInputWindow::m_pWnd)
  {
    CTestRawInputWindow::m_pWnd->UnegisterDevices();
    CTestRawInputWindow::m_pWnd->UnloadCursors();

    if (::IsWindow(CTestRawInputWindow::m_pWnd->m_hWnd))
      CTestRawInputWindow::m_pWnd->DestroyWindow();

    delete CTestRawInputWindow::m_pWnd;
    CTestRawInputWindow::m_pWnd = nullptr;
  }
}

void CTestRawInputWindow::LoadCursors()
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  if (0 == m_hCursor)
  {
    m_hCursor = (HCURSOR)::LoadImage(
      AfxGetInstanceHandle(),
      MAKEINTRESOURCE(IDC_TEST_CURSOR),
      IMAGE_CURSOR,
      0,
      0,
      LR_DEFAULTSIZE
    );
    m_hOldCursor = 0;
  }
}

void CTestRawInputWindow::UnloadCursors()
{
  if (0 != m_hCursor)
  {
    ::DestroyCursor(m_hCursor);
    m_hCursor = 0;
  }
  m_hOldCursor = 0;
}

BOOL CTestRawInputWindow::RegisterDevices()
{
  RAWINPUTDEVICE rid;
  memset(&rid, 0, sizeof(rid));
  rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
  rid.usUsage = HID_USAGE_GENERIC_MOUSE;
  rid.dwFlags = 0;
  rid.hwndTarget = GetSafeHwnd();
  return RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

BOOL CTestRawInputWindow::UnegisterDevices()
{
  RAWINPUTDEVICE rid;
  memset(&rid, 0, sizeof(rid));
  rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
  rid.usUsage = HID_USAGE_GENERIC_MOUSE;
  rid.dwFlags = RIDEV_REMOVE;
  rid.hwndTarget = GetSafeHwnd();
  return RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

LRESULT CTestRawInputWindow::OnRawInput(WPARAM wParam, LPARAM lParam)
{
  UINT cbSize = 0;
  GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &cbSize, sizeof(RAWINPUTHEADER));
  if (cbSize == 0)
    return 1;

  LPBYTE pData = new BYTE[cbSize];
  UINT nResult = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, pData, &cbSize, sizeof(RAWINPUTHEADER));
  if (nResult == 0 || nResult != cbSize)
  {
    delete pData; // Don't leak...
    return 1;
  }

  PRAWINPUT pRaw = (PRAWINPUT)pData;
  if (pRaw->header.dwType == RIM_TYPEMOUSE)
  {
    if (pRaw->header.hDevice == m_hDevice)
    {
      CRhinoView* pView = RhinoApp().ActiveView();
      if (pView)
      {
        CPoint screen_pt;
        GetCursorPos(&screen_pt);

        ON_2iPoint client_pt(screen_pt.x, screen_pt.y);
        pView->ScreenToClient(client_pt);

        ON_Line world_line;
        if (pView->ActiveViewport().VP().GetFrustumLine(client_pt.x, client_pt.y, world_line))
        {
          double t = ON_UNSET_VALUE;
          if (ON_Intersect(world_line, pView->ActiveViewport().m_v.m_cplane.m_plane, &t))
          {
            ON_3dPoint point = world_line.PointAt(t);

            UINT nFlags = 0;
            if (pRaw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
              nFlags |= MK_LBUTTON;
            if (pRaw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
              nFlags |= MK_RBUTTON;
            if (pRaw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
              nFlags |= MK_MBUTTON;

            HCURSOR hOldCursor = RhinoSetCursor(m_hCursor);
            if (hOldCursor != m_hCursor)
              m_hOldCursor = hOldCursor;

            TestDigitizerPlugIn().SendPoint(point, nFlags);

            return 0;
          }
        }
      }
    }
    else
    {
      if (0 != m_hOldCursor)
      {
        RhinoSetCursor(m_hOldCursor);
        m_hOldCursor = 0;
      }
    }
  }

  delete pData; // Don't leak...

  return 1;
}
