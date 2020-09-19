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
	class GpuBuffer;
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
	*	A GpuBuffer and an offset from the GpuBuffer.
	*\~french
	*\brief
	*	Un GpuBuffer et un offset dans le GpuBuffer.
	*/
	struct GpuBufferOffset;
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
	*	Regroups all uniform buffer pools.
	*\~french
	*\brief
	*	Regroupe les pools d'uniform buffers.
	*/
	class UniformBufferPools;

	CU_DeclareSmartPtr( UniformBufferBase );
	CU_DeclareSmartPtr( UniformBufferPools );
	CU_DeclareSmartPtr( PoolUniformBuffer );
	CU_DeclareSmartPtr( UniformBufferPool );
	CU_DeclareTemplateSmartPtr( UniformBuffer );

	using GpuBufferBuddyAllocator = castor::BuddyAllocatorT< GpuBufferBuddyAllocatorTraits >;
	using GpuBufferBuddyAllocatorUPtr = std::unique_ptr< GpuBufferBuddyAllocator >;

	//@}
}

#endif
