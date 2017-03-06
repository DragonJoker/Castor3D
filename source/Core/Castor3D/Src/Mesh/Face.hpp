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
#ifndef ___C3D_FACE_H___
#define ___C3D_FACE_H___

#include "FaceIndices.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Face implementation
	\remark		A face is constituted from 3 vertices
	\~french
	\brief		Implémentation d'un face
	\remark		Une face est constituée de 3 vertices
	*/
	class Face
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	a, b, c	The three vertices indices
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	a, b, c	Les indices des 3 vertices
		 */
		C3D_API Face( uint32_t a, uint32_t b, uint32_t c );
		/**
		 *\~english
		 *\brief		Retrieves the vertex index
		 *\param[in]	p_index	The index of the concerned vertex
		 *\return		The vertex index
		 *\~french
		 *\brief		Récupère l'indice du vertex voulu
		 *\param[in]	p_index	L'index du vertex concerné
		 *\return		L'indice
		 */
		inline uint32_t operator[]( uint32_t p_index )const
		{
			REQUIRE( p_index < 3 );
			return m_face.m_index[p_index];
		}

	private:
		//!\~english The face vertex indices	\~french Les indices des sommets de la face
		FaceIndices m_face;
	};
}

#endif
