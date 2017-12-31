/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BUFFER_ELEMENT_DECLARATION_H___
#define ___C3D_BUFFER_ELEMENT_DECLARATION_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
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
			, m_divisor( 0u )
		{
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name		The associated variable name.
		 *\param[in]	p_usages	Element possible usages (ElementUsage combination).
		 *\param[in]	p_type		Element type.
		 *\param[in]	p_offset	Offset in the stream.
		 *\param[in]	p_divisor	Instanciation attribute increment.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name		Le nom de la variable associée.
		 *\param[in]	p_usages	Les utilisations possibles de l'élément (combinaison de ElementUsage)
		 *\param[in]	p_type		Type de l'élément.
		 *\param[in]	p_offset	Offset dans le tampon.
		 *\param[in]	p_divisor	Incrément d'attribut d'instanciation.
		 */
		BufferElementDeclaration( castor::String const & p_name, castor::FlagCombination< ElementUsage > const & p_usages, ElementType p_type, uint32_t p_offset = 0u, uint32_t p_divisor = 0u )
			: m_dataType( p_type )
			, m_offset( p_offset )
			, m_name( p_name )
			, m_usages( p_usages )
			, m_divisor( p_divisor )
		{
		}

		//!\~english	The associated variable name.
		//!\~french		Le nom de la variable associée.
		castor::String m_name;
		//!\~english	Element usage.
		//!\~french		Utilisation de l'élément.
		castor::FlagCombination< ElementUsage > m_usages;
		//!\~english	Element type.
		//!\~french		Type de l'élément.
		ElementType m_dataType;
		//!\~english	Offset in buffer.
		//!\~french		Offset dans le tampon.
		uint32_t m_offset;
		//!\~english	Instantiation attribute increment.
		//!\~french		Incrément d'attribut d'instanciation.
		uint32_t m_divisor;
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
			   && p_lhs.m_usages == p_rhs.m_usages
			   && p_lhs.m_divisor == p_rhs.m_divisor;
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
			   || p_lhs.m_usages != p_rhs.m_usages
			   || p_lhs.m_divisor != p_rhs.m_divisor;
	}
}

#endif
