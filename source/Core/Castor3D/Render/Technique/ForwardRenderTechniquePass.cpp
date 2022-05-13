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
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
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
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

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
		, castor::String const & category
		, castor::String const & name
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderTechniquePass{ parent
			, pass
			, context
			, graph
			, device
			, typeName
			, category
			, name
			, renderPassDesc
			, techniquePassDesc }
	{
	}

	void ForwardRenderTechniquePass::accept( RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_typeID )
		{
			auto flags = visitor.getFlags();
			auto shaderProgram = doGetProgram( flags );
			visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
			visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		auto & renderSystem = *getEngine()->getRenderSystem();

		using namespace sdw;
		FragmentWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures();
		bool hasDiffuseGI = checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing )
			|| checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
			|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );

		shader::Utils utils{ writer, *renderSystem.getEngine() };
		shader::CookTorranceBRDF cookTorrance{ writer, utils };

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
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
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		auto lightsIndex = index++;
		auto c3d_mapOcclusion = writer.declCombinedImg< FImg2DR32 >( "c3d_mapOcclusion"
			, ( m_ssao ? index++ : 0u )
			, RenderPipeline::eBuffers
			, m_ssao != nullptr );
		auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRg32 >( "c3d_mapBrdf"
			, index++
			, RenderPipeline::eBuffers );
		auto lightingModel = shader::LightingModel::createModel( utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, lightsIndex
			, RenderPipeline::eBuffers
			, shader::ShadowOptions{ flags.sceneFlags, true, false }
			, nullptr
			, index
			, RenderPipeline::eBuffers
			, m_mode != RenderMode::eTransparentOnly );
		auto reflections = lightingModel->getReflectionModel( index
			, uint32_t( RenderPipeline::eBuffers ) );
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( index
			, RenderPipeline::eBuffers
			, flags.sceneFlags );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		shader::Fog fog{ writer };

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );
		auto pxl_velocity( writer.declOutput< Vec4 >( "pxl_velocity", 1, m_hasVelocity ) );

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
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				auto opacity = writer.declLocale( "opacity"
					, material.opacity );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive ) );
				auto lightMat = lightingModel->declMaterial( "lightMat" );
				lightMat->create( material );
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent )
					, checkFlag( flags.submeshFlags, SubmeshFlag::eTangents ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent )
					, checkFlag( flags.submeshFlags, SubmeshFlag::eTangents ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );

				if ( m_ssao )
				{
					occlusion *= c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i );
				}

				auto texCoord0 = writer.declLocale( "texCoord0"
					, in.texture0
					, hasTextures );
				auto texCoord1 = writer.declLocale( "texCoord1"
					, in.texture1
					, hasTextures && checkFlag( flags.submeshFlags, SubmeshFlag::eTexcoords1 ) );
				auto texCoord2 = writer.declLocale( "texCoord2"
					, in.texture2
					, hasTextures && checkFlag( flags.submeshFlags, SubmeshFlag::eTexcoords2 ) );
				auto texCoord3 = writer.declLocale( "texCoord3"
					, in.texture3
					, hasTextures && checkFlag( flags.submeshFlags, SubmeshFlag::eTexcoords3 ) );
				lightingModel->computeMapContributions( flags.passFlags
					, flags.textures
					, textureConfigs
					, textureAnims
					, c3d_maps
					, modelData.getTextures0()
					, modelData.getTextures1()
					, texCoord0
					, texCoord1
					, texCoord2
					, texCoord3
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
					surface.create( in.fragCoord.xy(), in.viewPosition.xyz(), in.worldPosition.xyz(), normal);
					lightingModel->computeCombined( *lightMat
						, c3d_sceneData
						, surface
						, worldEye
						, modelData.isShadowReceiver()
						, output );

					auto ambient = writer.declLocale( "ambient"
						, lightMat->getAmbient( c3d_sceneData.ambientLight ) );
					auto reflected = writer.declLocale( "reflected"
						, vec3( 0.0_f ) );
					auto refracted = writer.declLocale( "refracted"
						, vec3( 0.0_f ) );
					reflections->computeDeferred( *lightMat
						, surface
						, c3d_sceneData
						, modelData.getEnvMapIndex()
						, material.hasReflection
						, material.hasRefraction
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
						, lightMat->getRoughness()
						, indirectOcclusion
						, lightIndirectDiffuse.w()
						, c3d_mapBrdf );
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
						, in.worldPosition.xyz()
						, c3d_sceneData );
				}

				pxl_velocity.xy() = in.getVelocity();
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
