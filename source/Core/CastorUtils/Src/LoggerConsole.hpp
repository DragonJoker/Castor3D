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
#ifndef ___CU_CONSOLE_H___
#define ___CU_CONSOLE_H___

#include "CastorUtilsPrerequisites.hpp"

#include "ELogType.hpp"

namespace Castor
{
	/** Console info class, Platform dependent
	*/
	class IConsole
	{
	public:
		/** Constructor
		*/
		IConsole()
		{
		}

		/** Destructor
		*/
		virtual ~IConsole()
		{
		}

		/** Configures console info for the given log type
		*\param[in]	logLevel		 	The log leve
		*/
		virtual void BeginLog( ELogType p_logLevel ) = 0;

		/** Prints a text in the console, adds the line end character if asked
		*\param[in]	toLog		The text to lo
		*\param[in]	newLine		 	Tells if the line end character must be adde
		*/
		virtual void Print( String const & toLog, bool newLine ) = 0;
	};

	/* Console base class
	*/
	class ProgramConsole
	{
	public:
		/** Constructor
		*/
		ProgramConsole()
		{
		}

		/** Destructor
		*/
		virtual ~ProgramConsole()
		{
		}

		/** Configures console info for the given log type
		*\param[in]	logLevel		The log typ
		*/
		virtual void BeginLog( ELogType logLevel ) = 0;

		/** Prints a text in the console, adds the line end character if asked
		*\param[in]	toLog		The text to lo
		*\param[in]	newLine		Tells if the line end character must be adde
		*/
		virtual void Print( String const & toLog, bool newLine ) = 0;

	protected:
		//! The console
		std::unique_ptr< IConsole > m_console;
	};

	/* Traditional consol class
	*/
	class DefaultConsole
		: public ProgramConsole
	{
	public:
		/** Constructor
		*/
		DefaultConsole();

		/** Destructor
		*/
		virtual ~DefaultConsole();

		/** @copydoc Database::ProgramConsole::BeginLog
		*/
		virtual void BeginLog( ELogType logLevel );

		/** @copydoc Database::ProgramConsole::Print
		*/
		virtual void Print( String const & toLog, bool newLine );
	};

	/** Debug console class, adds colour on MSW
	*/
	class DebugConsole
		: public ProgramConsole
	{
	public:
		/** Constructor
		*/
		DebugConsole();

		/** Destructor
		*/
		virtual ~DebugConsole();

		/** @copydoc Database::ProgramConsole::BeginLog
		*/
		virtual void BeginLog( ELogType logLevel );

		/** @copydoc Database::ProgramConsole::Print
		*/
		virtual void Print( String const & toLog, bool newLine );
	};
}

#endif
