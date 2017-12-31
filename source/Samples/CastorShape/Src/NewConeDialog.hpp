/* See LICENSE file in root folder */
#ifndef ___CS_NEW_CONE_DIALOG_H___
#define ___CS_NEW_CONE_DIALOG_H___

#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewConeDialog
		: public NewGeometryDialog
	{
	public:
		NewConeDialog( Castor3D::Scene & p_scene, wxWindow * parent, wxWindowID p_id );
		~NewConeDialog();

		Castor::real GetRadius()const;
		Castor::real GetHeight()const;
		int GetFacesCount()const;

	private:
		virtual void DoCreateProperties();
	};
}

#endif
