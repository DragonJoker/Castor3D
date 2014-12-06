#ifndef ___CS_NEWPLANEDIALOG___
#define ___CS_NEWPLANEDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewPlaneDialog : public NewGeometryDialog
	{
	public:
		NewPlaneDialog( Castor3D::Engine * p_pEngine, wxWindow * parent, wxWindowID p_id );
		~NewPlaneDialog();

		Castor::real GetGeometryWidth()const;
		Castor::real GetGeometryDepth()const;
		int GetNbWidthSubdiv()const;
		int GetNbDepthSubdiv()const;
		Castor::String GetNbWidthSubdivStr()const;
		Castor::String GetNbDepthSubdivStr()const;

	private:
		virtual void DoInitialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
