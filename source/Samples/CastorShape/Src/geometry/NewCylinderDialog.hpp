#ifndef ___CS_NEWCYLINDERDIALOG___
#define ___CS_NEWCYLINDERDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewCylinderDialog : public NewGeometryDialog
	{
	public:
		NewCylinderDialog( Castor3D::Engine * p_pEngine, wxWindow * parent, wxWindowID p_id );
		~NewCylinderDialog();

		Castor::real GetCylinderRadius()const;
		Castor::real GetCylinderHeight()const;
		int GetFacesNumber()const;
		Castor::String	GetFacesNumberStr()const;

	private:
		virtual void DoInitialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
