#ifndef ___CS_NEWCYLINDERDIALOG___
#define ___CS_NEWCYLINDERDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewCylinderDialog : public NewGeometryDialog
	{
	public:
		NewCylinderDialog( wxWindow * parent, wxWindowID p_id);
		~NewCylinderDialog();

		real GetCylinderRadius()const;
		real GetCylinderHeight()const;
		int GetFacesNumber()const;
		String	GetFacesNumberStr()const;

	private:
		virtual void _initialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
