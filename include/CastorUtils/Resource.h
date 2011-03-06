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
#ifndef ___Castor_Resource___
#define ___Castor_Resource___

#include "Buffer.h"
#include "Manager.h"

namespace Castor
{	namespace Resources
{
	//! External resource representation
	/*!
	A resource can be created only once, can be loaded and unloaded
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <typename ResType>
	class Resource
	{
	protected:
		typedef Templates::Manager<ResType> ResourceManager;

		int * m_references;		//!< The number of times this resource is used
		ResourceManager * m_pManager;
		String m_strName;

		/**
		 * Constructor, needs the name
		 */
		Resource( ResourceManager * p_pManager, const String & p_name)
			:	m_references( new int( 1))
			,	m_pManager( p_pManager)
			,	m_strName( p_name)
		{
		}

	public:
		/**
		 * virtual destructor
		 */
		virtual ~Resource()
		{
		}
		void Ref()
		{
			*m_references++;
		}
		void Release()
		{
			*m_references--;

			if (*m_references == 0)
			{
				m_pManager->RemoveElement( m_strName);
				delete this;
			}
		}
		void SetName( const String & p_strName)
		{
			if ( ! m_pManager->HasElement( p_strName))
			{
				shared_ptr<ResType> l_pThis = m_pManager->RemoveElement( m_strName);

				if (l_pThis != NULL)
				{
					m_strName = p_strName;
					m_pManager->AddElement( l_pThis);
				}
			}
		}
		inline String GetName()const { return m_strName; }
	};
}
}

#endif
