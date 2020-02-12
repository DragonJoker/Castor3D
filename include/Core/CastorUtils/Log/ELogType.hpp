/*
See LICENSE file in root folder
*/
#ifndef ___CU_LOG_TYPE_H___
#define ___CU_LOG_TYPE_H___

#include <string>
#include <deque>

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Defines the various log types
	\~french
	\brief		Définit les différents types de log
	*/
	enum class LogType
		: uint8_t
	{
		//!\~english	Trace type log.
		//!\~french		Log de type trace.
		eTrace,
		//!\~english	Debug type log.
		//!\~french		Log de type debug.
		eDebug,
		//!\~english	Information type log.
		//!\~french		Log de type information.
		eInfo,
		//!\~english	Warning type log.
		//!\~french		Log de type avertissement.
		eWarning,
		//!\~english	Error type log.
		//!\~french		Log de type erreur.
		eError,
		//!\~english	Log types count.
		//!\~french		Compte des logs.
		eCount,
	};
	/**
	\author 	Sylvain DOREMUS
	\date 		27/08/2012
	\version	0.7.0.0
	\~english
	\brief		Message representation
	\~french
	\brief		Représentation d'un message
	*/
	struct Message
	{
		//! The message type.
		LogType m_type;
		//! The message text.
		std::string m_message;
		//! Tells if the new line character is printed.
		bool m_newLine;
	};
	//! The message queue.
	using MessageQueue = std::deque< Message >;
}

#endif
