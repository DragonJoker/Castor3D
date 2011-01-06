#ifndef ___CS_NEWCUBEDIALOG___
#define ___CS_NEWCUBEDIALOG___

#include "Module_Geometry.h"
#include "NewGeometryDialog.h"

namespace CastorShape
{
	class NewCubeDialog : public NewGeometryDialog
	{
	public:
		NewCubeDialog( Castor3D::MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id);
		~NewCubeDialog();

		real GetCubeWidth()const;
		real GetCubeHeight()const;
		real GetCubeDepth()const;

	private:
		virtual void _initialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
