#ifndef ___CS_NEWTORUSDIALOG___
#define ___CS_NEWTORUSDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewTorusDialog : public NewGeometryDialog
	{
	public:
		NewTorusDialog( wxWindow * parent, wxWindowID p_id);
		~NewTorusDialog();

		real GetInternalRadius()const;
		real GetExternalRadius()const;
		int GetInternalNbFaces()const;
		int GetExternalNbFaces()const;
		String GetInternalNbFacesStr()const;
		String GetExternalNbFacesStr()const;

	private:
		virtual void _initialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
