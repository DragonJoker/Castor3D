//******************************************************************************
#ifndef ___CS_NEWCONEDIALOG___
#define ___CS_NEWCONEDIALOG___
//******************************************************************************
#include "Module_Geometry.h"
#include "NewGeometryDialog.h"
//******************************************************************************
class CSNewConeDialog : public CSNewGeometryDialog
{
public:
	CSNewConeDialog( wxWindow * parent, wxWindowID p_id);
	~CSNewConeDialog();

	real GetConeRadius()const;
	real GetConeHeight()const;
	int GetFacesNumber()const;
	String GetFacesNumberStr()const;
};
//******************************************************************************
#endif
//******************************************************************************
