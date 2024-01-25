/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PoolUniformBuffer_H___
#define ___C3D_PoolUniformBuffer_H___

#include "BufferModule.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/Signal.hpp>

#include <ashespp/Buffer/UniformBuffer.hpp>

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
		 *\param[in]	sharingMode		The sharing mode.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	usage			Les indicateurs d'utilisation du tampon.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 *\param[in]	debugName		Le nom debug du tampon.
		 *\param[in]	sharingMode		Le mode de partage.
		 */
		C3D_API PoolUniformBuffer( RenderSystem const & renderSystem
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags flags
			, castor::String debugName
			, ashes::QueueShare sharingMode = {} );
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API uint32_t initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleans up the GPU buffer.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Nettoie le tampon GPU.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void cleanup( RenderDevice const & device )noexcept;
		/**
		 *\~english
		 *\brief		Makes current local modifications available in VRAM.
		 *\~french
		 *\brief		Rend disponible en VRAM les modifications locales.
		 */
		C3D_API void flush();
		/**
		 *\~english
		 *\param		size	The size wanted.
		 *\return		\p true if there is enough remaining memory for a new element.
		 *\~french
		 *\param		size	La taille voulue.
		 *\return		\p true s'il y a assez de mémoire restante pour un nouvel élément.
		 */
		C3D_API bool hasAvailable( VkDeviceSize size )const noexcept;
		/**
		 *\~english
		 *\return		\p true if at least one element has been allocated.
		 *\~french
		 *\return		\p true si au moins un élément a été alloué.
		 */
		C3D_API bool hasAllocated()const noexcept;
		/**
		 *\~english
		 *\brief		Allocates a memory chunk for a CPU buffer.
		 *\param		size	The size wanted.
		 *\return		The memory chunk offset.
		 *\~french
		 *\brief		Alloue une zone mémoire pour un CPU buffer.
		 *\param		size	La taille voulue.
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
		C3D_API void deallocate( VkDeviceSize offset )noexcept;
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		 *\param[in] offset
		 *	The memory chunk offset.
		*\~french
		*\return
		*	La n-ème instance des données.
		 *\param[in] offset
		*	L'offset de la zone mémoire.
		*/
		template< typename DataT >
		DataT const & getData( VkDeviceSize offset )const noexcept
		{
			using DataCPtr = DataT const *;
			return *reinterpret_cast< DataCPtr >( m_data.data() + offset );
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		 *\param[in] offset
		 *	The memory chunk offset.
		*\~french
		*\return
		*	La n-ème instance des données.
		 *\param[in] offset
		*	L'offset de la zone mémoire.
		*/
		template< typename DataT >
		DataT & getData( VkDeviceSize offset )noexcept
		{
			using DataPtr = DataT *;
			return *reinterpret_cast< DataPtr >( m_data.data() + offset );
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		castor::ByteArrayView const & getDatas()const noexcept
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
		castor::ByteArrayView & getDatas()noexcept
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
		bool hasBuffer()const noexcept
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
		ashes::UniformBuffer const & getBuffer()const noexcept
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
		ashes::UniformBuffer & getBuffer()noexcept
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
		uint32_t getElementSize()const noexcept
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
		uint32_t getAlignedSize( uint32_t size )const noexcept
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
		uint32_t getAlignedSize()const noexcept
		{
			return getAlignedSize( getElementSize() );
		}

	private:
		RenderSystem const & m_renderSystem;
		VkBufferUsageFlags m_usage;
		VkMemoryPropertyFlags m_flags;
		ashes::QueueShare m_sharingMode;
		castor::Set< MemChunk > m_allocated;
		ashes::UniformBufferPtr m_buffer;
		castor::String m_debugName;
		castor::ByteArrayView m_data;
	};

	inline PoolUniformBufferUPtr makePoolUniformBuffer( RenderSystem const & renderSystem
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, castor::String name
		, ashes::QueueShare sharingMode = {} )
	{
		return castor::makeUnique< PoolUniformBuffer >( renderSystem
			, usage
			, flags
			, castor::move( name )
			, castor::move( sharingMode ) );
	}
}

#endif
