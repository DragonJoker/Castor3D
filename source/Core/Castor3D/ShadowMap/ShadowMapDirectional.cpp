#include "Castor3D/ShadowMap/ShadowMapDirectional.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Miscellaneous/GaussianBlur.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"
#include "Castor3D/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Texture/Sampler.hpp"
#include "Castor3D/Texture/TextureView.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <Ashes/Image/Texture.hpp>
#include <Ashes/Image/TextureView.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/RenderPass/RenderSubpass.hpp>
#include <Ashes/RenderPass/RenderSubpassState.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Ashes/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		TextureUnit doInitialiseVariance( Engine & engine
			, Size const & size
			, uint32_t cascadeCount )
		{
			String const name = cuT( "ShadowMap_Directional_Variance" );
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
				sampler->setBorderColour( ashes::BorderColour::eFloatOpaqueBlack );
			}

			ashes::ImageCreateInfo image{};
			image.arrayLayers = cascadeCount;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
			image.format = ShadowMapDirectional::VarianceFormat;

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal
				, cuT( "ShadowMapDirectional_Variance" ) );
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
			, Size const & size
			, uint32_t cascadeCount )
		{
			String const name = cuT( "ShadowMap_Directional_Depth" );
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
			}

			ashes::ImageCreateInfo image{};
			image.arrayLayers = cascadeCount;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
			image.format = ShadowMapDirectional::LinearDepthFormat;

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal
				, cuT( "ShadowMapDirectional_Linear" ) );
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
			, ShadowMap & shadowMap
			, uint32_t cascadeCount )
		{
			std::vector< ShadowMap::PassData > result;
			auto const width = ShadowMapPassDirectional::TextureSize;
			auto const height = ShadowMapPassDirectional::TextureSize;
			auto const w = float( width );
			auto const h = float( height );
			Viewport viewport{ engine };
			viewport.setOrtho( -w / 2, w / 2, -h / 2, h / 2, -5120.0_r, 5120.0_r );
			viewport.resize( { width, height } );
			viewport.update();

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( engine ),
					std::make_shared< Camera >( cuT( "ShadowMapDirectional_" ) + string::toString( cascade + 1, std::locale{ "C" } )
						, scene
						, scene.getCameraRootNode()
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

	ShadowMapDirectional::ShadowMapDirectional( Engine & engine
		, Scene & scene
		, uint32_t cascades )
		: ShadowMap{ engine
			, doInitialiseVariance( engine, Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize }, cascades )
			, doInitialiseLinearDepth( engine, Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize }, cascades )
			, createPasses( engine, scene, *this, cascades )
			, 1u }
		, m_frameBuffers( m_passes.size() )
		, m_cascades{ cascades }
	{
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

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			m_passes[cascade].pass->update( camera, queues, light, cascade );
		}
	}

	ashes::Semaphore const & ShadowMapDirectional::render( ashes::Semaphore const & toWait
		, uint32_t index )
	{
		static ashes::ClearColorValue const black{ 0.0f, 0.0f, 0.0f, 1.0f };
		static ashes::DepthStencilClearValue const zero{ 1.0f, 0 };
		auto & myTimer = m_passes[0].pass->getTimer();
		auto timerBlock = myTimer.start();

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			m_passes[cascade].pass->updateDeviceDependent( cascade );
		}

		m_commandBuffer->begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			auto & pass = m_passes[cascade];
			auto & timer = pass.pass->getTimer();
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_frameBuffers[cascade];

			timer.notifyPassRender();
			timer.beginPass( *m_commandBuffer );
			m_commandBuffer->beginRenderPass( pass.pass->getRenderPass()
				, *frameBuffer.frameBuffer
				, { zero, black, black }
				, ashes::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { pass.pass->getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			timer.endPass( *m_commandBuffer );
		}

		m_commandBuffer->end();
		auto & device = getCurrentDevice( *this );
		auto * result = &toWait;
		device.getGraphicsQueue().submit( *m_commandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
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

	void ShadowMapDirectional::debugDisplay( ashes::RenderPass const & renderPass
		, ashes::FrameBuffer const & frameBuffer
		, castor::Size const & size
		, uint32_t index )
	{
		Size displaySize{ 256u, 256u };
		Position position{ int32_t( displaySize.getWidth() * ( 0 + index * 2 ) ), int32_t( displaySize.getHeight() * 3u ) };
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

	void ShadowMapDirectional::doInitialiseDepth()
	{
		ashes::Extent2D const size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize };
		auto & device = getCurrentDevice( *this );

		ashes::ImageCreateInfo depth{};
		depth.extent.width = size.width;
		depth.extent.height = size.height;
		depth.arrayLayers = m_cascades;
		depth.imageType = ashes::TextureType::e2D;
		depth.usage = ashes::ImageUsageFlag::eDepthStencilAttachment;
		depth.format = ShadowMapDirectional::RawDepthFormat;
		m_depthTexture = device.createTexture( depth, ashes::MemoryPropertyFlag::eDeviceLocal );
		device.debugMarkerSetObjectName(
			{
				ashes::DebugReportObjectType::eImage,
				m_depthTexture.get(),
				"DirectionalShadowMapDepth"
			} );
	}

	void ShadowMapDirectional::doInitialiseFramebuffers()
	{
		ashes::Extent2D const size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize };
		auto & variance = m_shadowMap.getTexture()->getTexture();
		auto & linear = m_linearMap.getTexture()->getTexture();
		auto & depth = *m_depthTexture;

		ashes::ImageViewCreateInfo varianceView;
		varianceView.format = variance.getFormat();
		varianceView.viewType = ashes::TextureViewType::e2D;
		varianceView.subresourceRange.aspectMask = ashes::ImageAspectFlag::eColour;

		ashes::ImageViewCreateInfo linearView;
		linearView.format = linear.getFormat();
		linearView.viewType = ashes::TextureViewType::e2D;
		linearView.subresourceRange.aspectMask = ashes::ImageAspectFlag::eColour;

		ashes::ImageViewCreateInfo depthView;
		depthView.format = depth.getFormat();
		depthView.viewType = ashes::TextureViewType::e2D;
		depthView.subresourceRange.aspectMask = ashes::ImageAspectFlag::eDepth;

		for ( uint32_t cascade = 0u; cascade < m_passes.size(); ++cascade )
		{
			auto & pass = m_passes[cascade];
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_frameBuffers[cascade];
			depthView.subresourceRange.baseArrayLayer = cascade;
			varianceView.subresourceRange.baseArrayLayer = cascade;
			linearView.subresourceRange.baseArrayLayer = cascade;
			frameBuffer.depthView = depth.createView( depthView );
			frameBuffer.varianceView = variance.createView( varianceView );
			frameBuffer.linearView = linear.createView( linearView );
			ashes::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 0u ), *frameBuffer.depthView );
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 1u ), *frameBuffer.linearView );
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 2u ), *frameBuffer.varianceView );
			frameBuffer.frameBuffer = renderPass.createFrameBuffer( size, std::move( attaches ) );

			frameBuffer.blur = std::make_unique< GaussianBlur >( *getEngine()
				, *frameBuffer.varianceView
				, size
				, variance.getFormat()
				, 5u );
		}
	}

	void ShadowMapDirectional::doInitialise()
	{
		doInitialiseDepth();
		doInitialiseFramebuffers();
		m_commandBuffer = getCurrentDevice( *this ).getGraphicsCommandPool().createCommandBuffer();
	}

	void ShadowMapDirectional::doCleanup()
	{
		m_commandBuffer.reset();
		m_frameBuffers.clear();
		m_depthTexture.reset();
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
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declInput< IVec4 >( cuT( "bone_ids1" )
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declInput< Vec4 >( cuT( "weights0" )
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declInput< Vec4 >( cuT( "weights1" )
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( cuT( "transform" )
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( cuT( "material" )
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declInput< Vec4 >( cuT( "position2" )
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declInput< Vec3 >( cuT( "normal2" )
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declInput< Vec3 >( cuT( "tangent2" )
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declInput< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto in = writer.getIn();

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );

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

			if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
			{
				auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" )
					, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
			}
			else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" )
					, transform );
			}
			else
			{
				auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" )
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

	ShaderPtr ShadowMapDirectional::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// Fragment Intputs
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u );
		Ubo shadowMap{ writer, ShadowMapPassDirectional::ShadowMapUbo, ShadowMapPassDirectional::UboBindingPoint, 0u };
		auto c3d_farPlane( shadowMap.declMember< Float >( ShadowMapPassDirectional::FarPlane ) );
		shadowMap.end();

		auto vtx_viewPosition = writer.declInput< Vec3 >( cuT( "vtx_viewPosition" )
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declInput< UInt >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( cuT( "c3d_maps" )
			, MinTextureIndex
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );
		auto in = writer.getIn();

		auto materials = shader::createMaterials( writer, flags.passFlags );
		materials->declare( getEngine()->getRenderSystem()->getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };
		textureConfigs.declare( getEngine()->getRenderSystem()->getGpuInformations().hasShaderStorageBuffers() );

		// Fragment Outputs
		auto pxl_linear( writer.declOutput< Float >( cuT( "pxl_linear" ), 0u ) );
		auto pxl_variance( writer.declOutput< Vec2 >( cuT( "pxl_variance" ), 1u ) );
		
		shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth(), renderSystem.isInvertedNormals() };

		writer.implementFunction< sdw::Void >( cuT( "main" )
			, [&]()
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

				auto depth = writer.declLocale( cuT( "depth" )
					, in.gl_FragCoord.z() );
				pxl_linear = in.gl_FragCoord.z();
				pxl_variance.x() = depth;
				pxl_variance.y() = pxl_variance.x() * pxl_variance.x();

				auto dx = writer.declLocale( cuT( "dx" )
					, dFdx( depth ) );
				auto dy = writer.declLocale( cuT( "dy" )
					, dFdy( depth ) );
				pxl_variance.y() += 0.25_f * writer.paren( dx * dx + dy * dy );
			} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}
}
