// dllmain.h : Declaration of module class.

class CSampleModuleModule : public ATL::CAtlDllModuleT< CSampleModuleModule >
{
public :
	DECLARE_LIBID(LIBID_SampleModuleLib)
};

extern class CSampleModuleModule _AtlModule;
