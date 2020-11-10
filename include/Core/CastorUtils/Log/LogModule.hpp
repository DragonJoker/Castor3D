/*
See LICENSE file in root folder
*/
#ifndef ___CU_LogModule_HPP___
#define ___CU_LogModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

#include "ELogType.hpp"

#include <deque>

namespace castor
{
	/**@name Log */
	//@{
	/**
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
	/**
	\~english
	\brief		Log management class
	\remarks	Implements log facilities. Create a Log with a filename, then write logs into that file
	\~french
	\brief		Classe de gestion de logs
	\remarks	Implémente les fonctions de logging. Initialise un log avec un nom de fichier puis écrit dedans
	*/
	class Logger;
	/**
	\~english
	\brief		Helper class for Logger, level dependant
	\~french
	\brief		Classe d'aide pour Logger, dépendante du niveau de log
	*/
	class LoggerImpl;
	/**
	\~english
	\brief		Logger instance class.
	\remarks	Implements log facilities. Create a Log with a filename, then write logs into that file.
	\~french
	\brief		Instance de logger.
	\remarks	Implémente les fonctions de logging. Initialise un log avec un nom de fichier puis écrit dedans
	*/
	class LoggerInstance;
	/**
	\~english
	\brief		Console info class, platform dependent.
	\~french
	\brief		Classe d'informations de console, dépendante de la plateforme.
	*/
	class ConsoleImpl;
	template< typename CharT, typename TraitsT >
	class LoggerStreambufT;
	/**
	\~english
	\brief		Console base class.
	\~french
	\brief		Classe de base d'une console.
	*/
	class ProgramConsole;
	template< typename CharT
		, template< typename T > typename StreambufT >
	struct LoggerStreamT;
	/**
	 *\~english
	 *\brief		Logging callback function.
	 *\param[in]	text	The logged text.
	 *\param[in]	type	The log type.
	 *\param[in]	newLine	Tells if we add the end line character.
	 *\~french
	 *\brief		Fonction de callback de log.
	 *\param[in]	text	Le texte écrit.
	 *\param[in]	type	Le type de log.
	 *\param[in]	newLine	Dit si on ajoute le caractère de fin de ligne.
	 */
	using LogCallback = std::function< void( String const & text, LogType type, bool newLine ) >;

	using LoggerInstancePtr = std::unique_ptr< LoggerInstance >;
	//@}
}

#endif
