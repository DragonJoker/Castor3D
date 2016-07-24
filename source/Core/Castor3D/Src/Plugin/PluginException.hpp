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
#ifndef ___C3D_PLUGIN_EXCEPTION_H___
#define ___C3D_PLUGIN_EXCEPTION_H___

#include "Castor3DPrerequisites.hpp"
#include "Exception/Exception.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		01/04/2011
	\~english
	\brief		Plugin loading exception
	\~french
	\brief		Exception de chargement de plug-in
	*/
	class PluginException
		: public Castor::Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_critical		Tells if the exception is critical
		 *\param[in]	p_description	The exception description
		 *\param[in]	p_file			The file
		 *\param[in]	p_function		The function
		 *\param[in]	p_line			The file line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_critical		Dit si l'exception est critique
		 *\param[in]	p_description	La description de l'exception
		 *\param[in]	p_file			Le fichier
		 *\param[in]	p_function		La fonction
		 *\param[in]	p_line			La ligne dans le fichier
		 */
		C3D_API PluginException( bool p_critical, std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PluginException()throw();
		/**
		 *\~english
		 *\brief		Tells if the exception is critical
		 *\return		The status
		 *\~french
		 *\brief		Dit si l'exception est critique
		 *\return		Le statut
		 */
		inline bool IsCritical()const
		{
			return m_critical;
		}

	private:
		bool m_critical;
	};
	//! helper macro to ease the use of PluginException
#	define CASTOR_PLUGIN_EXCEPTION( description, critical ) throw PluginException( critical, description, __FILE__, __FUNCTION__, __LINE__)
}

#endif

