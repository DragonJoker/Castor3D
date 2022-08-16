#include "Castor3D/Render/Passes/DepthPass.hpp"

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

CU_ImplementCUSmartPtr( castor3d, DepthPass )

namespace castor3d
{
	//*********************************************************************************************

	castor::String const DepthPass::Type = "c3d.depth";

	DepthPass::DepthPass( RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, SsaoConfig const & ssaoConfig
		, RenderNodesPassDesc const & renderPassDesc )
		: RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, pass.getName()
			, "DepthPass"
			, nullptr
			, renderPassDesc
			, { false, ssaoConfig } }
	{
	}

	TextureFlags DepthPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eAll };
	}

	PassFlags DepthPass::doAdjustPassFlags( PassFlags flags )const
	{
		remFlag( flags, PassFlag::eAlphaBlending );
		return flags;
	}

	ProgramFlags DepthPass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		remFlag( flags, ProgramFlag::eLighting );
		addFlag( flags, ProgramFlag::eDepthPass );
		return flags;
	}

	SceneFlags DepthPass::doAdjustSceneFlags( SceneFlags flags )const
	{
		remFlag( flags, SceneFlag::eLpvGI );
		remFlag( flags, SceneFlag::eLayeredLpvGI );
		remFlag( flags, SceneFlag::eVoxelConeTracing );
		return flags;
	}

	ashes::PipelineDepthStencilStateCreateInfo DepthPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_TRUE
			, VK_TRUE };
	}

	ashes::PipelineColorBlendStateCreateInfo DepthPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderNodesPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, 3u );
	}

	ShaderPtr DepthPass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr DepthPass::doGetPixelShaderSource( PipelineFlags const & flags )const
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

		// Outputs
		auto data0 = writer.declOutput< Vec4 >( "data0", 0u );
		auto data1 = writer.declOutput< Vec4 >( "data1", 1u );
		auto velocity = writer.declOutput< Vec4 >( "velocity", 2u );

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
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( in.nodeId ) - 1u] );
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
					, flags.textures
					, hasTextures
					, textureConfigs
					, textureAnims
					, c3d_maps
					, modelData.getMaterialId()
					, in.passMultipliers
					, components );
				data0 = vec4( in.fragCoord.z()
					, length( in.worldPosition.xyz() - c3d_sceneData.cameraPosition )
					, writer.cast< sdw::Float >( in.nodeId )
					, ( checkFlag( flags.programFlags, ProgramFlag::eBillboards )
						? writer.cast< sdw::Float >( in.vertexId * 2_i + in.primitiveID )
						: writer.cast< sdw::Float >( in.primitiveID ) ) );
				data1 = vec4( components.normal(), 0.0_f );
				velocity = vec4( in.getVelocity(), 0.0_f, 0.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
