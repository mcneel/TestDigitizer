#pragma once

class CTestRawInputWindow : public CWnd
{
  DECLARE_DYNAMIC(CTestRawInputWindow)

public:
  CTestRawInputWindow();
  CTestRawInputWindow(HANDLE hDevice);
  virtual ~CTestRawInputWindow();

public:
  // Create the message-only window
  static bool CreateWnd(HANDLE hDevice);

  // Destroy the message-only window
  static void DestroyWnd();

protected:
  LRESULT CTestRawInputWindow::OnRawInput(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

private:
  BOOL RegisterDevices();
  BOOL UnegisterDevices();
  void LoadCursors();
  void UnloadCursors();

private:
  static CTestRawInputWindow* m_pWnd;
  HANDLE m_hDevice;
  HCURSOR m_hCursor;
  HCURSOR m_hOldCursor;
};
