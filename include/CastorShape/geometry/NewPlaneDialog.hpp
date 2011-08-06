#ifndef ___CS_NEWPLANEDIALOG___
#define ___CS_NEWPLANEDIALOG___

#include "Module_Geometry.hpp"
#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewPlaneDialog : public NewGeometryDialog
	{
	public:
		NewPlaneDialog( wxWindow * parent, wxWindowID p_id);
		~NewPlaneDialog();

		real GetGeometryWidth()const;
		real GetGeometryDepth()const;
		int GetNbWidthSubdiv()const;
		int GetNbDepthSubdiv()const;
		String GetNbWidthSubdivStr()const;
		String GetNbDepthSubdivStr()const;

	private:
		virtual void _initialise();
		DECLARE_EVENT_TABLE()
	};
}

#endif
