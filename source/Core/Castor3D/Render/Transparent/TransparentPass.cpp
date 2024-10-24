#include "Castor3D/Render/Transparent/TransparentPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"
#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Pipeline/PipelineColorBlendStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

namespace castor3d
{
	castor::String const TransparentPass::Type = cuT( "c3d.transparent.accumulation" );

	TransparentPass::TransparentPass( RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, Texture const & sceneImage
		, Texture const & depthObjImage
		, Texture const & normalsImage
		, crg::ImageViewIdArray targetImage
		, crg::ImageViewIdArray targetDepth
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, castor::move( targetImage )
			, castor::move( targetDepth )
			, renderPassDesc
			, techniquePassDesc }
		, m_sceneImage{ sceneImage }
		, m_depthObjImage{ depthObjImage }
		, m_normalsImage{ normalsImage }
	{
	}

	void TransparentPass::accept( RenderTechniqueVisitor & visitor )
	{
		doAccept( visitor );
	}

	ashes::PipelineDepthStencilStateCreateInfo TransparentPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_GREATER };
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
		};
		return ashes::PipelineColorBlendStateCreateInfo
		{
			0u,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			castor::move( attachments ),
		};
	}

	void TransparentPass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
		doAddPassSpecificsBindings( bindings, index );
		bindings.emplace_back( m_scene.getLightCache().createLayoutBinding( VK_SHADER_STAGE_FRAGMENT_BIT
			, index ) );
		++index;

		if ( hasSsao() )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) ); // c3d_mapOcclusion
			++index;
		}

		bindings.emplace_back( makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBrdf
		++index;

		doAddShadowBindings( m_scene, bindings, index );
		doAddEnvBindings( bindings, index );
		doAddBackgroundBindings( m_scene, bindings, index );
		doAddGIBindings( bindings, index );
		doAddClusteredLightingBindings( m_parent->getRenderTarget(), bindings, index );

		bindings.emplace_back( makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapScene
		++index;
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapDepthObj
		++index;
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapNormals
		++index;
	}

	void TransparentPass::doFillAdditionalDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, castor3d::ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
		doAddPassSpecificsDescriptor( descriptorWrites, index );
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( index ) );
		++index;

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
		doAddShadowDescriptor( m_scene, descriptorWrites, shadowMaps, shadowBuffer, index );
		doAddEnvDescriptor( descriptorWrites, index );
		doAddBackgroundDescriptor( m_scene, descriptorWrites, m_targetImage, index );
		doAddGIDescriptor( descriptorWrites, index );
		doAddClusteredLightingDescriptor( m_parent->getRenderTarget(), descriptorWrites, index );

		bindTexture( m_sceneImage.wholeView
			, *m_sceneImage.sampler
			, descriptorWrites
			, index );
		bindTexture( m_depthObjImage.wholeView
			, *m_depthObjImage.sampler
			, descriptorWrites
			, index );
		bindTexture( m_normalsImage.wholeView
			, *m_normalsImage.sampler
			, descriptorWrites
			, index );
	}

	void TransparentPass::doGetPixelShaderSource( PipelineFlags const & flags
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
		shader::SubmeshShaders submeshShaders{ getEngine()->getSubmeshComponentsRegister()
			, flags };
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
			, brdf
			, utils
			, shader::ShadowOptions{ flags.getShadowFlags(), true, false }
			, &sssProfiles
			, lightsIndex /* lightBinding */
			, RenderPipeline::eBuffers /* lightSet */
			, index /* shadowMapBinding */
			, RenderPipeline::eBuffers /* shadowMapSet */
			, true /* enableVolumetric */ };
		shader::ReflectionModel reflections{ writer
			, utils
			, index
			, RenderPipeline::eBuffers
			, lights.hasBackgroundReflectionsSupport()
			, lights.hasBackgroundRefractionSupport()
			, lights.hasIblSupport() };
		auto backgroundModel = shader::BackgroundModel::createModel( getScene()
			, writer
			, utils
			, makeExtent2D( m_size )
			, false
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
			, index
			, RenderPipeline::eBuffers );
		++index;
		auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj"
			, index
			, RenderPipeline::eBuffers );
		++index;
		auto c3d_mapNormals = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNormals"
			, index
			, RenderPipeline::eBuffers );
		++index;

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		// Fragment Outputs
		auto outAccumulation( writer.declOutput< sdw::Vec4 >( "outAccumulation", 0 ) );
		auto outRevealage( writer.declOutput< sdw::Float >( "outRevealage", 1 ) );

		writer.implementMainT< shader::FragmentSurfaceT, sdw::VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, submeshShaders
				, passShaders
				, flags }
			, sdw::FragmentOut{ writer }
			, [&]( sdw::FragmentInT< shader::FragmentSurfaceT > const & in
				, sdw::FragmentOut const & )
			{
				shader::DebugOutput output{ getDebugConfig()
					, cuT( "Default" )
					, c3d_cameraData.debugIndex()
					, outAccumulation
					, areDebugTargetsEnabled() };
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
					, false
					, flags
					, textureConfigs
					, textureAnims
					, c3d_maps
					, material
					, modelData.getMaterialId()
					, in.passMultipliers
					, components );
				output.registerOutput( cuT( "Surface" ), cuT( "Normal" ), fma( components.getRawNormal(), vec3( 0.5_f ), vec3( 0.5_f ) ) );
				output.registerOutput( cuT( "Surface" ), cuT( "Tangent" ), fma( in.tangent.xyz(), vec3( 0.5_f ), vec3( 0.5_f ) ) );
				output.registerOutput( cuT( "Surface" ), cuT( "Bitangent" ), fma( in.bitangent, vec3( 0.5_f ), vec3( 0.5_f ) ) );
				output.registerOutput( cuT( "Surface" ), cuT( "World Position" ), in.worldPosition );
				output.registerOutput( cuT( "Surface" ), cuT( "View Position" ), in.viewPosition );

				if ( components.occlusion )
				{
					occlusion *= components.occlusion;
				}

				auto colour = writer.declLocale( "colour"
					, vec3( 0.0_f ) );

				if ( auto lightingModel = lights.getLightingModel() )
				{
					IF( writer, material.lighting )
					{
						auto directLighting = writer.declLocale( "directLighting"
							, shader::DirectLighting{ writer } );
						// Direct Lighting
						auto surface = writer.declLocale( "surface"
							, shader::DerivSurface{ in.fragCoord.xyz()
								, { in.viewPosition, dFdx( in.viewPosition ), dFdy( in.viewPosition ) }
								, { in.worldPosition, dFdx( in.worldPosition ), dFdy( in.worldPosition ) }
								, normalize( components.getDerivNormal() ) } );
						lightingModel->finish( passShaders
							, surface
							, utils
							, c3d_cameraData.position()
							, components );
						auto lightSurface = shader::LightSurface::create( writer
							, "lightSurface"
							, c3d_cameraData.position()
							, surface.worldPosition
							, getXYZ( surface.viewPosition )
							, surface.clipPosition
							, surface.normal );
						lights.computeCombinedDifSpec( clusteredLights
							, components
							, *backgroundModel
							, lightSurface
							, modelData.isShadowReceiver()
							, lightSurface.clipPosition().xy()
							, lightSurface.viewPosition().value().z()
							, output
							, directLighting );
						directLighting.ambient() = components.ambientColour * c3d_sceneData.ambientLight() * components.ambientFactor;
						output.registerOutput( cuT( "Lighting" ), cuT( "Ambient" ), directLighting.ambient() );
						output.registerOutput( cuT( "Lighting" ), cuT( "Occlusion" ), occlusion );
						output.registerOutput( cuT( "Lighting" ), cuT( "Emissive" ), components.emissiveColour * components.emissiveFactor );

						// Indirect Lighting
						lightSurface.updateL( utils
							, components.getDerivNormal()
							, components.f0
							, components );
						auto indirectLighting = writer.declLocale( "indirectLighting"
							, shader::IndirectLighting{ writer } );
						indirect.computeCombinedDifSpec( flags.getGlobalIlluminationFlags()
							, hasDiffuseGI
							, cookTorrance
							, lightSurface
							, components.roughness
							, c3d_mapBrdf
							, indirectLighting
							, output );

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

						auto incident = writer.declLocale( "incident"
							, reflections.computeIncident( lightSurface.worldPosition().value().xyz(), c3d_cameraData.position() ) );
						lightSurface.updateN( utils
							, components.getDerivNormal()
							, components.f0
							, components );
						passShaders.computeReflRefr( reflections
							, components
							, lightSurface
							, lightSurface.worldPosition().value()
							, *backgroundModel
							, c3d_mapScene
							, c3d_cameraData
							, directLighting
							, indirectLighting
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

						if ( components.emissiveFactor )
						{
							components.emissiveFactor *= components.opacity;
						}

						output.registerOutput( cuT( "Reflections" ), cuT( "Incident" ), sdw::fma( incident, vec3( 0.5_f ), vec3( 0.5_f ) ) );

						// Combine
						colour = lightingModel->combine( output
							, components
							, incident
							, directLighting
							, indirectLighting
							, occlusion
							, components.emissiveColour * components.emissiveFactor
							, reflectedDiffuse
							, reflectedSpecular
							, refracted
							, coatReflected
							, sheenReflected );
					}
					ELSE
					{
						colour = components.colour;
					}
					FI
				}
				else
				{
					colour = components.colour;
				}

				outAccumulation = c3d_sceneData.computeAccumulation( utils
					, c3d_cameraData
					, in.fragCoord.z()
					, colour
					, components.opacity
					, components.bwAccumulationOperator );
				outRevealage = components.opacity;
			} );
	}
}
