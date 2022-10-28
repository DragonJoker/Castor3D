#include "Castor3D/Render/Transparent/TransparentPass.hpp"

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
#include "Castor3D/Render/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
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

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Pipeline/PipelineColorBlendStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

namespace castor3d
{
	castor::String const TransparentPass::Type = "c3d.transparent.accumulation";

	TransparentPass::TransparentPass( RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, Texture const & sceneImage
		, crg::ImageData const * targetImage
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, targetImage
			, renderPassDesc
			, techniquePassDesc }
		, m_sceneImage{ sceneImage }
	{
	}

	ComponentModeFlags TransparentPass::getComponentsMask()const
	{
		return ComponentModeFlag::eOpacity
			| ComponentModeFlag::eAlphaBlending
			| ComponentModeFlag::eColour
			| ComponentModeFlag::eDiffuseLighting
			| ComponentModeFlag::eSpecularLighting
			| ComponentModeFlag::eNormals
			| ComponentModeFlag::eGeometry
			| ComponentModeFlag::eOcclusion
			| ComponentModeFlag::eSpecifics;
	}

	void TransparentPass::accept( RenderTechniqueVisitor & visitor )
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

	void TransparentPass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		doAddPassSpecificsBindings( bindings, index );
		bindings.emplace_back( m_scene.getLightCache().createLayoutBinding( index++ ) );

		if ( m_ssao )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) ); // c3d_mapOcclusion
		}

		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBrdf

		doAddShadowBindings( bindings, index );
		doAddEnvBindings( bindings, index );
		doAddBackgroundBindings( bindings, index );
		doAddGIBindings( bindings, index );

		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapScene
	}

	void TransparentPass::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, castor3d::ShadowMapLightTypeArray const & shadowMaps )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		doAddPassSpecificsDescriptor( descriptorWrites, index );
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( index++ ) );

		if ( m_ssao )
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
		doAddShadowDescriptor( descriptorWrites, shadowMaps, index );
		doAddEnvDescriptor( descriptorWrites, index );
		doAddBackgroundDescriptor( descriptorWrites, *m_targetImage, index );
		doAddGIDescriptor( descriptorWrites, index );

		bindTexture( m_sceneImage.wholeView
			, *m_sceneImage.sampler
			, descriptorWrites
			, index );
	}

	ShaderPtr TransparentPass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		bool enableTextures = flags.enableTextures();
		bool hasDiffuseGI = flags.hasDiffuseGI();

		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };
		shader::CookTorranceBRDF cookTorrance{ writer, utils };
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
		auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRg32 >( "c3d_mapBrdf"
			, index++
			, RenderPipeline::eBuffers );
		auto lightingModel = shader::LightingModel::createModel( *getEngine()
			, materials
			, utils
			, getScene().getLightingModel()
			, lightsIndex
			, RenderPipeline::eBuffers
			, shader::ShadowOptions{ flags.getShadowFlags(), true, false }
			, nullptr
			, index
			, RenderPipeline::eBuffers
			, true );
		auto reflections = lightingModel->getReflectionModel( index
			, uint32_t( RenderPipeline::eBuffers ) );
		auto backgroundModel = shader::BackgroundModel::createModel( getScene()
			, writer
			, utils
			, makeExtent2D( m_size )
			, index
			, RenderPipeline::eBuffers );
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( index
			, RenderPipeline::eBuffers
			, flags.getGlobalIlluminationFlags() );
		auto c3d_mapScene = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapScene"
			, index++
			, RenderPipeline::eBuffers );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		// Fragment Outputs
		auto pxl_accumulation( writer.declOutput< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_revealage( writer.declOutput< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );
		auto pxl_velocity( writer.declOutput< Vec2 >( "pxl_velocity", 2 ) );

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
				materials.blendMaterials( false
					, flags
					, textureConfigs
					, textureAnims
					, c3d_maps
					, material
					, modelData.getMaterialId()
					, in.passMultipliers
					, components );

				auto worldEye = writer.declLocale( "worldEye"
					, c3d_sceneData.cameraPosition );
				auto colour = writer.declLocale( "colour"
					, vec3( 0.0_f ) );

				IF( writer, material.lighting != 0_u )
				{
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					auto lightScattering = writer.declLocale( "lightScattering"
						, vec3( 0.0_f ) );
					auto lightCoatingSpecular = writer.declLocale( "lightCoatingSpecular"
						, vec3( 0.0_f ) );
					shader::OutputComponents output{ lightDiffuse, lightSpecular, lightScattering, lightCoatingSpecular };
					auto surface = writer.declLocale( "surface"
						, shader::Surface{ in.fragCoord.xyz()
							, in.viewPosition.xyz()
							, in.worldPosition.xyz()
							, components.normal } );
					lightingModel->computeCombined( components
						, c3d_sceneData
						, *backgroundModel
						, surface
						, worldEye
						, modelData.isShadowReceiver()
						, output );

					auto directAmbient = writer.declLocale( "directAmbient"
						, c3d_sceneData.ambientLight );
					auto incident = writer.declLocale( "incident"
						, reflections->computeIncident( surface.worldPosition.xyz(), c3d_sceneData.cameraPosition ) );
					auto reflected = writer.declLocale( "reflected"
						, vec3( 0.0_f ) );
					auto refracted = writer.declLocale( "refracted"
						, vec3( 0.0_f ) );
					auto coatReflected = writer.declLocale( "coatReflected"
						, vec3( 0.0_f ) );
					auto sceneUv = writer.declLocale( "sceneUv"
						, in.fragCoord.xy() / c3d_sceneData.renderSize );

					if ( components.hasMember( "thicknessFactor" ) )
					{
						components.thicknessFactor *= length( modelData.getScale() );
					}

					reflections->computeCombined( components
						, incident
						, surface.worldPosition.xyz()
						, *backgroundModel
						, c3d_mapScene
						, c3d_matrixData
						, sceneUv
						, modelData.getEnvMapIndex()
						, components.hasReflection
						, components.refractionRatio
						, directAmbient
						, reflected
						, refracted
						, coatReflected );

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
						, indirect.computeAmbient( flags.getGlobalIlluminationFlags(), lightIndirectDiffuse.xyz() ) );
					auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
						, ( hasDiffuseGI
							? cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
								, c3d_sceneData.cameraPosition
								, surface.normal
								, components.specular
								, components.metalness
								, surface )
							: vec3( 0.0_f ) ) );

					colour = lightingModel->combine( components
						, incident
						, lightDiffuse
						, indirectDiffuse
						, lightSpecular
						, lightScattering
						, lightCoatingSpecular
						, lightIndirectSpecular
						, directAmbient
						, indirectAmbient
						, reflected
						, refracted
						, coatReflected );
				}
				ELSE
				{
					colour = components.colour;
				}
				FI;

				pxl_accumulation = c3d_sceneData.computeAccumulation( utils
					, in.fragCoord.z()
					, colour
					, components.opacity
					, components.bwAccumulationOperator );
				pxl_revealage = components.opacity;
				pxl_velocity = in.getVelocity();
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
