#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		std::vector< ShadowMap::PassData > createPasses( Engine & engine
			, Scene & scene
			, ShadowMap & shadowMap
			, uint32_t cascadeCount )
		{
			std::vector< ShadowMap::PassData > result;
			auto const width = ShadowMapPassDirectional::TextureSize;
			auto const height = ShadowMapPassDirectional::TextureSize;
			auto const w = float( width );
			auto const h = float( height );
			Viewport viewport{ engine };
			viewport.setOrtho( -w / 2, w / 2, -h / 2, h / 2, -5120.0, 5120.0 );
			viewport.resize( { width, height } );
			viewport.update();

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( engine ),
					std::make_shared< Camera >( cuT( "ShadowMapDirectional_" ) + string::toString( cascade + 1, std::locale{ "C" } )
						, scene
						, *scene.getCameraRootNode()
						, std::move( viewport )
						, true ),
					nullptr,
					nullptr,
				};
				passData.culler = std::make_unique< FrustumCuller >( scene, *passData.camera );
				passData.pass = std::make_shared< ShadowMapPassDirectional >( engine
					, *passData.matrixUbo
					, *passData.culler
					, shadowMap
					, cascade );
				result.emplace_back( std::move( passData ) );
			}

			return result;
		}
	}

	VkFormat ShadowMapDirectional::RawDepthFormat = VK_FORMAT_UNDEFINED;

	ShadowMapDirectional::ShadowMapDirectional( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, cuT( "ShadowMapDirectional" )
			, ShadowMapPassResult
			{
				engine,
				cuT( "Directional" ),
				0u,
				Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize },
				scene.getDirectionalShadowCascades(),
			}
			, createPasses( engine, scene, *this, scene.getDirectionalShadowCascades() )
			, 1u }
		, m_frameBuffers( m_passes.size() )
		, m_cascades{ scene.getDirectionalShadowCascades() }
	{
		log::trace << "Created ShadowMapDirectional" << std::endl;
	}

	ShadowMapDirectional::~ShadowMapDirectional()
	{
	}

	void ShadowMapDirectional::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_shadowType = light.getShadowType();
		auto node = light.getParent();
		node->update();

		auto & directional = *light.getDirectionalLight();

		if ( directional.updateShadow( camera ) )
		{
			for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
			{
				auto & culler = m_passes[cascade].pass->getCuller();
				auto & lightCamera = culler.getCamera();
				lightCamera.attachTo( *node );
				lightCamera.setProjection( directional.getProjMatrix( m_cascades - 1u ) );
				lightCamera.setView( directional.getViewMatrix( m_cascades - 1u ) );
				lightCamera.updateFrustum();

				m_passes[cascade].pass->update( camera
					, queues
					, light
					, cascade );
			}
		}
	}

	void ShadowMapDirectional::doInitialiseFramebuffers()
	{
		VkExtent2D const size
		{
			ShadowMapPassDirectional::TextureSize,
			ShadowMapPassDirectional::TextureSize,
		};
		auto & depth = m_result[SmTexture::eDepth].getTexture()->getArray2D();
		auto & linear = m_result[SmTexture::eLinearNormal].getTexture()->getArray2D();
		auto & variance = m_result[SmTexture::eVariance].getTexture()->getArray2D();
		auto & position = m_result[SmTexture::ePosition].getTexture()->getArray2D();
		auto & flux = m_result[SmTexture::eFlux].getTexture()->getArray2D();

		for ( uint32_t cascade = 0u; cascade < m_passes.size(); ++cascade )
		{
			std::string debugName = "ShadowMapDirectionalCascade" + std::to_string( cascade );
			auto & pass = m_passes[cascade];
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_frameBuffers[cascade];
			frameBuffer.depthView = depth.layers[cascade].view->getView();
			frameBuffer.linearView = linear.layers[cascade].view->getView();
			frameBuffer.varianceView = variance.layers[cascade].view->getView();
			frameBuffer.positionView = position.layers[cascade].view->getView();
			frameBuffer.fluxView = flux.layers[cascade].view->getView();
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( frameBuffer.depthView );
			attaches.emplace_back( frameBuffer.linearView );
			attaches.emplace_back( frameBuffer.varianceView );
			attaches.emplace_back( frameBuffer.positionView );
			attaches.emplace_back( frameBuffer.fluxView );
			frameBuffer.frameBuffer = renderPass.createFrameBuffer( debugName, size, std::move( attaches ) );

			frameBuffer.blur = std::make_unique< GaussianBlur >( *getEngine()
				, debugName
				, frameBuffer.varianceView
				, 5u );
		}
	}

	void ShadowMapDirectional::doInitialise()
	{
		doInitialiseFramebuffers();
		auto & device = getCurrentRenderDevice( *getEngine() );
		m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( m_name );
	}

	void ShadowMapDirectional::doCleanup()
	{
		m_commandBuffer.reset();
		m_frameBuffers.clear();
	}

	bool ShadowMapDirectional::isUpToDate( uint32_t index )const
	{
		return std::all_of( m_passes.begin()
			, m_passes.begin() + m_cascades
			, []( ShadowMap::PassData const & data )
			{
				return data.pass->isUpToDate();
			} );
	}

	void ShadowMapDirectional::updateDeviceDependent( uint32_t index )
	{
		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			m_passes[cascade].pass->updateDeviceDependent( cascade );
		}
	}
	
	ashes::Semaphore const & ShadowMapDirectional::doRender( ashes::Semaphore const & toWait
		, uint32_t index )
	{
		auto & myTimer = m_passes[0].pass->getTimer();
		auto timerBlock = myTimer.start();
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		m_commandBuffer->beginDebugBlock(
			{
				m_name + " generation " + std::to_string( index ),
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			auto & pass = m_passes[cascade];
			auto & timer = pass.pass->getTimer();
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_frameBuffers[cascade];

			m_commandBuffer->beginDebugBlock(
				{
					m_name + " " + std::to_string( index ) + " cascade " + std::to_string( cascade ),
					makeFloatArray( getEngine()->getNextRainbowColour() ),
				} );
			timer.notifyPassRender();
			timer.beginPass( *m_commandBuffer );
			m_commandBuffer->beginRenderPass( pass.pass->getRenderPass()
				, *frameBuffer.frameBuffer
				, getClearValues()
				, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			m_commandBuffer->executeCommands( { pass.pass->getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			timer.endPass( *m_commandBuffer );
			m_commandBuffer->endDebugBlock();
			pass.pass->setUpToDate();
		}

		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();
		auto & device = getCurrentRenderDevice( *getEngine() );
		auto * result = &toWait;
		device.graphicsQueue->submit( *m_commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		if ( m_shadowType == ShadowType::eVariance )
		{
			for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
			{
				result = &m_frameBuffers[cascade].blur->blur( *result );
			}
		}
		return *result;
	}

	void ShadowMapDirectional::doUpdateFlags( PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, ProgramFlag::eShadowMapDirectional );
	}

	ShaderPtr ShadowMapDirectional::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );
		using namespace sdw;
		VertexWriter writer;

		// Vertex inputs
		auto position = writer.declInput< Vec4 >( "position"
			, RenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declInput< Vec3 >( "normal"
			, RenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declInput< Vec3 >( "tangent"
			, RenderPass::VertexInputs::TangentLocation );
		auto uv = writer.declInput< Vec3 >( "uv"
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declInput< IVec4 >( "bone_ids0"
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declInput< IVec4 >( "bone_ids1"
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declInput< Vec4 >( "weights0"
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declInput< Vec4 >( "weights1"
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( "transform"
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( "material"
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declInput< Vec4 >( "position2"
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declInput< Vec3 >( "normal2"
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declInput< Vec3 >( "tangent2"
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declInput< Vec3 >( "texture2"
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto in = writer.getIn();

		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );
		UBO_SHADOWMAP( writer, ShadowMapUbo::BindingPoint, 0 );

		// Outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declOutput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_normal = writer.declOutput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declOutput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declOutput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declOutput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		std::function< void() > main = [&]()
		{
			auto vertexPosition = writer.declLocale( "vertexPosition"
				, vec4( position.xyz(), 1.0_f ) );
			auto v4Normal = writer.declLocale( "v4Normal"
				, vec4( normal, 0.0_f ) );
			auto v4Tangent = writer.declLocale( "v4Tangent"
				, vec4( tangent, 0.0_f ) );
			vtx_texture = uv;

			if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
			{
				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
			}
			else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, transform );
			}
			else
			{
				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, c3d_curMtxModel );
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = writer.cast< UInt >( material );
			}
			else
			{
				vtx_material = writer.cast< UInt >( c3d_materialIndex );
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
			{
				vertexPosition = vec4( sdw::mix( vertexPosition.xyz(), position2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Normal = vec4( sdw::mix( v4Normal.xyz(), normal2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Tangent = vec4( sdw::mix( v4Tangent.xyz(), tangent2.xyz(), vec3( c3d_time ) ), 1.0_f );
				vtx_texture = vtx_texture * ( 1.0_f - c3d_time ) + texture2 * c3d_time;
			}

			auto mtxModel = writer.getVariable< Mat4 >( "mtxModel" );
			auto mtxNormal = writer.declLocale( "mtxNormal"
				, transpose( inverse( mat3( mtxModel ) ) ) );

			if ( checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) )
			{
				vtx_normal = normalize( mtxNormal * -v4Normal.xyz() );
			}
			else
			{
				vtx_normal = normalize( mtxNormal * v4Normal.xyz() );
			}

			vtx_tangent = normalize( mtxNormal * v4Tangent.xyz() );
			vtx_tangent = normalize( sdw::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );

			vertexPosition = mtxModel * vertexPosition;
			vtx_worldPosition = vertexPosition.xyz();
			vertexPosition = c3d_lightView * vertexPosition;
			vtx_viewPosition = vertexPosition.xyz();
			out.vtx.position = c3d_lightProjection * vertexPosition;
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapDirectional::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto in = writer.getIn();

		shader::LegacyMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );
		UBO_SHADOWMAP( writer, ShadowMapUbo::BindingPoint, 0u );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, getMinTextureIndex()
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );
		index += flags.texturesCount;

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, false // rsm
			, index
			, true );

		// Fragment Outputs
		auto pxl_linearNormal( writer.declOutput< Vec4 >( "pxl_linearNormal", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto normal = writer.declLocale( "normal"
					, normalize( vtx_normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( vtx_tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( vtx_bitangent ) );
				auto material = materials.getMaterial( vtx_material );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
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
				auto alpha = writer.declLocale( "alpha"
					, material.m_opacity );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material.m_alphaRef );
				shader::PhongLightingModel::computeMapContributions( writer
					, utils
					, flags
					, gamma
					, textureConfigs
					, c3d_textureConfig
					, tangent
					, bitangent
					, c3d_maps
					, vtx_texture
					, normal
					, diffuse
					, specular
					, emissive
					, shininess
					, alpha
					, occlusion
					, transmittance );
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, alphaRef );

				auto lightDiffuse = writer.declLocale( "lightDiffuse"
					, vec3( 0.0_f ) );
				auto lightSpecular = writer.declLocale( "lightSpecular"
					, vec3( 0.0_f ) );
				shader::OutputComponents output{ lightDiffuse, lightSpecular };
				lighting->compute( lighting->getDirectionalLight( writer.cast< Int >( c3d_lightIndex ) )
					, vec3( 0.0_f ) // TODO
					, shininess
					, 0_i
					, shader::FragmentInput( in.fragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
					, output );
				pxl_flux.rgb() = lightDiffuse;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
				pxl_linearNormal.x() = in.fragCoord.z();
				pxl_linearNormal.yzw() = normal;
				pxl_position.xyz() = vtx_worldPosition;

				pxl_variance.x() = depth;
				pxl_variance.y() = pxl_variance.x() * pxl_variance.x();

				auto dx = writer.declLocale( "dx"
					, dFdx( depth ) );
				auto dy = writer.declLocale( "dy"
					, dFdy( depth ) );
				pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapDirectional::doGetPbrMrPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto in = writer.getIn();

		shader::PbrMRMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );
		UBO_SHADOWMAP( writer, ShadowMapUbo::BindingPoint, 0u );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, getMinTextureIndex()
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );
		index += flags.texturesCount;

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		auto lighting = shader::MetallicBrdfLightingModel::createModel( writer
			, utils
			, false // rsm
			, index
			, true );

		// Fragment Outputs
		auto pxl_linearNormal( writer.declOutput< Vec4 >( "pxl_linearNormal", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto normal = writer.declLocale( "normal"
					, normalize( vtx_normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( vtx_tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( vtx_bitangent ) );
				auto material = materials.getMaterial( vtx_material );
				auto metallic = writer.declLocale( "metallic"
					, material.m_metallic );
				auto roughness = writer.declLocale( "roughness"
					, material.m_roughness );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto albedo = writer.declLocale( "albedo"
					, utils.removeGamma( gamma, material.m_albedo ) );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.m_emissive ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto alpha = writer.declLocale( "alpha"
					, material.m_opacity );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material.m_alphaRef );
				shader::MetallicBrdfLightingModel::computeMapContributions( writer
					, utils
					, flags
					, gamma
					, textureConfigs
					, c3d_textureConfig
					, vtx_tangent
					, vtx_bitangent
					, c3d_maps
					, vtx_texture
					, normal
					, albedo
					, metallic
					, emissive
					, roughness
					, alpha
					, occlusion
					, transmittance );
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, alphaRef );

				auto lightDiffuse = writer.declLocale( "lightDiffuse"
					, vec3( 0.0_f ) );
				auto lightSpecular = writer.declLocale( "lightSpecular"
					, vec3( 0.0_f ) );
				shader::OutputComponents output{ lightDiffuse, lightSpecular };
				lighting->compute( lighting->getDirectionalLight( writer.cast< Int >( c3d_lightIndex ) )
					, vec3( 0.0_f ) // TODO
					, albedo
					, metallic
					, roughness
					, 0_i
					, shader::FragmentInput( in.fragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
					, output );
				pxl_flux.rgb() = lightDiffuse;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
				pxl_linearNormal.x() = in.fragCoord.z();
				pxl_linearNormal.yzw() = normal;
				pxl_position.xyz() = vtx_worldPosition;

				pxl_variance.x() = depth;
				pxl_variance.y() = pxl_variance.x() * pxl_variance.x();

				auto dx = writer.declLocale( "dx"
					, dFdx( depth ) );
				auto dy = writer.declLocale( "dy"
					, dFdy( depth ) );
				pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapDirectional::doGetPbrSgPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto in = writer.getIn();

		shader::PbrSGMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );
		UBO_SHADOWMAP( writer, ShadowMapUbo::BindingPoint, 0u );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, getMinTextureIndex()
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );
		index += flags.texturesCount;

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		auto lighting = shader::SpecularBrdfLightingModel::createModel( writer
			, utils
			, false // rsm
			, index
			, true );

		// Fragment Outputs
		auto pxl_linearNormal( writer.declOutput< Vec4 >( "pxl_linearNormal", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto normal = writer.declLocale( "normal"
					, normalize( vtx_normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( vtx_tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( vtx_bitangent ) );
				auto material = materials.getMaterial( vtx_material );
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
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto alpha = writer.declLocale( "alpha"
					, material.m_opacity );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material.m_alphaRef );
				shader::SpecularBrdfLightingModel::computeMapContributions( writer
					, utils
					, flags
					, gamma
					, textureConfigs
					, c3d_textureConfig
					, vtx_tangent
					, vtx_bitangent
					, c3d_maps
					, vtx_texture
					, normal
					, albedo
					, specular
					, emissive
					, glossiness
					, alpha
					, occlusion
					, transmittance );
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, alphaRef );

				auto lightDiffuse = writer.declLocale( "lightDiffuse"
					, vec3( 0.0_f ) );
				auto lightSpecular = writer.declLocale( "lightSpecular"
					, vec3( 0.0_f ) );
				shader::OutputComponents output{ lightDiffuse, lightSpecular };
				lighting->compute( lighting->getDirectionalLight( writer.cast< Int >( c3d_lightIndex ) )
					, vec3( 0.0_f ) // TODO
					, specular
					, glossiness
					, 0_i
					, shader::FragmentInput( in.fragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
					, output );
				pxl_flux.rgb() = lightDiffuse;

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
				pxl_linearNormal.x() = in.fragCoord.z();
				pxl_linearNormal.yzw() = normal;
				pxl_position.xyz() = vtx_worldPosition;

				pxl_variance.x() = depth;
				pxl_variance.y() = pxl_variance.x() * pxl_variance.x();

				auto dx = writer.declLocale( "dx"
					, dFdx( depth ) );
				auto dy = writer.declLocale( "dy"
					, dFdy( depth ) );
				pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
