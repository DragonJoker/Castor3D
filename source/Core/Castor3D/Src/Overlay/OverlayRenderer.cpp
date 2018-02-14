#include "OverlayRenderer.hpp"

#include "Engine.hpp"

#include "BorderPanelOverlay.hpp"
#include "PanelOverlay.hpp"

#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/Buffer.hpp>
#include <Buffer/GeometryBuffers.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <RenderPass/RenderPass.hpp>
#include <Shader/ShaderProgram.hpp>

#include <Graphics/Font.hpp>

#include <GlslSource.hpp>
#include "Shader/Shaders/GlslMaterial.hpp"

using namespace castor;

#if defined( drawText )
#	undef drawText
#endif

namespace castor3d
{
	static int32_t constexpr C3D_MAX_CHARS_PER_BUFFER = 6000;

	namespace
	{
		static uint32_t constexpr MatrixUboBinding = 1u;
		static uint32_t constexpr OverlayUboBinding = 2u;
		static uint32_t constexpr TextMapBinding = 3u;
		static uint32_t constexpr DiffuseMapBinding = 4u;
		static uint32_t constexpr OpacityMapBinding = 5u;

		uint32_t doFillBuffers( OverlayCategory::VertexArray::const_iterator begin
			, uint32_t count
			, renderer::VertexBuffer< OverlayCategory::Vertex > & vbo )
		{
			OverlayCategory::Vertex const & vertex = *begin;
			auto size = count * sizeof( OverlayCategory::Vertex );

			if ( auto buffer = vbo.getBuffer().lock( 0u
				, uint32_t( size )
				, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
			{
				std::memcpy( buffer, reinterpret_cast< uint8_t const * >( &vertex ), size );
				vbo.getBuffer().unlock( uint32_t( size ), true );
			}

			return count;
		}

		uint32_t doFillBuffers( TextOverlay::VertexArray::const_iterator begin
			, uint32_t count
			, renderer::VertexBuffer< TextOverlay::Vertex > & vbo )
		{
			TextOverlay::Vertex const & vertex = *begin;
			auto size = count * sizeof( TextOverlay::Vertex );

			if ( auto buffer = vbo.getBuffer().lock( 0u
				, uint32_t( size )
				, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
			{
				std::memcpy( buffer, reinterpret_cast< uint8_t const * >( &vertex ), size );
				vbo.getBuffer().unlock( uint32_t( size ), true );
			}

			return count;
		}
	}

	OverlayRenderer::OverlayRenderer( RenderSystem & renderSystem
		, renderer::TextureView const & target )
		: OwnedBy< RenderSystem >( renderSystem )
		, m_target{ target }
		, m_matrixUbo{ *renderSystem.getEngine() }
		, m_overlayUbo{ *renderSystem.getEngine() }
	{
	}

	OverlayRenderer::~OverlayRenderer()
	{
	}

	void OverlayRenderer::initialise()
	{
		auto & device = *getRenderSystem()->getCurrentDevice();

		if ( !m_commandBuffer )
		{
			m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		}

		if ( !m_declaration )
		{
			m_declaration = device.createVertexLayout( 0u, sizeof( OverlayCategory::Vertex ) );
			m_declaration->createAttribute< Point2f >( 0u
				, offsetof( OverlayCategory::Vertex, coords ) );
			m_declaration->createAttribute< Point2f >( 1u
				, offsetof( OverlayCategory::Vertex, texture ) );
		}

		if ( !m_textDeclaration )
		{
			m_textDeclaration = device.createVertexLayout( 0u, sizeof( TextOverlay::Vertex ) );
			m_textDeclaration->createAttribute< Point2f >( 0u
				, offsetof( TextOverlay::Vertex, coords ) );
			m_textDeclaration->createAttribute< Point2f >( 1u
				, offsetof( TextOverlay::Vertex, text ) );
			m_textDeclaration->createAttribute< Point2f >( 2u
				, offsetof( TextOverlay::Vertex, texture ) );
		}

		if ( !m_panelVertexBuffer )
		{
			// Panel Overlays buffers
			m_panelVertexBuffer = renderer::makeVertexBuffer< OverlayCategory::Vertex >( device
				, uint32_t( m_panelVertex.size() )
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
			m_panelGeometryBuffers.m_noTexture = device.createGeometryBuffers( *m_panelVertexBuffer, 0u, *m_declaration );
			m_panelGeometryBuffers.m_textured = device.createGeometryBuffers( *m_panelVertexBuffer, 0u, *m_declaration );
		}

		if ( !m_borderVertexBuffer )
		{
			// Border Overlays buffers
			m_panelVertexBuffer = renderer::makeVertexBuffer< OverlayCategory::Vertex >( device
				, uint32_t( m_panelVertex.size() )
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );

			{
				auto & pipeline = doGetPipeline( 0u, m_panelPipelines );
				m_borderGeometryBuffers.m_noTexture = device.createGeometryBuffers( *m_panelVertexBuffer, 0u, *m_declaration );
			}
			{
				auto & pipeline = doGetPipeline( uint32_t( TextureChannel::eDiffuse ), m_panelPipelines );
				m_borderGeometryBuffers.m_textured = device.createGeometryBuffers( *m_panelVertexBuffer, 0u, *m_declaration );
			}
		}

		// create one text overlays buffer
		doCreateTextGeometryBuffers();
	}

	void OverlayRenderer::cleanup()
	{
		m_matrixUbo.cleanup();
		m_panelPipelines.clear();
		m_textPipelines.clear();
		m_mapPanelNodes.clear();
		m_mapTextNodes.clear();

		if ( m_panelVertexBuffer )
		{
			m_panelGeometryBuffers.m_noTexture.reset();
			m_panelGeometryBuffers.m_textured.reset();
			m_panelVertexBuffer.reset();
		}

		if ( m_borderVertexBuffer )
		{
			m_borderGeometryBuffers.m_noTexture.reset();
			m_borderGeometryBuffers.m_textured.reset();
			m_borderVertexBuffer.reset();
		}

		m_textsGeometryBuffers.clear();
		m_textsVertexBuffers.clear();
		m_commandBuffer.reset();
	}

	void OverlayRenderer::drawPanel( PanelOverlay & overlay )
	{
		MaterialSPtr material = overlay.getMaterial();

		if ( material )
		{
			m_overlayUbo.setPosition( overlay.getAbsolutePosition()
				, m_size
				, overlay.getRenderRatio( m_size ) );
			doDrawItem( *m_commandBuffer
				, *material
				, *m_panelVertexBuffer
				, m_panelGeometryBuffers
				, doFillBuffers( overlay.getPanelVertex().begin()
					, uint32_t( overlay.getPanelVertex().size() )
					, *m_panelVertexBuffer ) );
		}
	}

	void OverlayRenderer::drawBorderPanel( BorderPanelOverlay & overlay )
	{
		{
			auto material = overlay.getMaterial();

			if ( material )
			{
				m_overlayUbo.setPosition( overlay.getAbsolutePosition()
					, m_size
					, overlay.getRenderRatio( m_size ) );
				doDrawItem( *m_commandBuffer
					, *material
					, *m_panelVertexBuffer
					, m_panelGeometryBuffers
					, doFillBuffers( overlay.getPanelVertex().begin()
						, uint32_t( overlay.getPanelVertex().size() )
						, *m_panelVertexBuffer ) );
			}
		}
		{
			auto material = overlay.getBorderMaterial();

			if ( material )
			{
				m_overlayUbo.setPosition( overlay.getAbsolutePosition()
					, m_size
					, overlay.getRenderRatio( m_size ) );
				doDrawItem( *m_commandBuffer
					, *material
					, *m_borderVertexBuffer
					, m_borderGeometryBuffers
					, doFillBuffers( overlay.getBorderVertex().begin()
						, uint32_t( overlay.getBorderVertex().size() )
						, *m_borderVertexBuffer ) );
			}
		}
	}

	void OverlayRenderer::drawText( TextOverlay & overlay )
	{
		FontSPtr pFont = overlay.getFontTexture()->getFont();

		if ( pFont )
		{
			MaterialSPtr material = overlay.getMaterial();

			if ( material )
			{
				TextOverlay::VertexArray arrayVtx = overlay.getTextVertex();
				int32_t count = uint32_t( arrayVtx.size() );
				uint32_t index = 0;
				std::vector< std::reference_wrapper< OverlayGeometryBuffers > > geometryBuffers;
				renderer::VertexBufferCRefArray vertexBuffers;
				TextOverlay::VertexArray::const_iterator it = arrayVtx.begin();

				while ( count > C3D_MAX_CHARS_PER_BUFFER )
				{
					geometryBuffers.emplace_back( doFillTextPart( count, it, index ) );
					vertexBuffers.emplace_back( *m_textsVertexBuffers[index] );
					++index;
					count -= C3D_MAX_CHARS_PER_BUFFER;
				}

				if ( count > 0 )
				{
					geometryBuffers.emplace_back( doFillTextPart( count, it, index ) );
					vertexBuffers.emplace_back( *m_textsVertexBuffers[index] );
				}

				auto texture = overlay.getFontTexture()->getTexture();
				auto sampler = overlay.getFontTexture()->getSampler();
				count = uint32_t( arrayVtx.size() );
				m_overlayUbo.setPosition( overlay.getAbsolutePosition()
					, m_size
					, overlay.getRenderRatio( m_size ) );

				for ( auto pass : *material )
				{
					auto itV = vertexBuffers.begin();

					if ( checkFlag( pass->getTextureFlags(), TextureChannel::eDiffuse ) )
					{
						for ( auto & geoBuffers : geometryBuffers )
						{
							doDrawItem( *m_commandBuffer
								, *pass
								, itV->get()
								, *geoBuffers.get().m_textured
								, *texture
								, *sampler
								, std::min( count, C3D_MAX_CHARS_PER_BUFFER ) );
							count -= C3D_MAX_CHARS_PER_BUFFER;
							++itV;
						}
					}
					else
					{
						for ( auto & geoBuffers : geometryBuffers )
						{
							doDrawItem( *m_commandBuffer
								, *pass
								, itV->get()
								, *geoBuffers.get().m_noTexture
								, *texture
								, *sampler
								, std::min( count, C3D_MAX_CHARS_PER_BUFFER ) );
							count -= C3D_MAX_CHARS_PER_BUFFER;
							++itV;
						}
					}
				}
			}
		}
	}

	void OverlayRenderer::beginRender( Viewport const & viewport )
	{
		auto size = viewport.getSize();

		if ( m_size != size )
		{
			m_sizeChanged = true;
			m_size = size;
		}

		m_matrixUbo.update( viewport.getProjection() );
	}

	void OverlayRenderer::endRender()
	{
		m_sizeChanged = false;
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::doGetPanelNode( Pass & pass )
	{
		auto it = m_mapPanelNodes.find( &pass );

		if ( it == m_mapPanelNodes.end() )
		{
			auto flags = pass.getTextureFlags();
			auto & pipeline = doGetPipeline( flags, m_panelPipelines );
			it = m_mapPanelNodes.insert( { &pass
				, OverlayRenderNode{ pipeline
					, pass
					, m_overlayUbo
					, doCreateDescriptorSet( pipeline
						, flags
						, pass )
				}
			} ).first;
		}

		return it->second;
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::doGetTextNode( Pass & pass
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto it = m_mapTextNodes.find( &pass );

		if ( it == m_mapTextNodes.end() )
		{
			auto flags = pass.getTextureFlags();
			addFlag( flags, TextureChannel::eText );
			auto & pipeline = doGetPipeline( flags, m_textPipelines );
			it = m_mapTextNodes.insert( { &pass
				, OverlayRenderNode{ pipeline
					, pass
					, m_overlayUbo
					, doCreateDescriptorSet( pipeline
						, flags
						, pass
						, texture
						, sampler )
				}
			} ).first;
		}

		return it->second;
	}

	renderer::DescriptorSetPtr OverlayRenderer::doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
		, TextureChannels textureFlags
		, Pass const & pass )
	{
		remFlag( textureFlags, TextureChannel::eNormal );
		remFlag( textureFlags, TextureChannel::eSpecular );
		remFlag( textureFlags, TextureChannel::eGloss );
		remFlag( textureFlags, TextureChannel::eHeight );
		remFlag( textureFlags, TextureChannel::eEmissive );
		auto result = pipeline.descriptorPool->createDescriptorSet();

		// Pass buffer
		getRenderSystem()->getEngine()->getMaterialCache().getPassBuffer().createBinding( *result
			, pipeline.descriptorLayout->getBinding( PassBufferIndex ) );
		// Matrix UBO
		result->createBinding( pipeline.descriptorLayout->getBinding( MatrixUboBinding )
			, m_matrixUbo.getUbo()
			, 0u
			, 1u );
		// Overlay UBO
		result->createBinding( pipeline.descriptorLayout->getBinding( OverlayUboBinding )
			, m_overlayUbo.getUbo()
			, 0u
			, 1u );

		if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
		{
			result->createBinding( pipeline.descriptorLayout->getBinding( DiffuseMapBinding )
				, pass.getTextureUnit( TextureChannel::eDiffuse )->getTexture()->getView()
				, pass.getTextureUnit( TextureChannel::eDiffuse )->getSampler()->getSampler() );
		}

		if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
		{
			result->createBinding( pipeline.descriptorLayout->getBinding( OpacityMapBinding )
				, pass.getTextureUnit( TextureChannel::eOpacity )->getTexture()->getView()
				, pass.getTextureUnit( TextureChannel::eOpacity )->getSampler()->getSampler() );
		}

		return result;
	}

	renderer::DescriptorSetPtr OverlayRenderer::doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
		, TextureChannels textureFlags
		, Pass const & pass
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto result = doCreateDescriptorSet( pipeline
			, textureFlags
			, pass );

		if ( checkFlag( textureFlags, TextureChannel::eText ) )
		{
			result->createBinding( pipeline.descriptorLayout->getBinding( TextMapBinding )
				, texture.getView()
				, sampler.getSampler() );
		}

		return result;
	}

	OverlayRenderer::Pipeline & OverlayRenderer::doGetPipeline( TextureChannels textureFlags
		, std::map< uint32_t, Pipeline > & pipelines )
	{
		// Remove unwanted flags
		remFlag( textureFlags, TextureChannel::eNormal );
		remFlag( textureFlags, TextureChannel::eSpecular );
		remFlag( textureFlags, TextureChannel::eGloss );
		remFlag( textureFlags, TextureChannel::eHeight );
		remFlag( textureFlags, TextureChannel::eEmissive );
		auto it = pipelines.find( textureFlags );

		if ( it == pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			auto & device = *getRenderSystem()->getCurrentDevice();
			auto & program = doCreateOverlayProgram( textureFlags );

			renderer::ColourBlendState blState{};
			blState.addAttachment( renderer::ColourBlendStateAttachment{
				true,
				renderer::BlendFactor::eSrcAlpha,
				renderer::BlendFactor::eInvSrcAlpha,
				renderer::BlendOp::eAdd,
				renderer::BlendFactor::eSrcAlpha,
				renderer::BlendFactor::eInvSrcAlpha,
				renderer::BlendOp::eAdd
			} );
			renderer::DescriptorSetLayoutBindingArray bindings;

			// Pass buffer
			bindings.emplace_back( getRenderSystem()->getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding() );
			// Matrix UBO
			bindings.emplace_back( MatrixUboBinding
				, renderer::DescriptorType::eUniformBuffer
				, renderer::ShaderStageFlag::eFragment );
			// Matrix UBO
			bindings.emplace_back( OverlayUboBinding
				, renderer::DescriptorType::eUniformBuffer
				, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment );

			if ( checkFlag( textureFlags, TextureChannel::eText ) )
			{
				bindings.emplace_back( TextMapBinding
					, renderer::DescriptorType::eCombinedImageSampler
					, renderer::ShaderStageFlag::eFragment );
			}

			if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
			{
				bindings.emplace_back( DiffuseMapBinding
					, renderer::DescriptorType::eCombinedImageSampler
					, renderer::ShaderStageFlag::eFragment );
			}

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				bindings.emplace_back( OpacityMapBinding
					, renderer::DescriptorType::eCombinedImageSampler
					, renderer::ShaderStageFlag::eFragment );
			}

			auto descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
			auto descriptorPool = descriptorLayout->createPool( 1000u );
			auto pipelineLayout = device.createPipelineLayout( *descriptorLayout );
			auto pipeline = pipelineLayout->createPipeline( program
				, { *m_declaration }
				, *m_renderPass
				, renderer::PrimitiveTopology::eTriangleStrip
				, renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eBack }
				, blState );
			pipeline->depthStencilState( renderer::DepthStencilState{ 0u, false, false } );
			pipeline->finish();
			it = pipelines.emplace( textureFlags
				, Pipeline{ std::move( descriptorLayout )
					, std::move( descriptorPool )
					, std::move( pipelineLayout )
					, std::move( pipeline )
				} ).first;
		}

		return it->second;
	}

	OverlayRenderer::OverlayGeometryBuffers & OverlayRenderer::doCreateTextGeometryBuffers()
	{
		auto & device = *getRenderSystem()->getCurrentDevice();
		auto vertexBuffer = renderer::makeVertexBuffer< TextOverlay::Vertex >( *getRenderSystem()->getCurrentDevice()
			, C3D_MAX_CHARS_PER_BUFFER
			, renderer::BufferTarget::eVertexBuffer | renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
		OverlayGeometryBuffers geometryBuffers;
		geometryBuffers.m_noTexture = device.createGeometryBuffers( *vertexBuffer, 0u, *m_textDeclaration );
		geometryBuffers.m_textured = device.createGeometryBuffers( *vertexBuffer, 0u, *m_textDeclaration );
		m_textsVertexBuffers.push_back( std::move( vertexBuffer ) );
		m_textsGeometryBuffers.push_back( std::move( geometryBuffers ) );
		return m_textsGeometryBuffers.back();
	}

	void OverlayRenderer::doDrawItem( renderer::CommandBuffer const & commandBuffer
		, Pass & pass
		, renderer::VertexBufferBase const & vertexBuffer
		, renderer::GeometryBuffers const & geometryBuffers
		, uint32_t count )
	{
		auto & node = doGetPanelNode( pass );
		m_overlayUbo.update( pass.getId() );
		commandBuffer.bindPipeline( *node.m_pipeline.pipeline );
		commandBuffer.bindDescriptorSet( *node.m_descriptorSet
			, *node.m_pipeline.pipelineLayout );
		commandBuffer.bindGeometryBuffers( geometryBuffers );
		commandBuffer.draw( count
			, 1u
			, 0u
			, 0u );
	}

	void OverlayRenderer::doDrawItem( renderer::CommandBuffer const & commandBuffer
		, Pass & pass
		, renderer::VertexBufferBase const & vertexBuffer
		, renderer::GeometryBuffers const & geometryBuffers
		, TextureLayout const & texture
		, Sampler const & sampler
		, uint32_t count )
	{
		auto & node = doGetTextNode( pass, texture, sampler );
		m_overlayUbo.update( pass.getId() );
		commandBuffer.bindPipeline( *node.m_pipeline.pipeline );
		commandBuffer.bindDescriptorSet( *node.m_descriptorSet
			, *node.m_pipeline.pipelineLayout );
		commandBuffer.bindGeometryBuffers( geometryBuffers );
		commandBuffer.draw( count
			, 1u
			, 0u
			, 0u );
	}

	void OverlayRenderer::doDrawItem( renderer::CommandBuffer const & commandBuffer
		, Material & material
		, renderer::VertexBufferBase const & vertexBuffer
		, OverlayRenderer::OverlayGeometryBuffers const & geometryBuffers
		, uint32_t count )
	{
		for ( auto pass : material )
		{
			if ( checkFlag( pass->getTextureFlags(), TextureChannel::eDiffuse ) )
			{
				doDrawItem( commandBuffer
					, *pass
					, vertexBuffer
					, *geometryBuffers.m_textured
					, count );
			}
			else
			{
				doDrawItem( commandBuffer
					, *pass
					, vertexBuffer
					, *geometryBuffers.m_noTexture
					, count );
			}
		}
	}

	OverlayRenderer::OverlayGeometryBuffers & OverlayRenderer::doFillTextPart( int32_t count
		, TextOverlay::VertexArray::const_iterator & it
		, uint32_t index )
	{
		if ( m_textsGeometryBuffers.size() <= index )
		{
			doCreateTextGeometryBuffers();
		}

		if ( m_textsGeometryBuffers.size() > index )
		{
			auto & vertexBuffer = m_textsVertexBuffers[index];
			count = std::min( count, C3D_MAX_CHARS_PER_BUFFER );
			doFillBuffers( it, count, *vertexBuffer );
			it += count;
			auto it = m_textsGeometryBuffers.begin();

			for ( auto i = 0u; i < index; ++i )
			{
				++it;
			}

			return *it;
		}

		static OverlayGeometryBuffers dummy;
		return dummy;
	}

	renderer::ShaderProgram & OverlayRenderer::doCreateOverlayProgram( TextureChannels const & textureFlags )
	{
		using namespace glsl;
		using namespace shader;

		// Shader program
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		renderer::ShaderProgram & program = cache.getNewProgram( false );

		// Vertex shader
		glsl::Shader vtx;
		{
			auto writer = getRenderSystem()->createGlslWriter();

			UBO_MATRIX( writer, MatrixUboBinding, 0u );
			UBO_OVERLAY( writer, OverlayUboBinding, 0u );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ) );
			auto text = writer.declAttribute< Vec2 >( cuT( "text" ), checkFlag( textureFlags, TextureChannel::eText ) );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ), checkFlag( textureFlags, TextureChannel::eDiffuse ) );

			// Shader outputs
			auto vtx_text = writer.declOutput< Vec2 >( cuT( "vtx_text" ), checkFlag( textureFlags, TextureChannel::eText ) );
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				if ( checkFlag( textureFlags, TextureChannel::eText ) )
				{
					vtx_text = text;
				}

				if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
				{
					vtx_texture = texture;
				}

				auto size = writer.declLocale( cuT( "size" )
					, c3d_renderRatio * c3d_renderSize );
				gl_Position = c3d_projection * vec4( size * writer.paren( c3d_position + position )
					, 0.0
					, 1.0 );
			} );

			vtx = writer.finalise();
		}

		// Pixel shader
		glsl::Shader pxl;
		{
			auto writer = getRenderSystem()->createGlslWriter();

			std::unique_ptr< Materials > materials;

			switch ( getRenderSystem()->getEngine()->getMaterialsType() )
			{
			case MaterialType::eLegacy:
				materials = std::make_unique< shader::LegacyMaterials >( writer );
				break;

			case MaterialType::ePbrMetallicRoughness:
				materials = std::make_unique< PbrMRMaterials >( writer );
				break;

			case MaterialType::ePbrSpecularGlossiness:
				materials = std::make_unique< PbrSGMaterials >( writer );
				break;
			}

			materials->declare();
			UBO_OVERLAY( writer, OverlayUboBinding, 0u );

			// Shader inputs
			auto vtx_text = writer.declInput< Vec2 >( cuT( "vtx_text" )
				, checkFlag( textureFlags, TextureChannel::eText ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" )
				, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto c3d_mapText = writer.declSampler< Sampler2D >( cuT( "c3d_mapText" )
				, TextMapBinding
				, 0u
				, checkFlag( textureFlags, TextureChannel::eText ) );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" )
				, DiffuseMapBinding
				, 0u
				, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto c3d_mapOpacity = writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
				, OpacityMapBinding
				, 0u
				, checkFlag( textureFlags, TextureChannel::eOpacity ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto material = materials->getBaseMaterial( c3d_materialIndex );
				auto diffuse = writer.declLocale( cuT( "diffuse" )
					, material->m_diffuse() );
				auto alpha = writer.declLocale( cuT( "alpha" )
					, material->m_opacity() );

				if ( checkFlag( textureFlags, TextureChannel::eText ) )
				{
					alpha *= texture( c3d_mapText, vtx_text ).r();
				}

				if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
				{
					diffuse = texture( c3d_mapDiffuse, vtx_texture ).xyz();
				}

				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, vtx_texture ).r();
				}

				pxl_fragColor = vec4( diffuse.xyz(), alpha );
			} );

			pxl = writer.finalise();
		}

		program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
		program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
		return program;
	}
}
