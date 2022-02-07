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
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

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
		, SceneRenderPassDesc const & renderPassDesc )
		: RenderTechniquePass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, pass.getName()
			, "DepthPass"
			, renderPassDesc
			, { false, ssaoConfig } }
	{
	}

	TextureFlags DepthPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eOpacity
			| TextureFlag::eNormal
			| TextureFlag::eHeight };
	}

	void DepthPass::doUpdateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.programFlags, ProgramFlag::eInvertNormals );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		remFlag( flags.sceneFlags, SceneFlag::eLpvGI );
		remFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );
		remFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing );
		addFlag( flags.programFlags, ProgramFlag::eDepthPass );
	}

	void DepthPass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}

	ashes::PipelineDepthStencilStateCreateInfo DepthPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_TRUE
			, VK_TRUE };
	}

	ashes::PipelineColorBlendStateCreateInfo DepthPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return SceneRenderPass::createBlendState( BlendMode::eNoBlend
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
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		auto materials = shader::createMaterials( writer, flags.passFlags );
		materials->declare( uint32_t( NodeUboIdx::eMaterials )
			, RenderPipeline::eBuffers );
		shader::TextureConfigurations textureConfigs{ writer };
		shader::TextureAnimations textureAnims{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( uint32_t( NodeUboIdx::eTexConfigs )
				, RenderPipeline::eBuffers );
			textureAnims.declare( uint32_t( NodeUboIdx::eTexAnims )
				, RenderPipeline::eBuffers );
		}

		UBO_MODEL( writer
			, uint32_t( NodeUboIdx::eModel )
			, RenderPipeline::eBuffers );

		UBO_SCENE( writer
			, uint32_t( PassUboIdx::eScene )
			, RenderPipeline::eAdditional );

		// Outputs
		auto data0 = writer.declOutput< Vec4 >( "data0", 0u );
		auto data1 = writer.declOutput< Vec4 >( "data1", 1u );
		auto velocity = writer.declOutput< Vec4 >( "velocity", 2u );

		shader::Utils utils{ writer, *renderSystem.getEngine() };

		writer.implementMainT< shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, FragmentOut{ writer }
			, [&]( FragmentInT< shader::FragmentSurfaceT > in
				, FragmentOut out )
			{
				auto material = materials->getMaterial( in.material );
				auto opacity = writer.declLocale( "opacity"
					, material.opacity );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material.alphaRef );
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent ) );

				if ( hasTextures )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, in.texture0 );

					for ( uint32_t index = 0u; index < flags.textures.size(); ++index )
					{
						auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
						auto id = writer.declLocale( "c3d_id" + name
							, c3d_modelData.getTexture( index ) );

						IF( writer, id > 0_u )
						{
							auto config = writer.declLocale( "config" + name
								, textureConfigs.getTextureConfiguration( id ) );
							auto anim = writer.declLocale( "anim" + name
								, textureAnims.getTextureAnimation( id ) );

							IF( writer, config.isGeometry() )
							{
								utils.computeGeometryMapContribution( flags.passFlags
									, name
									, config
									, anim
									, c3d_maps[nonuniform( id - 1_u )]
									, texCoord
									, opacity
									, normal
									, tangent
									, bitangent
									, in.tangentSpaceViewPosition
									, in.tangentSpaceFragPosition );
							}
							FI;
						}
						FI;
					}
				}

				utils.applyAlphaFunc( flags.alphaFunc
					, opacity
					, alphaRef );
				auto matFlags = writer.declLocale( "flags"
					, 0.0_f );
				utils.encodeMaterial( c3d_modelData.isShadowReceiver()
					, ( checkFlag( flags.passFlags, PassFlag::eReflection ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eLighting ) ) ? 1_i : 0_i
					, c3d_modelData.getEnvMapIndex()
					, matFlags );
				data0 = vec4( in.fragCoord.z()
					, length( in.worldPosition.xyz() - c3d_sceneData.cameraPosition )
					, writer.cast< sdw::Float >( in.nodeId )
					, writer.cast< sdw::Float >( in.material ) );
				data1 = vec4( normal, matFlags );
				velocity = vec4( in.getVelocity(), 0.0_f, 0.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
