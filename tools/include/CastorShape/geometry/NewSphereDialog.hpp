#ifndef ___CS_NEWSPHEREDIALOG___
#define ___CS_NEWSPHEREDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewSphereDialog : public NewGeometryDialog
	{
	public:
		NewSphereDialog( Castor3D::Engine * p_pEngine, wxWindow * parent, wxWindowID p_id);
		~NewSphereDialog();
		
		Castor::real GetSphereRadius()const;
		int GetFacesNumber()const;
		Castor::String GetFacesNumberStr()const;

	protected:
		virtual void DoInitialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
