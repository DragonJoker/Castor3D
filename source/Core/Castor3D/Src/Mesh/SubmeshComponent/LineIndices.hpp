/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LineIndices_H___
#define ___C3D_LineIndices_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		12/11/2017
	\~english
	\brief		Holds the 2 vertex indices of a ligne.
	\~french
	\brief		Contient les index des 2 sommets d'une ligne.
	*/
	struct LineIndices
	{
		//!\~english	The indices.
		//!\~french		Les indices.
		std::array< uint32_t, 2 > m_index;
	};
}

#endif
