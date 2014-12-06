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
#ifndef ___CASTOR_CONSOLE_H___
#define ___CASTOR_CONSOLE_H___

#include "StringUtils.hpp"
#include "MultiThreadConfig.hpp"

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
	typedef enum eLOG_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eLOG_TYPE_DEBUG,	//!<\~english Debug type log		\~french Log de type debug
		eLOG_TYPE_MESSAGE,	//!<\~english Message type log		\~french Log de type message
		eLOG_TYPE_WARNING,	//!<\~english Warning type log		\~french Log de type avertissement
		eLOG_TYPE_ERROR,	//!<\~english Error type log		\~french Log de type erreur
		eLOG_TYPE_COUNT,	//!<\~english Number of log types	\~french Compte des logs
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
		IConsoleInfo() {}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~IConsoleInfo() {}
		/**
		 *\~english
		 *\brief		Configures console info for the given log type
		 *\param[in]	p_eLogType	The log type
		 *\~french
		 *\brief		Configure les informationss de la console en fonction du type donné
		 *\param[in]	p_eLogType	Le type de log donné
		 */
		virtual void BeginLog( eLOG_TYPE p_eLogType ) = 0;
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
		virtual void Print( String const & p_strToLog, bool p_bNewLine ) = 0;
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
		ProgramConsole() {}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ProgramConsole() {}
		/**
		 *\~english
		 *\brief		Configures console info for the given log type
		 *\param[in]	p_eLogType	The log type
		 *\~french
		 * Configure les informationss de la console en fonction du type donné
		 *\param[in]	p_eLogType	Le type de log donné
		 */
		virtual void BeginLog( eLOG_TYPE p_eLogType ) = 0;
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
		virtual void Print( String const & p_strToLog, bool p_bNewLine ) = 0;
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
	class DummyConsole
		: public ProgramConsole
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		DummyConsole() {}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DummyConsole() {}
		/**
		 *\~english
		 *\brief		Configures console info for the given log type
		 *\param[in]	p_eLogType	The log type
		 *\~french
		 * Configure les informationss de la console en fonction du type donné
		 *\param[in]	p_eLogType	Le type de log donné
		 */
		virtual void BeginLog( eLOG_TYPE ) {}
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
		virtual void Print( String const &, bool ) {}
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
	class DebugConsole
		: public ProgramConsole
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
}

#pragma warning( pop )

#endif
