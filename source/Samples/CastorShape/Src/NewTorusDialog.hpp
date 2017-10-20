/* See LICENSE file in root folder */
#ifndef ___CS_NEW_TORUS_DIALOG_H___
#define ___CS_NEW_TORUS_DIALOG_H___

#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewTorusDialog : public NewGeometryDialog
	{
	public:
		NewTorusDialog( Castor3D::Scene & p_scene, wxWindow * parent, wxWindowID p_id );
		~NewTorusDialog();

		Castor::real GetInternalRadius()const;
		Castor::real GetExternalRadius()const;
		int GetInternalFacesCount()const;
		int GetExternalFacesCount()const;

	private:
		virtual void DoCreateProperties();
	};
}

#endif
