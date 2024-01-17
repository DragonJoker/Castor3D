/*
See LICENSE file in root folder
*/
#ifndef ___CU_OBJECT_POOL_H___
#define ___CU_OBJECT_POOL_H___

#include "CastorUtils/Design/NonCopyable.hpp"
#include "CastorUtils/Pool/MemoryDataTyper.hpp"
#include "CastorUtils/Pool/FixedGrowingSizeMarkedMemoryData.hpp"
#include "CastorUtils/Pool/FixedGrowingSizeMemoryData.hpp"
#include "CastorUtils/Pool/FixedSizeMarkedMemoryData.hpp"
#include "CastorUtils/Pool/FixedSizeMemoryData.hpp"

namespace castor
{
	template< typename Object >
	struct NewDeletePolicy
	{
		template< typename ... TParams >
		static inline Object * create( Object * space, TParams ... params )
		{
			return new( space )Object( params... );
		}

		template< typename ... TParams >
		static inline void destroy( Object * space )noexcept
		{
			space->~Object();
		}
	};

	template< typename Object, MemoryDataType MemDataType >
	class ObjectPool
		: private MemoryDataTyper< Object, MemDataType >::Type
	{
		using MemoryData = typename MemoryDataTyper< Object, MemDataType >::Type;

		ObjectPool( ObjectPool const & ) = delete;
		ObjectPool( ObjectPool && )noexcept = delete;
		ObjectPool & operator=( ObjectPool const & ) = delete;
		ObjectPool & operator=( ObjectPool && )noexcept = delete;

	public:
		/**
		 *\~english
		 *\brief		Constructor, initialises the pool with given elements count.
		 *\param[in]	count	The elements count.
		 *\~french
		 *\brief		Constructeur, initialise le pool au nombre d'éléments donné.
		 *\param[in]	count	Le compte des objets.
		 */
		explicit ObjectPool( size_t count )noexcept
			: m_count( count )
		{
			MemoryData::initialise( m_count );
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ObjectPool()noexcept
		{
			MemoryData::cleanup();
		}
		/**
		 *\~english
		 *\brief		Resets the pool.
		 *\~french
		 *\brief		Réinitialise le pool.
		 */
		void Reset()noexcept
		{
			MemoryData::cleanup();
			MemoryData::initialise( m_count );
		}
		/**
		 *\~english
		 *\brief		Allocates and builds an object from the pool's memory.
		 *\param[in]	params	The object's constructor parameters.
		 *\return		The built object, or nullptr.
		 *\~french
		 *\brief		Alloue et construit un objet à partir de la mémoire du pool.
		 *\param[in]	params	Les paramètres du constructeur de l'objet.
		 *\return		L'objet construit, ou nullptr.
		 */
		template< typename ... TParams >
		Object * allocate( TParams ... params )
		{
			Object * space = MemoryData::allocate();

			if ( !space )
			{
				return nullptr;
			}

			return new( space )Object( params... );
		}
		/**
		 *\~english
		 *\brief		Deallocates an object (gives its memory back to the pool).
		 *\param[in]	object	The object to deallocate.
		 *\return		\p true if the object came from the pool.
		 *\~french
		 *\brief		Désalloue un objet (rend sa mémoire au pool).
		 *\param[in]	object	L'objet à désallouer.
		 *\return		\p true Si l'objet provenait du pool.
		 */
		bool deallocate( Object * object )noexcept
		{
			if ( MemoryData::deallocate( object ) )
			{
				object->~Object();
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
		uint32_t getCount()noexcept
		{
			return m_count;
		}

	private:
		size_t m_count;
	};

	template< typename Object, MemoryDataType MemDataType, uint32_t Align >
	class AlignedObjectPool
		: private AlignedMemoryDataTyper< Object, Align, MemDataType >::Type
	{
		using MemoryData = typename AlignedMemoryDataTyper< Object, Align, MemDataType >::Type;

		AlignedObjectPool( AlignedObjectPool const & ) = delete;
		AlignedObjectPool( AlignedObjectPool && )noexcept = delete;
		AlignedObjectPool & operator=( AlignedObjectPool const & ) = delete;
		AlignedObjectPool & operator=( AlignedObjectPool && )noexcept = delete;

	public:
		/**
		 *\~english
		 *\brief		Constructor, initialises the pool with given elements count.
		 *\param[in]	count	The elements count.
		 *\~french
		 *\brief		Constructeur, initialise le pool au nombre d'éléments donné.
		 *\param[in]	count	Le compte des objets.
		 */
		explicit AlignedObjectPool( size_t count )noexcept
			: m_count( count )
		{
			MemoryData::initialise( m_count );
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~AlignedObjectPool()noexcept
		{
			MemoryData::cleanup();
		}
		/**
		 *\~english
		 *\brief		Resets the pool.
		 *\~french
		 *\brief		Réinitialise le pool.
		 */
		void Reset()noexcept
		{
			MemoryData::cleanup();
			MemoryData::initialise( m_count );
		}
		/**
		 *\~english
		 *\brief		Allocates and builds an object from the pool's memory.
		 *\param[in]	params	The object's constructor parameters.
		 *\return		The built object, or nullptr.
		 *\~french
		 *\brief		Alloue et construit un objet à partir de la mémoire du pool.
		 *\param[in]	params	Les paramètres du constructeur de l'objet.
		 *\return		L'objet construit, ou nullptr.
		 */
		template< typename ... TParams >
		Object * allocate( TParams ... params )
		{
			Object * space = MemoryData::allocate();

			if ( !space )
			{
				return nullptr;
			}

			return NewDeletePolicy< Object >::create( space );
		}
		/**
		 *\~english
		 *\brief		Deallocates an object (gives its memory back to the pool).
		 *\param[in]	object	The object to deallocate.
		 *\return		\p true if the object came from the pool.
		 *\~french
		 *\brief		Désalloue un objet (rend sa mémoire au pool).
		 *\param[in]	object	L'objet à désallouer.
		 *\return		\p true Si l'objet provenait du pool.
		 */
		bool deallocate( Object * object )noexcept
		{
			if ( MemoryData::deallocate( object ) )
			{
				NewDeletePolicy< Object >::destroy( object );
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
		uint32_t getCount()noexcept
		{
			return m_count;
		}

	private:
		size_t m_count;
	};
}

#endif
