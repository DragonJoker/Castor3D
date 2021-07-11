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
#include "Castor3D/Shader/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongReflection.hpp"
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

		shader::CookTorranceBRDF cookTorrance{ writer, utils };
		cookTorrance.declareDiffuse();

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
		auto lightsIndex = index++;
		auto c3d_mapOcclusion = writer.declSampledImage< FImg2DR32 >( "c3d_mapOcclusion"
			, ( m_ssao ? index++ : 0u )
			, RenderPipeline::eAdditional
			, m_ssao != nullptr );
		shader::PhongReflectionModel reflections{ writer
			, utils
			, flags.passFlags
			, index
			, uint32_t( RenderPipeline::eAdditional ) };
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
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
				auto lightMat = writer.declLocale< shader::PhongLightMaterial >( "lightMat" );
				lightMat.create< MaterialType::ePhong >( material.diffuse
					, material.gamma
					, material.specular
					, material.shininess );
				auto emissive = writer.declLocale( "emissive"
					, lightMat.albedo * material.emissive );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_sceneData.getCameraPosition() );
				auto texCoord = writer.declLocale( "texCoord"
					, inSurface.texture );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 1.0_f );
				auto opacity = writer.declLocale( "opacity"
					, material.opacity );
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
					, lightMat
					, inSurface.tangentSpaceViewPosition
					, inSurface.tangentSpaceFragPosition );
				utils.applyAlphaFunc( flags.blendAlphaFunc
					, opacity
					, material.alphaRef
					, false );

				if ( checkFlag( flags.passFlags, PassFlag::eLighting ) )
				{
					auto lightMat = writer.declLocale< shader::PhongLightMaterial >( "lightMat" );
					lightMat.create< MaterialType::ePhong >( lightMat.albedo
						, lightMat.specular
						, lightMat.shininess );
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					shader::OutputComponents output{ lightDiffuse, lightSpecular };
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( in.fragCoord.xy(), inSurface.viewPosition, inSurface.worldPosition, normal );
					lighting->computeCombined( worldEye
						, lightMat
						, c3d_modelData.isShadowReceiver()
						, c3d_sceneData
						, surface
						, output );
					lightSpecular *= lightMat.specular;

					auto ambient = writer.declLocale( "ambient"
						, clamp( c3d_sceneData.getAmbientLight() * lightMat.albedo
							, vec3( 0.0_f )
							, vec3( 1.0_f ) ) );
					auto reflected = writer.declLocale( "reflected"
						, vec3( 0.0_f ) );
					auto refracted = writer.declLocale( "refracted"
						, vec3( 0.0_f ) );
					reflections.computeForward( material.refractionRatio
						, lightMat
						, material.transmission
						, surface
						, c3d_sceneData
						, ambient
						, lightMat.albedo
						, reflected
						, refracted );
					auto roughness = writer.declLocale( "roughness"
						, lighting->computeRoughness( lighting->computeGlossiness( lightMat.shininess ) ) );
					auto indirectOcclusion = writer.declLocale( "indirectOcclusion"
						, 1.0_f );
					auto lightIndirectDiffuse = indirect.computeDiffuse( flags.sceneFlags
						, surface
						, indirectOcclusion );
					auto lightIndirectSpecular = indirect.computeSpecular( flags.sceneFlags
						, worldEye
						, c3d_sceneData.getPosToCamera( surface.worldPosition )
						, surface
						, lightMat.specular
						, roughness
						, indirectOcclusion
						, lightIndirectDiffuse.w() );

					bool hasDiffuseGI = checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing )
						|| checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
						|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );
					auto indirectAmbient = writer.declLocale( "indirectAmbient"
						, material.ambient * indirect.computeAmbient( flags.sceneFlags, lightIndirectDiffuse.xyz() ) );
					auto pbrLightMat = writer.declLocale< shader::PbrLightMaterial >( "pbrLightMat"
						, hasDiffuseGI );
					pbrLightMat.create< MaterialType::ePhong >( lightMat.albedo
						, lightMat.specular
						, lightMat.shininess );
					auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
						, ( hasDiffuseGI
							? cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
								, c3d_sceneData.getCameraPosition()
								, surface.worldNormal
								, pbrLightMat
								, surface )
							: vec3( 0.0_f ) ) );

					auto colour = writer.declLocale( "colour"
						, shader::PhongLightingModel::combine( lightDiffuse
							, indirectDiffuse
							, lightSpecular
							, lightIndirectSpecular
							, ambient
							, indirectAmbient
							, occlusion
							, emissive
							, reflected
							, refracted
							, lightMat.albedo ) );
				}
				else
				{
					auto colour = writer.declLocale( "colour"
						, lightMat.albedo );
				}

				auto colour = writer.getVariable < Vec3 > ( "colour" );
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
		auto lightsIndex = index++;
		auto c3d_mapOcclusion = writer.declSampledImage< FImg2DR32 >( "c3d_mapOcclusion"
			, ( m_ssao ? index++ : 0u )
			, RenderPipeline::eAdditional
			, m_ssao != nullptr );
		shader::PbrReflectionModel reflections{ writer
			, utils
			, flags.passFlags
			, index
			, uint32_t( RenderPipeline::eAdditional ) };
		auto lighting = shader::PbrLightingModel::createModel( writer
			, utils
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

		shader::CookTorranceBRDF cookTorrance{ writer, utils };
		cookTorrance.declareDiffuse();

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
				auto gamma = writer.declLocale( "gamma"
					, material.gamma );
				auto lightMat = writer.declLocale< shader::PbrLightMaterial >( "lightMat" );
				lightMat.create< MaterialType::eMetallicRoughness >( material.albedo
					, material.metalness
					, material.roughness );
				auto emissive = writer.declLocale( "emissive"
					, lightMat.albedo * material.emissive );
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
					, material.opacity );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inSurface.tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inSurface.tangentSpaceFragPosition );

				if ( m_ssao )
				{
					occlusion *= c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i );
				}

				lighting->computeMapContributions( flags.passFlags
					, MaterialType::eMetallicRoughness
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
					, lightMat
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
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
					lighting->computeCombined( worldEye
						, lightMat
						, c3d_modelData.isShadowReceiver()
						, c3d_sceneData
						, surface
						, output );
					auto reflected = writer.declLocale( "reflected"
						, vec3( 0.0_f ) );
					auto refracted = writer.declLocale( "refracted"
						, vec3( 0.0_f ) );
					reflections.computeForward( material.refractionRatio
						, lightMat.albedo
						, lightMat
						, material.transmission
						, surface
						, c3d_sceneData
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
						, lightMat.specular
						, lightMat.roughness
						, indirectOcclusion
						, lightIndirectDiffuse.w() );
					auto indirectAmbient = writer.declLocale( "indirectAmbient"
						, indirect.computeAmbient( flags.sceneFlags, lightIndirectDiffuse.xyz() ) );
					lightIndirectDiffuse.xyz() = cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
						, c3d_sceneData.getCameraPosition()
						, -normal
						, lightMat
						, surface );

					auto colour = writer.declLocale( "colour"
						, shader::PbrLightingModel::combine( lightDiffuse
							, lightIndirectDiffuse.xyz()
							, lightSpecular
							, lightIndirectSpecular
							, ambient
							, indirectAmbient
							, occlusion
							, emissive
							, reflected
							, refracted
							, lightMat.albedo ) );
				}
				else
				{
					auto colour = writer.declLocale( "colour"
						, lightMat.albedo );
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
		auto lightsIndex = index++;
		auto c3d_mapOcclusion = writer.declSampledImage< FImg2DR32 >( "c3d_mapOcclusion"
			, ( m_ssao ? index++ : 0u )
			, RenderPipeline::eAdditional
			, m_ssao != nullptr );
		shader::PbrReflectionModel reflections{ writer
			, utils
			, flags.passFlags
			, index
			, uint32_t( RenderPipeline::eAdditional ) };
		auto lighting = shader::PbrLightingModel::createModel( writer
			, utils
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

		shader::CookTorranceBRDF cookTorrance{ writer, utils };
		cookTorrance.declareDiffuse();

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
				auto gamma = writer.declLocale( "gamma"
					, material.gamma );
				auto lightMat = writer.declLocale< shader::PbrLightMaterial >( "lightMat" );
				lightMat.create< MaterialType::eSpecularGlossiness >( material.albedo
					, material.specular
					, material.glossiness );
				auto emissive = writer.declLocale( "emissive"
					, lightMat.albedo * material.emissive );
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
					, material.opacity );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inSurface.tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inSurface.tangentSpaceFragPosition );

				if ( m_ssao )
				{
					occlusion *= c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i );
				}

				lighting->computeMapContributions( flags.passFlags
					, MaterialType::eSpecularGlossiness
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
					, lightMat
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
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
					lighting->computeCombined( worldEye
						, lightMat
						, c3d_modelData.isShadowReceiver()
						, c3d_sceneData
						, surface
						, output );
					auto reflected = writer.declLocale( "reflected"
						, vec3( 0.0_f ) );
					auto refracted = writer.declLocale( "refracted"
						, vec3( 0.0_f ) );
					reflections.computeForward( material.refractionRatio
						, lightMat.albedo
						, lightMat
						, material.transmission
						, surface
						, c3d_sceneData
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
						, lightMat.specular
						, lightMat.roughness
						, indirectOcclusion
						, lightIndirectDiffuse.w() );
					auto indirectAmbient = writer.declLocale( "indirectAmbient"
						, indirect.computeAmbient( flags.sceneFlags, lightIndirectDiffuse.xyz() ) );
					lightIndirectDiffuse.xyz() = cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
						, c3d_sceneData.getCameraPosition()
						, -normal
						, lightMat
						, surface );

					auto colour = writer.declLocale( "colour"
						, shader::PbrLightingModel::combine( lightDiffuse
							, lightIndirectDiffuse.xyz()
							, lightSpecular
							, lightIndirectSpecular
							, ambient
							, indirectAmbient
							, occlusion
							, emissive
							, reflected
							, refracted
							, lightMat.albedo ) );
				}
				else
				{
					auto colour = writer.declLocale( "colour"
						, lightMat.albedo );
				}

				auto colour = writer.getVariable < Vec3 > ( "colour" );
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

	void TransparentPass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}
}
