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
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
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
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	target	The buffer type.
		 *\param[in]	count	The wanted buffer element count.
		 *\param[in]	flags	The buffer memory flags.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	target	Le type de tampon.
		 *\param[in]	count	Le nombre d'éléments voulu pour le tampon.
		 *\param[in]	flags	Les indicateurs de mémoire du tampon.
		 *\return		Le tampon GPU.
		 */
		ObjectBufferOffset getBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		void putBuffer( ObjectBufferOffset const & bufferOffset );

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
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
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
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	target	The buffer type.
		 *\param[in]	count	The wanted buffer element count.
		 *\param[in]	flags	The buffer memory flags.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	target	Le type de tampon.
		 *\param[in]	count	Le nombre d'éléments voulu pour le tampon.
		 *\param[in]	flags	Les indicateurs de mémoire du tampon.
		 *\return		Le tampon GPU.
		 */
		ObjectBufferOffset getBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		void putBuffer( ObjectBufferOffset const & bufferOffset );

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

#endif
