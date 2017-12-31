/* See LICENSE file in root folder */
#ifndef ___CS_NEW_CUBE_DIALOG_H___
#define ___CS_NEW_CUBE_DIALOG_H___

#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewCubeDialog
		: public NewGeometryDialog
	{
	public:
		NewCubeDialog( Castor3D::Scene & p_scene, wxWindow * parent, wxWindowID p_id );
		~NewCubeDialog();

		Castor::real GetWidth()const;
		Castor::real GetHeight()const;
		Castor::real GetDepth()const;

	private:
		virtual void DoCreateProperties();
	};
}

#endif
