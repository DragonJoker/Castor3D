#include "Castor3D/Render/Passes/PickingPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/Picking.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, PickingPass )

namespace castor3d
{
	uint32_t const PickingPass::UboBindingPoint = 7u;
	castor::String const PickingPass::Type = "c3d.pick";

	PickingPass::PickingPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::Size const & size
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, SceneCuller & culler )
		: RenderNodesPass{ pass
			, context
			, graph
			, device
			, Type
			, cuT( "Picking" )
			, cuT( "Picking" )
			, nullptr
			, RenderNodesPassDesc{ { size.getWidth(), size.getHeight(), 1u }, matrixUbo, sceneUbo, culler, RenderMode::eBoth, true, false }
				.meshShading( true ) }
	{
	}

	void PickingPass::addScene( Scene & scene, Camera & camera )
	{
	}

	void PickingPass::updateArea( VkRect2D const & scissor )
	{
		ShadowMapLightTypeArray shadowMaps;
		m_renderQueue->update( shadowMaps, scissor );
	}

	TextureFlags PickingPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eOpacity };
	}

	SubmeshFlags PickingPass::doAdjustSubmeshFlags( SubmeshFlags flags )const
	{
		remFlag( flags, SubmeshFlag::eNormals );
		remFlag( flags, SubmeshFlag::eTangents );
		remFlag( flags, SubmeshFlag::eColours );
		return flags;
	}

	bool PickingPass::doIsValidPass( Pass const & pass )const
	{
		if ( !checkFlag( pass.getPassFlags(), PassFlag::ePickable) )
		{
			return false;
		}

		return RenderNodesPass::doAreValidPassFlags( pass.getPassFlags() );
	}

	void PickingPass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
	}

	void PickingPass::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps )
	{
	}

	ShaderPtr PickingPass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		shader::Utils utils{ writer };

		// UBOs
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, hasTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, hasTextures };

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		sdw::PushConstantBuffer pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< UVec4 >( "pxl_fragColor", 0 ) );

		writer.implementMainT< shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, FragmentOut{ writer }
			, [&]( FragmentInT< shader::FragmentSurfaceT > in
				, FragmentOut out )
			{
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( in.nodeId ) - 1u] );
				shader::OpacityBlendComponents components{ writer
					, "out"
					, in.texture0
					, 1.0_f };
				materials.blendMaterials( utils
					, flags.alphaFunc
					, flags.passFlags
					, flags.submeshFlags
					, flags.textures
					, hasTextures
					, textureConfigs
					, textureAnims
					, c3d_maps
					, modelData.getMaterialId()
					, in.passMultipliers
					, components );
				pxl_fragColor = uvec4( ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) ? 2_u : 1_u )
					, writer.cast< sdw::UInt >( in.nodeId )
					, writer.cast< sdw::UInt >( in.primitiveID )
					, 0_u );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	PassFlags PickingPass::doAdjustPassFlags( PassFlags flags )const
	{
		remFlag( flags, PassFlag::eAlphaBlending );
		return flags;
	}

	ProgramFlags PickingPass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		remFlag( flags, ProgramFlag::eLighting );
		addFlag( flags, ProgramFlag::ePicking );
		return flags;
	}

	ashes::PipelineDepthStencilStateCreateInfo PickingPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo PickingPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderNodesPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
	}
}
