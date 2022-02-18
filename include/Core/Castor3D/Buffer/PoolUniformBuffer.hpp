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
		 *\param[in]	data			The mapped data.
		 *\param[in]	usage			The buffer usage flags.
		 *\param[in]	flags			The buffer memory flags.
		 *\param[in]	debugName		The buffer debug name.
		 *\param[in]	sharingMode		The sharing mode.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	data			Les données mappées
		 *\param[in]	usage			Les indicateurs d'utilisation du tampon.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 *\param[in]	debugName		Le nom debug du tampon.
		 *\param[in]	sharingMode		Le mode de partage.
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
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\param		size	The size wanted.
		 *\return		\p true if there is enough remaining memory for a new element.
		 *\~french
		 *\param		size	La taille voulue.
		 *\return		\p true s'il y a assez de mémoire restante pour un nouvel élément.
		 */
		C3D_API bool hasAvailable( VkDeviceSize size )const;
		/**
		 *\~english
		 *\return		\p true if at least one element has been allocated.
		 *\~french
		 *\return		\p true si au moins un élément a été alloué.
		 */
		C3D_API bool hasAllocated()const;
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
		C3D_API void deallocate( VkDeviceSize offset );
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
		DataT const & getData( VkDeviceSize offset )const
		{
			return *reinterpret_cast< DataT const * >( m_data.data() + offset );
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
		DataT & getData( VkDeviceSize offset )
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
		castor::ArrayView< uint8_t > const & getDatas()const
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
		castor::ArrayView< uint8_t > & getDatas()
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
		bool hasBuffer()const
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
		ashes::UniformBuffer const & getBuffer()const
		{
			return *m_buffer;
		}
		operator ashes::UniformBuffer const & ()const
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
		ashes::UniformBuffer & getBuffer()
		{
			return *m_buffer;
		}
		operator ashes::UniformBuffer & ()
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
		uint32_t getElementSize()const
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
		uint32_t getAlignedSize( uint32_t size )const
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
		uint32_t getAlignedSize()const
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
		return castor::makeUnique< PoolUniformBuffer >( renderSystem
			, data
			, usage
			, flags
			, std::move( name )
			, std::move( sharingMode ) );
	}
}

#endif
