#ifndef ___CS_NEWCONEDIALOG___
#define ___CS_NEWCONEDIALOG___

#include "Module_Geometry.h"
#include "NewGeometryDialog.h"

namespace CastorShape
{
	class NewConeDialog : public NewGeometryDialog
	{
	public:
		NewConeDialog( Castor3D::MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id);
		~NewConeDialog();

		real GetConeRadius()const;
		real GetConeHeight()const;
		int GetFacesNumber()const;
		String GetFacesNumberStr()const;

	private:
		virtual void _initialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
