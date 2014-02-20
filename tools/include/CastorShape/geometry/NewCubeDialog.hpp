#ifndef ___CS_NEWCUBEDIALOG___
#define ___CS_NEWCUBEDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewCubeDialog : public NewGeometryDialog
	{
	public:
		NewCubeDialog( Castor3D::Engine * p_pEngine, wxWindow * parent, wxWindowID p_id);
		~NewCubeDialog();

		Castor::real GetCubeWidth()const;
		Castor::real GetCubeHeight()const;
		Castor::real GetCubeDepth()const;

	private:
		virtual void DoInitialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
