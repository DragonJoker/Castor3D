/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
	template< typename Object, MemoryDataType MemDataType >
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
	template< typename Object, MemoryDataType MemDataType, uint32_t Align >
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
