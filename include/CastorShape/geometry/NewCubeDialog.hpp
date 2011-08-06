#ifndef ___CS_NEWCUBEDIALOG___
#define ___CS_NEWCUBEDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewCubeDialog : public NewGeometryDialog
	{
	public:
		NewCubeDialog( wxWindow * parent, wxWindowID p_id);
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
