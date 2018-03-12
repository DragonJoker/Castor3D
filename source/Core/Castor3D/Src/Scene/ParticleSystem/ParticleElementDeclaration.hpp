/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ParticleElementDeclaration_H___
#define ___C3D_ParticleElementDeclaration_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Particle element description.
	\remark		Describes usage and type of an element in a particle.
	\~french
	\brief		Description d'un élément de particule.
	\remark		Décrit l'utilisation et le type d'un élément de particule.
	*/
	struct ParticleElementDeclaration
	{
		C3D_API ParticleElementDeclaration() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name	The associated variable name.
		 *\param[in]	usages	Element possible usages (ElementUsage combination).
		 *\param[in]	type	Element type.
		 *\param[in]	offset	Offset in the stream.
		 *\param[in]	divisor	Instanciation attribute increment.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name	Le nom de la variable associée.
		 *\param[in]	usages	Les utilisations possibles de l'élément (combinaison de ElementUsage)
		 *\param[in]	type	Type de l'élément.
		 *\param[in]	offset	Offset dans le tampon.
		 *\param[in]	divisor	Incrément d'attribut d'instanciation.
		 */
		inline ParticleElementDeclaration( castor::String const & name
			, castor::FlagCombination< ElementUsage > const & usages
			, ParticleFormat type
			, uint32_t offset = 0u
			, uint32_t divisor = 0u )
			: m_dataType( type )
			, m_offset( offset )
			, m_name( name )
			, m_usages( usages )
			, m_divisor( divisor )
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
		ParticleFormat m_dataType{ ParticleFormat::eVec4f };
		//!\~english	Offset in buffer.
		//!\~french		Offset dans le tampon.
		uint32_t m_offset{ 0u };
		//!\~english	Instantiation attribute increment.
		//!\~french		Incrément d'attribut d'instanciation.
		uint32_t m_divisor{ 0u };
	};
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	lhs, rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	lhs, rhs	Les 2 objets à comparer
	 */
	inline bool operator==( ParticleElementDeclaration const & lhs, ParticleElementDeclaration const & rhs )
	{
		return lhs.m_dataType == rhs.m_dataType
			&& lhs.m_name == rhs.m_name
			&& lhs.m_offset == rhs.m_offset
			&& lhs.m_usages == rhs.m_usages
			&& lhs.m_divisor == rhs.m_divisor;
	}
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	lhs, rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	lhs, rhs	Les 2 objets à comparer
	 */
	inline bool operator!=( ParticleElementDeclaration const & lhs, ParticleElementDeclaration const & rhs )
	{
		return lhs.m_dataType != rhs.m_dataType
			|| lhs.m_name != rhs.m_name
			|| lhs.m_offset != rhs.m_offset
			|| lhs.m_usages != rhs.m_usages
			|| lhs.m_divisor != rhs.m_divisor;
	}
}

#endif
