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
#ifndef ___Castor_DynamicLibrary___
#define ___Castor_DynamicLibrary___

#include "CastorUtils.hpp"

namespace Castor
{ namespace Utils
{
	//! Platform independant library
	/*!
	Loads a library and gives access to it's functions in a platform independant way
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class DynamicLibrary : public MemoryTraced<DynamicLibrary>
	{
	private:
		void * m_library;
		Path m_pathLibrary;

	public:
		DynamicLibrary()throw()
			:	m_library ( NULL)
		{
		}

		~DynamicLibrary()throw()
		{
			_close();
		}

	public:
		/**
		 * Opens a library from a path
		 */
		bool Open( xchar const * p_name)throw();
		bool Open( String const & p_name)throw();

		/**@name Accessors */
		//@{
		void * GetFunction( xchar const * p_name)throw();
		void * GetFunction( String const & p_name)throw();

		inline bool			IsOpen	()const { return m_library != NULL; }
		inline Path const & GetPath	()const { return m_pathLibrary; }
		//@}


	private:
		void _close()throw();
	};

	typedef shared_ptr<DynamicLibrary> DynamicLibraryPtr;
}
}

#endif
