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
#ifndef ___Castor_ResourceLoader___
#define ___Castor_ResourceLoader___

#include "Exception.h"
#include "Resource.h"

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
	class ResourceLoadingError : public Castor::Utils::Exception
	{
	public:
		ResourceLoadingError(	const String & p_description, const String & p_file,
			const String & p_function, unsigned int p_line)
			:	Exception( p_description, p_file, p_function, p_line)
		{}
	};

	//! Helper macro to use ResourceLoadingError
#	define LOADER_ERROR( p_text) throw ResourceLoadingError( p_text, __FILE__, __FUNCTION__, __LINE__)

	//! Unsupported format exception
	/*!
	Thrown if no resource loader is defined for wanted format
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class UnsupportedFormatError : public Castor::Utils::Exception
	{
	public:
		UnsupportedFormatError(	const String & p_description, const xchar * p_file,
			const xchar * p_function, unsigned int p_line)
			:	Exception( p_description, p_file, p_function, p_line)
		{}
	};

	//! Helper macro to use UnsupportedFormatError
#	define UNSUPPORTED_ERROR( p_text) throw UnsupportedFormatError( p_text, __FILE__, __FUNCTION__, __LINE__)

	//! Main resource loader class
	/*!
	Holds the two functions needed for a resource loader : load and save
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <class T>
	class ResourceLoader
	{
	public:
		/**
		 * Loads a resource from a file
		 */
		shared_ptr<T> LoadFromFile( Templates::Manager<T> * p_pManager, const String & p_file)
		{
			LOADER_ERROR( "Le loader enregistré pour ce format ne prend pas en charge l'importation");
		}
		/**
		 * Saves a resource to a file
		 */
		bool SaveToFile( const String & p_file, shared_ptr<T> p_resource)
		{
			LOADER_ERROR( "Le loader enregistré pour ce format ne prend pas en charge l'exportation");
		}
	};
}
}

#endif
