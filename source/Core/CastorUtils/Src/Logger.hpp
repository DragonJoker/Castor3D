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
#ifndef ___CASTOR_LOGGER_H___
#define ___CASTOR_LOGGER_H___

#include "CastorUtilsPrerequisites.hpp"
#include "LoggerImpl.hpp"

#include <cstdarg>

namespace Castor
{
	/**
	 *\~english
	 *\brief		Write formatted data from variable argument list to sized buffer
	 *\param[out]	p_text		Receives the formatted text
	 *\param[in]	p_count		The output buffer size
	 *\param[in]	p_format	The format string used to interprete the arguments list
	 *\param[in]	p_valist	The arguments list
	 *\return		The number of characters that would have been written if p_count had been sufficiently large, not counting the terminating null character
	 *\~french
	 *\brief		Ecrit des données formatées à partir d'une liste variable d'arguments, dans un tampon à taille fixe
	 *\param[out]	p_text		Reçoit le texte formaté
	 *\param[in]	p_count		La taille du tampon
	 *\param[in]	p_format	La chaîne contenant le format permettant l'interprétation de la liste d'arguments
	 *\param[in]	p_valist	La liste d'arguments
	 *\return		Le nombre de caractères qui auraient été écrits si p_count avait été suffisamment grand, sans compter le caractère de terminaison
	*/
	int Vsnprintf( char * p_text, size_t p_count, const char * p_format, va_list p_valist );
	/**
	 *\~english
	 *\brief		Write formatted data from variable argument list to sized buffer
	 *\param[out]	p_text		Receives the formatted text
	 *\param[in]	p_format	The format string used to interprete the arguments list
	 *\param[in]	p_valist	The arguments list
	 *\return		The number of characters that would have been written if p_count had been sufficiently large, not counting the terminating null character
	 *\~french
	 *\brief		Ecrit des données formatées à partir d'une liste variable d'arguments, dans un tampon à taille fixe
	 *\param[out]	p_text		Reçoit le texte formaté
	 *\param[in]	p_format	La chaîne contenant le format permettant l'interprétation de la liste d'arguments
	 *\param[in]	p_valist	La liste d'arguments
	 *\return		Le nombre de caractères qui auraient été écrits si p_count avait été suffisamment grand, sans compter le caractère de terminaison
	*/
	template< size_t Count >
	int Vsnprintf( char( &p_text )[Count], const char * p_format, va_list p_valist )
	{
		return Vsnprintf( p_text, Count, p_format, p_valist );
	}
	/**
	 *\~english
	 *\brief		Write formatted data from variable argument list to sized buffer
	 *\param[out]	p_text		Receives the formatted text
	 *\param[in]	p_count		The output buffer size
	 *\param[in]	p_format	The format string used to interprete the arguments list
	 *\param[in]	p_valist	The arguments list
	 *\return		The number of characters that would have been written if p_count had been sufficiently large, not counting the terminating null character
	 *\~french
	 *\brief		Ecrit des données formatées à partir d'une liste variable d'arguments, dans un tampon à taille fixe
	 *\param[out]	p_text		Reçoit le texte formaté
	 *\param[in]	p_count		La taille du tampon
	 *\param[in]	p_format	La chaîne contenant le format permettant l'interprétation de la liste d'arguments
	 *\param[in]	p_valist	La liste d'arguments
	 *\return		Le nombre de caractères qui auraient été écrits si p_count avait été suffisamment grand, sans compter le caractère de terminaison
	*/
	int Vswprintf( wchar_t * p_text, size_t p_count, const wchar_t * p_format, va_list p_valist );
	/**
	 *\~english
	 *\brief		Write formatted data from variable argument list to sized buffer
	 *\param[out]	p_text		Receives the formatted text
	 *\param[in]	p_format	The format string used to interprete the arguments list
	 *\param[in]	p_valist	The arguments list
	 *\return		The number of characters that would have been written if p_count had been sufficiently large, not counting the terminating null character
	 *\~french
	 *\brief		Ecrit des données formatées à partir d'une liste variable d'arguments, dans un tampon à taille fixe
	 *\param[out]	p_text		Reçoit le texte formaté
	 *\param[in]	p_format	La chaîne contenant le format permettant l'interprétation de la liste d'arguments
	 *\param[in]	p_valist	La liste d'arguments
	 *\return		Le nombre de caractères qui auraient été écrits si p_count avait été suffisamment grand, sans compter le caractère de terminaison
	*/
	template< size_t Count >
	int Vswprintf( wchar_t ( &p_text )[Count], const wchar_t * p_format, va_list p_valist )
	{
		return Vswprintf( p_text, Count, p_format, p_valist );
	}
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Log management class
	\remark		Implements log facilities. Create a Log with a filename, then write logs into that file
	\~french
	\brief		Classe de gestion de logs
	\remark		Implémente les fonctions de logging. Initialise un log avec un nom de fichier puis écrit dedans
	*/
	class Logger
	{
	private:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Logger();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Logger();

	public:
		/**
		 *\~english
		 *\brief		Initialises this Logger instance to another one
		 *\param[in]	p_pLogger	The logger
		 *\~french
		 *\brief		Initialise l'instance de ce Logger à une autre
		 *\param[in]	p_pLogger	Le logger
		 */
		static void Initialise( Logger * p_pLogger );
		/**
		 *\~english
		 *\brief		Initialises this logger instance level
		 *\param[in]	p_eLogLevel		The log level
		 *\~french
		 *\brief		Initialise l'instance du logger avec le niveau donné
		 *\param[in]	p_eLogLevel		Le niveau de log
		 */
		static void Initialise( eLOG_TYPE p_eLogLevel );
		/**
		 *\~english
		 *\brief		Destroys the Logger instance
		 *\~french
		 *\brief		Détruit l'instance du Logger
		 */
		static void Cleanup();
		/**
		 *\~english
		 *\brief		Defines the logging callback
		 *\param[in]	p_pfnCallback	The callback
		 *\param[in]	p_pCaller		Pointer to user data
		 *\~french
		 *\brief		Définit la callback de log
		 *\param[in]	p_pfnCallback	La callback
		 *\param[in]	p_pCaller		Pointeur sur des données utilisateur
		 */
		static void SetCallback( PLogCallback p_pfnCallback, void * p_pCaller );
		/**
		 *\~english
		 *\brief		Sets the log file address
		 *\param[in]	p_logFilePath	The log file path
		 *\param[in]	p_eLogType		The log type
		 *\~french
		 *\brief		Définit le chemin du fichier de log
		 *\param[in]	p_logFilePath	Le chemin du fichier
		 *\param[in]	p_eLogType		Le type de log concerné
		 */
		static void SetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType = eLOG_TYPE_COUNT );
		/**
		 *\~english
		 *\brief		Logs a debug message in the log file, using va_args
		 *\param[in]	p_format	The line format
		 *\param[in]	...			POD arguments, following printf format
		 *\~french
		 *\brief		Log un message debug dans le fichier de log, en utilisant va_args
		 *\param[in]	p_format	Le format de la ligne
		 *\param[in]	...			Paramètres POD, utilise le format de printf
		 */
		static void LogDebug( char const * p_format, ... );
		/**
		 *\~english
		 *\brief		Logs a debug message, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::string
		 *\param[in]	p_msg	The line to log
		 */
		static void LogDebug( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a unicode debug message in the log file, using va_args
		 *\param[in]	p_format	The line format
		 *\param[in]	...			POD arguments, following printf format
		 *\~french
		 *\brief		Log un message unicode debug dans le fichier de log, en utilisant va_args
		 *\param[in]	p_format	Le format de la ligne
		 *\param[in]	...			Paramètres POD, utilise le format de printf
		 */
		static void LogDebug( wchar_t const * p_format , ... );
		/**
		 *\~english
		 *\brief		Logs a unicode debug message, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message debug, à partir d'un std::wstring
		 *\param[in]	p_msg	The line to log
		 */
		static void LogDebug( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a message in the log file, using va_args
		 *\param[in]	p_format	The line format
		 *\param[in]	...			POD arguments, following printf format
		 *\~french
		 *\brief		Log un message dans le fichier de log, en utilisant va_args
		 *\param[in]	p_format	Le format de la ligne
		 *\param[in]	...			Paramètres POD, utilise le format de printf
		 */
		static void LogMessage( char const * p_format, ... );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::string
		 *\param[in]	p_msg	The line to log
		 */
		static void LogMessage( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a unicode line in the log file, using va_args
		 *\param[in]	p_format	The line format
		 *\param[in]	...			POD arguments, following printf format
		 *\~french
		 *\brief		Log une ligne unicode dans le fichier de log, en utilisant va_args
		 *\param[in]	p_format	Le format de la ligne
		 *\param[in]	...			Paramètres POD, utilise le format de printf
		 */
		static void LogMessage( wchar_t const * p_format , ... );
		/**
		 *\~english
		 *\brief		Logs a message, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un message, à partir d'un std::wstring
		 *\param[in]	p_msg	The line to log
		 */
		static void LogMessage( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a warning in the log file, using va_args
		 *\param[in]	p_format	The line format
		 *\param[in]	...			POD arguments, following printf format
		 *\~french
		 *\brief		Log un avertissement dans le fichier de log, en utilisant va_args
		 *\param[in]	p_format	Le format de la ligne
		 *\param[in]	...			Paramètres POD, utilise le format de printf
		 */
		static void LogWarning( char const * p_format, ... );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::string
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::string
		 *\param[in]	p_msg	The line to log
		 */
		static void LogWarning( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a unicode warning in the log file, using va_args
		 *\param[in]	p_format	The line format
		 *\param[in]	...			POD arguments, following printf format
		 *\~french
		 *\brief		Log un avertissement en unicode dans le fichier de log, en utilisant va_args
		 *\param[in]	p_format	Le format de la ligne
		 *\param[in]	...			Paramètres POD, utilise le format de printf
		 */
		static void LogWarning( wchar_t const * p_format , ... );
		/**
		 *\~english
		 *\brief		Logs a warning, from a std::wstring
		 *\param[in]	p_msg	The line to log
		 *\~french
		 *\brief		Log un avertissement, à partir d'un std::wstring
		 *\param[in]	p_msg	The line to log
		 */
		static void LogWarning( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Logs an error in the log file, using va_args
		 *\param[in]	p_format	The line format
		 *\param[in]	...			POD arguments, following printf format
		 *\~french
		 *\brief		Log une erreur dans le fichier de log, en utilisant va_args
		 *\param[in]	p_format	Le format de la ligne
		 *\param[in]	...			Paramètres POD, utilise le format de printf
		 */
		static void LogError( char const * p_format, ... );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::string
		 *\param[in]	p_msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\param[in]	p_msg		The line to log
		 */
		static void LogError( std::string const & p_msg );
		/**
		 *\~english
		 *\brief		Logs a unicode error in the log file, using va_args
		 *\param[in]	p_format	The line format
		 *\param[in]	...			POD arguments, following printf format
		 *\~french
		 *\brief		Log une erreur en unicode dans le fichier de log, en utilisant va_args
		 *\param[in]	p_format	Le format de la ligne
		 *\param[in]	...			Paramètres POD, utilise le format de printf
		 */
		static void LogError( wchar_t const * p_format , ... );
		/**
		 *\~english
		 *\brief		Logs an error, from a std::wstring
		 *\param[in]	p_msg		The line to log
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wstring
		 *\param[in]	p_msg		The line to log
		 */
		static void LogError( std::wstring const & p_msg );
		/**
		 *\~english
		 *\brief		Returns a reference over the instance
		 *\return		The instance
		 *\~french
		 *\brief		Retourne une référence sur l'instance
		 *\return		L'instance
		 */
		static Logger & GetSingleton();
		/**
		 *\~english
		 *\brief		Returns a pointer over the instance
		 *\return		The instance
		 *\~french
		 *\brief		Retourne un pointeur sur l'instance
		 *\return		L'instance
		 */
		static Logger * GetSingletonPtr();

	private:
		void DoSetCallback( PLogCallback p_pfnCallback, void * p_pCaller );
		void DoSetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType = eLOG_TYPE_COUNT );

	private:
		friend class ILoggerImpl;
		static bool m_bOwnInstance;
		static Logger * m_pSingleton;
		static uint32_t m_uiCounter;
		ILoggerImpl * m_pImpl;
		std::mutex m_mutex;
		String m_strHeaders[eLOG_TYPE_COUNT];
	};
}

#endif
