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
#ifndef ___CU_POOL_MANAGED_OBJECT_H___
#define ___CU_POOL_MANAGED_OBJECT_H___

#include "UniqueObjectPool.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		Pool managed object.
	\remarks	Uses a policy to change behaviour easily.
	\param		Object		The pool objects type.
	\param		MemDataType	The allocation/deallocation policy type.
	\~french
	\brief		Objet géré par un pool.
	\remarks	Utilisation d'une politique pour permettre de changer le comportement assez facilement.
	\param		Object		Le type d'objet.
	\param		MemDataType	Le type de la politique d'allocation/désallocation.
	*/
	template< typename Object, MemoryDataType MemDataType >
	class PoolManagedObject
		: public Object
	{
	public:
		using MyUniqueObjectPool = UniqueObjectPool< Object, MemDataType >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_params	The Object constructor parameters.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_params	Les paramètre du constructeur de Object.
		 */
		template< typename ... Params >
		PoolManagedObject( Params ... p_params )noexcept
			: Object( p_params... )
		{
		}
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\param[in]	p_rhs	The other object.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	p_rhs	L'autre objet.
		 */
		PoolManagedObject( PoolManagedObject const & p_rhs )noexcept
			: Object( p_rhs )
		{
		}
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\param[in]	p_rhs	The other object.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	p_rhs	L'autre objet.
		 */
		PoolManagedObject( Object const & p_rhs )noexcept
			: Object( p_rhs )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~PoolManagedObject()noexcept
		{
		}
		/**
		 *\~english
		 *\brief		New operator overload.
		 *\remarks		Uses the objects pool to allocate memory.
		 *\param[in]	p_size	The allocation size.
		 *\~french
		 *\brief		Surcharge de l'opérateur new.
		 *\remarks		Utilise le pool d'objets pour allouer la mémoire.
		 *\param[in]	p_size	La taille à allouer.
		 */
		static void * operator new( std::size_t p_size )
		{
			return MyUniqueObjectPool::Get().Allocate();
		}
		/**
		 *\~english
		 *\brief		Delete operator overload.
		 *\remarks		Uses the objects pool to deallocate memory.
		 *\param[in]	p_ptr	The pointer to deallocate.
		 *\param[in]	p_size	The deallocation size.
		 *\~french
		 *\brief		Surcharge de l'opérateur delete.
		 *\remarks		Utilise le pool d'objets pour désallouer la mémoire.
		 *\param[in]	p_ptr	Le pointeur à désallouer.
		 *\param[in]	p_size	La taille à désallouer.
		 */
		static void operator delete( void * p_ptr, std::size_t p_size )
		{
			MyUniqueObjectPool::Get().Deallocate( reinterpret_cast< Object * >( p_ptr ) );
		}
		/**
		 *\~english
		 *\brief		Delete array operator overload.
		 *\remarks		Uses the objects pool to deallocate memory.
		 *\param[in]	p_ptr	The pointer to deallocate.
		 *\param[in]	p_size	The deallocation size.
		 *\~french
		 *\brief		Surcharge de l'opérateur delete de tableau.
		 *\remarks		Utilise le pool d'objets pour désallouer la mémoire.
		 *\param[in]	p_ptr	Le pointeur à désallouer.
		 *\param[in]	p_size	La taille à désallouer.
		 */
		static void operator delete[]( void * p_ptr, std::size_t p_size )
		{
			MyUniqueObjectPool::Get().Deallocate( reinterpret_cast< Object * >( p_ptr ) );
		}
	};
}

#endif
