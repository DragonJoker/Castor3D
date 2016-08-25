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
#ifndef ___C3D_BUFFER_ELEMENT_DECLARATION_H___
#define ___C3D_BUFFER_ELEMENT_DECLARATION_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Buffer element description.
	\remark		Describes usage and type of an element in a vertex buffer.
	\~french
	\brief		Description d'un élément de tampon.
	\remark		Décrit l'utilisation et le type d'un élément de tampon de sommets.
	*/
	struct BufferElementDeclaration
	{
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		BufferElementDeclaration()
			: m_dataType()
			, m_offset()
			, m_name()
			, m_usages( 0u )
		{
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name		The associated variable name.
		 *\param[in]	p_usages	Element possible usages (ElementUsage combination).
		 *\param[in]	p_type		Element type.
		 *\param[in]	p_offset	Offset in the stream.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name		Le nom de la variable associée.
		 *\param[in]	p_usages	Les utilisations possibles de l'élément (combinaison de ElementUsage)
		 *\param[in]	p_type		Type de l'élément.
		 *\param[in]	p_offset	Offset dans le tampon.
		 */
		BufferElementDeclaration( Castor::String const & p_name, uint32_t p_usages, ElementType p_type, uint32_t p_offset = 0 )
			: m_dataType( p_type )
			, m_offset( p_offset )
			, m_name( p_name )
			, m_usages( p_usages )
		{
		}

		//!\~english The associated variable name.	\~french Le nom de la variable associée.
		Castor::String m_name;
		//!\~english Element usage.	\~french Utilisation de l'élément.
		uint32_t m_usages;
		//!\~english Element type.	\~french Type de l'élément.
		ElementType m_dataType;
		//!\~english Offset in buffer.	\~french Offset dans le tampon.
		uint32_t m_offset;
	};
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	p_lhs, p_rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les 2 objets à comparer
	 */
	inline bool operator==( BufferElementDeclaration const & p_lhs, BufferElementDeclaration const & p_rhs )
	{
		return p_lhs.m_dataType == p_rhs.m_dataType
			   && p_lhs.m_name == p_rhs.m_name
			   && p_lhs.m_offset == p_rhs.m_offset
			   && p_lhs.m_usages == p_rhs.m_usages;
	}
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	p_lhs, p_rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les 2 objets à comparer
	 */
	inline bool operator!=( BufferElementDeclaration const & p_lhs, BufferElementDeclaration const & p_rhs )
	{
		return p_lhs.m_dataType != p_rhs.m_dataType
			   || p_lhs.m_name != p_rhs.m_name
			   || p_lhs.m_offset != p_rhs.m_offset
			   || p_lhs.m_usages != p_rhs.m_usages;
	}
}

#endif
