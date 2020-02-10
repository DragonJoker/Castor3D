/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VERSION_H___
#define ___C3D_VERSION_H___

#include "MiscellaneousModule.hpp"

#include "Castor3D/RequiredVersion.hpp"

namespace castor3d
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
		C3D_API Version( int major = C3D_VersionMajor
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
		C3D_API Version( uint32_t vk );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Version();
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
		inline uint16_t getMajor()const
		{
			return m_major;
		}
		/**
		 *\~english
		 *\return		The major version number.
		 *\~french
		 *\return		Le numéro mineur de version.
		 */
		inline uint16_t getMinor()const
		{
			return m_minor;
		}
		/**
		 *\~english
		 *\return		The build version number.
		 *\~french
		 *\return		Le numéro build de version.
		 */
		inline uint16_t getBuild()const
		{
			return m_build;
		}

	private:
		//!~english		The major version number.
		//!~french		Le numéro majeur.
		int m_major;
		//!~english		The minor version number.
		//!~french		Le numéro mineur.
		int m_minor;
		//!~english		The build version number.
		//!~french		Le numéro de build.
		int m_build;
	};
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	a, b	The versions to compare.
	 *\return		false if one version number is different from a to b.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	a, b	Les versions à comparer.
	 *\return		false si un numéro de version est différent entre a et b.
	 */
	C3D_API bool operator==( castor3d::Version const & a, castor3d::Version const & b );
	/**
	 *\~english
	 *\brief		Difference operator.
	 *\param[in]	a, b	The versions to compare.
	 *\return		true if one version number is different from a to b.
	 *\~french
	 *\brief		Opérateur de différence.
	 *\param[in]	a, b	Les versions à comparer.
	 *\return		true si un numéro de version est différent entre a et b.
	 */
	C3D_API bool operator!=( castor3d::Version const & a, castor3d::Version const & b );
	/**
	 *\~english
	 *\brief		Less than operator.
	 *\param[in]	a, b	The versions to compare.
	 *\return		true if a.major is less than b.major or if they are equal and a.minor is less than b.minor or if majors and minors are equal and a.build is less than b.build.
	 *\~french
	 *\brief		Opérateur inférieur.
	 *\param[in]	a, b	Les versions à comparer.
	 *\return		true si a.major est inférieur à b.major ou s'ils sont égaux et a.minor est inférieur à b.minor ou si majors et minors sont égaux et a.build est inférieur à b.build.
	 */
	C3D_API bool operator<( castor3d::Version const & a, castor3d::Version const & b );
	/**
	 *\~english
	 *\brief		Less than operator.
	 *\param[in]	a, b	The versions to compare.
	 *\return		true if a.major is greater than b.major or if they are equal and a.minor is greater than b.minor or if majors and minors are equal and a.build is greater than b.build.
	 *\~french
	 *\brief		Opérateur inférieur.
	 *\param[in]	a, b	Les versions à comparer.
	 *\return		true si a.major est supérieur à b.major ou s'ils sont égaux et a.minor est supérieur à b.minor ou si majors et minors sont égaux et a.build est supérieur à b.build.
	 */
	C3D_API bool operator>( castor3d::Version const & a, castor3d::Version const & b );
	/**
	 *\~english
	 *\brief		Less than operator.
	 *\param[in]	a, b	The versions to compare.
	 *\return		false if a.major is greater than b.major or if they are equal and a.minor is greater than b.minor or if majors and minors are equal and a.build is greater than b.build.
	 *\~french
	 *\brief		Opérateur inférieur.
	 *\param[in]	a, b	Les versions à comparer.
	 *\return		false si a.major est supérieur à b.major ou s'ils sont égaux et a.minor est supérieur à b.minor ou si majors et minors sont égaux et a.build est supérieur à b.build.
	 */
	C3D_API bool operator<=( castor3d::Version const & a, castor3d::Version const & b );
	/**
	 *\~english
	 *\brief		Less than operator.
	 *\param[in]	a, b	The versions to compare.
	 *\return		false if a.major is less than b.major or if they are equal and a.minor is less than b.minor or if majors and minors are equal and a.build is less than b.build.
	 *\~french
	 *\brief		Opérateur inférieur.
	 *\param[in]	a, b	Les versions à comparer.
	 *\return		false si a.major est inférieur à b.major ou s'ils sont égaux et a.minor est inférieur à b.minor ou si majors et minors sont égaux et a.build est inférieur à b.build.
	 */
	C3D_API bool operator>=( castor3d::Version const & a, castor3d::Version const & b );

	C3D_API std::ostream & operator<<( std::ostream & stream, castor3d::Version const & version );
	C3D_API std::wostream & operator<<( std::wostream & stream, castor3d::Version const & version );
	C3D_API castor::String & operator<<( castor::String & stream, castor3d::Version const & version );
}

#endif
