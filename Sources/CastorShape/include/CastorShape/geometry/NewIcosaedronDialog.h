//******************************************************************************
#ifndef ___CS_NEWICOSAEDRONDIALOG___
#define ___CS_NEWICOSAEDRONDIALOG___
//******************************************************************************
#include "Module_Geometry.h"
#include "NewGeometryDialog.h"
//******************************************************************************
class CSNewIcosaedronDialog : public CSNewGeometryDialog
{
public:
	CSNewIcosaedronDialog( wxWindow * parent, wxWindowID p_id);
	~CSNewIcosaedronDialog();
	
	real GetIcosaedronRadius()const;
	int GetNbSubdiv()const;
	String GetNbSubdivStr()const;
};
//******************************************************************************
#endif
//******************************************************************************
