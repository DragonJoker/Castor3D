#ifndef ___CS_NEWCONEDIALOG___
#define ___CS_NEWCONEDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewConeDialog : public NewGeometryDialog
	{
	public:
		NewConeDialog( wxWindow * parent, wxWindowID p_id);
		~NewConeDialog();

		real GetConeRadius()const;
		real GetConeHeight()const;
		int GetFacesNumber()const;
		String GetFacesNumberStr()const;

	private:
		virtual void _initialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
