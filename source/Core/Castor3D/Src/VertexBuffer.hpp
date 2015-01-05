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
#ifndef ___C3D_VERTEX_BUFFER_H___
#define ___C3D_VERTEX_BUFFER_H___

#include "CpuBuffer.hpp"
#include "BufferElementDeclaration.hpp"
#include "BufferDeclaration.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API VertexBuffer
		:	public CpuBuffer< uint8_t >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\param[in]	p_pElements		The elements descriptions
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\param[in]	p_pElements		Les descriptions des éléments
		 */
		template< uint32_t N >
		VertexBuffer( RenderSystem * p_pRenderSystem, BufferElementDeclaration const( & p_pElements )[N] )
			:	CpuBuffer< uint8_t >( p_pRenderSystem )
			,	m_bufferDeclaration( p_pElements, N )
		{
		}
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\param[in]	p_pElements		The elements descriptions
		 *\param[in]	p_uiNbElements	The elements descriptions count
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\param[in]	p_pElements		Les descriptions des éléments
		 *\param[in]	p_uiNbElements	Le compte des descriptions des éléments
		 */
		VertexBuffer( RenderSystem * p_pRenderSystem, BufferElementDeclaration const * p_pElements, uint32_t p_uiNbElements );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~VertexBuffer();
		/**
		 *\~english
		 *\brief		Gets buffer declaration
		 *\return		The buffer declaration
		 *\~french
		 *\brief		Récupère la déclaration du tampon
		 *\return		La déclaration du tampon
		 */
		BufferDeclaration const & GetDeclaration()const
		{
			return m_bufferDeclaration;
		}

	private:
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\~french
		 *\brief		Fonction d'initialisation
		 */
		virtual bool DoCreateBuffer();

	protected:
		//!\~english Buffer elements description	\~french Description des élément du tampon
		BufferDeclaration m_bufferDeclaration;
	};
}

#pragma warning( pop )

#endif
