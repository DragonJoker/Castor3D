/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectBufferPool_HPP___
#define ___C3D_ObjectBufferPool_HPP___

#include "Castor3D/Buffer/GpuBufferPackedAllocator.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>

namespace castor3d
{
	class VertexBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		struct ModelBuffers
		{
			explicit ModelBuffers( GpuPackedBuffer vtx )
				: vertex{ std::move( vtx ) }
			{
			}

			GpuPackedBuffer vertex;
		};
		using BufferArray = std::vector< std::unique_ptr< ModelBuffers > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	debugName		Le nom debug.
		 */
		C3D_API explicit VertexBufferPool( RenderDevice const & device
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( ashes::CommandBuffer const & cb );
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	vertexCount	The wanted buffer element count.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	vertexCount	Le nombre d'éléments voulu pour le tampon.
		 *\return		Le tampon GPU.
		 */
		template< typename VertexT >
		ObjectBufferOffset getBuffer( VkDeviceSize vertexCount );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		C3D_API void putBuffer( ObjectBufferOffset const & bufferOffset );

	private:
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize size
			, BufferArray & array );

	private:
		RenderDevice const & m_device;
		castor::String m_debugName;
		BufferArray m_buffers;
	};

	class ObjectBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		struct ModelBuffers
		{
			ModelBuffers( GpuPackedBuffer vtx
				, GpuPackedBuffer idx )
				: vertex{ std::move( vtx ) }
				, index{ std::move( idx ) }
			{
			}

			GpuPackedBuffer vertex;
			GpuPackedBuffer index;
		};
		using BufferArray = std::vector< std::unique_ptr< ModelBuffers > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	debugName		Le nom debug.
		 */
		C3D_API explicit ObjectBufferPool( RenderDevice const & device
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( ashes::CommandBuffer const & cb );
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	vertexCount	The wanted vertex count.
		 *\param[in]	indexCount	The wanted index count.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	vertexCount	Le nombre de sommets voulus.
		 *\param[in]	indexCount	Le nombre d'indices voulus.
		 *\return		Le tampon GPU.
		 */
		C3D_API ObjectBufferOffset getBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		C3D_API void putBuffer( ObjectBufferOffset const & bufferOffset );

	private:
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount
			, BufferArray & array );

	private:
		RenderDevice const & m_device;
		castor::String m_debugName;
		BufferArray m_buffers;
	};

	class SkinnedObjectBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		struct ModelBuffers
		{
			ModelBuffers( GpuPackedBuffer vtx
				, GpuPackedBuffer bon
				, GpuPackedBuffer idx )
				: vertex{ std::move( vtx ) }
				, bones{ std::move( bon ) }
				, index{ std::move( idx ) }
			{
			}

			GpuPackedBuffer vertex;
			GpuPackedBuffer bones;
			GpuPackedBuffer index;
		};
		using BufferArray = std::vector< std::unique_ptr< ModelBuffers > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	debugName		Le nom debug.
		 */
		C3D_API explicit SkinnedObjectBufferPool( RenderDevice const & device
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( ashes::CommandBuffer const & cb );
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	vertexCount	The wanted vertex count.
		 *\param[in]	indexCount	The wanted index count.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	vertexCount	Le nombre de sommets voulus.
		 *\param[in]	indexCount	Le nombre d'indices voulus.
		 *\return		Le tampon GPU.
		 */
		C3D_API ObjectBufferOffset getBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		C3D_API void putBuffer( ObjectBufferOffset const & bufferOffset );

	private:
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount
			, BufferArray & array );

	private:
		RenderDevice const & m_device;
		castor::String m_debugName;
		BufferArray m_buffers;
	};
}

#include "ObjectBufferPool.inl"

#endif
