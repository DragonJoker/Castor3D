/*
See LICENSE file in root folder
*/
#ifndef ___CU_Exception_H___
#define ___CU_Exception_H___

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <string>
#include <cstdint>

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <sstream>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	class Exception
		: public std::exception
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	description	The exception description
		 *\param[in]	file		The file name
		 *\param[in]	function	The function name
		 *\param[in]	line		The line number
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	description	La description de l'exception
		 *\param[in]	file		Le nom du fichier
		 *\param[in]	function	Le nom de la fonction
		 *\param[in]	line		Le numéro de ligne
		 */
		Exception( MbString const & description
			, char const * file
			, char const * function
			, uint32_t line )
			: m_line{ line }
			, m_description{ description }
			, m_filename{ file ? file : "" }
			, m_functionName{ function ? function : "" }
		{
			auto stream = makeStringStream();
			stream << debug::Backtrace{};
			m_callStack = toUtf8( stream.str() );
		}
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	description	The exception description
		 *\param[in]	file		The file name
		 *\param[in]	function	The function name
		 *\param[in]	line		The line number
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	description	La description de l'exception
		 *\param[in]	file		Le nom du fichier
		 *\param[in]	function	Le nom de la fonction
		 *\param[in]	line		Le numéro de ligne
		 */
		Exception( WString const & description
			, char const * file
			, char const * function
			, uint32_t line )
			: Exception{ toUtf8( description ), file, function, line }
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
		char const * what()const noexcept override
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
		MbString const & getFileName()const noexcept
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
		MbString const & getFunction()const noexcept
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
		uint32_t getLine()const noexcept
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
		MbString const & getDescription()const noexcept
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
		MbString getFullDescription()const noexcept
		{
			MbString result;

			if ( !m_filename.empty() )
			{
				result += m_filename + " ";
			}
			else
			{
				result += "<Unknown file> ";
			}

			if ( !m_functionName.empty() )
			{
				result += m_functionName + " ";
			}
			else
			{
				result += "<Unknown function> ";
			}

			if ( m_line )
			{
				auto stream = makeStringStreamT< char >();
				stream << m_line;
				result += stream.str() + " : ";
			}
			else
			{
				result += "<Unknown line> : ";
			}

			if ( !m_description.empty() )
			{
				result += m_description;
			}
			else
			{
				result += "<Unknown exception> : ";
			}

			if ( !m_callStack.empty() )
			{
				result += "\n" + m_callStack;
			}

			return result;
		}

	protected:
		uint32_t m_line;
		MbString m_description;
		MbString m_filename;
		MbString m_functionName;
		MbString m_callStack;
	};
}
/**
*\~english
*\brief		Macro to ease the use of castor::Exception
*\~french
*\brief		Macro définie pour faciliter l'utilisation de castor::Exception
*/
#define CU_Exception( text )\
	throw castor::Exception{ text, __FILE__, __FUNCTION__, uint32_t( __LINE__ ) }
/**
*\~english
*\brief		Macro to ease the use of castor::Exception
*\~french
*\brief		Macro définie pour faciliter l'utilisation de castor::Exception
*/
#define CU_SrcException( source, text )\
	throw castor::Exception{ source ": " text, __FILE__, __FUNCTION__, uint32_t( __LINE__ ) }

#endif
