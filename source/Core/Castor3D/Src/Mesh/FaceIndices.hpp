/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_FACE_INDICES_H___
#define ___C3D_FACE_INDICES_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		23/07/2012
	\~english
	\brief		Holds the 3 vertex indices
	\~french
	\brief		Contient simplement les index des 3 vertex d'une face
	*/
	struct stFACE_INDICES
	{
		//!\~english The indices	\~french Les indices
		uint32_t m_uiVertexIndex[3];
	};
}

#endif
