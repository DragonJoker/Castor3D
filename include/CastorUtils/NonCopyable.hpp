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
#ifndef ___Castor_NonCopyable___
#define ___Castor_NonCopyable___

namespace Castor
{ namespace Theory
{
	//! The noncopyable representation
	/*!
	Forbids a class wgicg derivates from this to be copied, either way
	*/
	class NonCopyable
	{
	protected:
		/**
		 * Constructor
		 */
		NonCopyable(){}
		/**
		 * Destructor
		 */
		~NonCopyable(){}

	private:
		/**
		 * Private constructor copy, to forbid copy by construction
		 */
		NonCopyable( NonCopyable const &);
		/**
		 * Private assignation operator, to forbid assignation copy
		 */
		NonCopyable const & operator =( NonCopyable const &);
	};
}
}
//! A define to Castor::Theory::noncopyable class
#define d_noncopyable	private Castor::Theory::NonCopyable
//! A define to Castor::Theory::noncopyable class
#define d_unique		private Castor::Theory::NonCopyable
#define d_debug_check_noncopyable

#endif
