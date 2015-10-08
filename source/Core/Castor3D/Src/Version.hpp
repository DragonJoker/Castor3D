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
#ifndef ___C3D_VERSION_H___
#define ___C3D_VERSION_H___

#include "Castor3DPrerequisites.hpp"

#include "RequiredVersion.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/01/2011
	\~english
	\brief		Version management class
	\remark		Class used to manage versions and versions dependencies for plugins
	\~french
	\brief		Classe de gestion de version
	\remark		Classe utilisee pour gerer les versions et dependances de version pour les plugins
	*/
	class C3D_API Version
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_iMajor	The major version number
		 *\param[in]	p_iMinor	The minor version number
		 *\param[in]	p_iBuild	The build version number
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_iMajor	Le numéro majeur
		 *\param[in]	p_iMinor	Le numéro mineur
		 *\param[in]	p_iBuild	Le numéro de build
		 */
		Version( int p_iMajor = CASTOR_VERSION_MAJOR, int p_iMinor = CASTOR_VERSION_MINOR, int p_iBuild = CASTOR_VERSION_BUILD );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_version	The source
		 *\~french
		 *\brief		Constructeur par recopie
		 *\param[in]	p_version	La source
		 */
		Version( Version const & p_version );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_version	The source
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_version	La source
		 */
		Version( Version && p_version );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Version();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_version	The source
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par recopie
		 *\param[in]	p_version	La source
		 *\return		Une référence sur cet objet
		 */
		Version & operator=( Version const & p_version );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_version	The source
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_version	La source
		 *\return		Une référence sur cet objet
		 */
		Version & operator=( Version && p_version );

	public:
		//!~english The major version number	\~french Le numéro majeur
		int m_iMajor;
		//!~english The minor version number	\~french Le numéro mineur
		int m_iMinor;
		//!~english The build version number	\~french Le numéro build
		int m_iBuild;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_a, p_b	The versions to compare
	 *\return		false if one version number is different from p_a to p_b
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_a, p_b	Les versions à comparer
	 *\return		false si un numéro de version est différent entre p_a et p_b
	 */
	C3D_API bool operator==( Castor3D::Version const & p_a, Castor3D::Version const & p_b );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_a, p_b	The versions to compare
	 *\return		true if one version number is different from p_a to p_b
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_a, p_b	Les versions à comparer
	 *\return		true si un numéro de version est différent entre p_a et p_b
	 */
	C3D_API bool operator!=( Castor3D::Version const & p_a, Castor3D::Version const & p_b );
	/**
	 *\~english
	 *\brief		Less than operator
	 *\param[in]	p_a, p_b	The versions to compare
	 *\return		true if p_a.major is less than p_b.major or if they are equal and p_a.minor is less than p_b.minor or if majors and minors are equal and p_a.build is less than p_b.build
	 *\~french
	 *\brief		Opérateur inférieur
	 *\param[in]	p_a, p_b	Les versions à comparer
	 *\return		true si p_a.major est inférieur à p_b.major ou s'ils sont égaux et p_a.minor est inférieur à p_b.minor ou si majors et minors sont égaux et p_a.build est inférieur à p_b.build
	 */
	C3D_API bool operator<( Castor3D::Version const & p_a, Castor3D::Version const & p_b );
	/**
	 *\~english
	 *\brief		Less than operator
	 *\param[in]	p_a, p_b	The versions to compare
	 *\return		true if p_a.major is greater than p_b.major or if they are equal and p_a.minor is greater than p_b.minor or if majors and minors are equal and p_a.build is greater than p_b.build
	 *\~french
	 *\brief		Opérateur inférieur
	 *\param[in]	p_a, p_b	Les versions à comparer
	 *\return		true si p_a.major est supérieur à p_b.major ou s'ils sont égaux et p_a.minor est supérieur à p_b.minor ou si majors et minors sont égaux et p_a.build est supérieur à p_b.build
	 */
	C3D_API bool operator>( Castor3D::Version const & p_a, Castor3D::Version const & p_b );
	/**
	 *\~english
	 *\brief		Less than operator
	 *\param[in]	p_a, p_b	The versions to compare
	 *\return		false if p_a.major is greater than p_b.major or if they are equal and p_a.minor is greater than p_b.minor or if majors and minors are equal and p_a.build is greater than p_b.build
	 *\~french
	 *\brief		Opérateur inférieur
	 *\param[in]	p_a, p_b	Les versions à comparer
	 *\return		false si p_a.major est supérieur à p_b.major ou s'ils sont égaux et p_a.minor est supérieur à p_b.minor ou si majors et minors sont égaux et p_a.build est supérieur à p_b.build
	 */
	C3D_API bool operator<=( Castor3D::Version const & p_a, Castor3D::Version const & p_b );
	/**
	 *\~english
	 *\brief		Less than operator
	 *\param[in]	p_a, p_b	The versions to compare
	 *\return		false if p_a.major is less than p_b.major or if they are equal and p_a.minor is less than p_b.minor or if majors and minors are equal and p_a.build is less than p_b.build
	 *\~french
	 *\brief		Opérateur inférieur
	 *\param[in]	p_a, p_b	Les versions à comparer
	 *\return		false si p_a.major est inférieur à p_b.major ou s'ils sont égaux et p_a.minor est inférieur à p_b.minor ou si majors et minors sont égaux et p_a.build est inférieur à p_b.build
	 */
	C3D_API bool operator>=( Castor3D::Version const & p_a, Castor3D::Version const & p_b );

	C3D_API std::ostream & operator<<( std::ostream & p_stream, Castor3D::Version const & p_version );
	C3D_API std::wostream & operator<<( std::wostream & p_stream, Castor3D::Version const & p_version );
	C3D_API Castor::String & operator<<( Castor::String & p_stream, Castor3D::Version const & p_version );
}

#endif
