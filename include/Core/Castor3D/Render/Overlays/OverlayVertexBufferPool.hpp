/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayVertexBufferPool_H___
#define ___C3D_OverlayVertexBufferPool_H___

#include "Castor3D/Render/Overlays/OverlaysModule.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
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
		static bool constexpr isPanel = std::is_same_v< VertexT, OverlayCategory::Vertex > && ( CountT == 6u );
		static bool constexpr isBorder = std::is_same_v< VertexT, OverlayCategory::Vertex > && ( CountT == 48u );
		static bool constexpr isGpuFilled = isPanel || isBorder;

		OverlayVertexBufferPoolT( Engine & engine
			, std::string const & debugName
			, RenderDevice const & device
			, CameraUbo const & cameraUbo
			, ashes::DescriptorSetLayout const & descriptorLayout
			, ashes::PipelineVertexInputStateCreateInfo const & noTexDecl
			, ashes::PipelineVertexInputStateCreateInfo const & texDecl
			, uint32_t count );
		template< typename OverlayT >
		OverlayVertexBufferIndexT< VertexT, CountT > fill( castor::Size const & renderSize
			, OverlayT const & overlay
			, OverlayRenderNode & node
			, bool secondary );
		void upload( ashes::CommandBuffer const & cb );

		Engine & engine;
		RenderDevice const & device;
		std::string name;
		ashes::BufferPtr< OverlayUboConfiguration > overlaysData;
		castor::ArrayView< OverlayUboConfiguration > overlaysBuffer;
		ashes::PipelineVertexInputStateCreateInfo const & noTexDeclaration;
		ashes::PipelineVertexInputStateCreateInfo const & texDeclaration;
		GpuBufferBase vertexBuffer;
		uint32_t allocated{};
		uint32_t index{};
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
