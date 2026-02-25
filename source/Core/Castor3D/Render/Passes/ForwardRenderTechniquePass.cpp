#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	String const ForwardRenderTechniquePass::Type = cuT( "c3d.forward" );

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, String const & typeName
		, String const & groupName
		, Texture & targetImage
		, Texture & targetDepth
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
			, &targetImage
			, &targetDepth
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
		doAddPassSpecificsLayoutBindings( bindings, index );
		m_scene.getLightCache().addLayoutBinding( bindings, index, VK_SHADER_STAGE_FRAGMENT_BIT );

		if ( hasSsao() )
			addDescriptorSetLayoutBinding( bindings, index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_ALL_GRAPHICS ); // c3d_mapOcclusion

		addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT );	// c3d_mapBrdf

		doAddShadowLayoutBindings( m_scene, bindings, index );
		doAddEnvLayoutBindings( bindings, index );
		doAddBackgroundLayoutBindings( m_scene, bindings, index );
		doAddGILayoutBindings( bindings, index );

		if ( m_parent )
			doAddClusteredLightingLayoutBindings( m_parent->getRenderTarget(), bindings, index );

		if ( m_mippedColour )
			m_mippedColour->addTextureLayoutBinding( bindings, index, VK_SHADER_STAGE_FRAGMENT_BIT ); // c3d_mapScene

		if ( m_parent && flags.pass.hasDeferredDiffuseLightingFlag
			&& m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly )
			addDescriptorSetLayoutBinding( bindings, index
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, VK_SHADER_STAGE_FRAGMENT_BIT );	// c3d_imgDiffuse
	}

	void ForwardRenderTechniquePass::doFillAdditionalDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
		doAddPassSpecificsDescriptorWrites( descriptorWrites, index );
		m_scene.getLightCache().addDescriptorWrite( descriptorWrites, index );

		if ( hasSsao() )
			m_ssao->addTextureDescriptorWrite( descriptorWrites, index );

		getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().addTextureDescriptorWrite( descriptorWrites, index );
		doAddShadowDescriptorWrites( m_scene, descriptorWrites, shadowMaps, shadowBuffer, index );
		doAddEnvDescriptorWrites( descriptorWrites, index );
		doAddBackgroundDescriptorWrites( m_scene, descriptorWrites, m_targetImage, index );
		doAddGIDescriptorWrites( descriptorWrites, index );

		if ( m_parent )
			doAddClusteredLightingDescriptorWrites( m_parent->getRenderTarget(), descriptorWrites, index );

		if ( m_mippedColour )
			m_mippedColour->addTextureDescriptorWrite( descriptorWrites, index );

		if ( m_parent && flags.pass.hasDeferredDiffuseLightingFlag
			&& m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly )
			m_parent->getSssDiffuse().addTextureDescriptorWrite( descriptorWrites, index );
	}

	void ForwardRenderTechniquePass::doGetPixelShaderSource( PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		sdw::FragmentWriter writer{ builder };
		bool enableTextures = flags.enableTextures();
		bool hasDiffuseGI = flags.hasDiffuseGI();

		shader::Utils utils{ writer };
		shader::BRDFHelpers brdfHelpers{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };
		shader::SubmeshShaders submeshShaders{ getEngine()->getSubmeshComponentsRegister()
			, flags };
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_Render( writer
			, GlobalBuffersIdx::eRender
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
		auto lightsIndex = index;
		++index;
		auto c3d_mapOcclusion = writer.declCombinedImg< FImg2DR32 >( "c3d_mapOcclusion"
			, ( hasSsao() ? index++ : 0u )
			, RenderPipeline::eBuffers
			, hasSsao() );
		auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBrdf"
			, index
			, RenderPipeline::eBuffers );
		++index;
		shader::Lights lights{ *getEngine()
			, flags.lightingModelId
			, flags.backgroundModelId
			, materials
			, brdfHelpers
			, utils
			, shader::ShadowOptions{ flags.getShadowFlags() }
			, nullptr/* sssProfiles */
			, nullptr /* sssDiffusionProfiles */
			, lightsIndex /* lightBinding */
			, RenderPipeline::eBuffers /* lightSet */
			, index /* shadowMapBinding */
			, RenderPipeline::eBuffers /* shadowMapSet */
			, checkFlag( m_filters, RenderFilter::eAlphaBlend ) /* enableVolumetric */ };
		shader::ReflectionModel reflections{ writer
			, utils
			, index
			, uint32_t( RenderPipeline::eBuffers )
			, lights.hasBackgroundReflectionsSupport()
			, lights.hasBackgroundRefractionSupport()
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
		auto outColour( writer.declOutput< sdw::Vec4 >( "outColour", outIndex ) );
		++outIndex;
		auto outVelocity( writer.declOutput< sdw::Vec4 >( "outVelocity", ( flags.writeVelocity() ? outIndex++ : 0u ), flags.writeVelocity() ) );
		auto outScattering( writer.declOutput< sdw::Vec4 >( "outScattering", ( m_outputScattering ? outIndex++ : 0u ), m_outputScattering ) );
		auto outDiffuse( writer.declOutput< sdw::Vec4 >( "outDiffuse", ( m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting ? outIndex++ : 0u ), m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting ) );

		writer.implementMainT< shader::FragmentSurfaceT, sdw::VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, submeshShaders
				, passShaders
				, flags }
			, sdw::FragmentOut{ writer }
			, [&]( sdw::FragmentInT< shader::FragmentSurfaceT > const & in
				, sdw::FragmentOut const & out )
			{
				shader::DebugOutput output{ getDebugConfig()
					, cuT( "Default" )
					, c3d_renderData.debugIndex()
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

				in.registerDebug( flags, components, output.pushBlock( cuT( "Surface" ) ) );
				auto incident = writer.declLocale( "incident"
					, shader::ReflectionModel::computeIncident( in.worldPosition.xyz(), c3d_cameraData.position() ) );

				if ( !checkFlag( m_filters, RenderFilter::eOpaque ) )
				{
					if ( components.transmissionFactor )
					{
						sdwIF( writer, components.transmissionFactor >= 0.05_f )
						{
							writer.demote();
						}
						sdwFI
					}
				}

				if ( auto lightingModel = lights.getLightingModel() )
				{
					sdwIF( writer, material.lighting )
					{
						auto surface = writer.declLocale( "surface"
							, shader::DerivSurface{ in.fragCoord.xyz()
								, { in.viewPosition, dFdx( in.viewPosition ), dFdy( in.viewPosition ) }
								, { in.worldPosition, dFdx( in.worldPosition ), dFdy( in.worldPosition ) }
								, normalize( components.getDerivNormal() ) } );
						components.finish( passShaders
							, surface
							, c3d_cameraData
							, modelData
							, utils );

						// Direct Lighting
						auto lightSurface = shader::LightSurface::create( writer
							, "lightSurface"
							, c3d_cameraData.position()
							, surface.worldPosition
							, getXYZ( surface.viewPosition )
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
								, lightSurface.viewPosition().value().z()
								, output.pushBlock( cuT( "Lighting" ) )
								, diffuse );
							outDiffuse = vec4( diffuse, components.transmittance );
							outScattering = vec4( 0.0_f );
							outColour = vec4( 0.0_f );
						}
						else
						{
							auto directLighting = writer.declLocale( "directLighting"
								, shader::DirectLighting{ writer } );

							if ( flags.pass.hasDeferredDiffuseLightingFlag
								&& m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly )
							{
								auto directDiffuse = writer.declLocale( "directDiffuse"
									, c3d_imgDiffuse.load( ivec2( in.fragCoord.xy() ) ).rgb() );
								lights.computeCombinedAllButDif( clusteredLights
									, components
									, *backgroundModel
									, lightSurface
									, modelData.isShadowReceiver()
									, lightSurface.clipPosition().xy()
									, lightSurface.viewPosition().value().z()
									, output.pushBlock( cuT( "Lighting" ) )
									, directDiffuse
									, directLighting );
							}
							else
							{
								lights.computeCombinedDifSpec( clusteredLights
									, components
									, *backgroundModel
									, lightSurface
									, modelData.isShadowReceiver()
									, lightSurface.clipPosition().xy()
									, lightSurface.viewPosition().value().z()
									, output.pushBlock( cuT( "Lighting" ) )
									, directLighting );
							}

							// Indirect Lighting
							lightSurface.updateL( components.getDerivNormal() );
							auto indirectLighting = writer.declLocale( "indirectLighting"
								, shader::IndirectLighting{ writer } );
							indirect.computeCombinedDifSpec( flags.getGlobalIlluminationFlags()
								, hasDiffuseGI
								, c3d_sceneData
								, components
								, lightSurface
								, components.perceptualRoughness
								, c3d_mapBrdf
								, indirectLighting );

							// Reflections/Refraction
							auto reflRefrResult = writer.declLocale( "reflRefrResult"
								, shader::ReflectionRefraction{ writer } );

							if ( components.hasMember( "thicknessFactor" ) )
							{
								components.thicknessFactor *= length( modelData.getScale() );
							}

							lightSurface.updateN( components.getDerivNormal() );
							passShaders.backgroundBrdfWithTransmission( reflections
								, components
								, lightSurface
								, *backgroundModel
								, c3d_mapScene
								, c3d_cameraData
								, c3d_renderData
								, directLighting
								, indirectLighting
								, in.fragCoord.xy()
								, modelData.getEnvMapIndex()
								, incident
								, reflRefrResult
								, output );

							// Combine
							outColour = vec4( lightingModel->combine( output
									, reflections
									, c3d_mapBrdf
									, components
									, lightSurface
									, incident
									, occlusion
									, directLighting
									, indirectLighting
									, reflRefrResult )
								, components.opacity );
							outScattering = vec4( directLighting.scattering, 1.0_f);

							if ( m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting )
							{
								outDiffuse = vec4( 0.0_f );
							}
						}
					}
					sdwELSE
					{
						outColour = vec4( components.baseColour + components.emissiveColour * components.emissiveFactor, components.opacity );
						outScattering = vec4( 0.0_f );

						if ( m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting )
						{
							outDiffuse = vec4( 0.0_f );
						}
					}
					sdwFI
				}
				else
				{
					outColour = vec4( components.baseColour + components.emissiveColour * components.emissiveFactor, components.opacity );
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
						outColour = fog.apply( c3d_sceneData.getBackgroundColour( utils, c3d_renderData.gamma() )
							, outColour
							, in.worldPosition.xyz()
							, c3d_cameraData.position()
							, c3d_sceneData );

						if ( m_outputScattering )
						{
							outScattering = fog.apply( c3d_sceneData.getBackgroundColour( utils, c3d_renderData.gamma() )
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
						, vec2( c3d_renderData.renderSize() )
						, c3d_cameraData.depthPlanes()
						, outColour );

					if ( m_outputScattering )
					{
						backgroundModel->applyVolume( in.fragCoord.xy()
							, linearDepth
							, vec2( c3d_renderData.renderSize() )
							, c3d_cameraData.depthPlanes()
							, outScattering );
					}

					outVelocity.xy() = in.getMotionVector( vec2( c3d_renderData.renderSize() ) );
				}
			} );
	}
}
