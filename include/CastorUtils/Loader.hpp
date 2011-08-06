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
#ifndef ___Castor_ResourceLoader___
#define ___Castor_ResourceLoader___

#include "Exception.hpp"
#include "Resource.hpp"

namespace Castor
{	namespace Resources
{
	//! Resource loading exception
	/*!
	Thrown if the resource loader has encountered an error
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class LoadingError : public Castor::Utils::Exception
	{
	public:
		LoadingError( String const & p_description, String const & p_file, String const & p_function, unsigned int p_line)
			:	Exception( p_description, p_file, p_function, p_line)
		{}
	};

	//! Helper macro to use ResourceLoadingError
#	define LOADER_ERROR( p_text) throw LoadingError( p_text, __FILE__, __FUNCTION__, __LINE__)

	//! Main resource loader class
	/*!
	Holds the two functions needed for a resource loader : load and save
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <class T>
	class Loader
	{
	private:
		typedef shared_ptr<T> obj_ptr;

		Loader(){}
		~Loader(){}
		Loader( Loader const &){}
		Loader & operator =( Loader const &){ return * this; }

	public:
		/**
		 * Loads a resource from a binary file
		 *@param p_object : [in/out] The object to load
		 *@param p_file : [in/out] The file where to load the object
		 */
		static bool Load( T & p_object, Utils::File & p_file)
		{
			LOADER_ERROR( "Binary import not supported by the loader registered for this format");
		}
		/**
		 * Loads a resource from a binary file
		 *@param p_object : [in/out] The object to load
		 *@param p_pathFile : [in] The path of the file where to load the object
		 */
		static bool Load( T & p_object, Utils::Path const & p_pathFile)
		{
			return Load( p_object, Utils::File( p_pathFile, Utils::File::eOPEN_MODE_READ));
		}
		/**
		 * Saves a resource to a binary file
		 *@param p_object : [in] The object to save
		 *@param p_file : [in/out] The file where to save the object
		 */
		static bool Save( T const & p_object, Utils::File & p_file)
		{
			LOADER_ERROR( "Binary export not supported by the loader registered for this format");
		}
		/**
		 * Saves a resource to a binary file
		 *@param p_object : [in] The object to save
		 *@param p_pathFile : [in] The path of the file where to save the object
		 */
		static bool Save( T const & p_object, Utils::Path const & p_pathFile)
		{
			return Save( p_object, Utils::File( p_pathFile, Utils::File::eOPEN_MODE_READ));
		}

		/**
		 * Reads a resource from a text file
		 *@param p_object : [in/out] The object to read
		 *@param p_file : [in/out] The file where to read the object
		 */
		static bool Read( T & p_object, Utils::File & p_file)
		{
			LOADER_ERROR( "Text import not supported by the loader registered for this format");
		}
		/**
		 * Reads a resource from a text file
		 *@param p_object : [in/out] The object to read
		 *@param p_pathFile : [in] The path of the file where to read the object
		 */
		static bool Read( T & p_object, Utils::Path const & p_pathFile)
		{
			return Read( p_object, Utils::File( p_pathFile, Utils::File::eOPEN_MODE_READ));
		}
		/**
		 * Writes a resource to a text file
		 *@param p_object : [in] The object to write
		 *@param p_file : [in/out] The file where to write the object
		 */
		static bool Write( T const & p_object, Utils::File & p_file)
		{
			LOADER_ERROR( "Text export not supported by the loader registered for this format");
		}
		/**
		 * Writes a resource to a text file
		 *@param p_object : [in] The object to write
		 *@param p_pathFile : [in] The path of the file where to write the object
		 */
		static bool Write( T const & p_object, Utils::Path const & p_pathFile)
		{
			return Write( p_object, Utils::File( p_pathFile, Utils::File::eOPEN_MODE_READ));
		}
	};
}
}

#endif
