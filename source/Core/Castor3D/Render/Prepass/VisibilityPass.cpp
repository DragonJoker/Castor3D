#include "Castor3D/Render/Prepass/VisibilityPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, VisibilityPass )

namespace castor3d
{
	//*********************************************************************************************

	castor::String const VisibilityPass::Type = "c3d.visibility";

	VisibilityPass::VisibilityPass( RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, nullptr
			, renderPassDesc
			, techniquePassDesc }
	{
	}

	void VisibilityPass::accept( RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_typeID
			&& visitor.config.allowProgramsVisit )
		{
			auto flags = visitor.getFlags();
			doUpdateFlags( flags );
			auto shaderProgram = doGetProgram( flags );
			visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
			visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}
	}

	TextureFlags VisibilityPass::getTexturesMask()const
	{
		// Normally, ( TextureFlag::eOpacity | TextureFlag::eHeight ) would be enough,
		// but to have the pipeline ID order synchronization with visibility resolve,
		// allow all.
		return TextureFlags{ TextureFlag::eAll };
	}

	ShaderFlags VisibilityPass::getShaderFlags()const
	{
		return ShaderFlag::eWorldSpace
			| ShaderFlag::eTangentSpace
			| ShaderFlag::eOpacity
			| ShaderFlag::eVelocity
			| ShaderFlag::eVisibility;
	}

	PassFlags VisibilityPass::doAdjustPassFlags( PassFlags flags )const
	{
		return flags & PassFlag::eAllVisibility;
	}

	ProgramFlags VisibilityPass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		return flags;
	}

	SceneFlags VisibilityPass::doAdjustSceneFlags( SceneFlags flags )const
	{
		return SceneFlag::eNone;
	}

	void VisibilityPass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
	}

	void VisibilityPass::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps )
	{
	}

	ashes::PipelineDepthStencilStateCreateInfo VisibilityPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_TRUE
			, VK_TRUE
			, ( flags.passLayerIndex > 0
				? VK_COMPARE_OP_LESS_OR_EQUAL
				: VK_COMPARE_OP_LESS ) };
	}

	ashes::PipelineColorBlendStateCreateInfo VisibilityPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderNodesPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, 3u );
	}

	ShaderPtr VisibilityPass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		bool enableTextures = flags.enableTextures();

		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, enableTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, enableTextures };

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		sdw::PushConstantBuffer pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();
		auto constexpr maxPipelinesSize = uint32_t( castor::getBitSize( MaxPipelines ) );

		// Outputs
		auto depthObj = writer.declOutput< Vec4 >( "depthObj", 0u );
		auto data = writer.declOutput< UVec2 >( "data", 1u );
		auto velocity = writer.declOutput< Vec2 >( "velocity", 2u, flags.writeVelocity() );

		shader::Utils utils{ writer };

		writer.implementMainT< shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, flags }
			, FragmentOut{ writer }
			, [&]( FragmentInT< shader::FragmentSurfaceT > in
				, FragmentOut out )
			{
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1u] );
				shader::GeometryBlendComponents components{ writer
					, "out"
					, in.texture0
					, in.texture1
					, in.texture2
					, in.texture3
					, 1.0_f
					, normalize( in.normal )
					, normalize( in.tangent )
					, normalize( in.bitangent )
					, in.tangentSpaceViewPosition
					, in.tangentSpaceFragPosition
					, { 1.0_f, false } };
				materials.blendMaterials( utils
					, flags
					, textureConfigs
					, textureAnims
					, c3d_maps
					, modelData.getMaterialId()
					, in.passMultipliers
					, components );
				depthObj = vec4( in.fragCoord.z()
					, length( in.worldPosition.xyz() - c3d_sceneData.cameraPosition )
					, writer.cast< sdw::Float >( in.nodeId )
					, 0.0_f );
				data = uvec2( ( in.nodeId << maxPipelinesSize ) | ( pipelineID )
					, ( flags.isBillboard()
						? in.vertexId * 2_u + writer.cast< sdw::UInt >( in.primitiveID )
						: writer.cast< sdw::UInt >( in.primitiveID ) ) );
				velocity = in.getVelocity();
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
