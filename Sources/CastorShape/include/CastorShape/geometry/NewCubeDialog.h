//******************************************************************************
#ifndef ___CS_NEWCUBEDIALOG___
#define ___CS_NEWCUBEDIALOG___
//******************************************************************************
#include "Module_Geometry.h"
#include "NewGeometryDialog.h"
//******************************************************************************
class CSNewCubeDialog : public CSNewGeometryDialog
{
public:
	CSNewCubeDialog( wxWindow * parent, wxWindowID p_id);
	~CSNewCubeDialog();

	real GetCubeWidth()const;
	real GetCubeHeight()const;
	real GetCubeDepth()const;
};
//******************************************************************************
#endif
//******************************************************************************
