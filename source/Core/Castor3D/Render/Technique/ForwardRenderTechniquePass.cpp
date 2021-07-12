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
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Image/ImageView.hpp>
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
			, bool hasVelocity
			, bool isTransparentOnly )
		{
			using MyTraits = shader::ShaderMaterialTraitsT< MaterialT >;
			using LightingModel = typename MyTraits::LightingModel;
			using LightMaterial = typename MyTraits::LightMaterial;
			using ReflectionModel = typename MyTraits::ReflectionModel;

			using namespace sdw;
			FragmentWriter writer;
			bool hasTextures = !flags.textures.empty();
			bool hasDiffuseGI = checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing )
				|| checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
				|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );

			shader::Utils utils{ writer };
			utils.declareApplyGamma();
			utils.declareRemoveGamma();
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
			ReflectionModel reflections{ writer
				, utils
				, flags.passFlags
				, index
				, uint32_t( RenderPipeline::eAdditional ) };
			auto lighting = LightingModel::createModel( writer
				, utils
				, lightsIndex
				, RenderPipeline::eAdditional
				, shader::ShadowOptions{ flags.sceneFlags, false }
				, index
				, RenderPipeline::eAdditional
				, !isTransparentOnly );
			shader::GlobalIllumination indirect{ writer, utils };
			indirect.declare( index
				, RenderPipeline::eAdditional
				, flags.sceneFlags );

			auto in = writer.getIn();
			shader::Fog fog{ getFogType( flags.sceneFlags ), writer };

			// Fragment Outputs
			auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );
			auto pxl_velocity( writer.declOutput< Vec4 >( "pxl_velocity", 1, hasVelocity ) );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto material = writer.declLocale( "material"
						, materials.getMaterial( inSurface.material ) );
					auto opacity = writer.declLocale( "opacity"
						, material.opacity );
					auto gamma = writer.declLocale( "gamma"
						, material.gamma );
					auto emissive = writer.declLocale( "emissive"
						, vec3( material.emissive ) );
					auto lightMat = writer.declLocale< LightMaterial >( "lightMat" );
					lightMat.create( material );
					auto normal = writer.declLocale( "normal"
						, normalize( inSurface.normal ) );
					auto tangent = writer.declLocale( "tangent"
						, normalize( inSurface.tangent ) );
					auto bitangent = writer.declLocale( "bitangent"
						, normalize( inSurface.bitangent ) );
					auto occlusion = writer.declLocale( "occlusion"
						, 1.0_f );
					auto transmittance = writer.declLocale( "transmittance"
						, 0.0_f );

					if ( hasSSAO )
					{
						occlusion *= c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i );
					}

					auto texCoord = writer.declLocale( "texCoord"
						, inSurface.texture );
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

					if ( isTransparentOnly )
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
							, c3d_sceneData.getCameraPosition() );
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

						auto ambient = writer.declLocale( "ambient"
							, lightMat.getAmbient( c3d_sceneData.getAmbientLight() ) );
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
							, reflected
							, refracted );
						lightMat.adjustDirectSpecular( lightSpecular );

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
							, lightMat.getRoughness()
							, indirectOcclusion
							, lightIndirectDiffuse.w() );
						auto indirectAmbient = writer.declLocale( "indirectAmbient"
							, lightMat.getIndirectAmbient( indirect.computeAmbient( flags.sceneFlags, lightIndirectDiffuse.xyz() ) ) );
						auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
							, ( hasDiffuseGI
								? cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
									, c3d_sceneData.getCameraPosition()
									, surface.worldNormal
									, lightMat.specular
									, lightMat.getMetalness()
									, surface )
								: vec3( 0.0_f ) ) );

						pxl_fragColor = vec4( LightingModel::combine( lightDiffuse
							, indirectDiffuse
							, lightSpecular
							, lightIndirectSpecular
							, ambient
							, indirectAmbient
							, occlusion
							, emissive
							, reflected
							, refracted
							, lightMat.albedo )
							, opacity );
					}
					else
					{
						pxl_fragColor = vec4( lightMat.albedo, opacity );
					}

					if ( getFogType( flags.sceneFlags ) != FogType::eDisabled )
					{
						pxl_fragColor = fog.apply( c3d_sceneData.getBackgroundColour( utils, gamma )
							, pxl_fragColor
							, length( inSurface.viewPosition )
							, inSurface.viewPosition.y()
							, c3d_sceneData );
					}

					pxl_velocity.xy() = inSurface.getVelocity();
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

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

	ShaderPtr ForwardRenderTechniquePass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		return getPixelShaderSourceT< MaterialType::ePhong >( *getEngine()->getRenderSystem()
			, flags
			, filterTexturesFlags( flags.textures )
			, getTexturesMask()
			, getShaderFlags()
			, m_ssao
			, m_hasVelocity
			, m_mode == RenderMode::eTransparentOnly );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPbrPixelShaderSource( PipelineFlags const & flags )const
	{
		return getPixelShaderSourceT< MaterialType::eMetallicRoughness >( *getEngine()->getRenderSystem()
			, flags
			, filterTexturesFlags( flags.textures )
			, getTexturesMask()
			, getShaderFlags()
			, m_ssao
			, m_hasVelocity
			, m_mode == RenderMode::eTransparentOnly );
	}
}
