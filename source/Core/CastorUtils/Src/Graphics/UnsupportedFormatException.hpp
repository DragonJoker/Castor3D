/*
This source file is part of ProceduralGenerator (https://sourceforge.net/projects/proceduralgene/)

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
#ifndef ___CASTOR_UNSUPPORTED_FORMAT_EXCEPTION_H___
#define ___CASTOR_UNSUPPORTED_FORMAT_EXCEPTION_H___

#include "Exception/Exception.hpp"
#include <vector>
#include <algorithm>
#include <numeric>

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Unsupported format exception
	\~french
	\brief		Unsupported format exception
	*/
	class UnsupportedFormatException
		: public Castor::Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_description	Error description
		 *\param[in]	p_file			Function file
		 *\param[in]	p_function		Function name
		 *\param[in]	p_line			Function line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_description	Description de l'erreur
		 *\param[in]	p_file			Fichier contenant la fonction
		 *\param[in]	p_function		Nom de la fonction
		 *\param[in]	p_line			Ligne dans la fonction
		 */
		UnsupportedFormatException(	std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line )
			: Exception( p_description, p_file, p_function, p_line )
		{
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		english Helper macro to use UnsupportedFormatException
	\~french
	\brief		Macro écrite pour faciliter l'utilisation de UnsupportedFormatException
	*/
#	define UNSUPPORTED_ERROR( p_text) throw UnsupportedFormatException( p_text, __FILE__, __FUNCTION__, __LINE__)
}

#endif
