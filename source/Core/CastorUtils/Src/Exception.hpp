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
#ifndef ___CASTOR_EXCEPTION_H___
#define ___CASTOR_EXCEPTION_H___

#include <string>
#include <sstream>
#include <cstdint>

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		More verbose exception class
	\remark		Gives File, Function, Line in addition to usual description
	\~french
	\brief		Classe d'exceptions plus verbeuse que celles de std
	\remark		Donne aussi le fichier, la fonction et la ligne en plus des informations usuelles
	*/
	class Exception
		: public std::exception
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	p_description	The exception description
		 *\param[in]	p_file			The file name
		 *\param[in]	p_function		The function name
		 *\param[in]	p_line			The line number
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_description	La description de l'exception
		 *\param[in]	p_file			Le nom du fichier
		 *\param[in]	p_function		Le nom de la fonction
		 *\param[in]	p_line			Le numéro de ligne
		 */
		Exception( std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line )
			: m_line( p_line )
			, m_description( p_description )
			, m_filename( p_file ? p_file : "" )
			, m_functionName( p_function ? p_function : "" )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Exception() throw()
		{
		}
		/**
		 *\~english
		 *\brief		Retrieves the exception description
		 *\return		The exception description
		 *\~french
		 *\brief		Récupère la description de l'exception
		 *\return		La description de l'exception
		 */
		inline virtual char const *	what()const throw()
		{
			return m_description.c_str();
		}
		/**
		 *\~english
		 *\brief		Retrieves the file name
		 *\return		The file name
		 *\~french
		 *\brief		Récupère le nom du fichier
		 *\return		Le nom du fichier
		 */
		inline std::string const & GetFilename()const throw()
		{
			return m_filename;
		}
		/**
		 *\~english
		 *\brief		Retrieves the function name
		 *\return		The function name
		 *\~french
		 *\brief		Récupère le nom de la fonction
		 *\return		Le nom de la fonction
		 */
		inline std::string const & GetFunction()const throw()
		{
			return m_functionName;
		}
		/**
		 *\~english
		 *\brief		Retrieves the line number
		 *\return		The line number
		 *\~french
		 *\brief		Récupère le numéro de ligne
		 *\return		Le numéro de ligne
		 */
		inline uint32_t GetLine()const throw()
		{
			return m_line;
		}
		/**
		 *\~english
		 *\brief		Retrieves the exception description
		 *\return		The exception description
		 *\~french
		 *\brief		Récupère la description de l'exception
		 *\return		La description de l'exception
		 */
		inline std::string const & GetDescription()const throw()
		{
			return m_description;
		}
		/**
		 *\~english
		 *\brief		Retrieves the full exception text (description, line, file and function
		 *\return		The full exception text
		 *\~french
		 *\brief		Récupère le texte complet de l'exception (description, ligne, fichier et fonction)
		 *\return		La texte complet de l'exception
		 */
		inline std::string GetFullDescription()const throw()
		{
			std::string l_strReturn;
			std::string l_strSep;

			if ( !m_filename.empty() )
			{
				l_strReturn += m_filename + " ";
			}
			else
			{
				l_strReturn += "<Unknown file> ";
			}

			if ( !m_functionName.empty() )
			{
				l_strReturn += m_functionName + " ";
			}
			else
			{
				l_strReturn += "<Unknown function> ";
			}

			if ( m_line )
			{
				std::stringstream l_stream;
				l_stream << m_line;
				l_strReturn += l_stream.str() + " : ";
			}
			else
			{
				l_strReturn += "<Unknown line> : ";
			}

			if ( !m_description.empty() )
			{
				l_strReturn += m_description;
			}
			else
			{
				l_strReturn += "<Unknown exception> : ";
			}

			return l_strReturn;
		}

	protected:
		//!\~english The line number	\~french Le numéro de ligne
		uint32_t m_line;
		//!\~english The exception description	\~french La description de l'exception
		std::string m_description;
		//!\~english The file name	\~french Le nom du fichier
		std::string m_filename;
		//!\~english The function name	\~french Le nom de la fonction
		std::string m_functionName;
	};
}
/*!
\author 	Sylvain DOREMUS
\date 		03/01/2011
\~english
\brief		Macro to ease the use of Castor::Exception
\~french
\brief		Macro définie pour faciliter l'utilisation de Castor::Exception
*/
#	define CASTOR_EXCEPTION( p_text ) throw Castor::Exception( (p_text), __FILE__, __FUNCTION__, __LINE__ )

#endif
