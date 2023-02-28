/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayVertexBufferPool_H___
#define ___C3D_OverlayVertexBufferPool_H___

#include "Castor3D/Render/Overlays/OverlaysModule.hpp"

#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

namespace castor3d
{
	template< typename VertexT, uint32_t CountT >
	struct OverlayVertexBufferPoolT
	{
		using MyBufferIndex = OverlayVertexBufferIndexT< VertexT, CountT >;
		using Quad = std::array< VertexT, CountT >;

		OverlayVertexBufferPoolT( Engine & engine
			, std::string const & debugName
			, RenderDevice const & device
			, CameraUbo const & cameraUbo
			, ashes::DescriptorSetLayout const & descriptorLayout
			, ashes::PipelineVertexInputStateCreateInfo const & noTexDecl
			, ashes::PipelineVertexInputStateCreateInfo const & texDecl
			, uint32_t count );
		OverlayVertexBufferIndexT< VertexT, CountT > allocate( OverlayRenderNode & node );
		void deallocate( OverlayVertexBufferIndexT< VertexT, CountT > const & index );
		void upload( ashes::CommandBuffer const & cb );

		Engine & engine;
		RenderDevice const & device;
		ashes::BufferPtr< OverlayUboConfiguration > overlaysData;
		castor::ArrayView< OverlayUboConfiguration > overlaysBuffer;
		ashes::PipelineVertexInputStateCreateInfo const & noTexDeclaration;
		ashes::PipelineVertexInputStateCreateInfo const & texDeclaration;
		VertexBufferPoolUPtr buffer;
		std::vector< ObjectBufferOffset > allocated;
		ashes::DescriptorSetPoolPtr descriptorPool;
		ashes::DescriptorSetPtr descriptorSet;

	private:
		ashes::DescriptorSetPtr doCreateDescriptorSet( CameraUbo const & cameraUbo
			, ashes::DescriptorSetLayout const & descriptorLayout
			, std::string const & debugName )const;
	};
}

#include "OverlayVertexBufferPool.inl"

#endif
