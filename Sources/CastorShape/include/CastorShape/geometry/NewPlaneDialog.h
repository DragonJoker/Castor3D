//******************************************************************************
#ifndef ___CS_NEWPLANEDIALOG___
#define ___CS_NEWPLANEDIALOG___
//******************************************************************************
#include "Module_Geometry.h"
#include "NewGeometryDialog.h"
//******************************************************************************
class CSNewPlaneDialog : public CSNewGeometryDialog
{
public:
	CSNewPlaneDialog( wxWindow * parent, wxWindowID p_id);
	~CSNewPlaneDialog();

	real GetGeometryWidth()const;
	real GetGeometryDepth()const;
	int GetNbWidthSubdiv()const;
	int GetNbDepthSubdiv()const;
	String GetNbWidthSubdivStr()const;
	String GetNbDepthSubdivStr()const;
};
//******************************************************************************
#endif
//******************************************************************************
