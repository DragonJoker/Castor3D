/* See LICENSE file in root folder */
#ifndef ___CS_NEW_ICOSAEDRON_DIALOG_H___
#define ___CS_NEW_ICOSAEDRON_DIALOG_H___

#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewIcosahedronDialog
		: public NewGeometryDialog
	{
	public:
		NewIcosahedronDialog( Castor3D::Scene & p_scene, wxWindow * parent, wxWindowID p_id );
		~NewIcosahedronDialog();

		Castor::real GetRadius()const;
		int GetSubdivisionCount()const;

	private:
		virtual void DoCreateProperties();
	};
}

#endif
