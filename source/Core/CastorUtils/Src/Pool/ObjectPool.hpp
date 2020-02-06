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
#ifndef ___CU_OBJECT_POOL_H___
#define ___CU_OBJECT_POOL_H___

#include "MemoryDataTyper.hpp"
#include "FixedGrowingSizeMarkedMemoryData.hpp"
#include "FixedGrowingSizeMemoryData.hpp"
#include "FixedSizeMarkedMemoryData.hpp"
#include "FixedSizeMemoryData.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		Policy used to construct/destruct object if needed.
	\~french
	\brief		Poliituqe utilisée pour construire/détruire un objet si nécessaire.
	*/
	template< typename Object >
	struct NewDeletePolicy
	{
		template< typename ... TParams >
		static inline Object * Ctor( Object * p_space, TParams ... p_params )
		{
			return new( p_space )Object( p_params... );
		}

		template< typename ... TParams >
		static inline void Dtor( Object * p_space )
		{
			p_space->~Object();
		}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		The objects pool.
	\remarks	Uses a policy to change behaviour easily.
	\param		Object		The pool objects type.
	\param		MemDataType	The allocation/deallocation policy type.
	\~french
	\brief		Le pool d'objets.
	\remarks	Utilisation d'une politique pour permettre de changer le comportement assez facilement.
	\param		Object		Le type des objets du pool.
	\param		MemDataType	Le type de la politique d'allocation/désallocation.
	*/
	template< typename Object, MemoryDataType MemDataType >
	class ObjectPool
		: private MemoryDataTyper< Object, MemDataType >::Type
	{
		typedef typename MemoryDataTyper< Object, MemDataType >::Type MemoryData;

	public:
		/**
		 *\~english
		 *\brief		Constructor, initialises the pool with given elements count.
		 *\param[in]	p_count	The elements count.
		 *\~french
		 *\brief		Constructeur, initialise le pool au nombre d'éléments donné.
		 *\param[in]	p_count	Le compte des objets.
		 */
		ObjectPool( size_t p_count )noexcept
			: m_count( p_count )
		{
			MemoryData::Initialise( m_count );
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ObjectPool()noexcept
		{
			MemoryData::Cleanup();
		}
		/**
		 *\~english
		 *\brief		Resets the pool.
		 *\~french
		 *\brief		Réinitialise le pool.
		 */
		void Reset()noexcept
		{
			MemoryData::Cleanup();
			MemoryData::Initialise( m_count );
		}
		/**
		 *\~english
		 *\brief		Allocates and builds an object from the pool's memory.
		 *\param[in]	p_params	The object's constructor parameters.
		 *\return		The built object, or nullptr.
		 *\~french
		 *\brief		Alloue et construit un objet à partir de la mémoire du pool.
		 *\param[in]	p_params	Les paramètres du constructeur de l'objet.
		 *\return		L'objet construit, ou nullptr.
		 */
		template< typename ... TParams >
		Object * Allocate( TParams ... p_params )
		{
			Object * l_space = MemoryData::Allocate();

			if ( !l_space )
			{
				return nullptr;
			}

			return new( l_space )Object( p_params... );
		}
		/**
		 *\~english
		 *\brief		Deallocates an object (gives its memory back to the pool).
		 *\param[in]	p_object	The object to deallocate.
		 *\return		\p true if the object came from the pool.
		 *\~french
		 *\brief		Désalloue un objet (rend sa mémoire au pool).
		 *\param[in]	p_object	L'objet à désallouer.
		 *\return		\p true Si l'objet provenait du pool.
		 */
		bool Deallocate( Object * p_object )noexcept
		{
			if ( MemoryData::Deallocate( p_object ) )
			{
				p_object->~Object();
				return true;
			}

			return false;
		}
		/**
		 *\~english
		 *\return		The objects count.
		 *\~french
		 *\return		Le compte d'objets.
		 */
		uint32_t GetCount()noexcept
		{
			return m_count;
		}

	private:
		size_t m_count;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		The aligned objects pool.
	\remarks	Uses a policy to change behaviour easily.
	\param		Object		The pool objects type.
	\param		MemDataType	The allocation/deallocation policy type.
	\~french
	\brief		Le pool d'objets alignés.
	\remarks	Utilisation d'une politique pour permettre de changer le comportement assez facilement.
	\param		Object		Le type des objets du pool.
	\param		MemDataType	Le type de la politique d'allocation/désallocation.
	*/
	template< typename Object, MemoryDataType MemDataType, uint32_t Align >
	class AlignedObjectPool
		: private AlignedMemoryDataTyper< Object, Align, MemDataType >::Type
	{
		typedef typename AlignedMemoryDataTyper< Object, Align, MemDataType >::Type MemoryData;

	public:
		/**
		 *\~english
		 *\brief		Constructor, initialises the pool with given elements count.
		 *\param[in]	p_count	The elements count.
		 *\~french
		 *\brief		Constructeur, initialise le pool au nombre d'éléments donné.
		 *\param[in]	p_count	Le compte des objets.
		 */
		AlignedObjectPool( size_t p_count )noexcept
			: m_count( p_count )
		{
			MemoryData::Initialise( m_count );
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~AlignedObjectPool()noexcept
		{
			MemoryData::Cleanup();
		}
		/**
		 *\~english
		 *\brief		Resets the pool.
		 *\~french
		 *\brief		Réinitialise le pool.
		 */
		void Reset()noexcept
		{
			MemoryData::Cleanup();
			MemoryData::Initialise( m_count );
		}
		/**
		 *\~english
		 *\brief		Allocates and builds an object from the pool's memory.
		 *\param[in]	p_params	The object's constructor parameters.
		 *\return		The built object, or nullptr.
		 *\~french
		 *\brief		Alloue et construit un objet à partir de la mémoire du pool.
		 *\param[in]	p_params	Les paramètres du constructeur de l'objet.
		 *\return		L'objet construit, ou nullptr.
		 */
		template< typename ... TParams >
		Object * Allocate( TParams ... p_params )
		{
			Object * l_space = MemoryData::Allocate();

			if ( !l_space )
			{
				return nullptr;
			}

			return NewDeletePolicy< Object >::Ctor( l_space );
		}
		/**
		 *\~english
		 *\brief		Deallocates an object (gives its memory back to the pool).
		 *\param[in]	p_object	The object to deallocate.
		 *\return		\p true if the object came from the pool.
		 *\~french
		 *\brief		Désalloue un objet (rend sa mémoire au pool).
		 *\param[in]	p_object	L'objet à désallouer.
		 *\return		\p true Si l'objet provenait du pool.
		 */
		bool Deallocate( Object * p_object )noexcept
		{
			if ( MemoryData::Deallocate( p_object ) )
			{
				NewDeletePolicy< Object >::Dtor( p_object );
				return true;
			}

			return false;
		}
		/**
		 *\~english
		 *\return		The objects count.
		 *\~french
		 *\return		Le compte d'objets.
		 */
		uint32_t GetCount()noexcept
		{
			return m_count;
		}

	private:
		size_t m_count;
	};
}

#endif
