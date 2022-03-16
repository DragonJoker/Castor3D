/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BufferModule_H___
#define ___C3D_BufferModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/Pool/BuddyAllocator.hpp>

#include <cstddef>

namespace castor3d
{
	/**@name Buffer */
	//@{

	/**
	*\~english
	*\brief
	*	Vertex Buffers, vertex layouts, and index buffer.
	*\~french
	*\brief
	*	Vertex Buffers, vertex layouts, et index buffer.
	*/
	struct GeometryBuffers;
	/**
	*\~english
	*\brief
	*	A GPU buffer, that uses a buddy allocator to allocate sub-buffers.
	*\~french
	*\brief
	*	Un tampon GPU, utilisant un buddy allocator pour allouer des sous-tampons.
	*\remark
	*/
	template< typename AllocatorT >
	class GpuBufferT;
	/**
	*\~english
	*\brief
	*	GPU buffer allocation traits for buddy allocator.
	*\~french
	*\brief
	*	Traits d'allocation de buffer GPU, pour le buddy allocator.
	*/
	struct GpuBufferBuddyAllocatorTraits;
	/**
	*\~english
	*\brief
	*	GPU buffer linear allocator, for elements of same size.
	*\~french
	*\brief
	*	Allocateur linéaire de buffer GPU, pour des éléments de même taille.
	*/
	struct GpuBufferLinearAllocator;
	/**
	*\~english
	*\brief
	*	GPU buffer contiguous allocator, for elements of various size.
	*\~french
	*\brief
	*	Allocateur contigü de buffer GPU, pour des éléments de tailles diverses.
	*/
	struct GpuBufferPackedAllocator;
	/**
	*\~english
	*\brief
	*	An offset and range of a GpuBuffer.
	*\~french
	*\brief
	*	Un intervalle d'un GpuBuffer.
	*\remark
	*/
	template< typename DataT >
	struct GpuBufferOffsetT;
	/**
	*\~english
	*\brief
	*	A GpuBuffer pool.
	*\~french
	*\brief
	*	Un pool de GpuBuffer.
	*\remark
	*/
	class GpuBufferPool;
	/**
	*\~english
	*\brief
	*	The ranges for model buffers (vertex, index and indirect).
	*\~french
	*\brief
	*	Les intervalles des buffers de modèle (vertex, index et indirect).
	*\remark
	*/
	struct ObjectBufferOffset;
	/**
	*\~english
	*\brief
	*	A GpuBuffer pool specific for VBO and IBO.
	*\~french
	*\brief
	*	Un pool de GpuBuffer pour les VBO et IBO.
	*\remark
	*/
	class ObjectBufferPool;
	/**
	*\~english
	*\brief
	*	A GpuBuffer pool specific for VBO and IBO.
	*\~french
	*\brief
	*	Un pool de GpuBuffer pour les VBO et IBO.
	*\remark
	*/
	class SkinnedObjectBufferPool;
	/**
	*\~english
	*\brief
	*	A uniform typed buffer, than can contain multiple sub-buffers.
	*\remarks
	*	Allocated from a pool.
	*\~french
	*\brief
	*	Un tampon typé d'uniformes, pouvant contenir de multiples sous-tampons.
	*\remarks
	*	Alloué depuis un pool.
	*\remark
	*/
	class PoolUniformBuffer;
	/**
	*\~english
	*\brief		A uniform buffer, than can contain multiple sub-buffers.
	*\~french
	*\brief		Un tampon d'uniformes, puovant contenir de multiples sous-tampons.
	*\remark
	*/
	class UniformBufferBase;
	/**
	*\~english
	*\brief
	*	A uniform typed buffer, than can contain multiple sub-buffers.
	*\~french
	*\brief
	*	Un tampon typé d'uniformes, puovant contenir de multiples sous-tampons.
	*\remark
	*/
	template< typename DataT >
	class UniformBufferT;
	/**
	*\~english
	*\brief
	*	A UniformBuffer and an offset from the GpuBuffer.
	*\~french
	*\brief
	*	Un UniformBuffer et un offset dans le GpuBuffer.
	*/
	template< typename DataT >
	struct UniformBufferOffsetT;
	/**
	*\~english
	*\brief
	*	Uniform buffer pool implementation.
	*\~french
	*\brief
	*	Implémentation d'un pool d'uniform buffers.
	*/
	class UniformBufferPool;
	/**
	*\~english
	*\brief
	*	A GpuBuffer pool specific for non VBO.
	*\~french
	*\brief
	*	Un pool de GpuBuffer pour les VBO.
	*\remark
	*/
	class VertexBufferPool;

	template< typename DataT >
	class GpuLinearAllocatorT;

	CU_DeclareCUSmartPtr( castor3d, GpuBufferPool, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, ObjectBufferPool, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SkinnedObjectBufferPool, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, PoolUniformBuffer, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, UniformBufferBase, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, UniformBufferPool, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, VertexBufferPool, C3D_API );
	CU_DeclareTemplateSmartPtr( UniformBuffer );

	using GpuBufferBuddyAllocator = castor::BuddyAllocatorT< GpuBufferBuddyAllocatorTraits >;
	using GpuBufferBuddyAllocatorUPtr = std::unique_ptr< GpuBufferBuddyAllocator >;
	using GpuBuddyBuffer = GpuBufferT< GpuBufferBuddyAllocator >;
	using GpuLinearBuffer = GpuBufferT< GpuBufferLinearAllocator >;
	using GpuPackedBuffer = GpuBufferT< GpuBufferPackedAllocator >;
	/**
	*\~english
	*\brief
	*	A memory range, in bytes.
	*\~french
	*\brief
	*	Un intervalle mémoire, en octets.
	*/
	struct MemChunk
	{
		VkDeviceSize offset;
		VkDeviceSize size;
		VkDeviceSize askedSize;
	};

	inline bool operator<( MemChunk const & lhs
		, MemChunk const & rhs )
	{
		return lhs.offset < rhs.offset;
	}

	C3D_API void copyBuffer( ashes::CommandBuffer const & commandBuffer
		, ashes::BufferBase const & src
		, ashes::BufferBase const & dst
		, VkDeviceSize offset
		, VkDeviceSize size
		, VkPipelineStageFlags flags );
	C3D_API void copyBuffer( ashes::CommandBuffer const & commandBuffer
		, ashes::BufferBase const & src
		, ashes::BufferBase const & dst
		, std::vector< VkBufferCopy > const & regions
		, VkAccessFlags dstAccessFlags
		, VkPipelineStageFlags dstPipelineFlags );
	C3D_API void updateBuffer( ashes::CommandBuffer const & commandBuffer
		, castor::ByteArray data
		, ashes::BufferBase const & dst
		, std::vector< VkBufferCopy > const & regions
		, VkAccessFlags dstAccessFlags
		, VkPipelineStageFlags dstPipelineFlags );
	//@}
}

#endif
