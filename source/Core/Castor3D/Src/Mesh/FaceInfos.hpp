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
#ifndef ___C3D_FACE_INFOS_H___
#define ___C3D_FACE_INFOS_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/07/2012
	\~english
	\brief		Holds face informations.
	\~french
	\brief		Contient des informations sur une face.
	*/
	struct stFACE_INFOS
	{
		/*!
		\author 	Sylvain DOREMUS
		\date 		20/07/2012
		\~english
		\brief		Holds vertex informations.
		\~french
		\brief		Contient les informations d'un sommet.
		\remark
		*/
		struct stVERTEX
		{
			//!\~english	Vertex index.
			//!\~french		Indice du sommet.
			int m_index{ -1 };
			//!\~english	Vertex normal.
			//!\~french		Normale du sommet.
		  std::array< real, 3 > m_fNormal{ { 0.0_r, 0.0_r, 0.0_r } };
			//!\~english	Vertex UV.
			//!\~french		UV du sommet.
			std::array< real, 2 > m_fTexCoords{ { 0.0_r, 0.0_r } };
			//!\~english	Tells the vertex has a normal.
			//!\~french		Dit que le sommet a une normale.
			bool m_bHasNormal{ false };
			//!\~english	Tells the vertex has UV.
			//!\~french		Dit que le sommet a un UV.
			bool m_bHasTexCoords{ false };
		};
		//!\~english	The 3 vertex informations.
		//!\~french		Les informations des 3 sommets.
		std::array< stVERTEX, 3 > m_vertex;
	};
}

#endif
