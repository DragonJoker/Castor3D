#ifndef ___CS_NEWSPHEREDIALOG___
#define ___CS_NEWSPHEREDIALOG___

#include "Module_Geometry.h"
#include "NewGeometryDialog.h"

namespace CastorShape
{
	class NewSphereDialog : public NewGeometryDialog
	{
	public:
		NewSphereDialog( Castor3D::MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id);
		~NewSphereDialog();
		
		real GetSphereRadius()const;
		int GetFacesNumber()const;
		String GetFacesNumberStr()const;

	protected:
		virtual void _initialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
