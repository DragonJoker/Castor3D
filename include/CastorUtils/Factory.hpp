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
#ifndef ___Castor_Factory___
#define ___Castor_Factory___

namespace Castor
{	namespace Resources
{
	//! Creator class

	//! Factory class
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <class Obj, class Key=String>
	class Factory
	{
	private:
		typedef shared_ptr<Obj> obj_ptr;
		typedef std::map<Key, obj_ptr> obj_map;

		static obj_map m_mapRegistered;

	private:
		Factory( Factory const &){}
		Factory & operator =( Factory const &){ return * this; }

	public:
		Factory(){}
		~Factory(){}
		/**
		 * Registers an object type
		 *@param p_key : [in] The object type
		 *@param p_object : [in] The object to register
		 */
		template <class SubObj>
		void Register( Key const & p_key, shared_ptr<SubObj> p_obj)
		{
			if (m_mapRegistered.find( p_key) == m_mapRegistered.end())
			{
				m_mapRegistered.insert( obj_map::value_type( p_key, static_pointer_cast<Obj>( p_obj)));
			}
		}
		/**
		 * Creates an object from a key
		 *@param p_key : [in] The object type
		 */
		obj_ptr Create( Key const & p_key)
		{
			obj_ptr l_pReturn;
			obj_map::iterator l_it = m_mapRegistered.find( p_key);

			if (l_it != m_mapRegistered.end())
			{
				l_pReturn = l_it->second->Clone();
			}

			return l_pReturn;
		}
	};

	template <class Final>
	class Clonable
	{
	public:
		Clonable(){}
		virtual ~Clonable(){}

		virtual shared_ptr<Final> Clone()=0;
	};
}
}

#endif
