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
#ifndef ___CASTOR_LOGGER_IMPL_H___
#define ___CASTOR_LOGGER_IMPL_H___

#include "Console.hpp"

#pragma warning( push )
#pragma warning( disable:4290 )

namespace Castor
{
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
	typedef void ( LogCallback )( void * p_pCaller, String const & p_strLog, eLOG_TYPE p_eLogType );
	/**
	 *\~english
	 *\brief		Typedef over a pointer to the logging callback function
	 *\~french
	 *\brief		Typedef d'un pointeur sur la fonction de callback de log
	 */
	typedef LogCallback * PLogCallback;
	/*!
	\author 	Sylvain DOREMUS
	\date 		27/08/2012
	\version	0.7.0.0
	\~english
	\brief		Helper class for Logger, level dependant
	\~french
	\brief		Classe d'aide pour Logger, dépendante du niveau de log
	*/
	class ILoggerImpl
	{
	private:
		struct stLOGGER_CALLBACK
		{
			PLogCallback m_pfnCallback;
			void * m_pCaller;
		};

		DECLARE_MAP( std::thread::id, stLOGGER_CALLBACK, LoggerCallback );

	public:
		CU_API ILoggerImpl( eLOG_TYPE p_eLogLevel );
		CU_API virtual ~ILoggerImpl();

		CU_API void Initialise( Logger * p_pLogger );
		CU_API void SetCallback( PLogCallback p_pfnCallback, void * p_pCaller );

		CU_API void SetFileName( String const & p_logFilePath, eLOG_TYPE p_eLogType );

		CU_API virtual void LogDebug( String const & p_strToLog );
		CU_API virtual void LogMessage( String const & p_strToLog );
		CU_API virtual void LogWarning( String const & p_strToLog );
		CU_API virtual bool LogError( String const & p_strToLog );

		inline eLOG_TYPE GetLogLevel()const
		{
			return m_eLogLevel;
		}

	private:
		void DoLogMessage( String const & p_strToLog, eLOG_TYPE p_eLogType );

	private:
		String m_logFilePath[eLOG_TYPE_COUNT];
		String m_strHeaders[eLOG_TYPE_COUNT];
		ProgramConsole * m_pConsole;
		eLOG_TYPE m_eLogLevel;
		LoggerCallbackMap m_mapCallbacks;
		std::mutex m_mutex;
	};
}

#pragma warning( pop)

#endif
