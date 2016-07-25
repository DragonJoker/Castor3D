/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
	struct VertexBoneData
	{
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API VertexBoneData();
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
		std::array< uint32_t, C3D_MAX_BONES_PER_VERTEX > m_ids;
		//!\~english The bones weights	\~french Les poids des bones
		std::array< real, C3D_MAX_BONES_PER_VERTEX > m_weights;
	};
}

#endif
