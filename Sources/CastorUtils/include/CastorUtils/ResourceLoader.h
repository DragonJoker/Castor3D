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
{	namespace Resource
{
	class ResourceLoadingError : public Castor::Utils::Exception
	{
	public:
		ResourceLoadingError(	const String & p_description, const String & p_file,
			const String & p_function, unsigned int p_line)
			:	Exception( p_description, p_file, p_function, p_line)
		{}
	};

	#	define LOADER_ERROR( p_text) throw ResourceLoadingError( p_text, __FILE__, __FUNCTION__, __LINE__)

	class UnsupportedFormatError : public Castor::Utils::Exception
	{
	public:
		UnsupportedFormatError(	const String & p_description, const Char * p_file,
			const Char * p_function, unsigned int p_line)
			:	Exception( p_description, p_file, p_function, p_line)
		{}
	};

	#	define UNSUPPORTED_ERROR( p_text) throw UnsupportedFormatError( p_text, __FILE__, __FUNCTION__, __LINE__)

	template <class T>
	class ResourceLoader
	{
	public:
		T * LoadFromFile( const String & p_file)
		{
			LOADER_ERROR( "Le loader enregistré pour ce format ne prend pas en charge l'importation");
		}
		bool SaveToFile( const String & p_file, T * p_resource)
		{
			LOADER_ERROR( "Le loader enregistré pour ce format ne prend pas en charge l'exportation");
		}
	};
}
}

#endif