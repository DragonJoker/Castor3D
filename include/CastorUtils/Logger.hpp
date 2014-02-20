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
#ifndef ___Castor_Logger___
#define ___Castor_Logger___

#include "AutoSingleton.hpp"
#include "String.hpp"
#include "MultiThreadConfig.hpp"

//Memory.hpp MUST be the last include
#include "Memory.hpp"

#pragma warning( push )
#pragma warning( disable:4290 )

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Defines the various log types
	\~french
	\brief		Définit les différents types de log
	*/
	typedef enum eLOG_TYPE CASTOR_TYPE( uint8_t )
	{	eLOG_TYPE_DEBUG		//!<\~english Debug type log		\~french Log de type debug
	,	eLOG_TYPE_MESSAGE	//!<\~english Message type log		\~french Log de type message
	,	eLOG_TYPE_WARNING	//!<\~english Warning type log		\~french Log de type avertissement
	,	eLOG_TYPE_ERROR		//!<\~english Error type log		\~french Log de type erreur
	,	eLOG_TYPE_COUNT		//!<\~english Number of log types	\~french Compte des logs
	}	eLOG_TYPE;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Console info class
	\remark		Platform dependent
	\~french
	\brief		Classe contenant les informations de la console
	\remark		Dépend de l'OS
	*/
	class IConsoleInfo
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		IConsoleInfo(){}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~IConsoleInfo(){}
		/**
		 *\~english
		 *\brief		Configures console info for the given log type
		 *\param[in]	p_eLogType	The log type
		 *\~french
		 *\brief		Configure les informationss de la console en fonction du type donné
		 *\param[in]	p_eLogType	Le type de log donné
		 */
		virtual void BeginLog( eLOG_TYPE p_eLogType )=0;
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked
		 *\param[in]	p_strToLog	The text to log
		 *\param[in]	p_bNewLine	Tells if the line end character must be added
		 *\~french
		 *\brief		Ecrit un texte dans la console, ajoute le caractère de fin de ligne si demandé
		 *\param[in]	p_strToLog	Le texte à écrire
		 *\param[in]	p_bNewLine	Dit si on doit ajouter le caractère de fin de ligne
		 */
		virtual void Print( String const & p_strToLog, bool p_bNewLine )=0;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		27/08/2012
	\version	0.7.0.0
	\~english
	\brief		Console base class
	\~french
	\brief		Classe de base d'une console
	*/
	class ProgramConsole
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		ProgramConsole(){}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ProgramConsole(){}
		/**
		 *\~english
		 *\brief		Configures console info for the given log type
		 *\param[in]	p_eLogType	The log type
		 *\~french
		 * Configure les informationss de la console en fonction du type donné
		 *\param[in]	p_eLogType	Le type de log donné
		 */
		virtual void BeginLog( eLOG_TYPE p_eLogType )=0;
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked
		 *\param[in]	p_strToLog	The text to log
		 *\param[in]	p_bNewLine	Tells if the line end character must be added
		 *\~french
		 *\brief		Ecrit un texte dans la console, ajoute le caractère de fin de ligne si demandé
		 *\param[in]	p_strToLog	Le texte à écrire
		 *\param[in]	p_bNewLine	Dit si on doit ajouter le caractère de fin de ligne
		 */
		virtual void Print( String const & p_strToLog, bool p_bNewLine )=0;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		27/08/2012
	\version	0.7.0.0
	\~english
	\brief		Console dummy class
	\~french
	\brief		Classe sans action de console
	*/
	class DummyConsole : public ProgramConsole
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		DummyConsole(){}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DummyConsole(){}
		/**
		 *\~english
		 *\brief		Configures console info for the given log type
		 *\param[in]	p_eLogType	The log type
		 *\~french
		 * Configure les informationss de la console en fonction du type donné
		 *\param[in]	p_eLogType	Le type de log donné
		 */
		virtual void BeginLog( eLOG_TYPE ){}
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked
		 *\param[in]	p_strToLog	The text to log
		 *\param[in]	p_bNewLine	Tells if the line end character must be added
		 *\~french
		 *\brief		Ecrit un texte dans la console, ajoute le caractère de fin de ligne si demandé
		 *\param[in]	p_strToLog	Le texte à écrire
		 *\param[in]	p_bNewLine	Dit si on doit ajouter le caractère de fin de ligne
		 */
		virtual void Print( String const & , bool ){}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Console management class
	\~french
	\brief		Classe de gestion de la console de debug
	*/
	class DebugConsole : public ProgramConsole
	{
	private:
		IConsoleInfo * m_pConsoleInfo;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		DebugConsole();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DebugConsole();
		/**
		 *\~english
		 *\brief		Configures console info for the given log type
		 *\param[in]	p_eLogType	The log type
		 *\~french
		 * Configure les informationss de la console en fonction du type donné
		 *\param[in]	p_eLogType	Le type de log donné
		 */
		virtual void BeginLog( eLOG_TYPE p_eLogType );
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked
		 *\param[in]	p_strToLog	The text to log
		 *\param[in]	p_bNewLine	Tells if the line end character must be added
		 *\~french
		 *\brief		Ecrit un texte dans la console, ajoute le caractère de fin de ligne si demandé
		 *\param[in]	p_strToLog	Le texte à écrire
		 *\param[in]	p_bNewLine	Dit si on doit ajouter le caractère de fin de ligne
		 */
		virtual void Print( String const & p_strToLog, bool p_bNewLine );
	};
	/**
	 *\~english
	 *\brief		Logging callback function
	 *\param[in]	p_pCaller	Pointer to the caller
	 *\param[in]	p_strLog	The logged text
	 *\param[in]	p_eLogType	The log type
	 *\~french
	 *\brief		Fonction de callback de log
	 *\param[in]	p_pCaller	Pointeur sur l'appelant
	 *\param[in]	p_strLog	Le texte écrit
	 *\param[in]	p_eLogType	Le type de log
	 */
	typedef void (LogCallback)( void * p_pCaller, String const & p_strLog, eLOG_TYPE p_eLogType );
	/**
	 *\~english
	 *\brief		Typedef over a pointer to the logging callback function
	 *\~french
	 *\brief		Typedef d'un pointeur sur la fonction de callback de log
	 */
	typedef LogCallback * PLogCallback;
	class Logger;
	/*!
	\author 	Sylvain DOREMUS
	\date 		27/08/2012
	\version	0.7.0.0
	\~english
	\brief		Helper class for Logger, level dependant
	\~french
	\brief		Class ed'aide pour Logger, dépendante du niveau de log
	*/
	class ILoggerImpl
	{
	private:
		struct stLOGGER_CALLBACK
		{
			PLogCallback	m_pfnCallback;
			void *			m_pCaller;
		};
		DECLARE_MAP( std::thread::id, stLOGGER_CALLBACK, LoggerCallback );
		String				m_logFilePath[eLOG_TYPE_COUNT];
		String				m_strHeaders[eLOG_TYPE_COUNT];
		ProgramConsole *	m_pConsole;
		eLOG_TYPE			m_eLogLevel;
		LoggerCallbackMap	m_mapCallbacks;
		std::mutex			m_mutex;

	public:
		ILoggerImpl( eLOG_TYPE p_eLogLevel );
		virtual ~ILoggerImpl();

		void Initialise		( Logger * p_pLogger );
		void SetCallback	( PLogCallback p_pfnCallback, void * p_pCaller );

		void SetFileName	( String const & p_logFilePath, eLOG_TYPE p_eLogType );

		virtual void LogDebug	( String const & p_strToLog );
		virtual void LogMessage	( String const & p_strToLog );
		virtual void LogWarning	( String const & p_strToLog );
		virtual bool LogError	( String const & p_strToLog );

		inline eLOG_TYPE	GetLogLevel	()const { return m_eLogLevel; }

	private:
		void DoLogMessage( String const & p_strToLog, eLOG_TYPE p_eLogType );
	};
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
		friend class ILoggerImpl;
		static bool				m_bOwnInstance;
		static Logger*			m_pSingleton;
		static uint32_t			m_uiCounter;
		ILoggerImpl *			m_pImpl;
		std::mutex				m_mutex;
		String					m_strHeaders[eLOG_TYPE_COUNT];

		DECLARE_MEMORY_MANAGER_INSTANCE( m_pMemoryManager );

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
		static void SetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType=eLOG_TYPE_COUNT );
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
		 *\param[in]	p_bThrow	Tells whether or not this functions throws an exception
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::string
		 *\param[in]	p_msg		The line to log
		 *\param[in]	p_bThrow	Définit si cette fonction lance une exception
		 */
		static void LogError( std::string const & p_msg, bool p_bThrow=true )throw( bool );
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
		 *\param[in]	p_bThrow	Tells whether or not this functions throws an exception
		 *\~french
		 *\brief		Log une erreur, à partir d'un std::wstring
		 *\param[in]	p_msg		The line to log
		 *\param[in]	p_bThrow	Définit si cette fonction lance une exception
		 */
		static void LogError( std::wstring const & p_msg, bool p_bThrow=true )throw( bool );
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
		void DoSetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType=eLOG_TYPE_COUNT );
	};
}

#pragma warning( pop)

#endif
