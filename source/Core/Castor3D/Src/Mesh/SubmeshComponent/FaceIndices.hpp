/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FACE_INDICES_H___
#define ___C3D_FACE_INDICES_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		23/07/2012
	\~english
	\brief		Holds the 3 vertex indices
	\~french
	\brief		Contient simplement les index des 3 vertex d'une face
	*/
	struct FaceIndices
	{
		//!\~english	The indices.
		//!\~french		Les indices.
		std::array< uint32_t, 3 > m_index;
	};
}

#endif
