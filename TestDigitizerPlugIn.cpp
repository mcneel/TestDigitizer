#include "StdAfx.h"
#include "rhinoSdkPlugInDeclare.h"
#include "TestDigitizerPlugIn.h"
#include "TestRawInputWindow.h"
#include "TestDialog.h"
#include "Resource.h"

#pragma warning(push)
#pragma warning(disable : 4073)
#pragma init_seg(lib)
#pragma warning(pop)

// Rhino plug-in declarations
RHINO_PLUG_IN_DECLARE
RHINO_PLUG_IN_NAME(L"TestDigitizer");
RHINO_PLUG_IN_ID(L"272DFB15-EED8-44E5-96E0-17D22CA20059");
RHINO_PLUG_IN_VERSION(__DATE__ "  " __TIME__)
RHINO_PLUG_IN_DESCRIPTION(L"TestDigitizer plug-in for Rhinoceros®");
RHINO_PLUG_IN_ICON_RESOURCE_ID(IDI_ICON);
RHINO_PLUG_IN_DEVELOPER_ORGANIZATION(L"Robert McNeel & Associates");
RHINO_PLUG_IN_DEVELOPER_ADDRESS(L"3670 Woodland Park Avenue North\r\nSeattle WA 98103");
RHINO_PLUG_IN_DEVELOPER_COUNTRY(L"United States");
RHINO_PLUG_IN_DEVELOPER_PHONE(L"206-545-6877");
RHINO_PLUG_IN_DEVELOPER_FAX(L"206-545-7321");
RHINO_PLUG_IN_DEVELOPER_EMAIL(L"devsupport@mcneel.com");
RHINO_PLUG_IN_DEVELOPER_WEBSITE(L"http://www.rhino3d.com");
RHINO_PLUG_IN_UPDATE_URL(L"https://github.com/mcneel/rhino-developer-samples");

// The one and only CTestDigitizerPlugIn object
static class CTestDigitizerPlugIn thePlugIn;

static class CTestRawInputWindow theInputWindow;


/////////////////////////////////////////////////////////////////////////////
// CTestDigitizerPlugIn definition

CTestDigitizerPlugIn& TestDigitizerPlugIn()
{
	return thePlugIn;
}

CTestDigitizerPlugIn::CTestDigitizerPlugIn()
{
	m_plugin_version = RhinoPlugInVersion();
}

/////////////////////////////////////////////////////////////////////////////
// Required overrides

const wchar_t* CTestDigitizerPlugIn::PlugInName() const
{
	return RhinoPlugInName();
}

const wchar_t* CTestDigitizerPlugIn::PlugInVersion() const
{
	return m_plugin_version;
}

GUID CTestDigitizerPlugIn::PlugInID() const
{
	// {272DFB15-EED8-44E5-96E0-17D22CA20059}
	return ON_UuidFromString(RhinoPlugInId());
}

/////////////////////////////////////////////////////////////////////////////
// Additional overrides

BOOL CTestDigitizerPlugIn::OnLoadPlugIn()
{
	return TRUE;
}

void CTestDigitizerPlugIn::OnUnloadPlugIn()
{
}

/////////////////////////////////////////////////////////////////////////////
// Digitizer overrides

bool CTestDigitizerPlugIn::EnableDigitizer(bool bEnable)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (bEnable)
	{
    CTestDialog dialog(CWnd::FromHandle(RhinoApp().MainWnd()));
    INT_PTR nResult = dialog.DoModal();
    if (nResult != IDOK)
      return false;

    CTestRawInputWindow::CreateWnd(dialog.m_hDevice);
	}
	else
	{
    CTestRawInputWindow::DestroyWnd();
	}

	return true;
}

ON::LengthUnitSystem CTestDigitizerPlugIn::UnitSystem() const
{
	return ON::LengthUnitSystem::Millimeters;
}

double CTestDigitizerPlugIn::PointTolerance() const
{
	return 0.01;
}

