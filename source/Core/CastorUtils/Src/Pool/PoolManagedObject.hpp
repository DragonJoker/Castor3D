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
	template< typename Object, eMEMDATA_TYPE MemDataType >
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
