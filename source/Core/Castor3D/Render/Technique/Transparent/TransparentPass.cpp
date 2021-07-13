#include "Castor3D/Render/Technique/Transparent/TransparentPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Pipeline/PipelineColorBlendStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		template< MaterialType MaterialT >
		ShaderPtr getPixelShaderSourceT( RenderSystem const & renderSystem
			, PipelineFlags const & flags
			, FilteredTextureFlags const & textures
			, TextureFlags const & texturesMask
			, ShaderFlags const & shaderFlags
			, bool hasSSAO
			, bool hasVelocity )
		{
			using namespace sdw;
			FragmentWriter writer;
			bool hasTextures = !flags.textures.empty();
			bool hasDiffuseGI = checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing )
				|| checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
				|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );

			shader::Utils utils{ writer, *renderSystem.getEngine() };
			utils.declareApplyGamma();
			utils.declareRemoveGamma();
			utils.declareLineariseDepth();
			utils.declareComputeAccumulation();
			utils.declareParallaxMappingFunc( flags.passFlags
				, texturesMask );

			shader::CookTorranceBRDF cookTorrance{ writer, utils };
			cookTorrance.declareDiffuse();

			// Fragment Intputs
			shader::InFragmentSurface inSurface{ writer
				, shaderFlags
				, hasTextures };

			shader::Materials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eMaterials )
				, RenderPipeline::eBuffers );
			shader::TextureConfigurations textureConfigs{ writer };

			if ( hasTextures )
			{
				textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
					, uint32_t( NodeUboIdx::eTextures )
					, RenderPipeline::eBuffers );
			}

			UBO_MODEL( writer
				, uint32_t( NodeUboIdx::eModel )
				, RenderPipeline::eBuffers );

			auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
				, 0u
				, RenderPipeline::eTextures
				, std::max( 1u, uint32_t( flags.textures.size() ) )
				, hasTextures ) );

			UBO_MATRIX( writer
				, uint32_t( PassUboIdx::eMatrix )
				, RenderPipeline::eAdditional );
			UBO_SCENE( writer
				, uint32_t( PassUboIdx::eScene )
				, RenderPipeline::eAdditional );
			auto index = uint32_t( PassUboIdx::eCount );
			auto lightsIndex = index++;
			auto c3d_mapOcclusion = writer.declSampledImage< FImg2DR32 >( "c3d_mapOcclusion"
				, ( hasSSAO ? index++ : 0u )
				, RenderPipeline::eAdditional
				, hasSSAO );
			auto reflections = shader::ReflectionModel::create( writer
				, utils
				, MaterialT
				, flags.passFlags
				, index
				, uint32_t( RenderPipeline::eAdditional ) );
			auto lightingModel = shader::LightingModel::createModel( utils
				, shader::getLightingModelName( MaterialT )
				, lightsIndex
				, RenderPipeline::eAdditional
				, shader::ShadowOptions{ flags.sceneFlags, false }
				, index
				, RenderPipeline::eAdditional
				, false );
			shader::GlobalIllumination indirect{ writer, utils };
			indirect.declare( index
				, RenderPipeline::eAdditional
				, flags.sceneFlags );

			auto in = writer.getIn();

			// Fragment Outputs
			auto pxl_accumulation( writer.declOutput< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
			auto pxl_revealage( writer.declOutput< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );
			auto pxl_velocity( writer.declOutput< Vec4 >( "pxl_velocity", 2 ) );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto normal = writer.declLocale( "normal"
						, normalize( inSurface.normal ) );
					auto tangent = writer.declLocale( "tangent"
						, normalize( inSurface.tangent ) );
					auto bitangent = writer.declLocale( "bitangent"
						, normalize( inSurface.bitangent ) );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( inSurface.material ) );
					auto gamma = writer.declLocale( "gamma"
						, material.gamma );
					auto opacity = writer.declLocale( "opacity"
						, material.opacity );
					auto lightMat = lightingModel->declMaterial( "lightMat" );
					lightMat->create( material );
					auto emissive = writer.declLocale( "emissive"
						, lightMat->albedo * material.emissive );
					auto worldEye = writer.declLocale( "worldEye"
						, c3d_sceneData.getCameraPosition() );
					auto texCoord = writer.declLocale( "texCoord"
						, inSurface.texture );
					auto occlusion = writer.declLocale( "occlusion"
						, 1.0_f );
					auto transmittance = writer.declLocale( "transmittance"
						, 1.0_f );

					if ( hasSSAO )
					{
						occlusion *= c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i );
					}

					lightingModel->computeMapContributions( flags.passFlags
						, textures
						, gamma
						, textureConfigs
						, c3d_maps
						, texCoord
						, normal
						, tangent
						, bitangent
						, emissive
						, opacity
						, occlusion
						, transmittance
						, *lightMat
						, inSurface.tangentSpaceViewPosition
						, inSurface.tangentSpaceFragPosition );
					utils.applyAlphaFunc( flags.blendAlphaFunc
						, opacity
						, material.alphaRef
						, false );

					if ( checkFlag( flags.passFlags, PassFlag::eLighting ) )
					{
						auto lightDiffuse = writer.declLocale( "lightDiffuse"
							, vec3( 0.0_f ) );
						auto lightSpecular = writer.declLocale( "lightSpecular"
							, vec3( 0.0_f ) );
						shader::OutputComponents output{ lightDiffuse, lightSpecular };
						auto surface = writer.declLocale< shader::Surface >( "surface" );
						surface.create( in.fragCoord.xy(), inSurface.viewPosition, inSurface.worldPosition, normal );
						lightingModel->computeCombined( *lightMat
							, c3d_sceneData
							, surface
							, worldEye
							, c3d_modelData.isShadowReceiver()
							, output );
						lightMat->adjustDirectSpecular( lightSpecular );

						auto ambient = writer.declLocale( "ambient"
							, lightMat->getAmbient( c3d_sceneData.getAmbientLight() ) );
						auto reflected = writer.declLocale( "reflected"
							, vec3( 0.0_f ) );
						auto refracted = writer.declLocale( "refracted"
							, vec3( 0.0_f ) );
						reflections->computeForward( *lightMat
							, surface
							, c3d_sceneData
							, material.refractionRatio
							, material.transmission
							, ambient
							, reflected
							, refracted );

						auto indirectOcclusion = writer.declLocale( "indirectOcclusion"
							, 1.0_f );
						auto lightIndirectDiffuse = indirect.computeDiffuse( flags.sceneFlags
							, surface
							, indirectOcclusion );
						auto lightIndirectSpecular = indirect.computeSpecular( flags.sceneFlags
							, worldEye
							, c3d_sceneData.getPosToCamera( surface.worldPosition )
							, surface
							, lightMat->specular
							, lightMat->getRoughness()
							, indirectOcclusion
							, lightIndirectDiffuse.w() );
						auto indirectAmbient = writer.declLocale( "indirectAmbient"
							, lightMat->getIndirectAmbient( indirect.computeAmbient( flags.sceneFlags, lightIndirectDiffuse.xyz() ) ) );
						auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
							, ( hasDiffuseGI
								? cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
									, c3d_sceneData.getCameraPosition()
									, surface.worldNormal
									, lightMat->specular
									, lightMat->getMetalness()
									, surface )
								: vec3( 0.0_f ) ) );

						auto colour = writer.declLocale( "colour"
							, lightingModel->combine( lightDiffuse
								, indirectDiffuse
								, lightSpecular
								, lightIndirectSpecular
								, ambient
								, indirectAmbient
								, occlusion
								, emissive
								, reflected
								, refracted
								, lightMat->albedo ) );
					}
					else
					{
						auto colour = writer.declLocale( "colour"
							, lightMat->albedo );
					}

					auto colour = writer.getVariable < Vec3 >( "colour" );
					pxl_accumulation = c3d_sceneData.computeAccumulation( utils
						, in.fragCoord.z()
						, colour
						, opacity
						, material.bwAccumulationOperator );
					pxl_revealage = opacity;
					pxl_velocity.xy() = inSurface.getVelocity();
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	TransparentPass::TransparentPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & category
		, castor::String const & name
		, SceneRenderPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: castor3d::RenderTechniquePass{ pass
			, context
			, graph
			, device
			, category
			, name
			, renderPassDesc
			, techniquePassDesc }
	{
	}

	TransparentPass::~TransparentPass()
	{
	}

	TextureFlags TransparentPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eAll };
	}

	void TransparentPass::accept( RenderTechniqueVisitor & visitor )
	{
		auto shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, visitor.getFlags() );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	ashes::PipelineDepthStencilStateCreateInfo TransparentPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, false };
	}

	ashes::PipelineColorBlendStateCreateInfo TransparentPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachments
		{
			VkPipelineColorBlendAttachmentState
			{
				true,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_OP_ADD,
				defaultColorWriteMask,
			},
			VkPipelineColorBlendAttachmentState
			{
				true,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				defaultColorWriteMask,
			},
			VkPipelineColorBlendAttachmentState
			{
				false,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_OP_ADD,
				defaultColorWriteMask,
			},
		};
		return ashes::PipelineColorBlendStateCreateInfo
		{
			0u,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			std::move( attachments ),
		};
	}

	ShaderPtr TransparentPass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		return getPixelShaderSourceT< MaterialType::ePhong >( *getEngine()->getRenderSystem()
			, flags
			, filterTexturesFlags( flags.textures )
			, getTexturesMask()
			, getShaderFlags()
			, m_ssao
			, m_hasVelocity );
	}

	ShaderPtr TransparentPass::doGetPbrPixelShaderSource( PipelineFlags const & flags )const
	{
		return getPixelShaderSourceT< MaterialType::eMetallicRoughness >( *getEngine()->getRenderSystem()
			, flags
			, filterTexturesFlags( flags.textures )
			, getTexturesMask()
			, getShaderFlags()
			, m_ssao
			, m_hasVelocity );
	}

	void TransparentPass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}
}
