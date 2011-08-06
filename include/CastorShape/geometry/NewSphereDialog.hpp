#ifndef ___CS_NEWSPHEREDIALOG___
#define ___CS_NEWSPHEREDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewSphereDialog : public NewGeometryDialog
	{
	public:
		NewSphereDialog( wxWindow * parent, wxWindowID p_id);
		~NewSphereDialog();
		
		real GetSphereRadius()const;
		int GetFacesNumber()const;
		String GetFacesNumberStr()const;

	protected:
		virtual void _initialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
