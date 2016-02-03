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
#ifndef ___C3D_VERTEX_LAYOUT_H___
#define ___C3D_VERTEX_LAYOUT_H___

#include "Castor3DPrerequisites.hpp"

#include "BufferElementDeclaration.hpp"
#include "BufferDeclaration.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		03/02/2016
	\~english
	\brief		VertexLayout class.
	\remark		Holds the vertex buffer declaration, initialised by the shader program when compiled.
	\~french
	\brief		Classe VertexLayout.
	\remark		Contient la déclaration des éléments du tampon de sommets, initialisé par le programme shader à la compilation.
	*/
	class VertexLayout
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API VertexLayout();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~VertexLayout();
		/**
		 *\~english
		 *\brief		Gets buffer declaration.
		 *\param[in]	p_name		Associated variable name.
		 *\param[in]	p_type		Element type.
		 *\param[in]	p_offset	Offset in the stream.
		 *\~french
		 *\brief		Récupère la déclaration du tampon.
		 *\param[in]	p_name		Nom de la variable associée.
		 *\param[in]	p_type		Type de l'élément.
		 *\param[in]	p_offset	Offset dans le tampon.
		 */
		C3D_API void AddAttribute( Castor::String const & p_name, eELEMENT_TYPE p_type, uint32_t p_offset = 0 );
		/**
		 *\~english
		 *\brief		Gets buffer declaration.
		 *\return		The buffer declaration.
		 *\~french
		 *\brief		Récupère la déclaration du tampon.
		 *\return		La déclaration du tampon.
		 */
		C3D_API BufferDeclaration CreateDeclaration()const;

	private:
		//!\~english Buffer elements description.	\~french Description des élément du tampon.
		std::vector< BufferElementDeclaration > m_bufferLayout;
	};
}

#endif
