#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

namespace castor3d
{
	castor::String const ForwardRenderTechniquePass::Type = "c3d.forward";

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & typeName
		, crg::ImageData const * targetImage
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, typeName
			, targetImage
			, renderPassDesc
			, techniquePassDesc }
	{
	}

	void ForwardRenderTechniquePass::accept( RenderTechniqueVisitor & visitor )
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

	ComponentModeFlags ForwardRenderTechniquePass::getComponentsMask()const
	{
		ComponentModeFlags result{ ( ComponentModeFlag::eColour
			| ComponentModeFlag::eDiffuseLighting
			| ComponentModeFlag::eSpecularLighting
			| ComponentModeFlag::eNormals
			| ComponentModeFlag::eGeometry
			| ComponentModeFlag::eOcclusion
			| ComponentModeFlag::eSpecifics ) };

		if ( !checkFlag( m_filters, RenderFilter::eAlphaTest )
			|| !checkFlag( m_filters, RenderFilter::eAlphaBlend ) )
		{
			result |= ComponentModeFlag::eOpacity;

			if ( !checkFlag( m_filters, RenderFilter::eAlphaBlend ) )
			{
				result |= ComponentModeFlag::eAlphaBlending;
			}
		}

		return result;
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		bool enableTextures = flags.enableTextures();
		bool hasDiffuseGI = flags.hasDiffuseGI();

		shader::Utils utils{ writer };
		shader::BRDFHelpers brdf{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };
		shader::CookTorranceBRDF cookTorrance{ writer, utils, brdf };
		auto index = uint32_t( GlobalBuffersIdx::eCount );

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ *getEngine()
			, writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers
			, index };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, enableTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, enableTextures };
		auto lightsIndex = index++;
		auto c3d_mapOcclusion = writer.declCombinedImg< FImg2DR32 >( "c3d_mapOcclusion"
			, ( m_ssao ? index++ : 0u )
			, RenderPipeline::eBuffers
			, m_ssao != nullptr );
		auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, RenderPipeline::eBuffers );
		auto lightingModel = shader::LightingModel::createModel( *getEngine()
			, materials
			, utils
			, brdf
			, getScene().getLightingModel()
			, lightsIndex
			, RenderPipeline::eBuffers
			, shader::ShadowOptions{ flags.getShadowFlags(), true, false }
			, nullptr
			, index
			, RenderPipeline::eBuffers
			, checkFlag( m_filters, RenderFilter::eAlphaBlend ) );
		auto reflections = lightingModel->getReflectionModel( index
			, uint32_t( RenderPipeline::eBuffers ) );
		auto backgroundModel = shader::BackgroundModel::createModel( getScene()
			, writer
			, utils
			, makeExtent2D( m_size )
			, true
			, index
			, RenderPipeline::eBuffers );
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( index
			, RenderPipeline::eBuffers
			, flags.getGlobalIlluminationFlags() );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		shader::Fog fog{ writer };

		// Fragment Outputs
		auto outColour( writer.declOutput< Vec4 >( "outColour", 0 ) );
		auto outVelocity( writer.declOutput< Vec4 >( "outVelocity", 1, flags.writeVelocity() ) );

		writer.implementMainT< shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
			, FragmentOut{ writer }
			, [&]( FragmentInT< shader::FragmentSurfaceT > in
				, FragmentOut out )
			{
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1u] );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				auto components = writer.declLocale( "components"
					, shader::BlendComponents{ materials
						, material
						, in } );
				components.occlusion = ( m_ssao
					? c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i )
					: 1.0_f );
				materials.blendMaterials( !checkFlag( m_filters, RenderFilter::eAlphaBlend )
					, flags
					, textureConfigs
					, textureAnims
					, c3d_maps
					, material
					, modelData.getMaterialId()
					, in.passMultipliers
					, components );
				auto incident = writer.declLocale( "incident"
					, reflections->computeIncident( in.worldPosition.xyz(), c3d_sceneData.cameraPosition ) );

				if ( !checkFlag( m_filters, RenderFilter::eOpaque ) )
				{
					if ( components.transmission.isEnabled() )
					{
						IF( writer, lightingModel->getFinalTransmission( components, incident ) >= 0.1_f )
						{
							writer.demote();
						}
						FI;
					}
				}

				IF( writer, material.lighting != 0_u )
				{
					auto worldEye = writer.declLocale( "worldEye"
						, c3d_sceneData.cameraPosition );
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					auto lightScattering = writer.declLocale( "lightScattering"
						, vec3( 0.0_f ) );
					auto lightCoatingSpecular = writer.declLocale( "lightCoatingSpecular"
						, vec3( 0.0_f ) );
					auto lightSheen = writer.declLocale( "lightSheen"
						, vec2( 0.0_f ) );
					shader::OutputComponents output{ lightDiffuse, lightSpecular, lightScattering, lightCoatingSpecular, lightSheen };
					auto surface = writer.declLocale( "surface"
						, shader::Surface{ in.fragCoord.xyz()
							, in.viewPosition.xyz()
							, in.worldPosition.xyz()
							, components.normal } );
					components.finish( passShaders
						, surface
						, utils
						, c3d_sceneData.cameraPosition );
					lightingModel->computeCombined( components
						, c3d_sceneData
						, *backgroundModel
						, surface
						, worldEye
						, modelData.isShadowReceiver()
						, output );

					auto directAmbient = writer.declLocale( "directAmbient"
						, c3d_sceneData.ambientLight );
					auto reflectedDiffuse = writer.declLocale( "reflectedDiffuse"
						, vec3( 0.0_f ) );
					auto reflectedSpecular = writer.declLocale( "reflectedSpecular"
						, vec3( 0.0_f ) );
					auto refracted = writer.declLocale( "refracted"
						, vec3( 0.0_f ) );
					auto coatReflected = writer.declLocale( "coatReflected"
						, vec3( 0.0_f ) );
					auto sheenReflected = writer.declLocale( "sheenReflected"
						, vec3( 0.0_f ) );

					if ( components.hasMember( "thicknessFactor" ) )
					{
						components.thicknessFactor *= length( modelData.getScale() );
					}

					reflections->computeCombined( components
						, incident
						, *backgroundModel
						, modelData.getEnvMapIndex()
						, components.hasReflection
						, components.refractionRatio
						, directAmbient
						, reflectedDiffuse
						, reflectedSpecular
						, refracted
						, coatReflected
						, sheenReflected );

					auto indirectOcclusion = writer.declLocale( "indirectOcclusion"
						, 1.0_f );
					auto lightIndirectDiffuse = indirect.computeDiffuse( flags.getGlobalIlluminationFlags()
						, surface
						, indirectOcclusion );
					auto lightIndirectSpecular = indirect.computeSpecular( flags.getGlobalIlluminationFlags()
						, worldEye
						, c3d_sceneData.getPosToCamera( surface.worldPosition.xyz() )
						, surface
						, components.roughness
						, indirectOcclusion
						, lightIndirectDiffuse.w()
						, c3d_mapBrdf );
					auto indirectAmbient = writer.declLocale( "indirectAmbient"
						, indirect.computeAmbient( flags.getGlobalIlluminationFlags()
							, lightIndirectDiffuse.xyz() ) );
					auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
						, ( hasDiffuseGI
							? cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
								, c3d_sceneData.cameraPosition
								, components.normal
								, components.specular
								, components.metalness
								, surface )
							: vec3( 0.0_f ) ) );

					outColour = vec4( lightingModel->combine( components
							, incident
							, lightDiffuse
							, indirectDiffuse
							, lightSpecular
							, lightScattering
							, lightCoatingSpecular
							, lightSheen
							, lightIndirectSpecular
							, directAmbient
							, indirectAmbient
							, reflectedDiffuse
							, reflectedSpecular
							, refracted
							, coatReflected
							, sheenReflected )
						, components.opacity );
				}
				ELSE
				{
					outColour = vec4( components.colour, components.opacity );
				}
				FI;

				if ( flags.hasFog() )
				{
					outColour = fog.apply( c3d_sceneData.getBackgroundColour( utils )
						, outColour
						, in.worldPosition.xyz()
						, c3d_sceneData );
				}

				backgroundModel->applyVolume( in.fragCoord.xy()
					, utils.lineariseDepth( in.fragCoord.z(), c3d_sceneData.nearPlane, c3d_sceneData.farPlane )
					, c3d_sceneData.renderSize
					, c3d_sceneData.cameraPlanes
					, outColour );
				outVelocity.xy() = in.getVelocity();
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
