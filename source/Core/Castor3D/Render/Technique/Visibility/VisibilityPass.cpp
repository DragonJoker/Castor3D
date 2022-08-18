#include "Castor3D/Render/Technique/Visibility/VisibilityPass.hpp"

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
		, castor::String const & category
		, castor::String const & name
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, category
			, name
			, nullptr
			, renderPassDesc
			, techniquePassDesc }
	{
	}

	void VisibilityPass::accept( RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_typeID )
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
		return TextureFlags{ TextureFlag::eAll };
	}

	ShaderFlags VisibilityPass::getShaderFlags()const
	{
		return ShaderFlag::eTangentSpace;
	}

	PassFlags VisibilityPass::doAdjustPassFlags( PassFlags flags )const
	{
		return flags & PassFlag::eAllVisibility;
	}

	ProgramFlags VisibilityPass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		remFlag( flags, ProgramFlag::eLighting );
		addFlag( flags, ProgramFlag::eVisibilityPass );
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
			, 1u );
	}

	ShaderPtr VisibilityPass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

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
		auto constexpr maxPipelinesSize = uint32_t( castor::getBitSize( MaxPipelines ) );

		// Outputs
		auto data = writer.declOutput< UVec2 >( "data", 0u );

		shader::Utils utils{ writer };

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
				auto usedTextureFlags = textureFlags & ( TextureFlag::eOpacity | TextureFlag::eHeight );
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
					, in.tangentSpaceFragPosition };
				materials.blendMaterials( utils
					, flags.alphaFunc
					, flags.passFlags
					, flags.submeshFlags
					, usedTextureFlags
					, !usedTextureFlags.empty()
					, textureConfigs
					, textureAnims
					, c3d_maps
					, modelData.getMaterialId()
					, in.passMultipliers
					, components );
				data = uvec2( ( in.nodeId << maxPipelinesSize ) | ( pipelineID )
					, ( checkFlag( flags.programFlags, ProgramFlag::eBillboards )
						? writer.cast< sdw::Float >( in.vertexId * 2_u + writer.cast< sdw::UInt >( in.primitiveID ) )
						: writer.cast< sdw::Float >( in.primitiveID ) ) );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
