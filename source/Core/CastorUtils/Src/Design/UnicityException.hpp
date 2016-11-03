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
#ifndef ___CASTOR_UNICITY_EXCEPTION___
#define ___CASTOR_UNICITY_EXCEPTION___

#include "Exception/Exception.hpp"

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
	enum class UnicityError
	{
		//\~english No instance found when expecting one.	\~french Pas d'intance alors qu'on en attend une.
		eNoInstance,
		//\~english An instance found when there shouldn't be one.	\~french Un intance alors qu'on ne devrait pas en avoir.
		eAnInstance,
		CASTOR_SCOPED_ENUM_BOUNDS( eNoInstance )
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		12/01/2016
	\~english
	\brief		Unicity error texts.
	\~french
	\brief		Textes des erreurs d'unicité
	*/
	static char const * const STR_UNICITY_ERROR[UnicityError::eCount] =
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
		UnicityException( UnicityError p_error, std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line )
			: Exception( STR_UNICITY_ERROR[uint32_t( p_error )] + p_description, p_file, p_function, p_line )
		{
		}
	};
	//!\~english Helper macro to use UnicityException	\~french Macro pour faciliter l'utilisation de UnicityException
#	define UNICITY_ERROR( p_error, p_text ) throw UnicityException( p_error, p_text, __FILE__, __FUNCTION__, __LINE__)
}

#endif
