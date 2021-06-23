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
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularPbrReflection.hpp"
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
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();
		auto & renderSystem = *getEngine()->getRenderSystem();

		shader::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();
		utils.declareComputeAccumulation();
		utils.declareParallaxMappingFunc( flags.passFlags
			, getTexturesMask() );
		utils.declareFresnelSchlick();

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };

		shader::PhongMaterials materials{ writer };
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
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_mapOcclusion = writer.declSampledImage< FImg2DR32 >( "c3d_mapOcclusion"
			, ( m_ssao ? index++ : 0u )
			, RenderPipeline::eAdditional
			, m_ssao != nullptr );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment"
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? index++ : 0u
			, RenderPipeline::eAdditional
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ) );
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, shader::ShadowOptions{ flags.sceneFlags, false }
			, index
			, RenderPipeline::eAdditional
			, false );
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( index
			, RenderPipeline::eAdditional
			, flags.sceneFlags );

		auto in = writer.getIn();

		shader::PhongReflectionModel reflections{ writer, utils };

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
				auto specular = writer.declLocale( "specular"
					, material.m_specular );
				auto shininess = writer.declLocale( "shininess"
					, material.m_shininess );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto diffuse = writer.declLocale( "diffuse"
					, utils.removeGamma( gamma, material.m_diffuse() ) );
				auto emissive = writer.declLocale( "emissive"
					, diffuse * material.m_emissive );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_sceneData.getCameraPosition() );
				auto texCoord = writer.declLocale( "texCoord"
					, inSurface.texture );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 1.0_f );
				auto opacity = writer.declLocale( "opacity"
					, material.m_opacity );
				auto environment = writer.declLocale( "environment"
					, 0_u );

				if ( m_ssao )
				{
					occlusion *= c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i );
				}

				lighting->computeMapContributions( flags.passFlags
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
					, diffuse
					, specular
					, shininess
					, inSurface.tangentSpaceViewPosition
					, inSurface.tangentSpaceFragPosition );
				utils.applyAlphaFunc( flags.blendAlphaFunc
					, opacity
					, material.m_alphaRef
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
					lighting->computeCombined( worldEye
						, shininess
						, c3d_modelData.isShadowReceiver()
						, c3d_sceneData
						, surface
						, output );
					lightSpecular *= specular;

					auto ambient = writer.declLocale( "ambient"
						, clamp( c3d_sceneData.getAmbientLight() * material.m_ambient * diffuse
							, vec3( 0.0_f )
							, vec3( 1.0_f ) ) );
					auto reflected = writer.declLocale( "reflected"
						, vec3( 0.0_f ) );
					auto refracted = writer.declLocale( "refracted"
						, vec3( 0.0_f ) );

					if ( checkFlag( flags.passFlags, PassFlag::eReflection )
						|| checkFlag( flags.passFlags, PassFlag::eRefraction ) )
					{
						auto incident = writer.declLocale( "incident"
							, reflections.computeIncident( inSurface.worldPosition, worldEye ) );
						ambient = vec3( 0.0_f );

						if ( checkFlag( flags.passFlags, PassFlag::eReflection )
							&& checkFlag( flags.passFlags, PassFlag::eRefraction ) )
						{
							reflections.computeReflRefr( incident
								, normal
								, c3d_mapEnvironment
								, material.m_refractionRatio
								, specular
								, material.m_transmission * diffuse
								, shininess
								, reflected
								, refracted );
						}
						else if ( checkFlag( flags.passFlags, PassFlag::eReflection ) )
						{
							reflected = reflections.computeRefl( incident
								, normal
								, c3d_mapEnvironment
								, shininess
								, specular );
						}
						else
						{
							reflections.computeRefr( incident
								, normal
								, c3d_mapEnvironment
								, material.m_refractionRatio
								, material.m_transmission * diffuse
								, shininess
								, reflected
								, refracted );
						}

						diffuse = vec3( 0.0_f );
					}

					auto roughness = writer.declLocale( "roughness"
						, ( 256.0_f - shininess ) / 256.0_f );
					auto indirectOcclusion = writer.declLocale( "indirectOcclusion"
						, 1.0_f );
					auto lightIndirectDiffuse = indirect.computeDiffuse( flags.sceneFlags
						, surface
						, indirectOcclusion );
					auto lightIndirectSpecular = indirect.computeSpecular( flags.sceneFlags
						, worldEye
						, surface
						, roughness
						, indirectOcclusion );
					auto V = writer.declLocale( "V"
						, normalize( c3d_sceneData.getPosToCamera( surface.worldPosition ) ) );
					auto NdotV = writer.declLocale( "NdotV"
						, max( 0.0_f, dot( surface.worldNormal, V ) ) );
					lightIndirectSpecular *= utils.fresnelSchlick( NdotV, specular, roughness );

					auto colour = writer.declLocale( "colour"
						, shader::PhongLightingModel::combine( lightDiffuse
							, lightIndirectDiffuse
							, lightSpecular
							, lightIndirectSpecular
							, ambient
							, indirect.computeAmbient( flags.sceneFlags, lightIndirectDiffuse )
							, occlusion
							, emissive
							, reflected + refracted
							, diffuse ) );
				}
				else
				{
					auto colour = writer.declLocale( "colour"
						, diffuse );
				}

				auto colour = writer.getVariable < Vec3 > ( "colour" );
				pxl_accumulation = c3d_sceneData.computeAccumulation( utils
					, in.fragCoord.z()
					, colour
					, opacity
					, material.m_bwAccumulationOperator );
				pxl_revealage = opacity;
				pxl_velocity.xy() = inSurface.getVelocity();
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr TransparentPass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();
		auto & renderSystem = *getEngine()->getRenderSystem();

		shader::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();
		utils.declareComputeAccumulation();
		utils.declareParallaxMappingFunc( flags.passFlags
			, getTexturesMask() );

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };

		shader::PbrMRMaterials materials{ writer };
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
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_mapOcclusion = writer.declSampledImage< FImg2DR32 >( "c3d_mapOcclusion"
			, ( m_ssao ? index++ : 0u )
			, RenderPipeline::eAdditional
			, m_ssao != nullptr );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment"
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? index++ : 0u
			, RenderPipeline::eAdditional
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ) );
		auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, RenderPipeline::eAdditional );
		auto lighting = shader::MetallicBrdfLightingModel::createModel( writer
			, utils
			, shader::ShadowOptions{ flags.sceneFlags, false }
			, index
			, RenderPipeline::eAdditional
			, false );
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( index
			, RenderPipeline::eAdditional
			, flags.sceneFlags );

		auto in = writer.getIn();
		shader::MetallicPbrReflectionModel reflections{ writer, utils };

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
				auto ambient = writer.declLocale( "ambient"
					, c3d_sceneData.getAmbientLight() );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inSurface.material ) );
				auto metalness = writer.declLocale( "metalness"
					, material.m_metallic );
				auto roughness = writer.declLocale( "roughness"
					, material.m_roughness );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto albedo = writer.declLocale( "albedo"
					, utils.removeGamma( gamma, material.m_albedo ) );
				auto emissive = writer.declLocale( "emissive"
					, albedo * material.m_emissive );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_sceneData.getCameraPosition() );
				auto envAmbient = writer.declLocale( "envAmbient"
					, vec3( 1.0_f ) );
				auto envDiffuse = writer.declLocale( "envDiffuse"
					, vec3( 1.0_f ) );
				auto texCoord = writer.declLocale( "texCoord"
					, inSurface.texture );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 1.0_f );
				auto opacity = writer.declLocale( "opacity"
					, material.m_opacity );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inSurface.tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inSurface.tangentSpaceFragPosition );

				if ( m_ssao )
				{
					occlusion *= c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i );
				}

				lighting->computeMapContributions( flags.passFlags
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
					, albedo
					, metalness
					, roughness
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
				utils.applyAlphaFunc( flags.blendAlphaFunc
					, opacity
					, material.m_alphaRef
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
					lighting->computeCombined( worldEye
						, albedo
						, metalness
						, roughness
						, c3d_modelData.isShadowReceiver()
						, c3d_sceneData
						, surface
						, output );
					auto reflected = writer.declLocale( "reflected"
						, vec3( 0.0_f ) );
					auto refracted = writer.declLocale( "refracted"
						, vec3( 0.0_f ) );

					if ( checkFlag( flags.passFlags, PassFlag::eReflection )
						|| checkFlag( flags.passFlags, PassFlag::eRefraction ) )
					{
						auto incident = writer.declLocale( "incident"
							, reflections.computeIncident( inSurface.worldPosition, worldEye ) );
						auto ratio = writer.declLocale( "ratio"
							, material.m_refractionRatio );

						if ( checkFlag( flags.passFlags, PassFlag::eReflection ) )
						{
							// Reflection from environment map.
							ambient = vec3( 0.0_f );
							reflected = reflections.computeRefl( incident
								, normal
								, c3d_mapEnvironment
								, albedo
								, metalness
								, roughness );

							if ( checkFlag( flags.passFlags, PassFlag::eRefraction ) )
							{
								// Refraction from environment map.
								reflections.computeRefrEnvMap( incident
									, normal
									, c3d_mapEnvironment
									, ratio
									, material.m_transmission * albedo
									, roughness
									, reflected
									, refracted );
							}
							else
							{
								IF( writer, ratio != 0.0_f )
								{
									// Refraction from background skybox.
									reflections.computeRefrSkybox( incident
										, normal
										, c3d_mapPrefiltered
										, material.m_refractionRatio
										, material.m_transmission * albedo
										, roughness
										, reflected
										, refracted );
								}
								FI;
							}
						}
						else
						{
							// Reflection from background skybox.
							ambient *= utils.computeMetallicIBL( surface
								, albedo
								, metalness
								, roughness
								, worldEye
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );

							if ( checkFlag( flags.passFlags, PassFlag::eRefraction ) )
							{
								// Refraction from environment map.
								reflections.computeRefrEnvMap( incident
									, normal
									, c3d_mapEnvironment
									, ratio
									, material.m_transmission * albedo
									, roughness
									, ambient
									, refracted );
							}
							else
							{
								IF( writer, ratio != 0.0_f )
								{
									// Refraction from background skybox.
									reflections.computeRefrSkybox( incident
										, normal
										, c3d_mapPrefiltered
										, material.m_refractionRatio
										, material.m_transmission * albedo
										, roughness
										, ambient
										, refracted );
								}
								FI;
							}
						}
					}
					else
					{
						// Reflection from background skybox.
						ambient *= utils.computeMetallicIBL( surface
							, albedo
							, metalness
							, roughness
							, worldEye
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
						auto ratio = writer.declLocale( "ratio"
							, material.m_refractionRatio );

						IF( writer, ratio != 0.0_f )
						{
							// Refraction from background skybox.
							auto incident = writer.declLocale( "incident"
								, reflections.computeIncident( inSurface.worldPosition, worldEye ) );
							reflections.computeRefrSkybox( incident
								, normal
								, c3d_mapPrefiltered
								, material.m_refractionRatio
								, material.m_transmission * albedo
								, roughness
								, ambient
								, refracted );
						}
						FI;
					}

					auto specular = writer.declLocale( "specular"
						, mix( vec3( 0.04_f ), albedo, vec3( metalness ) ) );
					auto indirectOcclusion = writer.declLocale( "indirectOcclusion"
						, 1.0_f );
					auto lightIndirectDiffuse = indirect.computeDiffuse( flags.sceneFlags
						, surface
						, indirectOcclusion );
					auto lightIndirectSpecular = indirect.computeSpecular( flags.sceneFlags
						, worldEye
						, surface
						, roughness
						, indirectOcclusion );
					auto V = writer.declLocale( "V"
						, normalize( c3d_sceneData.getPosToCamera( surface.worldPosition ) ) );
					auto NdotV = writer.declLocale( "NdotV"
						, max( 0.0_f, dot( surface.worldNormal, V ) ) );
					lightIndirectSpecular *= utils.fresnelSchlick( NdotV, specular, roughness );

					auto colour = writer.declLocale( "colour"
						, shader::MetallicBrdfLightingModel::combine( lightDiffuse
							, lightIndirectDiffuse
							, lightSpecular
							, lightIndirectSpecular
							, ambient
							, indirect.computeAmbient( flags.sceneFlags, lightIndirectDiffuse )
							, occlusion
							, emissive
							, reflected + refracted
							, albedo ) );
				}
				else
				{
					auto colour = writer.declLocale( "colour"
						, albedo );
				}

				auto colour = writer.getVariable < Vec3 >( "colour" );
				pxl_accumulation = c3d_sceneData.computeAccumulation( utils
					, in.fragCoord.z()
					, colour
					, opacity
					, material.m_bwAccumulationOperator );
				pxl_revealage = opacity;
				pxl_velocity.xy() = inSurface.getVelocity();
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr TransparentPass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();
		auto & renderSystem = *getEngine()->getRenderSystem();

		shader::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();
		utils.declareComputeAccumulation();
		utils.declareParallaxMappingFunc( flags.passFlags
			, getTexturesMask() );

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };

		shader::PbrSGMaterials materials{ writer };
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
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_mapOcclusion = writer.declSampledImage< FImg2DR32 >( "c3d_mapOcclusion"
			, ( m_ssao ? index++ : 0u )
			, RenderPipeline::eAdditional
			, m_ssao != nullptr );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment"
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? index++ : 0u
			, RenderPipeline::eAdditional
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ) );
		auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, RenderPipeline::eAdditional );
		auto lighting = shader::SpecularBrdfLightingModel::createModel( writer
			, utils
			, shader::ShadowOptions{ flags.sceneFlags, false }
			, index
			, RenderPipeline::eAdditional
			, false );
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( index
			, RenderPipeline::eAdditional
			, flags.sceneFlags );

		auto in = writer.getIn();
		shader::SpecularPbrReflectionModel reflections{ writer, utils };

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
				auto ambient = writer.declLocale( "ambient"
					, c3d_sceneData.getAmbientLight() );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inSurface.material ) );
				auto specular = writer.declLocale( "specular"
					, material.m_specular );
				auto glossiness = writer.declLocale( "glossiness"
					, material.m_glossiness );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto albedo = writer.declLocale( "albedo"
					, utils.removeGamma( gamma, material.m_diffuse() ) );
				auto emissive = writer.declLocale( "emissive"
					, albedo * material.m_emissive );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_sceneData.getCameraPosition() );
				auto envAmbient = writer.declLocale( "envAmbient"
					, vec3( 1.0_f ) );
				auto envDiffuse = writer.declLocale( "envDiffuse"
					, vec3( 1.0_f ) );
				auto texCoord = writer.declLocale( "texCoord"
					, inSurface.texture );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto opacity = writer.declLocale( "opacity"
					, material.m_opacity );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inSurface.tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inSurface.tangentSpaceFragPosition );

				if ( m_ssao )
				{
					occlusion *= c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i );
				}

				lighting->computeMapContributions( flags.passFlags
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
					, albedo
					, specular
					, glossiness
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
				utils.applyAlphaFunc( flags.blendAlphaFunc
					, opacity
					, material.m_alphaRef
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
					lighting->computeCombined( worldEye
						, specular
						, glossiness
						, c3d_modelData.isShadowReceiver()
						, c3d_sceneData
						, surface
						, output );
					auto reflected = writer.declLocale( "reflected"
						, vec3( 0.0_f ) );
					auto refracted = writer.declLocale( "refracted"
						, vec3( 0.0_f ) );

					if ( checkFlag( flags.passFlags, PassFlag::eReflection )
						|| checkFlag( flags.passFlags, PassFlag::eRefraction ) )
					{
						auto incident = writer.declLocale( "incident"
							, reflections.computeIncident( inSurface.worldPosition, worldEye ) );
						auto ratio = writer.declLocale( "ratio"
							, material.m_refractionRatio );

						if ( checkFlag( flags.passFlags, PassFlag::eReflection ) )
						{
							// Reflection from environment map.
							ambient = vec3( 0.0_f );
							reflected = reflections.computeRefl( incident
								, normal
								, c3d_mapEnvironment
								, specular
								, glossiness );

							if ( checkFlag( flags.passFlags, PassFlag::eRefraction ) )
							{
								// Refraction from environment map.
								reflections.computeRefrEnvMap( incident
									, normal
									, c3d_mapEnvironment
									, ratio
									, material.m_transmission * albedo
									, glossiness
									, reflected
									, refracted );
							}
							else
							{
								IF( writer, ratio != 0.0_f )
								{
									// Refraction from background skybox.
									reflections.computeRefrSkybox( incident
										, normal
										, c3d_mapPrefiltered
										, material.m_refractionRatio
										, material.m_transmission * albedo
										, glossiness
										, reflected
										, refracted );
								}
								FI;
							}
						}
						else
						{
							// Reflection from background skybox.
							ambient *= utils.computeSpecularIBL( surface
								, albedo
								, specular
								, glossiness
								, worldEye
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );

							if ( checkFlag( flags.passFlags, PassFlag::eRefraction ) )
							{
								// Refraction from environment map.
								reflections.computeRefrEnvMap( incident
									, normal
									, c3d_mapEnvironment
									, ratio
									, material.m_transmission * albedo
									, glossiness
									, ambient
									, refracted );
							}
							else
							{
								IF( writer, ratio != 0.0_f )
								{
									// Refraction from background skybox.
									reflections.computeRefrSkybox( incident
										, normal
										, c3d_mapPrefiltered
										, material.m_refractionRatio
										, material.m_transmission * albedo
										, glossiness
										, ambient
										, refracted );
								}
								FI;
							}
						}
					}
					else
					{
						// Reflection from background skybox.
						ambient *= utils.computeSpecularIBL( surface
							, albedo
							, specular
							, glossiness
							, worldEye
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
						auto ratio = writer.declLocale( "ratio"
							, material.m_refractionRatio );

						IF( writer, ratio != 0.0_f )
						{
							// Refraction from background skybox.
							auto incident = writer.declLocale( "incident"
								, reflections.computeIncident( inSurface.worldPosition, worldEye ) );
							reflections.computeRefrSkybox( incident
								, normal
								, c3d_mapPrefiltered
								, material.m_refractionRatio
								, material.m_transmission * albedo
								, glossiness
								, ambient
								, refracted );
						}
						FI;
					}

					auto roughness = writer.declLocale( "roughness"
						, 1.0_f - glossiness );
					auto indirectOcclusion = writer.declLocale( "indirectOcclusion"
						, 1.0_f );
					auto lightIndirectDiffuse = indirect.computeDiffuse( flags.sceneFlags
						, surface
						, indirectOcclusion );
					auto lightIndirectSpecular = indirect.computeSpecular( flags.sceneFlags
						, worldEye
						, surface
						, roughness
						, indirectOcclusion );
					auto V = writer.declLocale( "V"
						, normalize( c3d_sceneData.getPosToCamera( surface.worldPosition ) ) );
					auto NdotV = writer.declLocale( "NdotV"
						, max( 0.0_f, dot( surface.worldNormal, V ) ) );
					lightIndirectSpecular *= utils.fresnelSchlick( NdotV, specular, roughness );

					auto colour = writer.declLocale( "colour"
						, shader::MetallicBrdfLightingModel::combine( lightDiffuse
							, lightIndirectDiffuse
							, lightSpecular
							, lightIndirectSpecular
							, ambient
							, indirect.computeAmbient( flags.sceneFlags, lightIndirectDiffuse )
							, occlusion
							, emissive
							, reflected + refracted
							, albedo ) );
				}
				else
				{
					auto colour = writer.declLocale( "colour"
						, albedo );
				}

				auto colour = writer.getVariable < Vec3 > ( "colour" );
				pxl_accumulation = c3d_sceneData.computeAccumulation( utils
					, in.fragCoord.z()
					, colour
					, opacity
					, material.m_bwAccumulationOperator );
				pxl_revealage = opacity;
				pxl_velocity.xy() = inSurface.getVelocity();
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	void TransparentPass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}
}
