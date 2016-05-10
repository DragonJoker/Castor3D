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
#ifndef ___CU_CONSOLE_H___
#define ___CU_CONSOLE_H___

#include "CastorUtilsPrerequisites.hpp"

#include "ELogType.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Console info class, platform dependent.
	\~french
	\brief		Classe d'informations de console, dépendante de la plateforme.
	*/
	class IConsole
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		IConsole()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~IConsole()
		{
		}
		/**
		 *\~english
		 *\brief		Configures console for the given log type
		 *\param[in]	p_logLevel	The log level
		 *\~french
		 *\brief		Configure la console pour le niveau de log demandé
		 *\param[in]	p_logLevel	Le niveau de log
		 */
		virtual void BeginLog( ELogType p_logLevel ) = 0;
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked
		 *\param[in]	p_toLog		The text to log
		 *\param[in]	p_newLine	Tells if the line end character must be added
		 *\~french
		 *\brief		Affiche un texte dans la console, ajoute le caractère de fin de ligne si demandé
		 *\param[in]	p_toLog		Le texte à logger
		 *\param[in]	p_newLine	Dit si le caractère de fin de ligne doit être ajouté
		 */
		virtual void Print( String const & p_toLog, bool p_newLine ) = 0;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
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
		ProgramConsole()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ProgramConsole()
		{
		}
		/**
		 *\~english
		 *\brief		Configures console for the given log type
		 *\param[in]	p_logLevel	The log level
		 *\~french
		 *\brief		Configure la console pour le niveau de log demandé
		 *\param[in]	p_logLevel	Le niveau de log
		 */
		virtual void BeginLog( ELogType p_logLevel ) = 0;
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked
		 *\param[in]	p_toLog		The text to log
		 *\param[in]	p_newLine	Tells if the line end character must be added
		 *\~french
		 *\brief		Affiche un texte dans la console, ajoute le caractère de fin de ligne si demandé
		 *\param[in]	p_toLog		Le texte à logger
		 *\param[in]	p_newLine	Dit si le caractère de fin de ligne doit être ajouté
		 */
		virtual void Print( String const & p_toLog, bool p_newLine ) = 0;

	protected:
		//! The console
		std::unique_ptr< IConsole > m_console;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Traditional consol class
	\~french
	\brief		Classe de console traditionnelle
	*/
	class DefaultConsole
		: public ProgramConsole
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		DefaultConsole();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DefaultConsole();
		/**
		 *\~english
		 *\brief		Configures console for the given log type
		 *\param[in]	p_logLevel	The log level
		 *\~french
		 *\brief		Configure la console pour le niveau de log demandé
		 *\param[in]	p_logLevel	Le niveau de log
		 */
		virtual void BeginLog( ELogType p_logLevel );
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked
		 *\param[in]	p_toLog		The text to log
		 *\param[in]	p_newLine	Tells if the line end character must be added
		 *\~french
		 *\brief		Affiche un texte dans la console, ajoute le caractère de fin de ligne si demandé
		 *\param[in]	p_toLog		Le texte à logger
		 *\param[in]	p_newLine	Dit si le caractère de fin de ligne doit être ajouté
		 */
		virtual void Print( String const & p_toLog, bool p_newLine );
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Debug console class, adds colours when possible
	\~french
	\brief		Classe de console de débogage, ajoutant des couleurs si possible
	*/
	class DebugConsole
		: public ProgramConsole
	{
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
		 *\brief		Configures console for the given log type
		 *\param[in]	p_logLevel	The log level
		 *\~french
		 *\brief		Configure la console pour le niveau de log demandé
		 *\param[in]	p_logLevel	Le niveau de log
		 */
		virtual void BeginLog( ELogType p_logLevel );
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked
		 *\param[in]	p_toLog		The text to log
		 *\param[in]	p_newLine	Tells if the line end character must be added
		 *\~french
		 *\brief		Affiche un texte dans la console, ajoute le caractère de fin de ligne si demandé
		 *\param[in]	p_toLog		Le texte à logger
		 *\param[in]	p_newLine	Dit si le caractère de fin de ligne doit être ajouté
		 */
		virtual void Print( String const & p_toLog, bool p_newLine );
	};
}

#endif
