/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectBufferPool_HPP___
#define ___C3D_ObjectBufferPool_HPP___

#include "Castor3D/Buffer/GpuBufferPackedAllocator.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <unordered_map>

namespace c3d
{
	class VertexBufferPool
		: public OwnedBy< RenderSystem >
	{
	public:
		struct ModelBuffers
		{
			explicit ModelBuffers( GpuPackedBaseBufferUPtr vtx )
				: vertex{ c3d::move( vtx ) }
			{
			}

			GpuPackedBaseBufferUPtr vertex;
		};
		using BufferArray = Vector< ModelBuffers >;

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
			, crg::ResourcesCache & resources
			, String debugName );
		/**
		 *\~english
		 *\return		The allocation statistics.
		 *\~french
		 *\return		Les statistiques d'allocation.
		 */
		C3D_API AllocationStats getAllocationStats()const noexcept;
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
		template< typename VertexT >
		void putBuffer( ObjectBufferOffset const & bufferOffset )noexcept;

	private:
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize size
			, BufferArray & array )const;
		C3D_API Vector< Pair< size_t, BufferArray > >::iterator doInsertBuffers( size_t align );
		C3D_API Vector< Pair< size_t, BufferArray > >::const_iterator doFindBuffers( size_t align )const;

	private:
		RenderDevice const & m_device;
		crg::ResourcesCache & m_resources;
		String m_debugName;
		Vector < Pair< size_t, BufferArray > > m_buffers;
	};

	class IndexBufferPool
		: public OwnedBy< RenderSystem >
	{
	public:
		struct ModelBuffers
		{
			explicit ModelBuffers( GpuPackedBaseBufferUPtr idx )
				: index{ c3d::move( idx ) }
			{
			}

			GpuPackedBaseBufferUPtr index;
		};
		using BufferArray = Vector< ModelBuffers >;

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
		C3D_API explicit IndexBufferPool( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String debugName );
		/**
		 *\~english
		 *\return		The allocation statistics.
		 *\~french
		 *\return		Les statistiques d'allocation.
		 */
		C3D_API AllocationStats getAllocationStats()const noexcept;
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	indexCount	The wanted buffer element count.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	indexCount	Le nombre d'éléments voulu pour le tampon.
		 *\return		Le tampon GPU.
		 */
		template< typename IndexT >
		ObjectBufferOffset getBuffer( VkDeviceSize indexCount );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		C3D_API void putBuffer( ObjectBufferOffset const & bufferOffset )noexcept;

	private:
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize size
			, BufferArray & array )const;

	private:
		RenderDevice const & m_device;
		crg::ResourcesCache & m_resources;
		String m_debugName;
		BufferArray m_buffers;
	};

	class ObjectBufferPool
		: public OwnedBy< RenderSystem >
	{
	public:
		struct ModelBuffers
		{
			explicit ModelBuffers( Array< GpuPackedBaseBufferUPtr, size_t( SubmeshData::eCount ) > bufs = {} )
				: buffers{ c3d::move( bufs ) }
			{
			}

			Array< GpuPackedBaseBufferUPtr, size_t( SubmeshData::eCount ) > buffers;
		};
		using BufferArray = Vector< ModelBuffers >;

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
			, crg::ResourcesCache & resources
			, String debugName );
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\return		The allocation statistics.
		 *\~french
		 *\return		Les statistiques d'allocation.
		 */
		C3D_API AllocationStats getAllocationStats()const noexcept;
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	vertexCount		The wanted vertex count.
		 *\param[in]	indexCount		The wanted index count.
		 *\param[in]	meshletCount	The wanted meshlet count.
		 *\param[in]	components		The components for which the result will have allocated buffers.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	vertexCount		Le nombre de sommets voulus.
		 *\param[in]	indexCount		Le nombre d'indices voulus.
		 *\param[in]	meshletCount	Le nombre de meshlets voulus.
		 *\param[in]	components		Les composants pour lesquels le résultat aura un buffer alloué.
		 *\return		Le tampon GPU.
		 */
		C3D_API ObjectBufferOffset getBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount
			, VkDeviceSize meshletCount
			, SubmeshComponentCombine const & components );
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	vertexCount		The wanted vertex count.
		 *\param[in]	indexBuffer		The index buffer to link to the result.
		 *\param[in]	components		The components for which the result will have allocated buffers.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	vertexCount		Le nombre de sommets voulus.
		 *\param[in]	indexBuffer		Le buffer d'indices à lier au résultat.
		 *\param[in]	components		Les composants pour lesquels le résultat aura un buffer alloué.
		 *\return		Le tampon GPU.
		 */
		C3D_API ObjectBufferOffset getBuffer( VkDeviceSize vertexCount
			, BufferBase const * indexBuffer
			, SubmeshComponentCombine const & components );
		/**
		 *\~english
		 *\param[in]	buffer	The positions buffer.
		 *\return		The model buffers for the given positions buffer.
		 *\~french
		 *\param[in]	buffer	Le buffer de positions.
		 *\return		Les buffers de modèle liés au buffer de positions donné.
		 */
		C3D_API ModelBuffers const & getBuffers( BufferBase const & buffer );
		/**
		 *\~english
		 *\param[in]	buffer	The positions buffer.
		 *\return		The index buffer linked to the given positions buffer.
		 *\~french
		 *\param[in]	buffer	Le buffer de positions.
		 *\return		Le buffer d'indices lié buffer de positions donné.
		 */
		C3D_API BufferBase const & getIndexBuffer( BufferBase const & buffer );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		C3D_API void putBuffer( ObjectBufferOffset const & bufferOffset )noexcept;

	private:
		C3D_API ObjectBufferOffset doGetBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount
			, VkDeviceSize meshletCount
			, SubmeshComponentCombine const & components
			, bool isGpuComputed );
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount
			, VkDeviceSize meshletCount
			, BufferArray & array )const;

	private:
		RenderDevice const & m_device;
		crg::ResourcesCache & m_resources;
		String m_debugName;
		HashMap< size_t, BufferArray > m_buffers;
		HashMap< BufferBase const * , BufferBase const * > m_indexBuffers;
	};
}

#include "ObjectBufferPool.inl"

#endif
