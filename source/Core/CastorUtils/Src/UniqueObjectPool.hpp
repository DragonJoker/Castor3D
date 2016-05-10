/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CU_UNIQUE_OBJECT_POOL_H___
#define ___CU_UNIQUE_OBJECT_POOL_H___

#include "ObjectPool.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		Unique pool instance.
	\param		Object		The pool objects type.
	\param		MemDataType	The allocation/deallocation policy type.
	\~french
	\brief		Instance unique de pool.
	\param		Object		Le type des objets du pool.
	\param		MemDataType	Le type de la politique d'allocation/désallocation.
	*/
	template< typename Object, eMEMDATA_TYPE MemDataType >
	class UniqueObjectPool
	{
		using MyObjectPool = ObjectPool< Object, MemDataType >;

	public:
		/**
		 *\~english
		 *\return		The unique object pool instance.
		 *\~french
		 *\return		L'instance unique du pool d'objets.
		 */
		static inline MyObjectPool & Get()
		{
			return *DoGetInstance();
		}
		/**
		 *\~english
		 *\brief		Creates the unique object pool instance.
		 *\param[in]	p_count	The object pool initial objects count.
		 *\~french
		 *\brief		Crée l'instance unique du pool d'objets.
		 *\param[in]	p_count	Le compte initial des objets du pool.
		 */
		static inline void Create( uint32_t p_count = 256u )
		{
			if ( !DoGetInstance() )
			{
				DoGetInstance() = new MyObjectPool( p_count );
			}
		}
		/**
		 *\~english
		 *\brief		Destroys the unique object pool instance.
		 *\~french
		 *\brief		Détruit l'instance unique du pool d'objets.
		 */
		static inline void Destroy()
		{
			if ( DoGetInstance() )
			{
				delete DoGetInstance();
				DoGetInstance() = nullptr;
			}
		}

	private:
		static inline MyObjectPool *& DoGetInstance()
		{
			static MyObjectPool * s_instance = nullptr;
			return s_instance;
		}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		Unique pool instance.
	\param		Object		The pool objects type.
	\param		MemDataType	The allocation/deallocation policy type.
	\~french
	\brief		Instance unique de pool.
	\param		Object		Le type des objets du pool.
	\param		MemDataType	Le type de la politique d'allocation/désallocation.
	*/
	template< typename Object, eMEMDATA_TYPE MemDataType, uint32_t Align >
	class UniqueAlignedObjectPool
	{
		using MyAlignedObjectPool = AlignedObjectPool< Object, MemDataType, Align >;

	public:
		/**
		 *\~english
		 *\return		The unique object pool instance.
		 *\~french
		 *\return		L'instance unique du pool d'objets.
		 */
		static inline MyAlignedObjectPool & Get()
		{
			return *DoGetInstance();
		}
		/**
		 *\~english
		 *\brief		Creates the unique object pool instance.
		 *\param[in]	p_count	The object pool initial objects count.
		 *\~french
		 *\brief		Crée l'instance unique du pool d'objets.
		 *\param[in]	p_count	Le compte initial des objets du pool.
		 */
		static inline void Create( uint32_t p_count = 256u )
		{
			if ( !DoGetInstance() )
			{
				DoGetInstance() = new MyAlignedObjectPool( p_count );
			}
		}
		/**
		 *\~english
		 *\brief		Destroys the unique object pool instance.
		 *\~french
		 *\brief		Détruit l'instance unique du pool d'objets.
		 */
		static inline void Destroy()
		{
			if ( DoGetInstance() )
			{
				delete DoGetInstance();
				DoGetInstance() = nullptr;
			}
		}

	private:
		static inline MyAlignedObjectPool *& DoGetInstance()
		{
			static MyAlignedObjectPool * s_instance = nullptr;
			return s_instance;
		}
	};
}

#endif
