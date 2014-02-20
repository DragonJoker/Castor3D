#ifndef ___CS_NEWTORUSDIALOG___
#define ___CS_NEWTORUSDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewTorusDialog : public NewGeometryDialog
	{
	public:
		NewTorusDialog( Castor3D::Engine * p_pEngine, wxWindow * parent, wxWindowID p_id);
		~NewTorusDialog();

		Castor::real GetInternalRadius()const;
		Castor::real GetExternalRadius()const;
		int GetInternalNbFaces()const;
		int GetExternalNbFaces()const;
		Castor::String GetInternalNbFacesStr()const;
		Castor::String GetExternalNbFacesStr()const;

	private:
		virtual void DoInitialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
