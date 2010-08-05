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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___NON_COPYABLE_H___
#define ___NON_COPYABLE_H___

namespace General
{ namespace Theory
{
	//! The noncopyable representation
	/*!
	Forbids a class wgicg derivates from this to be copied, either way
	*/
	class noncopyable
	{
	protected:
		/**
		 * Constructor
		 */
		noncopyable(){}
		/**
		 * Destructor
		 */
		~noncopyable(){}

	private:
		/**
		 * Private constructor copy, to forbid copy by construction
		 */
		noncopyable( const noncopyable &);
		/**
		 * Private assignation operator, to forbid assignation copy
		 */
		const noncopyable & operator =( const noncopyable &);
	};
}
}
//! A define to General::Theory::noncopyable class
#define d_noncopyable	private General::Theory::noncopyable
//! A define to General::Theory::noncopyable class
#define d_unique		private General::Theory::noncopyable
#define d_debug_check_noncopyable

#endif
