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
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
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
		TextureUnit doInitialiseVariance( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Point_Shadow" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_LINEAR );
				sampler->setMagFilter( VK_FILTER_LINEAR );
				sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setBorderColour( VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE );
			}

			ashes::ImageCreateInfo image
			{
				VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
				VK_IMAGE_TYPE_2D,
				ShadowMapPoint::VarianceFormat,
				{ size[0], size[1], 1u },
				1u,
				6u * shader::getPointShadowMapCount(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			};
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, cuT( "ShadowMapPoint_Variance" ) );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

		TextureUnit doInitialiseLinearDepth( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Point_Depth" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_LINEAR );
				sampler->setMagFilter( VK_FILTER_LINEAR );
				sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setBorderColour( VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE );
			}

			ashes::ImageCreateInfo image
			{
				VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
				VK_IMAGE_TYPE_2D,
				ShadowMapPoint::LinearDepthFormat,
				{ size[0], size[1], 1u },
				1u,
				6u * shader::getPointShadowMapCount(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			};
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, cuT( "ShadowMapPoint_Linear" ) );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

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
					, *passData.matrixUbo
					, *passData.culler
					, shadowMap );
				result.emplace_back( std::move( passData ) );
			}

			return result;
		}
	}

	VkFormat ShadowMapPoint::RawDepthFormat = VK_FORMAT_UNDEFINED;

	ShadowMapPoint::ShadowMapPoint( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, cuT( "ShadowMapPoint" )
			, doInitialiseVariance( engine, Size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize } )
			, doInitialiseLinearDepth( engine, Size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize } )
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

	void ShadowMapPoint::debugDisplay( ashes::RenderPass const & renderPass
		, ashes::FrameBuffer const & frameBuffer
		, castor::Size const & size, uint32_t index )
	{
		//Size displaySize{ 128u, 128u };
		//Position position{ int32_t( displaySize.getWidth() * 4 * index), int32_t( displaySize.getHeight() * 4 ) };
		//getCurrentRenderDevice( *this )->renderVarianceCube( position
		//	, displaySize
		//	, *m_shadowMap.getTexture() );
		//position = Position{ int32_t( displaySize.getWidth() * 4 * ( index + 2 ) ), int32_t( displaySize.getHeight() * 4 ) };
		//getCurrentRenderDevice( *this )->renderDepthCube( position
		//	, displaySize
		//	, *m_linearMap.getTexture() );
	}

	ashes::ImageView const & ShadowMapPoint::getLinearView( uint32_t index )const
	{
		return m_passesData[index].linearView;
	}

	ashes::ImageView const & ShadowMapPoint::getVarianceView( uint32_t index )const
	{
		return m_passesData[index].varianceView;
	}

	void ShadowMapPoint::doInitialiseDepthFormat()
	{
		auto & device = getCurrentRenderDevice( *this );

		if ( RawDepthFormat == VK_FORMAT_UNDEFINED )
		{
			RawDepthFormat = device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
		}
	}

	void ShadowMapPoint::doInitialise()
	{
		auto & device = getCurrentRenderDevice( *this );
		VkExtent2D size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize };

		ashes::ImageCreateInfo depth
		{
			0u,
			VK_IMAGE_TYPE_2D,
			ShadowMapPoint::RawDepthFormat,
			{ size.height, size.height, 1u },
			1u,
			6u * shader::getPointShadowMapCount(),
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		};
		ashes::ImageViewCreateInfo view
		{
			0u,
			VK_NULL_HANDLE,
			VK_IMAGE_VIEW_TYPE_2D,
			depth->format,
			VkComponentMapping{},
			{ VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u },
		};

		for ( auto i = 0u; i < shader::getPointShadowMapCount(); ++i )
		{
			std::string debugName = "ShadowMapPoint" + std::to_string( i );
			auto & variance = m_shadowMap.getTexture()->getTexture();
			auto & linear = m_linearMap.getTexture()->getTexture();
			uint32_t face = i * 6u;
			PassData data;
			data.depthTexture = makeImage( device
				, depth
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "PointShadowMapDepth" );
			view->image = *data.depthTexture;
			data.depthView = data.depthTexture->createView( view );
			setDebugObjectName( device
				, data.depthView
				, debugName + "Depth" );
			data.varianceView = variance.createView( VK_IMAGE_VIEW_TYPE_CUBE
				, variance.getFormat()
				, 0u
				, 1u
				, face
				, 6u );
			setDebugObjectName( device
				, data.varianceView
				, debugName + "Variance" );
			data.linearView = linear.createView( VK_IMAGE_VIEW_TYPE_CUBE
				, linear.getFormat()
				, 0u
				, 1u
				, face
				, 6u );
			setDebugObjectName( device
				, data.linearView
				, debugName + "Linear" );

			for ( auto & frameBuffer : data.frameBuffers )
			{
				auto fbDebugName = debugName + "x" + std::to_string( face );
				auto & pass = m_passes[face];
				auto & renderPass = pass.pass->getRenderPass();
				frameBuffer.varianceView = variance.createView( VK_IMAGE_VIEW_TYPE_2D
					, variance.getFormat()
					, 0u
					, 1u
					, face
					, 1u );
				setDebugObjectName( device
					, frameBuffer.varianceView
					, fbDebugName + "Variance" );
				frameBuffer.linearView = linear.createView( VK_IMAGE_VIEW_TYPE_2D
					, linear.getFormat()
					, 0u
					, 1u
					, face
					, 1u );
				setDebugObjectName( device
					, frameBuffer.linearView
					, fbDebugName + "Linear" );
				ashes::ImageViewCRefArray attaches;
				attaches.emplace_back( data.depthView );
				attaches.emplace_back( frameBuffer.linearView );
				attaches.emplace_back( frameBuffer.varianceView );
				frameBuffer.frameBuffer = renderPass.createFrameBuffer( size, std::move( attaches ) );
				setDebugObjectName( device
					, *frameBuffer.frameBuffer
					, fbDebugName + "Fbo" );
				++face;
			}

			auto & device = getCurrentRenderDevice( *this );
			data.commandBuffer = device.graphicsCommandPool->createCommandBuffer();
			data.finished = device->createSemaphore();
			m_passesData.emplace_back( std::move( data ) );
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
		static float constexpr component = std::numeric_limits< float >::max();
		static auto const rgb32fMaxColor{ ashes::makeClearValue( VkClearColorValue{ component, component, component, component } ) };
		auto & myTimer = m_passes[0].pass->getTimer();
		auto timerBlock = myTimer.start();
		auto * result = &toWait;
		uint32_t offset = index * 6u;

		auto & commandBuffer = *m_passesData[index].commandBuffer;
		auto & finished = *m_passesData[index].finished;
		commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		auto col = index / ( ( shader::getPointShadowMapCount() - 1u ) * 2.0f );

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			auto & pass = m_passes[offset + face];
			auto & timer = pass.pass->getTimer();
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_passesData[index].frameBuffers[face];

			commandBuffer.beginDebugBlock(
				{
					"ShadowMapPoint index " + std::to_string( index ) + " face " + std::to_string( face ),
					{ face / 10.0f, col, 0.7f, 1.0f },
				} );
			timer.notifyPassRender();
			timer.beginPass( commandBuffer );
			commandBuffer.beginRenderPass( renderPass
				, *frameBuffer.frameBuffer
				, { defaultClearDepthStencil, rgb32fMaxColor, rgb32fMaxColor }
				, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			commandBuffer.executeCommands( { pass.pass->getCommandBuffer() } );
			commandBuffer.endRenderPass();
			timer.endPass( commandBuffer );
			commandBuffer.endDebugBlock();
			pass.pass->setUpToDate();
		}

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
		auto vtx_texture = writer.declOutput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declOutput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		std::function< void() > main = [&]()
		{
			auto vertexPosition = writer.declLocale( "vertexPosition"
				, vec4( position.xyz(), 1.0_f ) );
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
				auto time = writer.declLocale( "time"
					, 1.0_f - c3d_time );
				vertexPosition = vec4( vertexPosition.xyz() * time + position2.xyz() * c3d_time, 1.0_f );
				vtx_texture = vtx_texture * ( 1.0_f - c3d_time ) + texture2 * c3d_time;
			}

			auto mtxModel = writer.getVariable< Mat4 >( "mtxModel" );
			vertexPosition = mtxModel * vertexPosition;
			vtx_worldPosition = vertexPosition.xyz();
			vertexPosition = c3d_curView * vertexPosition;
			out.vtx.position = c3d_projection * vertexPosition;
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPoint::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapPassPoint::ShadowMapUbo, ShadowMapPassPoint::UboBindingPoint, 0u };
		auto c3d_worldLightPositionFarPlane( shadowMap.declMember< Vec4 >( ShadowMapPassPoint::WorldLightPosition ) );
		shadowMap.end();

		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, getMinTextureIndex()
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );

		auto materials = shader::createMaterials( writer, flags.passFlags );
		materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		// Fragment Outputs
		auto pxl_linear = writer.declOutput< Float >( "pxl_linear", 0u );
		auto pxl_variance = writer.declOutput< Vec2 >( "pxl_variance", 1u );
		shader::Utils utils{ writer };

		auto main = [&]()
		{
			auto material = materials->getBaseMaterial( vtx_material );
			auto alpha = writer.declLocale( "alpha"
				, material->m_opacity );
			auto alphaRef = writer.declLocale( "alphaRef"
				, material->m_alphaRef );
			utils.computeOpacityMapContribution( flags
				, textureConfigs
				, c3d_textureConfig
				, c3d_maps
				, vtx_texture
				, alpha );
			utils.applyAlphaFunc( flags.alphaFunc
				, alpha
				, alphaRef );

			auto depth = writer.declLocale( "depth"
				, length( vtx_worldPosition - c3d_worldLightPositionFarPlane.xyz() ) );
			pxl_linear = depth / c3d_worldLightPositionFarPlane.w();
			pxl_variance.x() = pxl_linear;
			pxl_variance.y() = pxl_linear * pxl_linear;

			auto dx = writer.declLocale( "dx"
				, dFdx( pxl_linear ) );
			auto dy = writer.declLocale( "dy"
				, dFdy( pxl_linear ) );
			pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
