/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ParticleElementDeclaration_H___
#define ___C3D_ParticleElementDeclaration_H___

#include "ParticleModule.hpp"

namespace c3d
{
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
		inline ParticleElementDeclaration( String const & name
			, ElementUsages const & usages
			, ParticleFormat type
			, uint32_t offset = 0u
			, uint32_t divisor = 0u )
			: m_name{ name }
			, m_usages{ usages }
			, m_dataType{ type }
			, m_offset{ offset }
			, m_divisor{ divisor }
		{
		}

		//!\~english	The associated variable name.
		//!\~french		Le nom de la variable associée.
		String m_name{};
		//!\~english	Element usage.
		//!\~french		Utilisation de l'élément.
		ElementUsages m_usages{};
		//!\~english	Element type.
		//!\~french		Type de l'élément.
		ParticleFormat m_dataType{ ParticleFormat::eVec4f };
		//!\~english	Offset in buffer.
		//!\~french		Offset dans le tampon.
		uint32_t m_offset{};
		//!\~english	Instantiation attribute increment.
		//!\~french		Incrément d'attribut d'instanciation.
		uint32_t m_divisor{};

	private:
		friend bool operator==( ParticleElementDeclaration const & lhs
			, ParticleElementDeclaration const & rhs )noexcept
		{
			return lhs.m_dataType == rhs.m_dataType
				&& lhs.m_name == rhs.m_name
				&& lhs.m_offset == rhs.m_offset
				&& lhs.m_usages == rhs.m_usages
				&& lhs.m_divisor == rhs.m_divisor;
		}
	};
}

#endif
