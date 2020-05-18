#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
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
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassPoint.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

#include <algorithm>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		std::vector< ShadowMap::PassData > createPasses( Engine & engine
			, Scene & scene
			, ShadowMap & shadowMap )
		{
			std::vector< ShadowMap::PassData > result;

			for ( auto i = 0u; i < 6u * shader::getPointShadowMapCount(); ++i )
			{
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( engine ),
					nullptr,
					std::make_unique< DummyCuller >( scene ),
					nullptr,
				};
				passData.pass = std::make_shared< ShadowMapPassPoint >( engine
					, i
					, *passData.matrixUbo
					, *passData.culler
					, shadowMap );
				result.emplace_back( std::move( passData ) );
			}

			return result;
		}
	}

	ShadowMapPoint::ShadowMapPoint( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, cuT( "ShadowMapPoint" )
			, ShadowMapPassResult
			{
				engine,
				cuT( "Point" ),
				VkImageCreateFlags( VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT ),
				Size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize },
				6u * shader::getPointShadowMapCount(),
			}
			, createPasses( engine, scene, *this )
			, shader::getPointShadowMapCount() }
	{
		log::trace << "Created ShadowMapPoint" << std::endl;
	}

	ShadowMapPoint::~ShadowMapPoint()
	{
	}

	void ShadowMapPoint::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_passesData[index].shadowType = light.getShadowType();

		for ( size_t face = index * 6u; face < ( index * 6u ) + 6u; ++face )
		{
			auto & pass = m_passes[face];
			pass.pass->update( camera, queues, light, index );
		}
	}

	ashes::ImageView const & ShadowMapPoint::getLinearView( uint32_t index )const
	{
		return m_passesData[index].linearView;
	}

	ashes::ImageView const & ShadowMapPoint::getVarianceView( uint32_t index )const
	{
		return m_passesData[index].varianceView;
	}

	void ShadowMapPoint::doInitialiseFramebuffers()
	{
		VkExtent2D size
		{
			ShadowMapPassPoint::TextureSize,
			ShadowMapPassPoint::TextureSize,
		};
		auto & depth = *m_result[SmTexture::eDepth].getTexture();
		auto & linear = *m_result[SmTexture::eLinearNormal].getTexture();
		auto & variance = *m_result[SmTexture::eVariance].getTexture();
		auto & position = *m_result[SmTexture::ePosition].getTexture();
		auto & flux = *m_result[SmTexture::eFlux].getTexture();

		for ( uint32_t layer = 0u; layer < shader::getPointShadowMapCount(); ++layer )
		{
			std::string debugName = "ShadowMapPoint" + std::to_string( layer );
			uint32_t passIndex = layer * 6u;
			PassData data{};
			data.depthView = depth.getLayerCubeView( layer ).getView();
			data.linearView = linear.getLayerCubeView( layer ).getView();
			data.varianceView = variance.getLayerCubeView( layer ).getView();
			data.positionView = position.getLayerCubeView( layer ).getView();
			data.fluxView = flux.getLayerCubeView( layer ).getView();

			for ( auto & frameBuffer : data.frameBuffers )
			{
				auto face = CubeMapFace( passIndex % 6u );
				auto fbDebugName = debugName + getName( face );
				auto & pass = m_passes[passIndex];
				auto & renderPass = pass.pass->getRenderPass();
				frameBuffer.depthView = depth.getLayerCubeFaceView( layer, face ).getView();
				frameBuffer.linearView = linear.getLayerCubeFaceView( layer, face ).getView();
				frameBuffer.varianceView = variance.getLayerCubeFaceView( layer, face ).getView();
				frameBuffer.positionView = position.getLayerCubeFaceView( layer, face ).getView();
				frameBuffer.fluxView = flux.getLayerCubeFaceView( layer, face ).getView();
				ashes::ImageViewCRefArray attaches;
				attaches.emplace_back( frameBuffer.depthView );
				attaches.emplace_back( frameBuffer.linearView );
				attaches.emplace_back( frameBuffer.varianceView );
				attaches.emplace_back( frameBuffer.positionView );
				attaches.emplace_back( frameBuffer.fluxView );
				frameBuffer.frameBuffer = renderPass.createFrameBuffer( fbDebugName
					, size
					, std::move( attaches ) );
				++passIndex;
			}

			m_passesData.emplace_back( std::move( data ) );
		}
	}

	void ShadowMapPoint::doInitialise()
	{
		doInitialiseFramebuffers();
		uint32_t index = 0u;

		for ( auto & data : m_passesData )
		{
			std::string debugName = "ShadowMapPoint" + std::to_string( index++ );
			auto & device = getCurrentRenderDevice( *this );
			data.commandBuffer = device.graphicsCommandPool->createCommandBuffer( debugName );
			data.finished = device->createSemaphore( debugName );
		}
	}

	void ShadowMapPoint::doCleanup()
	{
		m_passesData.clear();
	}

	void ShadowMapPoint::updateDeviceDependent( uint32_t index )
	{
		uint32_t offset = index * 6u;

		for ( uint32_t face = offset; face < offset + 6u; ++face )
		{
			m_passes[face].pass->updateDeviceDependent( face - offset );
		}
	}

	ashes::Semaphore const & ShadowMapPoint::doRender( ashes::Semaphore const & toWait
		, uint32_t index )
	{
		auto & myTimer = m_passes[0].pass->getTimer();
		auto timerBlock = myTimer.start();
		auto * result = &toWait;
		uint32_t offset = index * 6u;

		auto & commandBuffer = *m_passesData[index].commandBuffer;
		auto & finished = *m_passesData[index].finished;
		commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		commandBuffer.beginDebugBlock(
			{
				m_name + " generation " + std::to_string( index ),
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			auto & pass = m_passes[offset + face];
			auto & timer = pass.pass->getTimer();
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_passesData[index].frameBuffers[face];

			commandBuffer.beginDebugBlock(
				{
					m_name + " " + std::to_string( index ) + " face " + std::to_string( face ),
					makeFloatArray( getEngine()->getNextRainbowColour() ),
				} );
			timer.notifyPassRender();
			timer.beginPass( commandBuffer );
			commandBuffer.beginRenderPass( renderPass
				, *frameBuffer.frameBuffer
				, getClearValues()
				, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			commandBuffer.executeCommands( { pass.pass->getCommandBuffer() } );
			commandBuffer.endRenderPass();
			timer.endPass( commandBuffer );
			commandBuffer.endDebugBlock();
			pass.pass->setUpToDate();
		}

		commandBuffer.endDebugBlock();
		commandBuffer.end();
		auto & device = getCurrentRenderDevice( *this );

		device.graphicsQueue->submit( commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, finished
			, nullptr );
		result = &finished;

		return *result;
	}

	bool ShadowMapPoint::isUpToDate( uint32_t index )const
	{
		uint32_t offset = index * 6u;
		return std::all_of( m_passes.begin() + offset
			, m_passes.begin() + offset + 6u
			, []( ShadowMap::PassData const & data )
			{
				return data.pass->isUpToDate();
			} );
	}

	void ShadowMapPoint::doUpdateFlags( PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, ProgramFlag::eShadowMapPoint );
	}

	ShaderPtr ShadowMapPoint::doGetVertexShaderSource( PipelineFlags const & flags )const
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

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );

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
		auto vtx_cameraPosition = writer.declOutput< Vec3 >( "vtx_cameraPosition"
			, 12u );
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
			vtx_cameraPosition = c3d_curView[3].xyz();

			vertexPosition = mtxModel * vertexPosition;
			vtx_worldPosition = vertexPosition.xyz();
			vertexPosition = c3d_curView * vertexPosition;
			vtx_viewPosition = vertexPosition.xyz();
			out.vtx.position = c3d_projection * vertexPosition;
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPoint::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
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
		auto vtx_cameraPosition = writer.declInput< Vec3 >( "vtx_cameraPosition"
			, 12u );
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
			, index
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );
		index += flags.texturesCount;

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, LightType::ePoint
			, ShadowType::eNone
			, false // lightUbo
			, false // volumetric
			, false // rsm
			, index );

		// Fragment Outputs
		auto pxl_linearNormal( writer.declOutput< Vec4 >( "pxl_linearNormal", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		auto main = [&]()
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
			lighting->compute( lighting->getPointLight( writer.cast< Int >( c3d_lightIndex ) )
				, vtx_cameraPosition
				, shininess
				, 0_i
				, shader::FragmentInput( in.fragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
				, output );
			pxl_flux.rgb() = lightDiffuse;

			auto depth = writer.declLocale( "depth"
				, length( vtx_worldPosition - c3d_lightPosFarPlane.xyz() ) );
			pxl_linearNormal.x() = depth / c3d_lightPosFarPlane.w();
			pxl_linearNormal.yzw() = normal;
			pxl_position.xyz() = vtx_worldPosition;

			pxl_variance.x() = pxl_linearNormal.x();
			pxl_variance.y() = pxl_linearNormal.x() * pxl_linearNormal.x();

			auto dx = writer.declLocale( "dx"
				, dFdx( pxl_linearNormal.x() ) );
			auto dy = writer.declLocale( "dy"
				, dFdy( pxl_linearNormal.x() ) );
			pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPoint::doGetPbrMrPixelShaderSource( PipelineFlags const & flags )const
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
		auto vtx_cameraPosition = writer.declInput< Vec3 >( "vtx_cameraPosition"
			, 12u );
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
			, LightType::ePoint
			, ShadowType::eNone
			, false // lightUbo
			, false // volumetric
			, false // rsm
			, index );

		// Fragment Outputs
		auto pxl_linearNormal( writer.declOutput< Vec4 >( "pxl_linearNormal", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		auto main = [&]()
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
			lighting->compute( lighting->getPointLight( writer.cast< Int >( c3d_lightIndex ) )
				, vtx_cameraPosition
				, albedo
				, metallic
				, roughness
				, 0_i
				, shader::FragmentInput( in.fragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
				, output );
			pxl_flux.rgb() = lightDiffuse;

			auto depth = writer.declLocale( "depth"
				, length( vtx_worldPosition - c3d_lightPosFarPlane.xyz() ) );
			pxl_linearNormal.x() = depth / c3d_lightPosFarPlane.w();
			pxl_linearNormal.yzw() = normal;
			pxl_position.xyz() = vtx_worldPosition;

			pxl_variance.x() = pxl_linearNormal.x();
			pxl_variance.y() = pxl_linearNormal.x() * pxl_linearNormal.x();

			auto dx = writer.declLocale( "dx"
				, dFdx( pxl_linearNormal.x() ) );
			auto dy = writer.declLocale( "dy"
				, dFdy( pxl_linearNormal.x() ) );
			pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPoint::doGetPbrSgPixelShaderSource( PipelineFlags const & flags )const
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
		auto vtx_cameraPosition = writer.declInput< Vec3 >( "vtx_cameraPosition"
			, 12u );
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
			, LightType::ePoint
			, ShadowType::eNone
			, false // lightUbo
			, false // volumetric
			, false // rsm
			, index );

		// Fragment Outputs
		auto pxl_linearNormal( writer.declOutput< Vec4 >( "pxl_linearNormal", 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", 1u ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", 2u ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", 3u ) );

		auto main = [&]()
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
			lighting->compute( lighting->getPointLight( writer.cast< Int >( c3d_lightIndex ) )
				, vtx_cameraPosition
				, specular
				, glossiness
				, 0_i
				, shader::FragmentInput( in.fragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
				, output );
			pxl_flux.rgb() = lightDiffuse;

			auto depth = writer.declLocale( "depth"
				, length( vtx_worldPosition - c3d_lightPosFarPlane.xyz() ) );
			pxl_linearNormal.x() = depth / c3d_lightPosFarPlane.w();
			pxl_linearNormal.yzw() = normal;
			pxl_position.xyz() = vtx_worldPosition;

			pxl_variance.x() = pxl_linearNormal.x();
			pxl_variance.y() = pxl_linearNormal.x() * pxl_linearNormal.x();

			auto dx = writer.declLocale( "dx"
				, dFdx( pxl_linearNormal.x() ) );
			auto dy = writer.declLocale( "dy"
				, dFdy( pxl_linearNormal.x() ) );
			pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
