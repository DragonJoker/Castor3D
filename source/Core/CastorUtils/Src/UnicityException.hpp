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
#ifndef ___CASTOR_UNICITY_EXCEPTION___
#define ___CASTOR_UNICITY_EXCEPTION___

#include "Exception.hpp"
#include "Resource.hpp"
#include "TextFile.hpp"
#include "BinaryFile.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		12/01/2016
	\~english
	\brief		Unicity errors enumeration.
	\~french
	\brief		Enumération des erreurs d'unicité.
	*/
	typedef enum eUNICITY_ERROR
	{
		//\~english No instance found when expecting one.	\~french Pas d'intance alors qu'on en attend une.
		eUNICITY_ERROR_NO_INSTANCE,
		//\~english An instance found when there shouldn't be one.	\~french Un intance alors qu'on ne devrait pas en avoir.
		eUNICITY_ERROR_AN_INSTANCE,
		CASTOR_ENUM_BOUNDS( eUNICITY_ERROR, eUNICITY_ERROR_NO_INSTANCE )
	}	eUNICITY_ERROR;
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		12/01/2016
	\~english
	\brief		Unicity error texts.
	\~french
	\brief		Textes des erreurs d'unicité
	*/
	static char const * const STR_UNICITY_ERROR[eUNICITY_ERROR_COUNT] =
	{
		"No instance for Unique class ",
		"Duplicate instance for Unique class ",
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Unicity exception
	\~french
	\brief		Exception d'unicité
	*/
	class UnicityException
		: public Castor::Exception
	{
	public:
		UnicityException( eUNICITY_ERROR p_error, std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line )
			: Exception( STR_UNICITY_ERROR[p_error] + p_description, p_file, p_function, p_line )
		{
		}
	};
	//!\~english Helper macro to use UnicityException	\~french Macro pour faciliter l'utilisation de UnicityException
#	define UNICITY_ERROR( p_error, p_text ) throw UnicityException( p_error, p_text, __FILE__, __FUNCTION__, __LINE__)
}

#endif
