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
#ifndef ___CASTOR_PARSER_PARAMETER_TYPE_EXCEPTION_H___
#define ___CASTOR_PARSER_PARAMETER_TYPE_EXCEPTION_H___

#include "Exception/Exception.hpp"

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
