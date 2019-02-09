#include "ShadowMapSpot.hpp"

#include "Engine.hpp"

#include "Render/RenderPassTimer.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Culling/FrustumCuller.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Shader/Program.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "ShadowMap/ShadowMapPassSpot.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureView.hpp"
#include "Texture/TextureLayout.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>

#include <ShaderWriter/Source.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		TextureUnit doInitialiseVariance( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Spot_Variance" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( ashes::Filter::eLinear );
				sampler->setMagFilter( ashes::Filter::eLinear );
				sampler->setWrapS( ashes::WrapMode::eClampToBorder );
				sampler->setWrapT( ashes::WrapMode::eClampToBorder );
				sampler->setWrapR( ashes::WrapMode::eClampToBorder );
				sampler->setBorderColour( ashes::BorderColour::eFloatOpaqueWhite );
			}

			ashes::ImageCreateInfo image{};
			image.arrayLayers = shader::SpotShadowMapCount;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
			image.format = ShadowMapSpot::VarianceFormat;

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
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
			String const name = cuT( "ShadowMap_Spot_Depth" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( ashes::Filter::eLinear );
				sampler->setMagFilter( ashes::Filter::eLinear );
				sampler->setWrapS( ashes::WrapMode::eClampToEdge );
				sampler->setWrapT( ashes::WrapMode::eClampToEdge );
				sampler->setWrapR( ashes::WrapMode::eClampToEdge );
				sampler->setBorderColour( ashes::BorderColour::eFloatOpaqueWhite );
			}

			ashes::ImageCreateInfo image{};
			image.arrayLayers = shader::SpotShadowMapCount;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
			image.format = ShadowMapSpot::LinearDepthFormat;

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
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
			Viewport viewport{ engine };
			viewport.resize( Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } );

			for ( auto i = 0u; i < shader::SpotShadowMapCount; ++i )
			{
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( engine ),
					std::make_shared< Camera >( cuT( "ShadowMapSpot" )
						, scene
						, scene.getCameraRootNode()
						, std::move( viewport ) ),
					nullptr,
					nullptr,
				};
				passData.culler = std::make_unique< FrustumCuller >( scene, *passData.camera );
				passData.pass = std::make_shared< ShadowMapPassSpot >( engine
					, *passData.matrixUbo
					, *passData.culler
					, shadowMap );
				result.emplace_back( std::move( passData ) );
			}

			return result;
		}
	}

	ShadowMapSpot::ShadowMapSpot( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, doInitialiseVariance( engine, Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } )
			, doInitialiseLinearDepth( engine, Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } )
			, createPasses( engine, scene, *this )
			, shader::SpotShadowMapCount }
	{
	}

	ShadowMapSpot::~ShadowMapSpot()
	{
	}
	
	void ShadowMapSpot::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_passes[index].pass->update( camera, queues, light, index );
	}

	ashes::Semaphore const & ShadowMapSpot::render( ashes::Semaphore const & toWait
		, uint32_t index )
	{
		static ashes::ClearColorValue const black{ 0.0f, 0.0f, 0.0f, 1.0f };
		static ashes::DepthStencilClearValue const zero{ 1.0f, 0 };

		auto & pass = m_passes[index];
		auto & commandBuffer = *m_passesData[index].commandBuffer;
		auto & frameBuffer = *m_passesData[index].frameBuffer;
		auto & finished = *m_passesData[index].finished;
		auto & blur = *m_passesData[index].blur;

		pass.pass->updateDeviceDependent();
		auto & timer = pass.pass->getTimer();
		auto timerBlock = timer.start();

		commandBuffer.begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );
		timer.notifyPassRender();
		timer.beginPass( commandBuffer );
		commandBuffer.beginRenderPass( pass.pass->getRenderPass()
			, frameBuffer
			, { zero, black, black }
			, ashes::SubpassContents::eSecondaryCommandBuffers );
		commandBuffer.executeCommands( { pass.pass->getCommandBuffer() } );
		commandBuffer.endRenderPass();
		timer.endPass( commandBuffer );
		commandBuffer.end();

		auto & device = getCurrentDevice( *this );
		auto * result = &toWait;
		device.getGraphicsQueue().submit( commandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, finished
			, nullptr );
		result = &finished;

		if ( static_cast< ShadowMapPassSpot const & >( *pass.pass ).getShadowType() == ShadowType::eVariance )
		{
			result = &blur.blur( *result );
		}

		return *result;
	}

	void ShadowMapSpot::debugDisplay( ashes::RenderPass const & renderPass
		, ashes::FrameBuffer const & frameBuffer
		, castor::Size const & size
		, uint32_t index )
	{
		Size displaySize{ 256u, 256u };
		Position position{ int32_t( displaySize.getWidth() * ( 0 + index * 2 ) ), int32_t( displaySize.getHeight() * 2u ) };
		getEngine()->renderDepth( renderPass
			, frameBuffer
			, position
			, displaySize
			, *m_shadowMap.getTexture() );
		position.offset( int32_t( displaySize.getWidth() ), 0 );
		getEngine()->renderDepth( renderPass
			, frameBuffer
			, position
			, displaySize
			, *m_linearMap.getTexture() );
	}

	ashes::TextureView const & ShadowMapSpot::getView( uint32_t index )const
	{
		return m_shadowMap.getTexture()->getImage( index ).getView();
	}

	void ShadowMapSpot::doInitialise()
	{
		ashes::Extent2D size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize };
		auto & device = getCurrentDevice( *this );
		ashes::ImageCreateInfo depth{};
		depth.arrayLayers = 1u;
		depth.extent.width = size.width;
		depth.extent.height = size.height;
		depth.extent.depth = 1u;
		depth.imageType = ashes::TextureType::e2D;
		depth.mipLevels = 1u;
		depth.samples = ashes::SampleCountFlag::e1;
		depth.usage = ashes::ImageUsageFlag::eDepthStencilAttachment;
		depth.format = ShadowMapSpot::RawDepthFormat;

		ashes::ImageViewCreateInfo view;
		view.format = depth.format;
		view.viewType = ashes::TextureViewType::e2D;
		view.subresourceRange.aspectMask = ashes::ImageAspectFlag::eDepth;
		view.subresourceRange.baseArrayLayer = 0u;
		view.subresourceRange.layerCount = 1u;
		view.subresourceRange.baseMipLevel = 0u;
		view.subresourceRange.levelCount = 1u;

		for ( auto i = 0u; i < m_passes.size(); ++i )
		{
			auto depthTexture = device.createTexture( depth, ashes::MemoryPropertyFlag::eDeviceLocal );
			auto depthView = depthTexture->createView( view );

			auto & renderPass = m_passes[i].pass->getRenderPass();
			ashes::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 0u ), *depthView );
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 1u ), m_linearMap.getTexture()->getImage( i ).getView() );
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 2u ), m_shadowMap.getTexture()->getImage( i ).getView() );
			m_passesData.push_back(
				{
					device.getGraphicsCommandPool().createCommandBuffer(),
					renderPass.createFrameBuffer( size, std::move( attaches ) ),
					device.createSemaphore(),
					std::move( depthTexture ),
					std::move( depthView ),
					std::make_unique< GaussianBlur >( *getEngine()
						, m_shadowMap.getTexture()->getImage( i ).getView()
						, size
						, m_shadowMap.getTexture()->getPixelFormat()
						, 5u )
				} );
		}
	}

	void ShadowMapSpot::doCleanup()
	{
		m_passesData.clear();
	}

	void ShadowMapSpot::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapSpot );
	}

	ShaderPtr ShadowMapSpot::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );
		using namespace sdw;
		VertexWriter writer;

		// Vertex inputs
		auto position = writer.declInput< Vec4 >( cuT( "position" )
			, RenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declInput< Vec3 >( cuT( "normal" )
			, RenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declInput< Vec3 >( cuT( "tangent" )
			, RenderPass::VertexInputs::TangentLocation );
		auto uv = writer.declInput< Vec3 >( cuT( "uv" )
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declInput< IVec4 >( cuT( "bone_ids0" )
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declInput< IVec4 >( cuT( "bone_ids1" )
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declInput< Vec4 >( cuT( "weights0" )
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declInput< Vec4 >( cuT( "weights1" )
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( cuT( "transform" )
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( cuT( "material" )
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declInput< Vec4 >( cuT( "position2" )
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declInput< Vec3 >( cuT( "normal2" )
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declInput< Vec3 >( cuT( "tangent2" )
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declInput< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto in = writer.getIn();

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, programFlags );

		// Outputs
		auto vtx_viewPosition = writer.declOutput< Vec3 >( cuT( "vtx_viewPosition" )
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declOutput< UInt >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		std::function< void() > main = [&]()
		{
			auto vertexPosition = writer.declLocale( cuT( "vertexPosition" )
				, vec4( position.xyz(), 1.0 ) );
			vtx_texture = uv;

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" )
					, SkinningUbo::computeTransform( skinningData, writer, programFlags ) );
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" )
					, transform );
			}
			else
			{
				auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" )
					, c3d_curMtxModel );
			}

			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = writer.cast< UInt >( material );
			}
			else
			{
				vtx_material = writer.cast< UInt >( c3d_materialIndex );
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.declLocale( cuT( "time" ), 1.0_f - c3d_time );
				vertexPosition = vec4( vertexPosition.xyz() * time + position2.xyz() * c3d_time, 1.0 );
				vtx_texture = vtx_texture * writer.paren( 1.0_f - c3d_time ) + texture2 * c3d_time;
			}

			auto mtxModel = writer.getVariable< Mat4 >( cuT( "mtxModel" ) );
			vertexPosition = mtxModel * vertexPosition;
			vertexPosition = c3d_curView * vertexPosition;
			vtx_viewPosition = vertexPosition.xyz();
			out.gl_out.gl_Position = c3d_projection * vertexPosition;
		};

		writer.implementFunction< sdw::Void >( cuT( "main" ), main );
		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapSpot::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		using namespace sdw;
		FragmentWriter writer;

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapPassSpot::ShadowMapUbo, ShadowMapPassSpot::UboBindingPoint, 0u };
		auto c3d_farPlane( shadowMap.declMember< Float >( ShadowMapPassSpot::FarPlane ) );
		shadowMap.end();

		auto vtx_viewPosition = writer.declInput< Vec3 >( cuT( "vtx_viewPosition" )
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declInput< UInt >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_mapOpacity( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapOpacity" )
			, MinBufferIndex
			, 1u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );
		auto in = writer.getIn();

		auto materials = shader::createMaterials( writer, passFlags );
		materials->declare( getEngine()->getRenderSystem()->getGpuInformations().hasShaderStorageBuffers() );

		// Fragment Outputs
		auto pxl_linear( writer.declOutput< Float >( cuT( "pxl_linear" ), 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( cuT( "pxl_variance" ), 1u ) );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			doDiscardAlpha( writer
				, textureFlags
				, alphaFunc
				, vtx_material
				, *materials );

			auto depth = writer.declLocale( cuT( "depth" )
				, in.gl_FragCoord.z() );
			pxl_variance.x() = depth;
			pxl_variance.y() = pxl_variance.x() * pxl_variance.x();
			//pxl_linear = length( vtx_viewPosition ) / c3d_farPlane;
			pxl_linear = abs( vtx_viewPosition.z() ) / c3d_farPlane;

			auto dx = writer.declLocale( cuT( "dx" )
				, dFdx( depth ) );
			auto dy = writer.declLocale( cuT( "dy" )
				, dFdy( depth ) );
			pxl_variance.y() += 0.25_f * writer.paren( dx * dx + dy * dy );
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}
}
