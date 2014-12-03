#ifndef ___CS_NEWCONEDIALOG___
#define ___CS_NEWCONEDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewConeDialog : public NewGeometryDialog
	{
	public:
		NewConeDialog( Castor3D::Engine * p_pEngine, wxWindow * parent, wxWindowID p_id );
		~NewConeDialog();

		Castor::real GetConeRadius()const;
		Castor::real GetConeHeight()const;
		int GetFacesNumber()const;
		Castor::String GetFacesNumberStr()const;

	private:
		virtual void DoInitialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
