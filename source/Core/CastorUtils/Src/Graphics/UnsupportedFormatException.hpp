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
