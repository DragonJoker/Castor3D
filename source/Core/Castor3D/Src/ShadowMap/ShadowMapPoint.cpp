#include "ShadowMapPoint.hpp"

#include "Engine.hpp"
#include "Cache/SamplerCache.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/Culling/DummyCuller.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Castor3DPrerequisites.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Program.hpp"
#include "ShadowMap/ShadowMapPassPoint.hpp"
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
			String const name = cuT( "ShadowMap_Point_Shadow" );
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
			image.flags = ashes::ImageCreateFlag::eCubeCompatible;
			image.arrayLayers = 6u * shader::PointShadowMapCount;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
			image.format = ShadowMapPoint::VarianceFormat;

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
			String const name = cuT( "ShadowMap_Point_Depth" );
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
			image.flags = ashes::ImageCreateFlag::eCubeCompatible;
			image.arrayLayers = 6u * shader::PointShadowMapCount;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
			image.format = ShadowMapPoint::LinearDepthFormat;

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

			for ( auto i = 0u; i < 6u * shader::PointShadowMapCount; ++i )
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

	ShadowMapPoint::ShadowMapPoint( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, doInitialiseVariance( engine, Size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize } )
			, doInitialiseLinearDepth( engine, Size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize } )
			, createPasses( engine, scene, *this )
			, shader::PointShadowMapCount }
	{
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

	ashes::Semaphore const & ShadowMapPoint::render( ashes::Semaphore const & toWait
		, uint32_t index )
	{
		static float constexpr component = std::numeric_limits< float >::max();
		static ashes::ClearColorValue const white{ component, component, component, component };
		static ashes::DepthStencilClearValue const zero{ 1.0f, 0 };
		auto & myTimer = m_passes[0].pass->getTimer();
		auto timerBlock = myTimer.start();
		auto * result = &toWait;
		auto offset = index * 6u;

		for ( size_t face = offset; face < offset + 6u; ++face )
		{
			m_passes[face].pass->updateDeviceDependent( uint32_t( face - offset ) );
		}

		auto & commandBuffer = *m_passesData[index].commandBuffer;
		auto & finished = *m_passesData[index].finished;
		commandBuffer.begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );

		for ( size_t face = 0u; face < 6u; ++face )
		{
			auto & pass = m_passes[face + ( index * 6u )];
			auto & timer = pass.pass->getTimer();
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_passesData[index].frameBuffers[face];

			timer.notifyPassRender();
			timer.beginPass( commandBuffer );
			commandBuffer.beginRenderPass( renderPass
				, *frameBuffer.frameBuffer
				, { zero, white, white }
				, ashes::SubpassContents::eSecondaryCommandBuffers );
			commandBuffer.executeCommands( { pass.pass->getCommandBuffer() } );
			commandBuffer.endRenderPass();
			timer.endPass( commandBuffer );
		}

		commandBuffer.end();
		auto & device = getCurrentDevice( *this );

		device.getGraphicsQueue().submit( commandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, finished
			, nullptr );
		result = &finished;

		return *result;
	}

	void ShadowMapPoint::debugDisplay( ashes::RenderPass const & renderPass
		, ashes::FrameBuffer const & frameBuffer
		, castor::Size const & size, uint32_t index )
	{
		//Size displaySize{ 128u, 128u };
		//Position position{ int32_t( displaySize.getWidth() * 4 * index), int32_t( displaySize.getHeight() * 4 ) };
		//getEngine()->getRenderSystem()->getCurrentDevice()->renderVarianceCube( position
		//	, displaySize
		//	, *m_shadowMap.getTexture() );
		//position = Position{ int32_t( displaySize.getWidth() * 4 * ( index + 2 ) ), int32_t( displaySize.getHeight() * 4 ) };
		//getEngine()->getRenderSystem()->getCurrentDevice()->renderDepthCube( position
		//	, displaySize
		//	, *m_linearMap.getTexture() );
	}

	ashes::TextureView const & ShadowMapPoint::getView( uint32_t index )const
	{
		return m_passesData[index].shadowType == ShadowType::eVariance
			? *m_passesData[index].varianceView
			: *m_passesData[index].linearView;
	}

	void ShadowMapPoint::doInitialise()
	{
		ashes::Extent2D size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize };
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
		depth.format = ShadowMapPoint::RawDepthFormat;

		ashes::ImageViewCreateInfo view;
		view.format = depth.format;
		view.viewType = ashes::TextureViewType::e2D;
		view.subresourceRange.aspectMask = ashes::ImageAspectFlag::eDepth;
		view.subresourceRange.baseArrayLayer = 0u;
		view.subresourceRange.layerCount = 1u;
		view.subresourceRange.baseMipLevel = 0u;
		view.subresourceRange.levelCount = 1u;

		for ( auto i = 0u; i < shader::PointShadowMapCount; ++i )
		{
			auto & variance = m_shadowMap.getTexture()->getTexture();
			auto & linear = m_linearMap.getTexture()->getTexture();
			uint32_t face = i * 6u;
			PassData data;
			data.depthTexture = device.createTexture( depth, ashes::MemoryPropertyFlag::eDeviceLocal );
			data.depthView = data.depthTexture->createView( view );
			data.varianceView = variance.createView( ashes::TextureViewType::eCube
				, variance.getFormat()
				, 0u
				, 1u
				, face
				, 6u );
			data.linearView = linear.createView( ashes::TextureViewType::eCube
				, linear.getFormat()
				, 0u
				, 1u
				, face
				, 6u );

			for ( auto & frameBuffer : data.frameBuffers )
			{
				auto & pass = m_passes[face];
				auto & renderPass = pass.pass->getRenderPass();
				frameBuffer.varianceView = variance.createView( ashes::TextureViewType::e2D
					, variance.getFormat()
					, 0u
					, 1u
					, face
					, 1u );
				frameBuffer.linearView = linear.createView( ashes::TextureViewType::e2D
					, linear.getFormat()
					, 0u
					, 1u
					, face
					, 1u );
				ashes::FrameBufferAttachmentArray attaches;
				attaches.emplace_back( *( renderPass.getAttachments().begin() + 0u ), *data.depthView );
				attaches.emplace_back( *( renderPass.getAttachments().begin() + 1u ), *frameBuffer.linearView );
				attaches.emplace_back( *( renderPass.getAttachments().begin() + 2u ), *frameBuffer.varianceView );
				frameBuffer.frameBuffer = renderPass.createFrameBuffer( size, std::move( attaches ) );
				++face;
			}

			data.commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
			data.finished = device.createSemaphore();
			m_passesData.emplace_back( std::move( data ) );
		}
	}

	void ShadowMapPoint::doCleanup()
	{
		m_passesData.clear();
	}

	void ShadowMapPoint::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapPoint );
	}

	ShaderPtr ShadowMapPoint::doGetVertexShaderSource( PassFlags const & passFlags
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
		auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" )
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
				vtx_material = material;
			}
			else
			{
				vtx_material = c3d_materialIndex;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.declLocale( cuT( "time" ), 1.0_f - c3d_time );
				vertexPosition = vec4( vertexPosition.xyz() * time + position2.xyz() * c3d_time, 1.0 );
				vtx_texture = vtx_texture * writer.paren( 1.0_f - c3d_time ) + texture2 * c3d_time;
			}

			auto mtxModel = writer.getVariable< Mat4 >( cuT( "mtxModel" ) );
			vertexPosition = mtxModel * vertexPosition;
			vtx_worldPosition = vertexPosition.xyz();
			vertexPosition = c3d_curView * vertexPosition;
			out.gl_out.gl_Position = c3d_projection * vertexPosition;
		};

		writer.implementFunction< sdw::Void >( cuT( "main" ), main );
		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPoint::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		using namespace sdw;
		FragmentWriter writer;

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapPassPoint::ShadowMapUbo, ShadowMapPassPoint::UboBindingPoint, 0u };
		auto c3d_wordLightPosition( shadowMap.declMember< Vec3 >( ShadowMapPassPoint::WorldLightPosition ) );
		auto c3d_farPlane( shadowMap.declMember< Float >( ShadowMapPassPoint::FarPlane ) );
		shadowMap.end();

		auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_mapOpacity( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapOpacity" )
			, MinBufferIndex
			, 1u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );

		auto materials = shader::createMaterials( writer, passFlags );
		materials->declare( getEngine()->getRenderSystem()->getGpuInformations().hasShaderStorageBuffers() );

		// Fragment Outputs
		auto pxl_linear = writer.declOutput< Float >( cuT( "pxl_linear" ), 0u );
		auto pxl_variance = writer.declOutput< Vec2 >( cuT( "pxl_variance" ), 1u );

		auto main = [&]()
		{
			doDiscardAlpha( writer
				, textureFlags
				, alphaFunc
				, vtx_material
				, *materials );

			auto depth = writer.declLocale( cuT( "depth" )
				, length( vtx_worldPosition - c3d_wordLightPosition ) );
			pxl_linear = depth / c3d_farPlane;
			pxl_variance.x() = pxl_linear;
			pxl_variance.y() = pxl_linear * pxl_linear;

			auto dx = writer.declLocale( cuT( "dx" )
				, dFdx( pxl_linear ) );
			auto dy = writer.declLocale( cuT( "dy" )
				, dFdy( pxl_linear ) );
			pxl_variance.y() += 0.25_f * writer.paren( dx * dx + dy * dy );
		};

		writer.implementFunction< sdw::Void >( cuT( "main" ), main );
		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}
}
