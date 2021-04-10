#include "Castor3D/Render/Technique/ForwardRenderTechniquePass.hpp"

#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Node/RenderNode_Render.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( castor::String const & category
		, castor::String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config
		, LpvGridConfigUbo const * lpvConfigUbo
		, LayeredLpvGridConfigUbo const * llpvConfigUbo
		, VoxelizerUbo const * vctConfigUbo )
		: RenderTechniquePass{ category
			, name
			, matrixUbo
			, culler
			, environment
			, ignored
			, config
			, lpvConfigUbo
			, llpvConfigUbo
			, vctConfigUbo }
	{
	}

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( castor::String const & category
		, castor::String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool oit
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config
		, LpvGridConfigUbo const * lpvConfigUbo
		, LayeredLpvGridConfigUbo const * llpvConfigUbo
		, VoxelizerUbo const * vctConfigUbo )
		: RenderTechniquePass{ category
			, name
			, matrixUbo
			, culler
			, oit
			, environment
			, ignored
			, config
			, lpvConfigUbo
			, llpvConfigUbo
			, vctConfigUbo }
	{
	}

	void ForwardRenderTechniquePass::initialiseRenderPass( RenderDevice const & device
		, ashes::ImageView const & colourView
		, ashes::ImageView const & depthView
		, castor::Size const & size
		, bool clear )
	{
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				depthView.getFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				( clear
					? VK_ATTACHMENT_LOAD_OP_CLEAR
					: VK_ATTACHMENT_LOAD_OP_LOAD ),
				VK_ATTACHMENT_STORE_OP_STORE,
				( ( clear && ashes::isDepthStencilFormat( depthView.getFormat() ) )
					? VK_ATTACHMENT_LOAD_OP_CLEAR
					: VK_ATTACHMENT_LOAD_OP_DONT_CARE ),
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				( clear
					? VK_IMAGE_LAYOUT_UNDEFINED
					: VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ),
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			},
			{
				0u,
				colourView.getFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				( clear
					? VK_ATTACHMENT_LOAD_OP_CLEAR
					: VK_ATTACHMENT_LOAD_OP_LOAD ),
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				( clear
					? VK_IMAGE_LAYOUT_UNDEFINED
					: VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VkPipelineStageFlags( clear
					? VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					: VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ),
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VkAccessFlags( clear
					? VK_ACCESS_SHADER_READ_BIT
					: VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT ),
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VkPipelineStageFlags( clear
					? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					: VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT ),
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VkAccessFlags( clear
					? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					: VK_ACCESS_SHADER_READ_BIT ),
				VK_DEPENDENCY_BY_REGION_BIT,
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = device->createRenderPass( getName()
			, std::move( createInfo ) );
		ashes::ImageViewCRefArray fbAttaches;
		fbAttaches.emplace_back( depthView );
		fbAttaches.emplace_back( colourView );

		m_frameBuffer = m_renderPass->createFrameBuffer( getName()
			, { colourView.image->getDimensions().width, colourView.image->getDimensions().height }
			, std::move( fbAttaches ) );

		m_nodesCommands = device.graphicsCommandPool->createCommandBuffer( getName() );
	}

	void ForwardRenderTechniquePass::accept( RenderTechniqueVisitor & visitor )
	{
		auto shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, visitor.getFlags() );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	ashes::Semaphore const & ForwardRenderTechniquePass::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;

		static ashes::VkClearValueArray const clearValues
		{
			defaultClearDepthStencil,
			opaqueBlackClearColor,
		};

		RenderPassTimerBlock timerBlock{ getTimer().start() };

		m_nodesCommands->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		m_nodesCommands->beginDebugBlock(
			{
				"Forward Pass",
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );
		timerBlock->beginPass( *m_nodesCommands );
		timerBlock->notifyPassRender();
		m_nodesCommands->beginRenderPass( getRenderPass()
			, *m_frameBuffer
			, clearValues
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );

		if ( hasNodes() )
		{
			m_nodesCommands->executeCommands( { getCommandBuffer() } );
		}

		m_nodesCommands->endRenderPass();
		timerBlock->endPass( *m_nodesCommands );
		m_nodesCommands->endDebugBlock();
		m_nodesCommands->end();

		device.graphicsQueue->submit( { *m_nodesCommands }
			, { *result }
			, { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }
			, { getSemaphore() }
			, nullptr );
		result = &getSemaphore();

		return *result;
	}

	void ForwardRenderTechniquePass::doCleanup( RenderDevice const & device )
	{
		m_nodesCommands.reset();
		m_frameBuffer.reset();
		RenderTechniquePass::doCleanup( device );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		// Fragment Intputs
		auto inWorldPosition = writer.declInput< Vec3 >( "inWorldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto inViewPosition = writer.declInput< Vec3 >( "inViewPosition"
			, SceneRenderPass::VertexOutputs::ViewPositionLocation );
		auto inCurPosition = writer.declInput< Vec3 >( "inCurPosition"
			, SceneRenderPass::VertexOutputs::CurPositionLocation );
		auto inPrvPosition = writer.declInput< Vec3 >( "inPrvPosition"
			, SceneRenderPass::VertexOutputs::PrvPositionLocation );
		auto inTangentSpaceFragPosition = writer.declInput< Vec3 >( "inTangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto inTangentSpaceViewPosition = writer.declInput< Vec3 >( "inTangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto inBitangent = writer.declInput< Vec3 >( "inBitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto inInstance = writer.declInput< UInt >( "inInstance"
			, SceneRenderPass::VertexOutputs::InstanceLocation );
		auto inMaterial = writer.declInput< UInt >( "inMaterial"
			, SceneRenderPass::VertexOutputs::MaterialLocation );

		shader::LegacyMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		index += uint32_t( flags.textures.size() );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment"
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ) );
		shader::Utils utils{ writer };
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( 1u, index, flags.sceneFlags );

		auto in = writer.getIn();

		shader::Fog fog{ getFogType( flags.sceneFlags ), writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareParallaxMappingFunc( flags );
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, shader::ShadowOptions{ flags.sceneFlags, false }
			, index
			, m_mode != RenderMode::eTransparentOnly );
		shader::PhongReflectionModel reflections{ writer, utils };

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inMaterial ) );
				auto opacity = writer.declLocale( "opacity"
					, material.m_opacity );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto normal = writer.declLocale( "normal"
					, normalize( inNormal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( inTangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( inBitangent ) );
				auto diffuse = writer.declLocale( "diffuse"
					, utils.removeGamma( gamma, material.m_diffuse() ) );
				auto specular = writer.declLocale( "specular"
					, material.m_specular );
				auto shininess = writer.declLocale( "shininess"
					, material.m_shininess );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.m_emissive ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inTangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inTangentSpaceFragPosition );

				if ( hasTextures )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, inTexture );
					lighting->computeMapContributions( flags
						, gamma
						, textureConfigs
						, c3d_textureConfig
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
						, tangentSpaceViewPosition
						, tangentSpaceFragPosition );
				}

				utils.applyAlphaFunc( flags.blendAlphaFunc
					, opacity
					, material.m_alphaRef
					, false );
				utils.applyAlphaFunc( flags.alphaFunc
					, opacity
					, material.m_alphaRef );
				emissive *= diffuse;
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_cameraPosition.xyz() );
				auto lightDiffuse = writer.declLocale( "lightDiffuse"
					, vec3( 0.0_f ) );
				auto lightSpecular = writer.declLocale( "lightSpecular"
					, vec3( 0.0_f ) );
				shader::OutputComponents output{ lightDiffuse, lightSpecular };
				auto surface = writer.declLocale< shader::Surface >( "surface" );
				surface.create( in.fragCoord.xy(), inViewPosition, inWorldPosition, normal );
				lighting->computeCombined( worldEye
					, shininess
					, c3d_shadowReceiver
					, surface
					, output );
				lightSpecular *= specular;

				auto ambient = writer.declLocale( "ambient"
					, clamp( c3d_ambientLight.xyz() * material.m_ambient * diffuse
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
						, reflections.computeIncident( inWorldPosition, c3d_cameraPosition.xyz() ) );
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
				else
				{
					diffuse *= lightDiffuse;
				}

				ambient *= occlusion;
				auto colour = writer.declLocale( "colour"
					, indirect.computeDiffuse( flags.sceneFlags
						, surface
						, diffuse
						, diffuse + reflected + refracted + emissive
						, ambient
						, occlusion ) );
				colour += indirect.computeSpecular( flags.sceneFlags
					, worldEye
					, surface
					, ( 256.0_f - shininess ) / 256.0_f
					, specular
					, lightSpecular
					, occlusion );
				pxl_fragColor = vec4( colour, opacity );

				if ( getFogType( flags.sceneFlags ) != FogType::eDisabled )
				{
					pxl_fragColor = fog.apply( vec4( utils.removeGamma( gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
						, pxl_fragColor
						, length( inViewPosition )
						, inViewPosition.y()
						, c3d_fogInfo
						, c3d_cameraPosition );
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		// Fragment Intputs
		auto inWorldPosition = writer.declInput< Vec3 >( "inWorldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto inViewPosition = writer.declInput< Vec3 >( "inViewPosition"
			, SceneRenderPass::VertexOutputs::ViewPositionLocation );
		auto inCurPosition = writer.declInput< Vec3 >( "inCurPosition"
			, SceneRenderPass::VertexOutputs::CurPositionLocation );
		auto inPrvPosition = writer.declInput< Vec3 >( "inPrvPosition"
			, SceneRenderPass::VertexOutputs::PrvPositionLocation );
		auto inTangentSpaceFragPosition = writer.declInput< Vec3 >( "inTangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto inTangentSpaceViewPosition = writer.declInput< Vec3 >( "inTangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto inBitangent = writer.declInput< Vec3 >( "inBitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto inInstance = writer.declInput< UInt >( "inInstance"
			, SceneRenderPass::VertexOutputs::InstanceLocation );
		auto inMaterial = writer.declInput< UInt >( "inMaterial"
			, SceneRenderPass::VertexOutputs::MaterialLocation );

		shader::PbrMRMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		index += uint32_t( flags.textures.size() );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment"
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ) );
		auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, index++
			, 1u );
		auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, index++
			, 1u );
		auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, 1u );
		shader::Utils utils{ writer };
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( 1u, index, flags.sceneFlags );

		auto in = writer.getIn();

		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();
		utils.declareInvertVec3Y();
		utils.declareParallaxMappingFunc( flags );
		auto lighting = shader::MetallicBrdfLightingModel::createModel( writer
			, utils
			, shader::ShadowOptions{ flags.sceneFlags, false }
			, index
			, m_mode != RenderMode::eTransparentOnly );
		shader::MetallicPbrReflectionModel reflections{ writer, utils };
		shader::Fog fog{ getFogType( flags.sceneFlags ), writer };

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inMaterial ) );
				auto opacity = writer.declLocale( "opacity"
					, material.m_opacity );
				auto normal = writer.declLocale( "normal"
					, normalize( inNormal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( inTangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( inBitangent ) );
				auto ambient = writer.declLocale( "ambient"
					, c3d_ambientLight.xyz() );
				auto metalness = writer.declLocale( "metalness"
					, material.m_metallic );
				auto roughness = writer.declLocale( "roughness"
					, material.m_roughness );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto albedo = writer.declLocale( "albedo"
					, utils.removeGamma( gamma, material.m_albedo ) );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.m_emissive ) );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_cameraPosition.xyz() );
				auto envAmbient = writer.declLocale( "envAmbient"
					, vec3( 1.0_f ) );
				auto envDiffuse = writer.declLocale( "envDiffuse"
					, vec3( 1.0_f ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inTangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inTangentSpaceFragPosition );

				if ( hasTextures )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, inTexture );
					lighting->computeMapContributions( flags
						, gamma
						, textureConfigs
						, c3d_textureConfig
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
				}

				utils.applyAlphaFunc( flags.blendAlphaFunc
					, opacity
					, material.m_alphaRef
					, false );
				utils.applyAlphaFunc( flags.alphaFunc
					, opacity
					, material.m_alphaRef );
				emissive *= albedo;
				auto lightDiffuse = writer.declLocale( "lightDiffuse"
					, vec3( 0.0_f ) );
				auto lightSpecular = writer.declLocale( "lightSpecular"
					, vec3( 0.0_f ) );
				shader::OutputComponents output{ lightDiffuse, lightSpecular };
				auto surface = writer.declLocale< shader::Surface >( "surface" );
				surface.create( in.fragCoord.xy(), inViewPosition, inWorldPosition, normal );
				lighting->computeCombined( worldEye
					, albedo
					, metalness
					, roughness
					, c3d_shadowReceiver
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
						, reflections.computeIncident( inWorldPosition, worldEye ) );
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
							, c3d_cameraPosition.xyz()
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
						, c3d_cameraPosition.xyz()
						, c3d_mapIrradiance
						, c3d_mapPrefiltered
						, c3d_mapBrdf );
					auto ratio = writer.declLocale( "ratio"
						, material.m_refractionRatio );

					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						auto incident = writer.declLocale( "incident"
							, reflections.computeIncident( inWorldPosition, worldEye ) );
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

				ambient *= occlusion;
				auto colour = writer.declLocale( "colour"
					, indirect.computeDiffuse( flags.sceneFlags
						, surface
						, lightDiffuse * albedo
						, lightDiffuse * albedo + reflected + refracted + emissive
						, ambient
						, occlusion ) );
				colour += indirect.computeSpecular( flags.sceneFlags
					, worldEye
					, surface
					, roughness
					, mix( vec3( 0.04_f ), albedo, vec3( metalness ) )
					, lightSpecular
					, occlusion );
				pxl_fragColor = vec4( colour, opacity );

				if ( getFogType( flags.sceneFlags ) != FogType::eDisabled )
				{
					pxl_fragColor = fog.apply( vec4( utils.removeGamma( gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
						, pxl_fragColor
						, length( inViewPosition )
						, inViewPosition.y()
						, c3d_fogInfo
						, c3d_cameraPosition );
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		// Fragment Intputs
		auto inWorldPosition = writer.declInput< Vec3 >( "inWorldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto inViewPosition = writer.declInput< Vec3 >( "inViewPosition"
			, SceneRenderPass::VertexOutputs::ViewPositionLocation );
		auto inCurPosition = writer.declInput< Vec3 >( "inCurPosition"
			, SceneRenderPass::VertexOutputs::CurPositionLocation );
		auto inPrvPosition = writer.declInput< Vec3 >( "inPrvPosition"
			, SceneRenderPass::VertexOutputs::PrvPositionLocation );
		auto inTangentSpaceFragPosition = writer.declInput< Vec3 >( "inTangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto inTangentSpaceViewPosition = writer.declInput< Vec3 >( "inTangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto inBitangent = writer.declInput< Vec3 >( "inBitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto inInstance = writer.declInput< UInt >( "inInstance"
			, SceneRenderPass::VertexOutputs::InstanceLocation );
		auto inMaterial = writer.declInput< UInt >( "inMaterial"
			, SceneRenderPass::VertexOutputs::MaterialLocation );

		shader::PbrSGMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		index += uint32_t( flags.textures.size() );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment"
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ) );
		auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, index++
			, 1u );
		auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, index++
			, 1u );
		auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, 1u );
		shader::Utils utils{ writer };
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( 1u, index, flags.sceneFlags );

		auto in = writer.getIn();

		shader::Fog fog{ getFogType( flags.sceneFlags ), writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();
		utils.declareParallaxMappingFunc( flags );
		auto lighting = shader::SpecularBrdfLightingModel::createModel( writer
			, utils
			, shader::ShadowOptions{ flags.sceneFlags, false }
			, index
			, m_mode != RenderMode::eTransparentOnly );
		shader::SpecularPbrReflectionModel reflections{ writer, utils };

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inMaterial ) );
				auto opacity = writer.declLocale( "opacity"
					, material.m_opacity );
				auto normal = writer.declLocale( "normal"
					, normalize( inNormal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( inTangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( inBitangent ) );
				auto ambient = writer.declLocale( "ambient"
					, c3d_ambientLight.xyz() );
				auto specular = writer.declLocale( "specular"
					, material.m_specular );
				auto glossiness = writer.declLocale( "glossiness"
					, material.m_glossiness );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto albedo = writer.declLocale( "albedo"
					, utils.removeGamma( gamma, material.m_diffuse() ) );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.m_emissive ) );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_cameraPosition.xyz() );
				auto envAmbient = writer.declLocale( "envAmbient"
					, vec3( 1.0_f ) );
				auto envDiffuse = writer.declLocale( "envDiffuse"
					, vec3( 1.0_f ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inTangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inTangentSpaceFragPosition );

				if ( hasTextures )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, inTexture );
					lighting->computeMapContributions( flags
						, gamma
						, textureConfigs
						, c3d_textureConfig
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
				}

				utils.applyAlphaFunc( flags.blendAlphaFunc
					, opacity
					, material.m_alphaRef
					, false );
				utils.applyAlphaFunc( flags.alphaFunc
					, opacity
					, material.m_alphaRef );
				emissive *= albedo;
				auto lightDiffuse = writer.declLocale( "lightDiffuse"
					, vec3( 0.0_f ) );
				auto lightSpecular = writer.declLocale( "lightSpecular"
					, vec3( 0.0_f ) );
				shader::OutputComponents output{ lightDiffuse, lightSpecular };
				auto surface = writer.declLocale< shader::Surface >( "surface" );
				surface.create( in.fragCoord.xy(), inViewPosition, inWorldPosition, normal );
				lighting->computeCombined( worldEye
					, specular
					, glossiness
					, c3d_shadowReceiver
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
						, reflections.computeIncident( inWorldPosition, worldEye ) );
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
							, c3d_cameraPosition.xyz()
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
						, c3d_cameraPosition.xyz()
						, c3d_mapIrradiance
						, c3d_mapPrefiltered
						, c3d_mapBrdf );
					auto ratio = writer.declLocale( "ratio"
						, material.m_refractionRatio );

					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						auto incident = writer.declLocale( "incident"
							, reflections.computeIncident( inWorldPosition, worldEye ) );
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

				ambient *= occlusion;
				auto colour = writer.declLocale( "colour"
					, indirect.computeDiffuse( flags.sceneFlags
						, surface
						, lightDiffuse * albedo
						, lightDiffuse * albedo + reflected + refracted + emissive
						, ambient
						, occlusion ) );
				colour += indirect.computeSpecular( flags.sceneFlags
					, worldEye
					, surface
					, ( 1.0_f - glossiness )
					, specular
					, lightSpecular
					, occlusion );
				pxl_fragColor = vec4( colour, opacity );

				if ( getFogType( flags.sceneFlags ) != FogType::eDisabled )
				{
					pxl_fragColor = fog.apply( vec4( utils.removeGamma( gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
						, pxl_fragColor
						, length( inViewPosition )
						, inViewPosition.y()
						, c3d_fogInfo
						, c3d_cameraPosition );
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	//*********************************************************************************************
}
