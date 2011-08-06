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
#ifndef ___Castor_Collection___
#define ___Castor_Collection___

#include "SmartPtr.hpp"
#include "Value.hpp"

#pragma warning( push)
#pragma warning( disable:4661)

namespace Castor
{ namespace Templates
{
	//! Element collection class
	/*!
	A collection class, allowing you to store named objects, removing, finding or adding them as you wish.
	It uses the monostate design pattern, to have only one manager of each type of object,
	which holds all objects created by each instance of it
	*/
	template <typename TObj, typename TKey>
	class Collection
	{
	public:
		typedef shared_ptr<TObj>						obj_ptr;
		typedef std::map<TKey, obj_ptr>					obj_map;
		typedef typename obj_map::iterator				iterator;
		typedef typename obj_map::const_iterator		const_iterator;
		typedef typename obj_map::value_type			value_type;
		typedef typename call_traits<TKey>::param_type	key_param_type;

	private:
		static obj_map m_objectMap;
		Castor::MultiThreading::RecursiveMutex m_mutex;
		mutable bool m_bLocked;

	private:
		Collection( Collection const & p_manager);
		Collection & operator =( Collection const &);

	public:
		Collection();
		virtual ~Collection();

		inline void				Lock			()const;
		inline void				Unlock			()const;
		inline iterator			Begin			();
		inline const_iterator	Begin			()const;
		inline const_iterator	End				()const;
		inline void				Clear			() throw();
		inline obj_ptr			GetElement		( key_param_type p_key);
		inline size_t			GetElementCount	()const;
		inline bool				AddElement		( key_param_type p_key, obj_ptr p_element);
		inline bool				HasElement		( key_param_type p_key)const;
		inline obj_ptr			RemoveElement	( key_param_type p_key);
	};

#	include "Collection.inl"
	template <typename Obj, typename Key>
	typename Castor::Templates::Collection<Obj, Key>::obj_map Castor::Templates::Collection<Obj, Key>::m_objectMap;
}
}

#pragma warning( pop)

#endif
