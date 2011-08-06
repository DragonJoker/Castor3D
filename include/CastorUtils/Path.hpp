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
#ifndef ___Castor_Path___
#define ___Castor_Path___

#ifdef _WIN32
#	define d_path_slash	cuT( '\\')
#else
#	define d_path_slash cuT( '/')
#endif

namespace Castor
{ namespace Utils
{
	//! Path management class
	/*!
	Defines platform dependant paths. Allows access to the path and the leaf
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class Path : public String
	{
	public:
#ifdef _WIN32
		static const xchar Separator = cuT( '\\');
#else
		static const xchar Separator = cuT( '/');
#endif
	public:
		/**@name Constructors */
		//@{
		Path();
		Path( char const * p_data);
		Path( wchar_t const * p_data);
		Path( String const & p_data);
		Path( Path const & p_data);
		//@}

		/**
		 * Adds a path to the current path. Adds the needed separator if needed
		 *@param p_path : [in] The path to add
		 */
		Path & operator /=( Path const & p_path);
		/**
		 * Adds a string to the current path. Adds the needed separator if needed
		 *@param p_path : [in] The path to add
		 */
		Path & operator /=( String const & p_string);
		/**
		 * Adds a C string to the current path. Adds the needed separator if needed
		 *@param p_path : [in] The path to add
		 */
		Path & operator /=( char const * p_pBuffer);
		/**
		 * Adds a C string to the current path. Adds the needed separator if needed
		 *@param p_path : [in] The path to add
		 */
		Path & operator /=( wchar_t const * p_pBuffer);

		/**@name Accessors */
		//@{
		Path	GetPath			()const;
		String	GetFileName		()const;
		String	GetFullFileName	()const;
		String	GetExtension	()const;
		//@}

	private:
		void _normalise();
	};

	Path operator /( Path const & p_pathA, Path const & p_pathB);
	Path operator /( Path const & p_path, String const & p_string);
	Path operator /( Path const & p_path, char const * p_pBuffer);
	Path operator /( Path const & p_path, wchar_t const * p_pBuffer);
	Path operator /( String const & p_string, Path const & p_path);
	Path operator /( char const * p_pBuffer, Path const & p_path);
	Path operator /( wchar_t const * p_pBuffer, Path const & p_path);
}
}

#endif
