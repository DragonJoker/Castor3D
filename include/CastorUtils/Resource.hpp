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
#ifndef ___Castor_Resource___
#define ___Castor_Resource___

#include "Buffer.hpp"
#include "Collection.hpp"

namespace Castor
{	namespace Resources
{
	//! External resource representation
	/*!
	A resource can be created only once, can be loaded and unloaded
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <class ResType>
	class Resource
	{
	protected:
		typedef Templates::Collection<ResType, String> collection;
		String m_strName;

		/**
		 * Constructor, from the name
		 */
		Resource( String const & p_name);

	public:
		Resource( Resource const & p_name);
		Resource & operator =( Resource const & p_name);
		virtual ~Resource();

		inline void AddRef();
		inline void Release();
		inline void SetName( String const & p_strName);
		inline String GetName()const { return m_strName; }
	};

#	include "Resource.inl"
}
}

#endif
