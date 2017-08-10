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
#ifndef ___CASTOR_EXCEPTION_H___
#define ___CASTOR_EXCEPTION_H___

#include <string>
#include <sstream>
#include <cstdint>

namespace castor
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
	\remark		donne aussi le fichier, la fonction et la ligne en plus des informations usuelles
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
		Exception( std::string const & p_description
			, char const * p_file
			, char const * p_function
			, uint32_t p_line )
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
		virtual char const * what()const throw()
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
		inline std::string const & getFileName()const throw()
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
		inline std::string const & getFunction()const throw()
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
		inline uint32_t getLine()const throw()
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
		inline std::string const & getDescription()const throw()
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
		inline std::string getFullDescription()const throw()
		{
			std::string strReturn;
			std::string strSep;

			if ( !m_filename.empty() )
			{
				strReturn += m_filename + " ";
			}
			else
			{
				strReturn += "<Unknown file> ";
			}

			if ( !m_functionName.empty() )
			{
				strReturn += m_functionName + " ";
			}
			else
			{
				strReturn += "<Unknown function> ";
			}

			if ( m_line )
			{
				std::stringstream stream;
				stream << m_line;
				strReturn += stream.str() + " : ";
			}
			else
			{
				strReturn += "<Unknown line> : ";
			}

			if ( !m_description.empty() )
			{
				strReturn += m_description;
			}
			else
			{
				strReturn += "<Unknown exception> : ";
			}

			return strReturn;
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
\brief		Macro to ease the use of castor::Exception
\~french
\brief		Macro définie pour faciliter l'utilisation de castor::Exception
*/
#define CASTOR_EXCEPTION( p_text ) throw castor::Exception( (p_text), __FILE__, __FUNCTION__, __LINE__ )

#endif
