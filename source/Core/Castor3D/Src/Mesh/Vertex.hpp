/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshVertex_H___
#define ___C3D_SubmeshVertex_H___

#include "Castor3DPrerequisites.hpp"

#include <Math/Point.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.11.0
	\date		17/01/2018
	\~english
	\brief		Representation of a vertex inside a submesh.
	\~fench
	\brief		Représentation d'un sommet, dans un sous-maillage.
	*/
	struct SubmeshVertex
	{
		castor::Point3f m_position;
		castor::Point3f m_normal;
		castor::Point3f m_tangent;
		castor::Point3f m_bitangent;
		castor::Point3f m_texture;
	};
}

#endif
