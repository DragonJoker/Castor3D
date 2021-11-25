#include "Castor3D/Render/Technique/ForwardRenderTechniquePass.hpp"

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
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	ForwardRenderTechniquePass::ForwardRenderTechniquePass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & category
		, castor::String const & name
		, SceneRenderPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderTechniquePass{ pass
			, context
			, graph
			, device
			, category
			, name
			, renderPassDesc
			, techniquePassDesc }
	{
	}

	void ForwardRenderTechniquePass::accept( RenderTechniqueVisitor & visitor )
	{
		auto flags = visitor.getFlags();
		auto shaderProgram = doGetProgram( flags );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		auto & renderSystem = *getEngine()->getRenderSystem();

		using namespace sdw;
		FragmentWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();
		bool hasDiffuseGI = checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing )
			|| checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
			|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );

		shader::Utils utils{ writer, *renderSystem.getEngine() };
		shader::CookTorranceBRDF cookTorrance{ writer, utils };

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
			, ( m_ssao ? index++ : 0u )
			, RenderPipeline::eAdditional
			, m_ssao != nullptr );
		auto lightingModel = shader::LightingModel::createModel( utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, lightsIndex
			, RenderPipeline::eAdditional
			, shader::ShadowOptions{ flags.sceneFlags, false }
			, index
			, RenderPipeline::eAdditional
			, m_mode != RenderMode::eTransparentOnly );
		auto reflections = lightingModel->getReflectionModel( flags.passFlags
			, index
			, uint32_t( RenderPipeline::eAdditional ) );
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( index
			, RenderPipeline::eAdditional
			, flags.sceneFlags );

		shader::Fog fog{ writer };

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );
		auto pxl_velocity( writer.declOutput< Vec4 >( "pxl_velocity", 1, m_hasVelocity ) );

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
				auto material = writer.declLocale( "material"
					, materials.getMaterial( in.material ) );
				auto opacity = writer.declLocale( "opacity"
					, material.opacity );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive ) );
				auto lightMat = lightingModel->declMaterial( "lightMat" );
				lightMat->create( material );
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );

				if ( m_ssao )
				{
					occlusion *= c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i );
				}

				auto texCoord = writer.declLocale( "texCoord"
					, in.texture );
				lightingModel->computeMapContributions( flags.passFlags
					, filterTexturesFlags( flags.textures )
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
					, in.tangentSpaceViewPosition
					, in.tangentSpaceFragPosition );

				if ( m_mode == RenderMode::eTransparentOnly )
				{
					utils.applyAlphaFunc( flags.blendAlphaFunc
						, opacity
						, material.alphaRef
						, false );
				}
				else
				{
					utils.applyAlphaFunc( flags.alphaFunc
						, opacity
						, material.alphaRef );
				}

				if ( checkFlag( flags.passFlags, PassFlag::eLighting ) )
				{
					auto worldEye = writer.declLocale( "worldEye"
						, c3d_sceneData.cameraPosition );
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					shader::OutputComponents output{ lightDiffuse, lightSpecular };
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( in.fragCoord.xy(), in.viewPosition, in.worldPosition, normal );
					lightingModel->computeCombined( *lightMat
						, c3d_sceneData
						, surface
						, worldEye
						, c3d_modelData.isShadowReceiver()
						, output );

					auto ambient = writer.declLocale( "ambient"
						, lightMat->getAmbient( c3d_sceneData.ambientLight ) );
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
					lightMat->adjustDirectSpecular( lightSpecular );

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
								, c3d_sceneData.cameraPosition
								, surface.worldNormal
								, lightMat->specular
								, lightMat->getMetalness()
								, surface )
							: vec3( 0.0_f ) ) );

					pxl_fragColor = vec4( lightingModel->combine( lightDiffuse
						, indirectDiffuse
						, lightSpecular
						, lightIndirectSpecular
						, ambient
						, indirectAmbient
						, occlusion
						, emissive
						, reflected
						, refracted
						, lightMat->albedo )
						, opacity );
				}
				else
				{
					pxl_fragColor = vec4( lightMat->albedo, opacity );
				}

				if ( getFogType( flags.sceneFlags ) != FogType::eDisabled )
				{
					pxl_fragColor = fog.apply( c3d_sceneData.getBackgroundColour( utils )
						, pxl_fragColor
						, length( in.viewPosition )
						, in.viewPosition.y()
						, c3d_sceneData );
				}

				pxl_velocity.xy() = in.getVelocity();
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
