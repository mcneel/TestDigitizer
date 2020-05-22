#pragma once

class CTestDigitizerPlugIn : public CRhinoDigitizerPlugIn
{
public:
  CTestDigitizerPlugIn();
  ~CTestDigitizerPlugIn() = default;

  // Required overrides
  const wchar_t* PlugInName() const override;
  const wchar_t* PlugInVersion() const override;
  GUID PlugInID() const override;
  
  // Additional overrides
  BOOL OnLoadPlugIn() override;
  void OnUnloadPlugIn() override;

  // Digitizer overrides
  bool EnableDigitizer(bool bEnable) override;
  ON::LengthUnitSystem UnitSystem() const override;
  double PointTolerance() const override;

private:
  ON_wString m_plugin_version;
};

CTestDigitizerPlugIn& TestDigitizerPlugIn();



