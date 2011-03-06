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
#ifndef ___CU_Serialisable___
#define ___CU_Serialisable___

#pragma warning( push)
#pragma warning( disable : 4996)

namespace Castor
{	namespace Utils
{
	//! Object writable in text file
	/*!
	This is the main class for objects which can be written into and read from text files
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2011
	*/
	class Serialisable
	{
	public:
		/**
		 * Serialises the object in a binary archive
		 *@param p_archive : [in] The archive into which the object is written
		 *@param p_uiVersion : [in] The version into which the object is written
		 */
		virtual bool Save( File & p_file)const=0;
		/**
		 * Reads the object from a binary archive
		 *@param p_file : [in] The file from which the object is read
		 *@param p_uiVersion : [in] The version from which the object is read
		 */
		virtual bool Load( File & p_file)=0;
	};
	//! Object writable in text file
	/*!
	This is the main class for objects which can be written into and read from text files
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2011
	*/
	class Textable
	{
	public:
		/**
		 * Writes the object in a text archive
		 *@param p_file : [in] The file into which the object is written
		 *@param p_uiVersion : [in] The version into which the object is written
		 */
		virtual bool Write( File & p_file)const=0;
		/**
		 * Reads the object from a text archive
		 *@param p_file : [in] The file from which the object is read
		 *@param p_uiVersion : [in] The version from which the object is read
		 */
		virtual bool Read( File & p_file)=0;
	};
}
}

#pragma warning( pop)

#endif