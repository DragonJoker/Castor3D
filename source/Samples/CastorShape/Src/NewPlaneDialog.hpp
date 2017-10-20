/* See LICENSE file in root folder */
#ifndef ___CS_NEW_PLANE_DIALOG_H___
#define ___CS_NEW_PLANE_DIALOG_H___

#include "NewGeometryDialog.hpp"

namespace CastorShape
{
	class NewPlaneDialog
		: public NewGeometryDialog
	{
	public:
		NewPlaneDialog( Castor3D::Scene & p_scene, wxWindow * parent, wxWindowID p_id );
		~NewPlaneDialog();

		Castor::real GetWidth()const;
		Castor::real GetDepth()const;
		int GetWidthSubdivisionCount()const;
		int GetDepthSubdivisionCount()const;

	private:
		virtual void DoCreateProperties();
	};
}

#endif
