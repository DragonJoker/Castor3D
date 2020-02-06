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
#ifndef ___CASTOR_PARSER_PARAMETER_TYPE_EXCEPTION_H___
#define ___CASTOR_PARSER_PARAMETER_TYPE_EXCEPTION_H___

#include "Exception.hpp"
#include "ParserParameterBase.hpp"

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		Exception thrown when the user tries to retrieve a parameter of a wrong type
	\~french
	\brief		Exception lanc�e lorsque l'utilisateur se trompe de type de param�tre
	*/
	class ParserParameterTypeException
		: public Castor::Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eGivenType	The type asked by the user
		 *\param[in]	p_eExpectedType	The real parameter type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eGivenType	Le type demand� par l'utilisateur
		 *\param[in]	p_eExpectedType	Le type r�el du param�tre
		 */
		ParserParameterTypeException( ePARAMETER_TYPE p_eGivenType, ePARAMETER_TYPE p_eExpectedType );
	};
}

#endif
