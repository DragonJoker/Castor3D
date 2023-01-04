#include "Castor3D/Overlay/OverlayRenderer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <RenderGraph/FramePassTimer.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, OverlayRenderer )

#pragma GCC diagnostic ignored "-Wclass-memaccess"

namespace castor3d
{
	//*********************************************************************************************

	namespace ovrlrend
	{
		enum class OverlayBindingId : uint32_t
		{
			eMaterials,
			eTexConfigs,
			eTexAnims,
			eMatrix,
			eOverlay,
			eTextMap,
		};

		template< typename VertexT, uint32_t CountT >
		static uint32_t doFillBuffers( typename std::vector< VertexT >::const_iterator begin
			, uint32_t count
			, OverlayRenderer::VertexBufferIndexT< VertexT, CountT > & bufferIndex )
		{
			auto & bufferOffset = bufferIndex.geometryBuffers.textured.bufferOffset;
			std::copy( begin
				, std::next( begin, count )
				, bufferOffset.template getData< VertexT >( SubmeshFlag::ePositions ).begin() );
			bufferOffset.markDirty( SubmeshFlag::ePositions );
			return count;
		}

		template< typename T >
		static void doUpdateUbo( UniformBufferOffsetT< T > & overlayUbo
			, OverlayCategory const & overlay
			, Pass const & pass
			, castor::Size const & size )
		{
			auto & data = overlayUbo.getData();
			auto position = overlay.getAbsolutePosition();
			auto ratio = overlay.getRenderRatio( size );
			data.positionRatio = castor::Point4f
			{
				position[0],
				position[1],
				ratio[0],
				ratio[1],
			};
			data.renderSizeIndex = castor::Point4i
			{
				size.getWidth(),
				size.getHeight(),
				pass.getId(),
				0,
			};
		}

		template< typename OverlayT, typename QuadT >
		struct BorderSizeGetter
		{
			castor::Rectangle operator()( OverlayT const & overlay, castor::Size const & size )const
			{
				return castor::Rectangle{};
			}
		};

		template<>
		struct BorderSizeGetter< BorderPanelOverlay, OverlayRenderer::BorderPanelVertexBufferPool::Quad >
		{
			castor::Rectangle operator()( BorderPanelOverlay const & overlay, castor::Size const & size )const
			{
				castor::Rectangle result = overlay.getAbsoluteBorderSize( size );

				switch ( overlay.getBorderPosition() )
				{
				case BorderPosition::eMiddle:
					result.set( result.left() / 2
						, result.top() / 2
						, result.right() / 2
						, result.bottom() / 2 );
					break;
				case BorderPosition::eExternal:
					break;
				default:
					result = castor::Rectangle{};
					break;
				}

				return result;
			}
		};

		enum class OverlayTexture : uint32_t
		{
			eNone = 0x00,
			eText = 0x01,
			eColour = 0x02,
			eOpacity = 0x04,
		};

		static uint32_t makeKey( PassComponentRegister const & passComponents
			, TextureCombine const & textures
			, bool text )
		{
			auto tex{ uint32_t( text ? OverlayTexture::eText : OverlayTexture::eNone ) };

			if ( hasAny( textures, passComponents.getColourMapFlags() ) )
			{
				tex |= uint32_t( OverlayTexture::eColour );
			}

			if ( hasAny( textures, passComponents.getOpacityMapFlags() ) )
			{
				tex |= uint32_t( OverlayTexture::eOpacity );
			}

			uint32_t result{ tex << 8 };
			result |= uint32_t( textures.configCount );
			return result;
		}

		template< ast::var::Flag FlagT >
		struct OverlaySurfaceT
			: public sdw::StructInstance
		{
			OverlaySurfaceT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: StructInstance{ writer, std::move( expr ), enabled }
				, position{ this->getMember< sdw::Vec2 >( "position", true ) }
				, texture{ this->getMember< sdw::Vec2 >( "texture", true ) }
				, text{ this->getMember< sdw::Vec2 >( "text", true ) }
			{
			}

			SDW_DeclStructInstance( , OverlaySurfaceT );

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, bool hasPosition
				, bool isTextOverlay
				, bool hasTextures )
			{
				auto result = cache.getStruct( ast::type::MemoryLayout::eC
					, "C3D_OverlaySurface" );

				if ( result->empty() )
				{
					result->declMember( "position", ast::type::Kind::eVec2F, ast::type::NotArray, hasPosition );
					result->declMember( "texture", ast::type::Kind::eVec2F, ast::type::NotArray, hasTextures );
					result->declMember( "text", ast::type::Kind::eVec2F, ast::type::NotArray, isTextOverlay );
				}

				return result;
			}

			static ast::type::IOStructPtr makeIOType( ast::type::TypesCache & cache
				, bool hasPosition
				, bool isTextOverlay
				, bool hasTextures )
			{
				auto result = cache.getIOStruct( ast::type::MemoryLayout::eC
					, "C3D_" + ( FlagT == sdw::var::Flag::eShaderOutput
						? std::string{ "Out" }
						: std::string{ "In" } ) + "OverlaySurface"
					, FlagT );

				if ( result->empty() )
				{
					result->declMember( "position", ast::type::Kind::eVec2F, ast::type::NotArray, 0u, hasPosition );
					result->declMember( "texture", ast::type::Kind::eVec2F, ast::type::NotArray, 1u, hasTextures );
					result->declMember( "text", ast::type::Kind::eVec2F, ast::type::NotArray, 2u, isTextOverlay );
				}

				return result;
			}

			sdw::Vec2 position;
			sdw::Vec2 texture;
			sdw::Vec2 text;
		};
		using OverlaySurface = OverlaySurfaceT< sdw::var::Flag::eNone >;

		struct OverlayBlendComponents
			: public shader::BlendComponents
		{
			OverlayBlendComponents( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: shader::BlendComponents{ writer
					, std::move( expr )
					, enabled }
			{
			}

			OverlayBlendComponents( shader::Materials const & materials )
				: OverlayBlendComponents{ *materials.getWriter()
					, makeInit( *materials.getWriter(), materials )
					, true }
			{
			}

			template< ast::var::Flag FlagT >
			OverlayBlendComponents( shader::Materials const & materials
				, shader::Material const & material
				, OverlaySurfaceT< FlagT > const & surface )
				: OverlayBlendComponents{ *materials.getWriter()
					, makeInit( *materials.getWriter(), materials, material, surface )
					, true }
			{
			}

			SDW_DeclStructInstance( , OverlayBlendComponents );

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, shader::Materials const & materials
				, sdw::expr::ExprList & inits )
			{
				auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_OverlayBlendComponents" );
				shader::BlendComponents::fillType( *result, materials, inits );
				return result;
			}

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, shader::Materials const & materials )
			{
				sdw::expr::ExprList inits;
				return makeType( cache, materials, inits );
			}

			template< ast::var::Flag FlagT >
			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, shader::Materials const & materials
				, shader::Material const & material
				, OverlaySurfaceT< FlagT > const & surface
				, sdw::expr::ExprList & inits )
			{
				auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_OverlayBlendComponents" );
				shader::BlendComponents::fillType( *result, materials, material, surface, nullptr, inits );
				return result;
			}

			template< ast::var::Flag FlagT >
			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, shader::Materials const & materials
				, shader::Material const & material
				, OverlaySurfaceT< FlagT > const & surface )
			{
				sdw::expr::ExprList inits;
				return makeType( cache, materials, material, surface, inits );
			}

		private:
			static sdw::expr::ExprPtr makeInit( sdw::ShaderWriter & writer
				, shader::Materials const & materials )
			{
				sdw::expr::ExprList initializers;
				auto type = OverlayBlendComponents::makeType( writer.getTypesCache(), materials, initializers );
				return sdw::makeAggrInit( type, std::move( initializers ) );
			}

			template< ast::var::Flag FlagT >
			static sdw::expr::ExprPtr makeInit( sdw::ShaderWriter & writer
				, shader::Materials const & materials
				, shader::Material const & material
				, OverlaySurfaceT< FlagT > const & surface )
			{
				sdw::expr::ExprList initializers;
				auto type = OverlayBlendComponents::makeType( writer.getTypesCache(), materials, material, surface, initializers );
				return sdw::makeAggrInit( type, std::move( initializers ) );
			}
		};
	}

	//*********************************************************************************************

	OverlayRenderer::Preparer::Preparer( OverlayRenderer & renderer
		, RenderDevice const & device
		, VkRenderPass renderPass
		, VkFramebuffer framebuffer )
		: m_renderer{ renderer }
		, m_device{ device }
		, m_renderPass{ renderPass }
	{
		m_renderer.doBeginPrepare( m_renderPass, framebuffer );
	}

	OverlayRenderer::Preparer::Preparer( Preparer && rhs )
		: m_renderer{ rhs.m_renderer }
		, m_device{ rhs.m_device }
		, m_renderPass{ rhs.m_renderPass }
	{
		rhs.m_renderPass = VkRenderPass{};
	}

	OverlayRenderer::Preparer & OverlayRenderer::Preparer::operator=( Preparer && rhs )
	{
		m_renderPass = rhs.m_renderPass;

		rhs.m_renderPass = VkRenderPass{};

		return *this;
	}

	OverlayRenderer::Preparer::~Preparer()
	{
		if ( m_renderPass )
		{
			m_renderer.doEndPrepare();
		}
	}

	void OverlayRenderer::Preparer::visit( PanelOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
				{
					doPrepareOverlay< PanelVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, m_renderer.m_panelOverlays
						, m_renderer.m_panelVertexBuffers
						, overlay.getPanelVertex()
						, nullptr );
				}
			}
		}
	}

	void OverlayRenderer::Preparer::visit( BorderPanelOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
				{
					doPrepareOverlay< PanelVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, m_renderer.m_panelOverlays
						, m_renderer.m_panelVertexBuffers
						, overlay.getPanelVertex()
						, nullptr );
				}
			}
		}

		if ( auto material = overlay.getBorderMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
				{
					doPrepareOverlay< BorderPanelVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, m_renderer.m_borderPanelOverlays
						, m_renderer.m_borderVertexBuffers
						, overlay.getBorderVertex()
						, nullptr );
				}
			}
		}
	}

	void OverlayRenderer::Preparer::visit( TextOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
				{
					doPrepareOverlay< TextVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, m_renderer.m_textOverlays
						, m_renderer.m_textVertexBuffers
						, overlay.getTextVertex()
						, overlay.getFontTexture() );
				}
			}
		}
	}

	//*********************************************************************************************

	namespace ovrlrend
	{
		template< typename VertexT, uint32_t CountT >
		OverlayRenderer::VertexBufferIndexT< VertexT, CountT > & getVertexBuffer( std::vector< std::unique_ptr< OverlayRenderer::VertexBufferPoolT< VertexT, CountT > > > & pools
			, std::map< size_t, OverlayRenderer::VertexBufferIndexT< VertexT, CountT > > & overlays
			, Overlay const & overlay
			, Pass const & pass
			, OverlayRenderer::OverlayRenderNode & node
			, Engine & engine
			, RenderDevice const & device
			, ashes::PipelineVertexInputStateCreateInfo const & noTexLayout
			, ashes::PipelineVertexInputStateCreateInfo const & texLayout
			, uint32_t maxCount )
		{
			auto hash = std::hash< Overlay const * >{}( &overlay );
			hash = castor::hashCombinePtr( hash, pass );
			auto it = overlays.find( hash );

			if ( it == overlays.end() )
			{
				for ( auto & pool : pools )
				{
					if ( it == overlays.end() )
					{
						auto result = pool->allocate( node );

						if ( bool( result ) )
						{
							it = overlays.emplace( hash, std::move( result ) ).first;
						}
					}
				}

				if ( it == overlays.end() )
				{
					pools.emplace_back( std::make_unique< OverlayRenderer::VertexBufferPoolT< VertexT, CountT > >( engine
						, getName( overlay.getType() )
						, *device.uboPool
						, device
						, noTexLayout
						, texLayout
						, maxCount ) );
					auto result = pools.back()->allocate( node );
					it = overlays.emplace( hash, std::move( result ) ).first;
				}
			}

			return it->second;
		}
	}

	//*********************************************************************************************

	template< typename QuadT, typename OverlayT, typename VertexT, uint32_t CountT >
	void OverlayRenderer::Preparer::doPrepareOverlay( RenderDevice const & device
		, OverlayT const & overlay
		, Pass const & pass
		, std::map< size_t, OverlayRenderer::VertexBufferIndexT< VertexT, CountT > > & overlays
		, std::vector< OverlayRenderer::VertexBufferPoolPtrT< VertexT, CountT > > & vertexBuffers
		, std::vector < VertexT > const & vertices
		, FontTextureSPtr fontTexture )
	{
		if ( !vertices.empty() )
		{
			auto & bufferIndex = ovrlrend::getVertexBuffer< VertexT, CountT >( vertexBuffers
				, overlays
				, overlay.getOverlay()
				, pass
				, ( fontTexture
					? m_renderer.doGetTextNode( device, m_renderPass, pass, *fontTexture->getTexture(), *fontTexture->getSampler().lock() )
					: m_renderer.doGetPanelNode( device, m_renderPass, pass ) )
				, *pass.getOwner()->getEngine()
				, device
				, ( fontTexture ? m_renderer.m_noTexTextDeclaration : m_renderer.m_noTexDeclaration )
				, ( fontTexture ? m_renderer.m_texTextDeclaration : m_renderer.m_texDeclaration )
				, MaxOverlayPanelsPerBuffer );
			ovrlrend::doUpdateUbo( bufferIndex.overlayUbo
				, overlay
				, pass
				, m_renderer.m_size );
			ovrlrend::doFillBuffers( vertices.begin()
				, uint32_t( vertices.size() )
				, bufferIndex );
			auto ptr = fontTexture ? fontTexture->getTexture().get() : nullptr;
			auto ires = bufferIndex.descriptorSets.emplace( ptr, OverlayDescriptorConnection{} );
			auto & descriptorConnection = ires.first->second;

			if ( ires.second || !descriptorConnection.descriptorSet )
			{
				if ( fontTexture )
				{
					descriptorConnection.descriptorSet = m_renderer.doCreateDescriptorSet( bufferIndex.node.pipeline
						, pass
						, bufferIndex.overlayUbo
						, bufferIndex.index
						, *fontTexture->getTexture()
						, *fontTexture->getSampler().lock() );
					OverlayRenderer & renderer = m_renderer;
					descriptorConnection.connection = fontTexture->onChanged.connect( [&renderer, &descriptorConnection]( DoubleBufferedTextureLayout const & )
						{
							renderer.m_retired.emplace_back( std::move( descriptorConnection.descriptorSet ) );
						} );
				}
				else
				{
					descriptorConnection.descriptorSet = m_renderer.doCreateDescriptorSet( bufferIndex.node.pipeline
						, pass
						, bufferIndex.overlayUbo
						, bufferIndex.index );
				}
			}

			auto borderSize = ovrlrend::BorderSizeGetter< OverlayT, QuadT >{}( overlay, m_renderer.m_size );
			auto borderOffset = castor::Size{ uint32_t( borderSize.left() ), uint32_t( borderSize.top() ) };
			auto borderExtent = borderOffset + castor::Size{ uint32_t( borderSize.right() ), uint32_t( borderSize.bottom() ) };
			auto position = overlay.getAbsolutePosition( m_renderer.m_size ) - borderOffset;
			position->x = std::max( 0, position->x );
			position->y = std::max( 0, position->y );
			auto size = overlay.getAbsoluteSize( m_renderer.m_size ) + borderExtent;
			size->x = std::max( 1u, size->x );
			size->y = std::max( 1u, size->y );
			ashes::CommandBuffer & commandBuffer = *m_renderer.m_commands.commandBuffer;
			commandBuffer.bindPipeline( *bufferIndex.node.pipeline.pipeline );
			commandBuffer.setViewport( makeViewport( m_renderer.m_size ) );
			commandBuffer.setScissor( makeScissor( position, size ) );
			commandBuffer.bindDescriptorSets( { *descriptorConnection.descriptorSet, *device.renderSystem.getEngine()->getTextureUnitCache().getDescriptorSet() }
				, *bufferIndex.node.pipeline.pipelineLayout );
			commandBuffer.bindVertexBuffer( 0u
				, bufferIndex.geometryBuffers.noTexture.bufferOffset.getBuffer( SubmeshFlag::ePositions )
				, bufferIndex.geometryBuffers.noTexture.bufferOffset.getOffset( SubmeshFlag::ePositions ) );
			commandBuffer.draw( uint32_t( vertices.size() )
				, 1u
				, 0u
				, 0u );
		}
	}

	//*********************************************************************************************

	template< typename VertexT, uint32_t CountT >
	OverlayRenderer::VertexBufferPoolT< VertexT, CountT >::VertexBufferPoolT( Engine & engine
		, std::string const & debugName
		, UniformBufferPool & uboPool
		, RenderDevice const & device
		, ashes::PipelineVertexInputStateCreateInfo const & noTexDecl
		, ashes::PipelineVertexInputStateCreateInfo const & texDecl
		, uint32_t count )
		: engine{ engine }
		, device{ device }
		, uboPool{ uboPool }
		, noTexDeclaration{ noTexDecl }
		, texDeclaration{ texDecl }
		, buffer{ castor::makeUnique< VertexBufferPool >( device, debugName ) }
	{
	}

	template< typename VertexT, uint32_t CountT >
	OverlayRenderer::VertexBufferIndexT< VertexT, CountT > OverlayRenderer::VertexBufferPoolT< VertexT, CountT >::allocate( OverlayRenderNode & node )
	{
		auto it = std::find_if( allocated.begin()
			, allocated.end()
			, []( ObjectBufferOffset const & lookup )
			{
				return lookup.getBufferChunk( SubmeshFlag::ePositions ).buffer == nullptr;
			} );

		if ( it == allocated.end() )
		{
			allocated.emplace_back( buffer->getBuffer< Quad >( 1u ) );
			it = std::next( allocated.begin(), ptrdiff_t( allocated.size() - 1u ) );
		}

		OverlayRenderer::VertexBufferIndexT< VertexT, CountT > result{ *this
			, node
			, uint32_t( std::distance( allocated.begin(), it ) ) };
		ObjectBufferOffset & offsets = *it;
		result.overlayUbo = uboPool.getBuffer< Configuration >( 0u );

		result.geometryBuffers.noTexture.bufferOffset = offsets;
		result.geometryBuffers.noTexture.layouts.emplace_back( noTexDeclaration );

		result.geometryBuffers.textured.bufferOffset = offsets;
		result.geometryBuffers.textured.layouts.emplace_back( texDeclaration );

		return result;
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayRenderer::VertexBufferPoolT< VertexT, CountT >::deallocate( OverlayRenderer::VertexBufferIndexT< VertexT, CountT > const & index )
	{
		CU_Require( &index.pool == this );
		auto it = std::find_if( allocated.begin()
			, allocated.end()
			, [&index]( auto const & lookup )
			{
				return lookup.first == index.index;
			} );
		CU_Require( it != allocated.end() );
		buffer->putBuffer( *it );
		*it = {};
		index.geometryBuffers.noTexture.bufferOffset = {};
		index.geometryBuffers.noTexture.layouts.clear();
		index.geometryBuffers.textured.bufferOffset = {};
		index.geometryBuffers.textured.layouts.clear();
		uboPool.putBuffer( index.overlayUbo );
		uboPool.putBuffer( index.texturesUbo );
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayRenderer::VertexBufferPoolT< VertexT, CountT >::upload( ashes::CommandBuffer const & cb )
	{
		buffer->upload( cb );
	}

	//*********************************************************************************************

	OverlayRenderer::OverlayRenderer( RenderDevice const & device
		, Texture const & target
		, VkCommandBufferLevel level )
		: OwnedBy< RenderSystem >( device.renderSystem )
		, m_uboPool{ *device.uboPool }
		, m_target{ target }
		, m_commands{ device, *device.graphicsData(), "OverlayRenderer", level }
		, m_noTexDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( OverlayCategory::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( OverlayCategory::Vertex, coords ) } } }
		, m_texDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( OverlayCategory::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( OverlayCategory::Vertex, coords ) }
				, { 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( OverlayCategory::Vertex, texture ) } } }
		, m_noTexTextDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( TextOverlay::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, coords ) }
				, { 2u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, text ) } } }
		, m_texTextDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( TextOverlay::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, coords ) }
				, { 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, texture ) }
				, { 2u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, text ) } } }
		, m_size{ makeSize( m_target.getExtent() ) }
		, m_matrixUbo{ device }
	{
		// Create one panel overlays buffer pool
		m_panelVertexBuffers.emplace_back( std::make_unique< PanelVertexBufferPool >( *getRenderSystem()->getEngine()
			, "PanelOverlays"
			, m_uboPool
			, device
			, m_noTexDeclaration
			, m_texDeclaration
			, MaxOverlayPanelsPerBuffer ) );

		// Create one border overlays buffer pool
		m_borderVertexBuffers.emplace_back( std::make_unique< BorderPanelVertexBufferPool >( *getRenderSystem()->getEngine()
			, "BorderPanelOverlays"
			, m_uboPool
			, device
			, m_noTexDeclaration
			, m_texDeclaration
			, MaxOverlayPanelsPerBuffer ) );

		// create one text overlays buffer
		m_textVertexBuffers.emplace_back( std::make_unique< TextVertexBufferPool >( *getRenderSystem()->getEngine()
			, "TextOverlays"
			, m_uboPool
			, device
			, m_noTexTextDeclaration
			, m_texTextDeclaration
			, MaxOverlayPanelsPerBuffer ) );

		m_matrixUbo.cpuUpdate( getRenderSystem()->getOrtho( 0.0f
			, float( m_size.getWidth() )
			, 0.0f
			, float( m_size.getHeight() )
			, -1.0f
			, 1.0f ) );
	}

	OverlayRenderer::~OverlayRenderer()
	{
		m_panelOverlays.clear();
		m_borderPanelOverlays.clear();
		m_textOverlays.clear();
		m_mapPanelNodes.clear();
		m_mapTextNodes.clear();
		m_panelPipelines.clear();
		m_textPipelines.clear();
		m_panelVertexBuffers.clear();
		m_borderVertexBuffers.clear();
		m_textVertexBuffers.clear();
		m_commands = {};
	}

	void OverlayRenderer::update( GpuUpdater & updater )
	{
		auto size = updater.camera->getSize();

		if ( m_size != size )
		{
			m_sizeChanged = true;
			m_size = size;
			m_matrixUbo.cpuUpdate( getRenderSystem()->getOrtho( 0.0f
				, float( m_size.getWidth() )
				, 0.0f
				, float( m_size.getHeight() )
				, -1.0f
				, 1.0f ) );
		}
	}

	OverlayRenderer::Preparer OverlayRenderer::beginPrepare( RenderDevice const & device
		, VkRenderPass renderPass
		, VkFramebuffer framebuffer )
	{
		return Preparer{ *this, device, renderPass, framebuffer };
	}

	void OverlayRenderer::upload( ashes::CommandBuffer const & cb )
	{
		for ( auto & pool : m_panelVertexBuffers )
		{
			pool->upload( cb );
		}

		for ( auto & pool : m_borderVertexBuffers )
		{
			pool->upload( cb );
		}

		for ( auto & pool : m_textVertexBuffers )
		{
			pool->upload( cb );
		}
	}

	void OverlayRenderer::doBeginPrepare( VkRenderPass renderPass
		, VkFramebuffer framebuffer )
	{
		m_retired.clear();
		m_commands.commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( renderPass
				, 0u
				, framebuffer
				, VK_FALSE
				, 0u
				, 0u ) );
	}

	void OverlayRenderer::doEndPrepare()
	{
		m_commands.commandBuffer->end();
		m_sizeChanged = false;
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::doGetPanelNode( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass )
	{
		auto it = m_mapPanelNodes.find( &pass );

		if ( it == m_mapPanelNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, renderPass, pass, m_panelPipelines, false );
			it = m_mapPanelNodes.insert( { &pass, OverlayRenderNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::doGetTextNode( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto it = m_mapTextNodes.find( &pass );

		if ( it == m_mapTextNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, renderPass, pass, m_textPipelines, true );
			it = m_mapTextNodes.insert( { &pass, OverlayRenderNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	ashes::DescriptorSetPtr OverlayRenderer::doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
		, Pass const & pass
		, UniformBufferOffsetT< Configuration > const & overlayUbo
		, uint32_t index
		, bool update )
	{
		auto result = pipeline.descriptorPool->createDescriptorSet( "OverlayRenderer" + castor::string::toString( index ) );

		// Pass buffer
		getRenderSystem()->getEngine()->getMaterialCache().getPassBuffer().createBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( ovrlrend::OverlayBindingId::eMaterials ) ) );
		// Textures buffers
		getRenderSystem()->getEngine()->getMaterialCache().getTexConfigBuffer().createBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( ovrlrend::OverlayBindingId::eTexConfigs ) ) );
		getRenderSystem()->getEngine()->getMaterialCache().getTexAnimBuffer().createBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( ovrlrend::OverlayBindingId::eTexAnims ) ) );
		// Matrix UBO
		m_matrixUbo.createSizedBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( ovrlrend::OverlayBindingId::eMatrix ) ) );
		// Overlay UBO
		overlayUbo.createSizedBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( ovrlrend::OverlayBindingId::eOverlay ) ) );

		if ( update )
		{
			result->update();
		}

		return result;
	}

	ashes::DescriptorSetPtr OverlayRenderer::doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
		, Pass const & pass
		, UniformBufferOffsetT< Configuration > const & overlayUbo
		, uint32_t index
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto result = doCreateDescriptorSet( pipeline
			, pass
			, overlayUbo
			, index
			, false );
		result->createBinding( pipeline.descriptorLayout->getBinding( uint32_t( ovrlrend::OverlayBindingId::eTextMap ) )
			, texture.getDefaultView().getSampledView()
			, sampler.getSampler() );
		result->update();
		return result;
	}

	OverlayRenderer::Pipeline OverlayRenderer::doCreatePipeline( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass
		, ashes::PipelineShaderStageCreateInfoArray program
		, TextureCombine const & texturesFlags
		, bool text )
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachments{ { VK_TRUE
			, VK_BLEND_FACTOR_SRC_ALPHA
			, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
			, VK_BLEND_OP_ADD
			, VK_BLEND_FACTOR_SRC_ALPHA
			, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
			, VK_BLEND_OP_ADD
			, defaultColorWriteMask } };
		ashes::PipelineColorBlendStateCreateInfo blState{ 0u
			, VK_FALSE
			, VK_LOGIC_OP_COPY
			, std::move( attachments ) };
		auto & materials = getRenderSystem()->getEngine()->getMaterialCache();
		ashes::VkDescriptorSetLayoutBindingArray bindings;

		bindings.emplace_back( materials.getPassBuffer().createLayoutBinding( uint32_t( ovrlrend::OverlayBindingId::eMaterials ) ) );
		bindings.emplace_back( materials.getTexConfigBuffer().createLayoutBinding( uint32_t( ovrlrend::OverlayBindingId::eTexConfigs ) ) );
		bindings.emplace_back( materials.getTexAnimBuffer().createLayoutBinding( uint32_t( ovrlrend::OverlayBindingId::eTexAnims ) ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( ovrlrend::OverlayBindingId::eMatrix )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( ovrlrend::OverlayBindingId::eOverlay )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto vertexLayout = ( texturesFlags.configCount == 0u
			? &m_noTexDeclaration
			: &m_texDeclaration );

		if ( text )
		{
			vertexLayout = ( texturesFlags.configCount == 0u
				? &m_noTexTextDeclaration
				: &m_texTextDeclaration );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( ovrlrend::OverlayBindingId::eTextMap )
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		auto texLayout = getOwner()->getEngine()->getTextureUnitCache().getDescriptorLayout();
		auto descriptorLayout = device->createDescriptorSetLayout( std::move( bindings ) );
		auto descriptorPool = descriptorLayout->createPool( 1000u );
		auto pipelineLayout = device->createPipelineLayout( ashes::DescriptorSetLayoutCRefArray{ *descriptorLayout, *texLayout } );
		auto pipeline = device->createPipeline( { 0u
			, std::move( program )
			, *vertexLayout
			, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST }
			, ashes::nullopt
			, ashes::PipelineViewportStateCreateInfo{}
			, ashes::PipelineRasterizationStateCreateInfo{}
			, ashes::PipelineMultisampleStateCreateInfo{}
			, ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE }
			, std::move( blState )
			, ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } }
			, *pipelineLayout
			, renderPass } );
		return Pipeline{ std::move( descriptorLayout )
			, std::move( descriptorPool )
			, std::move( pipelineLayout )
			, std::move( pipeline ) };
	}

	OverlayRenderer::Pipeline & OverlayRenderer::doGetPipeline( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass
		, std::map< uint32_t, Pipeline > & pipelines
		, bool text )
	{
		// Remove unwanted flags
		auto & passComponents = device.renderSystem.getEngine()->getPassComponentsRegister();
		auto textures = passComponents.filterTextureFlags( ComponentModeFlag::eColour | ComponentModeFlag::eOpacity
			, pass.getTexturesMask() );
		auto key = ovrlrend::makeKey( passComponents, textures, text );
		auto it = pipelines.find( key );

		if ( it == pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			it = pipelines.emplace( key
				, doCreatePipeline( device
					, renderPass
					, pass
					, doCreateOverlayProgram( device, textures, text )
					, textures
					, text ) ).first;
		}

		return it->second;
	}

	ashes::PipelineShaderStageCreateInfoArray OverlayRenderer::doCreateOverlayProgram( RenderDevice const & device
		, TextureCombine const & texturesFlags
		, bool textOverlay )
	{
		using namespace sdw;
		using namespace shader;
		bool hasTexture = texturesFlags.configCount != 0u;

		// Vertex shader
		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "Overlay" };
		{
			VertexWriter writer;

			C3D_Matrix( writer
				, ovrlrend::OverlayBindingId::eMatrix
				, 0u );
			C3D_Overlay( writer
				, ovrlrend::OverlayBindingId::eOverlay
				, 0u );

			writer.implementMainT< ovrlrend::OverlaySurfaceT, ovrlrend::OverlaySurfaceT >( VertexInT< ovrlrend::OverlaySurfaceT >{ writer, true, textOverlay, hasTexture }
				, VertexOutT< ovrlrend::OverlaySurfaceT >{ writer, false, textOverlay, hasTexture }
				, [&]( VertexInT< ovrlrend::OverlaySurfaceT > in
					, VertexOutT< ovrlrend::OverlaySurfaceT > out )
				{
					out.texture = in.texture;
					out.text = in.text;
					auto size = writer.declLocale( "size"
						, c3d_overlayData.getOverlaySize() );
					out.vtx.position = c3d_matrixData.viewToProj( vec4( size * c3d_overlayData.modelToView( in.position )
							, 0.0_f
							, 1.0_f ) );
					out.position = out.vtx.position.xy();
				} );

			vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		// Pixel shader
		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "Overlay" };
		{
			FragmentWriter writer;

			shader::Utils utils{ writer };
			shader::PassShaders passShaders{ getOwner()->getEngine()->getPassComponentsRegister()
				, texturesFlags
				, ( ComponentModeFlag::eOpacity
					| ComponentModeFlag::eColour )
				, utils };
			shader::Materials materials{ writer
				, passShaders
				, uint32_t( ovrlrend::OverlayBindingId::eMaterials )
				, 0u };
			shader::TextureConfigurations textureConfigs{ writer
				, uint32_t( ovrlrend::OverlayBindingId::eTexConfigs )
				, 0u
				, hasTexture };
			shader::TextureAnimations textureAnims{ writer
				, uint32_t( ovrlrend::OverlayBindingId::eTexAnims )
				, 0u
				, hasTexture };

			C3D_Overlay( writer
				, ovrlrend::OverlayBindingId::eOverlay
				, 0u );

			auto c3d_mapText = writer.declCombinedImg< FImg2DR32 >( "c3d_mapText"
				, uint32_t( ovrlrend::OverlayBindingId::eTextMap )
				, 0u
				, textOverlay );

			auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
				, 0u
				, 1u
				, hasTexture ) );

			// Shader outputs
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0 );

			writer.implementMainT< ovrlrend::OverlaySurfaceT, VoidT >( FragmentInT< ovrlrend::OverlaySurfaceT >{ writer, false, textOverlay, hasTexture }
				, FragmentOut{ writer }
					, [&]( FragmentInT< ovrlrend::OverlaySurfaceT > in
						, FragmentOut out )
				{
					auto material = writer.declLocale( "material"
						, materials.getMaterial( c3d_overlayData.getMaterialIndex() ) );

					if ( textOverlay )
					{
						material.opacity *= c3d_mapText.sample( in.text, 0.0_f );
					}

					auto outComponents = writer.declLocale< ovrlrend::OverlayBlendComponents >( "outComponents"
						, ovrlrend::OverlayBlendComponents{ materials
							, material
							, in } );

					if ( hasTexture )
					{
						textureConfigs.computeMapsContributions( passShaders
							, texturesFlags
							, textureAnims
							, c3d_maps
							, material
							, outComponents );
					}

					outColour = vec4( outComponents.colour, outComponents.opacity );
				} );

			pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		return ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( device, vtx ),
			makeShaderState( device, pxl ),
		};
	}

	//*********************************************************************************************
}
