/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Version_H___
#define ___C3D_Version_H___

#include "MiscellaneousModule.hpp"

#include "Castor3D/RequiredVersion.hpp"

namespace c3d
{
	class Version
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	major	The major version number.
		 *\param[in]	minor	The minor version number.
		 *\param[in]	build	The build version number.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	major	Le numéro majeur.
		 *\param[in]	minor	Le numéro mineur.
		 *\param[in]	build	Le numéro de build.
		 */
		C3D_API explicit Version( int major = C3D_VersionMajor
			, int minor = C3D_VersionMinor
			, int build = C3D_VersionBuild );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	vk	The Vulkan compatible version number.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	vk	Le numéro de version pour Vulkan.
		 */
		C3D_API explicit Version( uint32_t vk );
		/**
		 *\~english
		 *\return		The Vulkan compatible version number.
		 *\~french
		 *\return		Le numéro de version pour Vulkan.
		 */
		uint32_t getVkVersion()const;
		/**
		 *\~english
		 *\return		The major version number.
		 *\~french
		 *\return		Le numéro majeur de version.
		 */
		uint16_t getMajor()const
		{
			return m_major;
		}
		/**
		 *\~english
		 *\return		The major version number.
		 *\~french
		 *\return		Le numéro mineur de version.
		 */
		uint16_t getMinor()const
		{
			return m_minor;
		}
		/**
		 *\~english
		 *\return		The build version number.
		 *\~french
		 *\return		Le numéro build de version.
		 */
		uint16_t getBuild()const
		{
			return m_build;
		}

	private:
		uint16_t m_major;
		uint16_t m_minor;
		uint16_t m_build;

		friend bool operator==( Version const & lhs, Version const & rhs )noexcept
		{
			return lhs.getMajor() == rhs.getMajor()
				&& lhs.getMinor() == rhs.getMinor()
				&& lhs.getBuild() == rhs.getBuild();
		}

		friend bool operator<( Version const & lhs, Version const & rhs )noexcept
		{
			return	lhs.getMajor() < rhs.getMajor()
				|| ( lhs.getMajor() == rhs.getMajor() && lhs.getMinor() < rhs.getMinor() )
				|| ( lhs.getMajor() == rhs.getMajor() && lhs.getMinor() == rhs.getMinor() && lhs.getBuild() < rhs.getBuild() );
		}

		friend bool operator>( Version const & lhs, Version const & rhs )noexcept
		{
			return	lhs.getMajor() > rhs.getMajor()
				|| ( lhs.getMajor() == rhs.getMajor() && lhs.getMinor() > rhs.getMinor() )
				|| ( lhs.getMajor() == rhs.getMajor() && lhs.getMinor() == rhs.getMinor() && lhs.getBuild() < rhs.getBuild() );
		}

		friend bool operator<=( Version const & lhs, Version const & rhs )noexcept
		{
			return !( lhs > rhs );
		}

		friend bool operator>=( Version const & lhs, Version const & rhs )noexcept
		{
			return !( lhs < rhs );
		}

		template< typename CharT >
		friend std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & stream, Version const & version )
		{
			stream << version.getMajor() << "." << version.getMinor() << "." << version.getBuild();
			return stream;
		}
	};
}

#endif
