/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VERSION_H___
#define ___C3D_VERSION_H___

#include "Castor3DPrerequisites.hpp"

#include "RequiredVersion.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/01/2011
	\~english
	\brief		Version management class
	\remark		Class used to manage versions and versions dependencies for plug-ins
	\~french
	\brief		Classe de gestion de version
	\remark		Classe utilisee pour gerer les versions et dependances de version pour les plug-ins
	*/
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
		C3D_API Version( int major = CASTOR_VERSION_MAJOR
			, int minor = CASTOR_VERSION_MINOR
			, int build = CASTOR_VERSION_BUILD );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Version();
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
