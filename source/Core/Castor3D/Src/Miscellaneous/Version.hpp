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
		 *\param[in]	p_major	The major version number.
		 *\param[in]	p_minor	The minor version number.
		 *\param[in]	p_build	The build version number.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_major	Le numéro majeur.
		 *\param[in]	p_minor	Le numéro mineur.
		 *\param[in]	p_build	Le numéro de build.
		 */
		C3D_API Version( int p_major = CASTOR_VERSION_MAJOR, int p_minor = CASTOR_VERSION_MINOR, int p_build = CASTOR_VERSION_BUILD );
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
	 *\param[in]	p_a, p_b	The versions to compare.
	 *\return		false if one version number is different from p_a to p_b.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_a, p_b	Les versions à comparer.
	 *\return		false si un numéro de version est différent entre p_a et p_b.
	 */
	C3D_API bool operator==( castor3d::Version const & p_a, castor3d::Version const & p_b );
	/**
	 *\~english
	 *\brief		Difference operator.
	 *\param[in]	p_a, p_b	The versions to compare.
	 *\return		true if one version number is different from p_a to p_b.
	 *\~french
	 *\brief		Opérateur de différence.
	 *\param[in]	p_a, p_b	Les versions à comparer.
	 *\return		true si un numéro de version est différent entre p_a et p_b.
	 */
	C3D_API bool operator!=( castor3d::Version const & p_a, castor3d::Version const & p_b );
	/**
	 *\~english
	 *\brief		Less than operator.
	 *\param[in]	p_a, p_b	The versions to compare.
	 *\return		true if p_a.major is less than p_b.major or if they are equal and p_a.minor is less than p_b.minor or if majors and minors are equal and p_a.build is less than p_b.build.
	 *\~french
	 *\brief		Opérateur inférieur.
	 *\param[in]	p_a, p_b	Les versions à comparer.
	 *\return		true si p_a.major est inférieur à p_b.major ou s'ils sont égaux et p_a.minor est inférieur à p_b.minor ou si majors et minors sont égaux et p_a.build est inférieur à p_b.build.
	 */
	C3D_API bool operator<( castor3d::Version const & p_a, castor3d::Version const & p_b );
	/**
	 *\~english
	 *\brief		Less than operator.
	 *\param[in]	p_a, p_b	The versions to compare.
	 *\return		true if p_a.major is greater than p_b.major or if they are equal and p_a.minor is greater than p_b.minor or if majors and minors are equal and p_a.build is greater than p_b.build.
	 *\~french
	 *\brief		Opérateur inférieur.
	 *\param[in]	p_a, p_b	Les versions à comparer.
	 *\return		true si p_a.major est supérieur à p_b.major ou s'ils sont égaux et p_a.minor est supérieur à p_b.minor ou si majors et minors sont égaux et p_a.build est supérieur à p_b.build.
	 */
	C3D_API bool operator>( castor3d::Version const & p_a, castor3d::Version const & p_b );
	/**
	 *\~english
	 *\brief		Less than operator.
	 *\param[in]	p_a, p_b	The versions to compare.
	 *\return		false if p_a.major is greater than p_b.major or if they are equal and p_a.minor is greater than p_b.minor or if majors and minors are equal and p_a.build is greater than p_b.build.
	 *\~french
	 *\brief		Opérateur inférieur.
	 *\param[in]	p_a, p_b	Les versions à comparer.
	 *\return		false si p_a.major est supérieur à p_b.major ou s'ils sont égaux et p_a.minor est supérieur à p_b.minor ou si majors et minors sont égaux et p_a.build est supérieur à p_b.build.
	 */
	C3D_API bool operator<=( castor3d::Version const & p_a, castor3d::Version const & p_b );
	/**
	 *\~english
	 *\brief		Less than operator.
	 *\param[in]	p_a, p_b	The versions to compare.
	 *\return		false if p_a.major is less than p_b.major or if they are equal and p_a.minor is less than p_b.minor or if majors and minors are equal and p_a.build is less than p_b.build.
	 *\~french
	 *\brief		Opérateur inférieur.
	 *\param[in]	p_a, p_b	Les versions à comparer.
	 *\return		false si p_a.major est inférieur à p_b.major ou s'ils sont égaux et p_a.minor est inférieur à p_b.minor ou si majors et minors sont égaux et p_a.build est inférieur à p_b.build.
	 */
	C3D_API bool operator>=( castor3d::Version const & p_a, castor3d::Version const & p_b );

	C3D_API std::ostream & operator<<( std::ostream & p_stream, castor3d::Version const & p_version );
	C3D_API std::wostream & operator<<( std::wostream & p_stream, castor3d::Version const & p_version );
	C3D_API castor::String & operator<<( castor::String & p_stream, castor3d::Version const & p_version );
}

#endif
