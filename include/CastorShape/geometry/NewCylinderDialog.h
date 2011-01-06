#ifndef ___CS_NEWCYLINDERDIALOG___
#define ___CS_NEWCYLINDERDIALOG___

#include "Module_Geometry.h"
#include "NewGeometryDialog.h"

namespace CastorShape
{
	class NewCylinderDialog : public NewGeometryDialog
	{
	public:
		NewCylinderDialog( Castor3D::MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id);
		~NewCylinderDialog();

		real GetCylinderRadius()const;
		real GetCylinderHeight()const;
		int GetFacesNumber()const;
		String	GetFacesNumberStr()const;

	private:
		virtual void _initialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
