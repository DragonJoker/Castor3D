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
#ifndef ___C3D_VERSION_EXCEPTION_H___
#define ___C3D_VERSION_EXCEPTION_H___

#include "Version.hpp"

#include <Exception/Exception.hpp>

namespace castor3d
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
		: public castor::Exception
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

#	define CASTOR_VERSION_EXCEPTION( p_given, p_expected ) throw castor3d::VersionException( p_given, p_expected, __FILE__, __FUNCTION__, __LINE__ )
}

#endif
