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
#ifndef ___C3D_VERTEX_GROUP_H___
#define ___C3D_VERTEX_GROUP_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		23/07/2012
	\~english
	\brief		Holds the all components of a vertex array
	\~french
	\brief		Contient toutes les composantes d'un tableau de vertex
	*/
	struct stVERTEX_GROUP
	{
		//!\~english The vertices count	\~french Le nombre de vertices
		uint32_t m_uiCount;
		//!\~english The vertices coordinates	\~french Les coordonnées des vertices
		real * m_pVtx;
		//!\~english The vertices normals	\~french Les normales des vertices
		real * m_pNml;
		//!\~english The vertices tangents	\~french Les tangentes des vertices
		real * m_pTan;
		//!\~english The vertices bitangents	\~french Les bitangentes des vertices
		real * m_pBin;
		//!\~english The vertices texture coordinates	\~french Les coordonnées de texture des vertices
		real * m_pTex;
	};
}

#endif
