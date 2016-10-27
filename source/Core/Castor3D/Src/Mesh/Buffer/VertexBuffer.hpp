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
#ifndef ___C3D_VERTEX_BUFFER_H___
#define ___C3D_VERTEX_BUFFER_H___

#include "CpuBuffer.hpp"
#include "BufferElementDeclaration.hpp"
#include "BufferDeclaration.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Vertex buffer representation
	\remark		Holds the submesh vertices informations
	\~french
	\brief		Représentation d'un tampon de sommets
	\remark		Contient les informations des sommets d'un submesh
	*/
	class VertexBuffer
		: public CpuBuffer< uint8_t >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_declaration	The buffer declaration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_declaration	La déclaration du tampon.
		 */
		C3D_API VertexBuffer( Engine & p_engine, BufferDeclaration const & p_declaration );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~VertexBuffer();
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Create();
		/**
		 *\~english
		 *\brief		Gets buffer declaration
		 *\return		The buffer declaration
		 *\~french
		 *\brief		Récupère la déclaration du tampon
		 *\return		La déclaration du tampon
		 */
		inline BufferDeclaration const & GetDeclaration()const
		{
			return m_bufferDeclaration;
		}
		/**
		 *\~english
		 *\brief		Link all elements to this buffer
		 *\param[in]	p_begin, p_end	The elements array iterators
		 *\~french
		 *\brief		Lie tous les éméents à ce tampon
		 *\param[in]	p_begin, p_end	Les itérateurs du tableau d'éléments
		 */
		template< typename ItType >
		inline void LinkCoords( ItType p_begin, ItType p_end )
		{
			uint32_t l_stride = m_bufferDeclaration.stride();
			uint8_t * l_buffer = CpuBuffer< uint8_t >::data();

			for ( auto l_it = p_begin; l_it != p_end; ++l_it )
			{
				( *l_it )->LinkCoords( l_buffer, l_stride );
				l_buffer += l_stride;
			}
		}

	protected:
		//!\~english	Buffer elements description.
		//!\~french		Description des élément du tampon.
		BufferDeclaration m_bufferDeclaration;
	};
}

#endif
