#ifndef ___CS_NEWICOSAEDRONDIALOG___
#define ___CS_NEWICOSAEDRONDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewIcosahedronDialog : public NewGeometryDialog
	{
	public:
		NewIcosahedronDialog( Castor3D::Engine * p_pEngine, wxWindow * parent, wxWindowID p_id );
		~NewIcosahedronDialog();

		Castor::real GetIcosahedronRadius()const;
		int GetNbSubdiv()const;
		Castor::String GetNbSubdivStr()const;

	private:
		virtual void DoInitialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
