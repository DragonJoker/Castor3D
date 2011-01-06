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
#ifndef ___Castor_Path___
#define ___Castor_Path___

#ifdef _WIN32
#	define d_path_slash	'\\'
#else
#	define d_path_slash '/'
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
		/**@name Constructors */
		//@{
		inline Path()
		{}
		inline Path( const char * p_data)
			:	String( p_data)
		{
			_normalise();
		}
		inline Path( const String & p_data)
			:	String( p_data)
		{
			_normalise();
		}
		inline Path( const Path & p_data)
			:	String( p_data)
		{
			_normalise();
		}
		//@}

		/**
		 * Returns a concatenated path from the current path and the one in parameter.
		 * Adds the needed separator if needed
		 *@param p_path : [in] The path to add
		 */
		inline const Path operator /(const Path & p_path)const
		{
			Path l_path( * this);
			l_path.push_back( d_path_slash);
			l_path.append( p_path);
			return l_path;
		}
		/**
		 * Adds a path to the current path. Adds the needed separator if needed
		 *@param p_path : [in] The path to add
		 */
		inline Path & operator /=( const Path & p_path)
		{
			push_back( d_path_slash);
			append( p_path);
			return *this;
		}

		/**@name Accessors */
		//@{
		inline const String GetPath()const
		{
			String l_strReturn;

			size_t l_index = find_last_of( d_path_slash);

			if (l_index != String::npos)
			{
				l_strReturn = substr( 0, l_index);
			}

			return l_strReturn;
		}
		inline const String GetLeaf()const
		{
			String l_strReturn = ( * this);
			size_t l_index = find_last_of( d_path_slash);

			if (l_index != String::npos)
			{
				l_strReturn = substr( l_index + 1, String::npos);
			}

			return l_strReturn;
		}
		//@}

	private:
		void _normalise()
		{
			Replace( '\\', d_path_slash);
			Replace( '/', d_path_slash);
		}
	};
}
}

#endif
