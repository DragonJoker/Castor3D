/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___Castor_Exception___
#define ___Castor_Exception___

#include "Module_Utils.h"

namespace Castor
{ namespace Utils
{
	class Exception : public std::exception
	{
	public:
		unsigned int	m_line;
		String			m_description;
		String			m_filename;
		String			m_functionName;
		const Char *	m_typeDesc;

	public:
		Exception(	const String & p_description, const String & p_file,
						const String & p_function, unsigned int p_line)
			:	m_line			(p_line),
				m_description	(p_description),
				m_filename		(p_file),
				m_functionName	(p_function)
		{}
		virtual ~Exception() throw() {}

	public:
		inline virtual const char * what() const throw() { return m_description.char_str(); }
		inline const String & GetFilename()const { return m_filename; }
		inline const String & GetFunction()const { return m_functionName; }
		inline unsigned int GetLine()const { return m_line; }
		inline const String & GetDescription()const { return m_description; }
	};
}
}

#	define CASTOR_EXCEPTION( p_text) throw Castor::Utils::Exception( p_text, __FILE__, __FUNCTION__, __LINE__)

#ifndef CASTOR_USE_ASSERT
#	define CASTOR_USE_ASSERT 1
#endif

#if CASTOR_USE_ASSERT
#	ifdef CASTOR_ASSERT
#		undef CASTOR_ASSERT
#	endif
#	define CASTOR_ASSERT( X) if ( ! (X)){ CASTOR_EXCEPTION( Castor::String( "Assert not respected : (") + #X + ") file " + __FILE__ + " @ L# " + Castor::ToString( __LINE__));}
#else
#	define CASTOR_ASSERT( X)
#endif

#endif
