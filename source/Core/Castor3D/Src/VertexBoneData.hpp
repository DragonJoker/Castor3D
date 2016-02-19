/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___C3D_VERTEX_BONE_DATA_H___
#define ___C3D_VERTEX_BONE_DATA_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	static const uint32_t C3D_MAX_BONES_PER_VERTEX = 8;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		26/06/2013
	\~english
	\brief		Holds bone data for one vertex
	\~french
	\brief		Contient les données de bones pour un vertice
	*/
	struct stVERTEX_BONE_DATA
	{
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API stVERTEX_BONE_DATA();
		/**
		 *\~english
		 *\brief		Adds bone informations to the vertex
		 *\param[in]	p_boneId	The bone ID
		 *\param[in]	p_weight	The bone weight
		 *\~french
		 *\brief		Ajoute des informations de bone au vertice
		 *\param[in]	p_boneId	L'ID du bone
		 *\param[in]	p_weight	Le poids du bone
		 */
		C3D_API void AddBoneData( uint32_t p_boneId, real p_weight );

		//!\~english The bones ID	\~french L'ID des bones
		uint32_t m_ids[C3D_MAX_BONES_PER_VERTEX];
		//!\~english The bones weights	\~french Les poids des bones
		real m_weights[C3D_MAX_BONES_PER_VERTEX];
	};
}

#endif
