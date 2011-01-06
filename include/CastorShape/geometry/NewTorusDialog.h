#ifndef ___CS_NEWTORUSDIALOG___
#define ___CS_NEWTORUSDIALOG___

#include "Module_Geometry.h"
#include "NewGeometryDialog.h"

namespace CastorShape
{
	class NewTorusDialog : public NewGeometryDialog
	{
	public:
		NewTorusDialog( Castor3D::MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id);
		~NewTorusDialog();

		real GetInternalRadius()const;
		real GetExternalRadius()const;
		int GetInternalNbFaces()const;
		int GetExternalNbFaces()const;
		String GetInternalNbFacesStr()const;
		String GetExternalNbFacesStr()const;

	private:
		virtual void _initialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
