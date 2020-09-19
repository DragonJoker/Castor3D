/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PoolUniformBuffer_H___
#define ___C3D_PoolUniformBuffer_H___

#include "BufferModule.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <set>

namespace castor3d
{
	class PoolUniformBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	usage			The buffer usage flags.
		 *\param[in]	flags			The buffer memory flags.
		 *\param[in]	debugName		The buffer debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	usage			Les indicateurs d'utilisation du tampon.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 *\param[in]	debugName		Le nom debug du tampon.
		 */
		C3D_API PoolUniformBuffer( RenderSystem const & renderSystem
			, castor::ArrayView< uint8_t > data
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags flags
			, castor::String debugName
			, ashes::QueueShare sharingMode = {} );
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 */
		C3D_API uint32_t initialise();
		/**
		 *\~english
		 *\brief		Cleans up the GPU buffer.
		 *\~french
		 *\brief		Nettoie le tampon GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\return		\p true if there is enough remaining memory for a new element.
		 *\~french
		 *\return		\p true s'il y a assez de mémoire restante pour un nouvel élément.
		 */
		C3D_API bool hasAvailable( VkDeviceSize size )const;
		/**
		 *\~english
		 *\brief		Allocates a memory chunk for a CPU buffer.
		 *\return		The memory chunk offset.
		 *\~french
		 *\brief		Alloue une zone mémoire pour un CPU buffer.
		 *\return		L'offset de la zone mémoire.
		 */
		C3D_API MemChunk allocate( VkDeviceSize size );
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	offset	The memory chunk offset.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	offset	L'offset de la zone mémoire.
		 */
		C3D_API void deallocate( VkDeviceSize offset );
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		template< typename DataT >
		inline DataT const & getData( VkDeviceSize offset )const
		{
			return *reinterpret_cast< DataT const * >( m_data.data() + offset );
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		template< typename DataT >
		inline DataT & getData( VkDeviceSize offset )
		{
			return *reinterpret_cast< DataT * >( m_data.data() + offset );
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		inline castor::ArrayView< uint8_t > const & getDatas()const
		{
			return m_data;
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		inline castor::ArrayView< uint8_t > & getDatas()
		{
			return m_data;
		}
		/**
		*\~english
		*\return
		*	\p false if the internal buffer doesn't exist.
		*\~french
		*\return
		*	\p false si le tampon interne n'existe pas.
		*/
		inline bool hasBuffer()const
		{
			return m_buffer != nullptr;
		}
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		inline ashes::UniformBuffer const & getBuffer()const
		{
			return *m_buffer;
		}
		inline operator ashes::UniformBuffer const & ()const
		{
			return *m_buffer;
		}
		/**
		*\~english
		*\return
		*	The internal buffer.
		*\~french
		*\return
		*	Le tampon interne.
		*/
		inline ashes::UniformBuffer & getBuffer()
		{
			return *m_buffer;
		}
		inline operator ashes::UniformBuffer & ()
		{
			return *m_buffer;
		}
		/**
		*\~english
		*\return
		*	The size of one element in the buffer.
		*\~french
		*\return
		*	La taille d'un élément du tampon.
		*/
		inline uint32_t getElementSize()const
		{
			return uint32_t( getBuffer().getElementSize() );
		}
		/**
		*\~english
		*\brief
		*	Retrieves the aligned size for an element.
		*\param[in] size
		*	The size of an element.
		*\return
		*	The aligned size.
		*\~french
		*\brief
		*	Récupère la taille alignée d'un élément.
		*\param[in] size
		*	La taille d'un élément.
		*\return
		*	La taille alignée.
		*/
		inline uint32_t getAlignedSize( uint32_t size )const
		{
			return uint32_t( getBuffer().getAlignedSize( size ) );
		}
		/**
		*\~english
		*\return
		*	The aligned size for an element.
		*\~french
		*\return
		*	La taille alignée d'un élément.
		*/
		inline uint32_t getAlignedSize()const
		{
			return getAlignedSize( getElementSize() );
		}

	private:
		RenderSystem const & m_renderSystem;
		VkBufferUsageFlags m_usage;
		VkMemoryPropertyFlags m_flags;
		ashes::QueueShare m_sharingMode;
		std::set< MemChunk > m_allocated;
		ashes::UniformBufferPtr m_buffer;
		castor::String m_debugName;
		castor::ArrayView< uint8_t > m_data;
	};

	inline PoolUniformBufferUPtr makePoolUniformBuffer( RenderSystem const & renderSystem
		, castor::ArrayView< uint8_t > data
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string name
		, ashes::QueueShare sharingMode = {} )
	{
		return std::make_unique< PoolUniformBuffer >( renderSystem
			, data
			, usage
			, flags
			, std::move( name )
			, std::move( sharingMode ) );
	}
}

#endif
