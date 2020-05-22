#include "stdafx.h"
#include "TestDigitizerApp.h"

BEGIN_MESSAGE_MAP(CTestDigitizerApp, CWinApp)
END_MESSAGE_MAP()

// The one and only CTestDigitizerApp object
static class CTestDigitizerApp theApp;

// CTestDigitizerApp initialization

BOOL CTestDigitizerApp::InitInstance()
{
  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  // Only standard MFC DLL instance initialization belongs here. 
  // All other significant initialization should take place in
  // CTestDigitizerPlugIn::OnLoadPlugIn().

	CWinApp::InitInstance();

	return TRUE;
}

int CTestDigitizerApp::ExitInstance()
{
  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  // Only standard MFC DLL instance clean up belongs here. 
  // All other significant cleanup should take place in either
  // CTestDigitizerPlugIn::OnSaveAllSettings() or
  // CTestDigitizerPlugIn::OnUnloadPlugIn().

  return CWinApp::ExitInstance();
}
