/* See LICENSE file in root folder */
#ifndef ___CS_NEW_SPHERE_DIALOG_H___
#define ___CS_NEW_SPHERE_DIALOG_H___

#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewSphereDialog
		: public NewGeometryDialog
	{
	public:
		NewSphereDialog( Castor3D::Scene & p_scene, wxWindow * parent, wxWindowID p_id );
		~NewSphereDialog();

		Castor::real GetRadius()const;
		int GetFacesCount()const;

	protected:
		virtual void DoCreateProperties();
	};
}

#endif
