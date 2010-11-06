//******************************************************************************
#ifndef ___CS_NEWTORUSDIALOG___
#define ___CS_NEWTORUSDIALOG___
//******************************************************************************
#include "Module_Geometry.h"
#include "NewGeometryDialog.h"
//******************************************************************************
class CSNewTorusDialog : public CSNewGeometryDialog
{
public:
	CSNewTorusDialog( wxWindow * parent, wxWindowID p_id);
	~CSNewTorusDialog();

	real GetInternalRadius()const;
	real GetExternalRadius()const;
	int GetInternalNbFaces()const;
	int GetExternalNbFaces()const;
	String GetInternalNbFacesStr()const;
	String GetExternalNbFacesStr()const;
};
//******************************************************************************
#endif
//******************************************************************************
