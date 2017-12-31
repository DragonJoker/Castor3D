/* See LICENSE file in root folder */
#ifndef ___CS_NEW_CYLINDER_DIALOG_H___
#define ___CS_NEW_CYLINDER_DIALOG_H___

#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewCylinderDialog : public NewGeometryDialog
	{
	public:
		NewCylinderDialog( Castor3D::Scene & p_scene, wxWindow * parent, wxWindowID p_id );
		~NewCylinderDialog();

		Castor::real GetRadius()const;
		Castor::real GetHeight()const;
		int GetFacesCount()const;

	private:
		virtual void DoCreateProperties();
	};
}

#endif
