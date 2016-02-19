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

#include "ParserParameterHelpers.hpp"

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		Exception thrown when the user tries to retrieve a parameter of a wrong type
	\~french
	\brief		Exception lancée lorsque l'utilisateur se trompe de type de paramètre
	*/
	template< ePARAMETER_TYPE GivenType >
	class ParserParameterTypeException
		: public Castor::Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_expectedType	The real parameter type.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_expectedType	Le type réel du paramètre.
		 */
		inline ParserParameterTypeException( ePARAMETER_TYPE p_expectedType )
			: Castor::Exception( "", "", "", 0 )
		{
			m_description = "Wrong parameter type in parser: user gave " + string::string_cast< xchar >( ParserParameterHelper< GivenType >::StringType ) + " while parameter base type is " + string::string_cast< xchar >( GetTypeName( p_expectedType ) );
		}
	};
}

#endif
