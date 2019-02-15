/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshVertex_H___
#define ___C3D_SubmeshVertex_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <CastorUtils/Math/Point.hpp>

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
		uint32_t m_index;
		InterleavedVertex m_vertex;
	};
}

#endif
