/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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

