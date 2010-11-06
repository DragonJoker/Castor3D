
//******************************************************************************
#ifndef ___CS_NEWSPHEREDIALOG___
#define ___CS_NEWSPHEREDIALOG___
//******************************************************************************
#include "Module_Geometry.h"
#include "NewGeometryDialog.h"
//******************************************************************************
class CSNewSphereDialog : public CSNewGeometryDialog
{
public:
	CSNewSphereDialog( wxWindow * parent, wxWindowID p_id);
	~CSNewSphereDialog();
	
	real GetSphereRadius()const;
	int GetFacesNumber()const;
	String GetFacesNumberStr()const;
};
//******************************************************************************
#endif
//******************************************************************************
