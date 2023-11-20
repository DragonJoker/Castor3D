#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
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
		, castor::String const & groupName
		, crg::ImageViewIdArray targetImage
		, crg::ImageViewIdArray targetDepth
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc
		, Texture const * mippedColour
		, bool hasEnvMap )
		: RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, typeName
			, std::move( targetImage )
			, std::move( targetDepth )
			, renderPassDesc
			, techniquePassDesc }
		, m_mippedColour{ mippedColour }
		, m_hasEnvMap{ hasEnvMap }
		, m_groupName{ groupName }
	{
		if ( !checkFlag( m_filters, RenderFilter::eAlphaTest )
			|| !checkFlag( m_filters, RenderFilter::eAlphaBlend ) )
		{
			m_componentsMask |= ComponentModeFlag::eOpacity;

			if ( !checkFlag( m_filters, RenderFilter::eAlphaBlend ) )
			{
				m_componentsMask |= ComponentModeFlag::eAlphaBlending;
			}
		}
	}

	void ForwardRenderTechniquePass::accept( RenderTechniqueVisitor & visitor )
	{
		doAccept( visitor );
	}

	void ForwardRenderTechniquePass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
		doAddPassSpecificsBindings( flags, bindings, index );
		bindings.emplace_back( m_scene.getLightCache().createLayoutBinding( VK_SHADER_STAGE_FRAGMENT_BIT
			, index++ ) );

		if ( hasSsao() )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_ALL_GRAPHICS ) ); // c3d_mapOcclusion
		}

		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBrdf

		doAddShadowBindings( m_scene, flags, bindings, index );
		doAddEnvBindings( flags, bindings, index );
		doAddBackgroundBindings( m_scene, bindings, index );
		doAddGIBindings( flags, bindings, index );

		if ( m_parent )
		{
			doAddClusteredLightingBindings( m_parent->getRenderTarget(), flags, bindings, index );
		}

		if ( m_mippedColour )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapScene
		}

		if ( flags.pass.hasDeferredDiffuseLightingFlag
			&& m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_imgDiffuse
		}
	}

	void ForwardRenderTechniquePass::doFillAdditionalDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
		doAddPassSpecificsDescriptor( flags, descriptorWrites, index );
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( index++ ) );

		if ( hasSsao() )
		{
			bindTexture( m_ssao->wholeView
				, *m_ssao->sampler
				, descriptorWrites
				, index );
		}

		bindTexture( getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().wholeView
			, *getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampler
			, descriptorWrites
			, index );
		doAddShadowDescriptor( m_scene, flags, descriptorWrites, shadowMaps, shadowBuffer, index );
		doAddEnvDescriptor( flags, descriptorWrites, index );
		doAddBackgroundDescriptor( m_scene, descriptorWrites, m_targetImage, index );
		doAddGIDescriptor( flags, descriptorWrites, index );

		if ( m_parent )
		{
			doAddClusteredLightingDescriptor( m_parent->getRenderTarget(), flags, descriptorWrites, index );
		}

		if ( m_mippedColour )
		{
			bindTexture( m_mippedColour->wholeView
				, *m_mippedColour->sampler
				, descriptorWrites
				, index );
		}

		if ( flags.pass.hasDeferredDiffuseLightingFlag
			&& m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly )
		{
			bindImage( getTechnique().getSssDiffuse().targetView
				, descriptorWrites
				, index );
		}
	}

	void ForwardRenderTechniquePass::doGetPixelShaderSource( PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		sdw::FragmentWriter writer{ builder };
		bool enableTextures = flags.enableTextures();
		bool hasDiffuseGI = flags.hasDiffuseGI();

		shader::Utils utils{ writer };
		shader::BRDFHelpers brdf{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };
		shader::CookTorranceBRDF cookTorrance{ writer, brdf };
		auto index = uint32_t( castor3d::GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
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
		shader::SssProfiles sssProfiles{ writer
			, uint32_t( GlobalBuffersIdx::eSssProfiles )
			, RenderPipeline::eBuffers
				, !C3D_DisableSSSTransmittance };
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
			, ( hasSsao() ? index++ : 0u )
			, RenderPipeline::eBuffers
			, hasSsao() );
		auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, RenderPipeline::eBuffers );
		shader::Lights lights{ *getEngine()
			, flags.lightingModelId
			, flags.backgroundModelId
			, materials
			, brdf
			, utils
			, shader::ShadowOptions{ flags.getShadowFlags(), true, false }
			, nullptr
			, lightsIndex /* lightBinding */
			, RenderPipeline::eBuffers /* lightSet */
			, index /* shadowMapBinding */
			, RenderPipeline::eBuffers /* shadowMapSet */
			, checkFlag( m_filters, RenderFilter::eAlphaBlend ) /* enableVolumetric */ };
		shader::ReflectionModel reflections{ writer
			, utils
			, index
			, uint32_t( RenderPipeline::eBuffers )
			, lights.hasIblSupport()
			, m_hasEnvMap };
		auto backgroundModel = shader::BackgroundModel::createModel( getScene()
			, writer
			, utils
			, makeExtent2D( m_size )
			, true
			, index
			, RenderPipeline::eBuffers );
		shader::GlobalIllumination indirect{ writer
			, utils
			, index
			, RenderPipeline::eBuffers
			, flags.getGlobalIlluminationFlags()
			, getIndirectLighting() };
		shader::ClusteredLights clusteredLights{ writer
			, index
			, RenderPipeline::eBuffers
			, getClustersConfig()
			, m_allowClusteredLighting };
		auto c3d_mapScene = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapScene"
			, ( m_mippedColour ? index++ : 0u )
			, RenderPipeline::eBuffers
			, m_mippedColour != nullptr );
		auto c3d_imgDiffuse = writer.declStorageImg< sdw::RImage2DRgba32 >( "c3d_imgDiffuse"
			, ( ( flags.pass.hasDeferredDiffuseLightingFlag && ( m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly ) ) ? index++ : 0u )
			, RenderPipeline::eBuffers
			, flags.pass.hasDeferredDiffuseLightingFlag && ( m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly ) );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		shader::Fog fog{ writer };

		// Fragment Outputs
		uint32_t outIndex{};
		auto outColour( writer.declOutput< sdw::Vec4 >( "outColour", outIndex++ ) );
		auto outVelocity( writer.declOutput< sdw::Vec4 >( "outVelocity", ( flags.writeVelocity() ? outIndex++ : 0u ), flags.writeVelocity() ) );
		auto outScattering( writer.declOutput< sdw::Vec4 >( "outScattering", ( m_outputScattering ? outIndex++ : 0u ), m_outputScattering ) );
		auto outDiffuse( writer.declOutput< sdw::Vec4 >( "outDiffuse", ( m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting ? outIndex++ : 0u ), m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting ) );

		writer.implementMainT< shader::FragmentSurfaceT, sdw::VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
			, sdw::FragmentOut{ writer }
			, [&]( sdw::FragmentInT< shader::FragmentSurfaceT > in
				, sdw::FragmentOut out )
			{
				shader::DebugOutput output{ getDebugConfig()
					, m_groupName
					, c3d_cameraData.debugIndex()
					, outColour
					, true };
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1u] );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				auto components = writer.declLocale( "components"
					, shader::BlendComponents{ materials
						, material
						, in } );
				auto occlusion = writer.declLocale( "occlusion"
					, ( hasSsao()
						? c3d_mapOcclusion.fetch( ivec2( in.fragCoord.xy() ), 0_i )
						: 1.0_f ) );
				materials.blendMaterials( output
					, checkFlag( m_filters, RenderFilter::eAlphaBlend )
					, flags
					, textureConfigs
					, textureAnims
					, c3d_maps
					, material
					, modelData.getMaterialId()
					, in.passMultipliers
					, components );

				if ( components.occlusion )
				{
					occlusion *= components.occlusion;
				}

				output.registerOutput( "Surface", "Normal", fma( components.normal, vec3( 0.5_f ), vec3( 0.5_f ) ) );
				output.registerOutput( "Surface", "Tangent", fma( in.tangent.xyz(), vec3( 0.5_f ), vec3( 0.5_f ) ) );
				output.registerOutput( "Surface", "Bitangent", fma( in.bitangent, vec3( 0.5_f ), vec3( 0.5_f ) ) );
				output.registerOutput( "Surface", "World Position", in.worldPosition );
				output.registerOutput( "Surface", "View Position", in.viewPosition );
				auto incident = writer.declLocale( "incident"
					, reflections.computeIncident( in.worldPosition.xyz(), c3d_cameraData.position() ) );

				if ( !checkFlag( m_filters, RenderFilter::eOpaque ) )
				{
					if ( components.transmission )
					{
						IF( writer, components.transmission >= 0.1_f )
						{
							writer.demote();
						}
						FI;
					}
				}

				if ( auto lightingModel = lights.getLightingModel() )
				{
					IF( writer, material.lighting )
					{
						// Direct Lighting
						auto surface = writer.declLocale( "surface"
							, shader::Surface{ in.fragCoord.xyz()
								, in.viewPosition
								, in.worldPosition
								, normalize( components.normal ) } );
						lightingModel->finish( passShaders
							, in
							, surface
							, utils
							, c3d_cameraData.position()
							, components );
						auto lightSurface = shader::LightSurface::create( writer
							, "lightSurface"
							, c3d_cameraData.position()
							, surface.worldPosition
							, surface.viewPosition.xyz()
							, surface.clipPosition
							, surface.normal );

						if ( flags.pass.hasDeferredDiffuseLightingFlag
							&& m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting )
						{
							auto diffuse = writer.declLocale( "diffuse", vec3( 0.0_f ) );
							lights.computeCombinedDif( clusteredLights
								, components
								, *backgroundModel
								, lightSurface
								, modelData.isShadowReceiver()
								, lightSurface.clipPosition().xy()
								, lightSurface.viewPosition().z()
								, output
								, diffuse );
							outDiffuse = vec4( diffuse, components.transmittance );
							outScattering = vec4( 0.0_f );
							outColour = vec4( 0.0_f );
						}
						else
						{
							shader::OutputComponents lighting{ writer, false };

							if ( flags.pass.hasDeferredDiffuseLightingFlag
								&& m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly )
							{
								lights.computeCombinedAllButDif( clusteredLights
									, components
									, *backgroundModel
									, lightSurface
									, modelData.isShadowReceiver()
									, lightSurface.clipPosition().xy()
									, lightSurface.viewPosition().z()
									, c3d_imgDiffuse.load( ivec2( in.fragCoord.xy() ) ).rgb()
									, output
									, lighting );
							}
							else
							{
								lights.computeCombinedDifSpec( clusteredLights
									, components
									, *backgroundModel
									, lightSurface
									, modelData.isShadowReceiver()
									, lightSurface.clipPosition().xy()
									, lightSurface.viewPosition().z()
									, output
									, lighting );
							}

							auto directAmbient = writer.declLocale( "directAmbient"
								, components.ambientColour * c3d_sceneData.ambientLight() * components.ambientFactor );
							output.registerOutput( "Lighting", "Ambient", directAmbient );
							output.registerOutput( "Lighting", "Occlusion", occlusion );
							output.registerOutput( "Lighting", "Emissive", components.emissiveColour * components.emissiveFactor );

							// Indirect Lighting
							lightSurface.updateL( utils
								, components.normal
								, components.f0
								, components );
							auto indirectOcclusion = indirect.computeOcclusion( flags.getGlobalIlluminationFlags()
								, lightSurface
								, output );
							auto lightIndirectDiffuse = indirect.computeDiffuse( flags.getGlobalIlluminationFlags()
								, lightSurface
								, indirectOcclusion
								, output );
							auto lightIndirectSpecular = indirect.computeSpecular( flags.getGlobalIlluminationFlags()
								, lightSurface
								, components.roughness
								, indirectOcclusion
								, lightIndirectDiffuse.w()
								, c3d_mapBrdf
								, output );
							auto indirectAmbient = indirect.computeAmbient( flags.getGlobalIlluminationFlags()
								, lightIndirectDiffuse.xyz()
								, output );
							auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
								, ( hasDiffuseGI
									? cookTorrance.computeDiffuse( normalize( lightIndirectDiffuse.xyz() )
										, length( lightIndirectDiffuse.xyz() )
										, lightSurface.difF() )
									: vec3( 0.0_f ) ) );
							output.registerOutput( "Indirect", "Diffuse", indirectDiffuse );

							// Reflections/Refraction
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

							lightSurface.updateN( utils
								, components.normal
								, components.f0
								, components );
							passShaders.computeReflRefr( reflections
								, components
								, lightSurface
								, lightSurface.worldPosition()
								, *backgroundModel
								, c3d_mapScene
								, c3d_cameraData
								, lighting
								, indirectAmbient
								, indirectDiffuse
								, in.fragCoord.xy()
								, modelData.getEnvMapIndex()
								, incident
								, components.hasReflection
								, components.hasRefraction
								, components.refractionRatio
								, reflectedDiffuse
								, reflectedSpecular
								, refracted
								, coatReflected
								, sheenReflected
								, output );

							output.registerOutput( "Reflections", "Incident", sdw::fma( incident, vec3( 0.5_f ), vec3( 0.5_f ) ) );

							// Combine
							outColour = vec4( lightingModel->combine( output
									, components
									, incident
									, lighting.diffuse
									, indirectDiffuse
									, lighting.specular
									, lighting.scattering
									, lighting.coatingSpecular
									, lighting.sheen
									, lightIndirectSpecular
									, directAmbient
									, indirectAmbient
									, occlusion
									, components.emissiveColour * components.emissiveFactor
									, reflectedDiffuse
									, reflectedSpecular
									, refracted
									, coatReflected
									, sheenReflected )
								, components.opacity );
							outScattering = vec4( lighting.scattering, 1.0_f );

							if ( m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting )
							{
								outDiffuse = vec4( 0.0_f );
							}
						}
					}
					ELSE
					{
						outColour = vec4( components.colour, components.opacity );
						outScattering = vec4( 0.0_f );

						if ( m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting )
						{
							outDiffuse = vec4( 0.0_f );
						}
					}
					FI;
				}
				else
				{
					outColour = vec4( components.colour, components.opacity );
					outScattering = vec4( 0.0_f );

					if ( m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting )
					{
						outDiffuse = vec4( 0.0_f );
					}
				}

				if ( !flags.pass.hasDeferredDiffuseLightingFlag
					|| m_deferredLightingFilter != DeferredLightingFilter::eDeferLighting )
				{
					if ( flags.hasFog() )
					{
						outColour = fog.apply( c3d_sceneData.getBackgroundColour( utils, c3d_cameraData.gamma() )
							, outColour
							, in.worldPosition.xyz()
							, c3d_cameraData.position()
							, c3d_sceneData );

						if ( m_outputScattering )
						{
							outScattering = fog.apply( c3d_sceneData.getBackgroundColour( utils, c3d_cameraData.gamma() )
								, outScattering
								, in.worldPosition.xyz()
								, c3d_cameraData.position()
								, c3d_sceneData );
						}
					}

					auto linearDepth = writer.declLocale( "linearDepth"
						, utils.lineariseDepth( in.fragCoord.z(), c3d_cameraData.nearPlane(), c3d_cameraData.farPlane() ) );
					backgroundModel->applyVolume( in.fragCoord.xy()
						, linearDepth
						, vec2( c3d_cameraData.renderSize() )
						, c3d_cameraData.depthPlanes()
						, outColour );

					if ( m_outputScattering )
					{
						backgroundModel->applyVolume( in.fragCoord.xy()
							, linearDepth
							, vec2( c3d_cameraData.renderSize() )
							, c3d_cameraData.depthPlanes()
							, outScattering );
					}

					outVelocity.xy() = in.getVelocity();
				}
			} );
	}
}
