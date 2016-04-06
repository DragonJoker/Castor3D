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
#ifndef ___C3D_VERSION_EXCEPTION_H___
#define ___C3D_VERSION_EXCEPTION_H___

#include "Castor3DPrerequisites.hpp"

#include <Exception.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		01/04/2011
	\~english
	\brief		Version comparison exception
	\~french
	\brief		Exception de comparaison de versions
	*/
	class VersionException
		: public Castor::Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_given			The given version
		 *\param[in]	p_expected		The expected version
		 *\param[in]	p_strFile		The file
		 *\param[in]	p_strFunction	The function
		 *\param[in]	p_uiLine		The file line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_given			La version donnée
		 *\param[in]	p_expected		La version voulue
		 *\param[in]	p_strFile		Le fichier
		 *\param[in]	p_strFunction	La fonction
		 *\param[in]	p_uiLine		La ligne dans le fichier
		 */
		C3D_API VersionException( Version const & p_given, Version const & p_expected, char const * p_strFile, char const * p_strFunction, uint32_t p_uiLine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~VersionException() throw() {}
	};

#	define CASTOR_VERSION_EXCEPTION( p_given, p_expected ) throw Castor3D::VersionException( p_given, p_expected, __FILE__, __FUNCTION__, __LINE__ )
}

#endif
