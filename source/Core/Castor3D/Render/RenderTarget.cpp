#include "Castor3D/Render/RenderTarget.hpp"

#include "Castor3D/Config.hpp"
#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Debug/DebugDrawer.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Overlays/OverlayPass.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"
#include "Castor3D/Render/PostEffect/PostEffect.hpp"
#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <RenderGraph/FramePassTimer.hpp>
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

CU_ImplementSmartPtr( castor3d, RenderTarget )

namespace castor3d
{
	namespace rendtgt
	{
		enum class CombineIdx
		{
			Scene = 0u,
			Overlays = 1u,
			HdrConfig = 2u,
		};

		class IntermediatesLister
			: public RenderTechniqueVisitor
		{
			struct ImageViewSlice
			{
				ImageViewSlice( VkImageViewCreateInfo info
					, uint32_t slice )
					: info{ castor::move( info ) }
					, slice{ slice }
				{
				}

				VkImageViewCreateInfo info;
				uint32_t slice;
			};

			struct ImageViewSliceComp
			{
				bool operator()( ImageViewSlice const & lhs
					, ImageViewSlice const & rhs )const
				{
					return ( lhs.info.image < rhs.info.image
						|| ( lhs.info.image == rhs.info.image
							&& ( lhs.info.subresourceRange.baseArrayLayer < rhs.info.subresourceRange.baseArrayLayer
								|| ( lhs.info.subresourceRange.baseArrayLayer == rhs.info.subresourceRange.baseArrayLayer
									&& ( lhs.info.subresourceRange.baseMipLevel < rhs.info.subresourceRange.baseMipLevel
										|| ( lhs.info.subresourceRange.baseMipLevel == rhs.info.subresourceRange.baseMipLevel
											&& ( lhs.info.subresourceRange.layerCount < rhs.info.subresourceRange.layerCount
												|| ( lhs.info.subresourceRange.layerCount == rhs.info.subresourceRange.layerCount
													&& ( lhs.info.subresourceRange.levelCount < rhs.info.subresourceRange.levelCount
														|| ( lhs.info.subresourceRange.levelCount == rhs.info.subresourceRange.levelCount
															&& ( lhs.info.viewType < rhs.info.viewType
																|| ( lhs.info.viewType == rhs.info.viewType
																	&& ( lhs.info.format < rhs.info.format
																		|| ( lhs.info.format == rhs.info.format
																			&& lhs.slice < rhs.slice ) ) ) ) ) ) ) ) ) ) ) ) ) );

				}
			};
			using ImageViewCache = castor::Set< ImageViewSlice, ImageViewSliceComp >;

		public:
			static void submit( Scene const & scene
				, RenderTechnique & technique
				, IntermediateViewArray & intermediates )
			{
				ImageViewCache cache;

				PipelineFlags flags{ PassComponentCombine{}
					, SubmeshComponentCombine{}
					, scene.getDefaultLightingModel()
					, scene.getBackgroundModelId()
					, ProgramFlag::eNone
					, TextureCombine{}
					, ShaderFlag::eNone
					, VK_COMPARE_OP_ALWAYS };
				IntermediatesLister visOpaque{ flags, scene, cache, intermediates };
				technique.accept( visOpaque );

				flags.m_shaderFlags |= ShaderFlag::eOpacity;
				addFlags( flags.pass, scene.getEngine()->getPassComponentsRegister().getAlphaBlendingFlag() );
				flags.alphaFunc = VK_COMPARE_OP_LESS;
				IntermediatesLister visTransparent{ flags, scene, cache, intermediates };
				technique.accept( visTransparent );
			}

			template< typename ValueT >
			static void submit( Scene const & scene
				, ValueT & value
				, IntermediateViewArray & intermediates )
			{
				ImageViewCache cache;

				PipelineFlags flags{ PassComponentCombine{}
					, SubmeshComponentCombine{}
					, scene.getDefaultLightingModel()
					, scene.getBackgroundModelId()
					, ProgramFlag::eNone
					, TextureCombine{}
					, ShaderFlag::eNone
					, VK_COMPARE_OP_ALWAYS };
				IntermediatesLister vis{ flags, scene, cache, intermediates };
				value.accept( vis );
			}

		protected:
			castor::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( castor::String const & category )override
			{
				return castor::RawUniquePtr< ConfigurationVisitorBase >( new IntermediatesLister{ getFlags(), getScene(), m_cache, m_result } );
			}

		private:
			IntermediatesLister( PipelineFlags const & flags
				, Scene const & scene
				, ImageViewCache & cache
				, IntermediateViewArray & result )
				: RenderTechniqueVisitor{ flags, scene, { false } }
				, m_handler{ scene.getEngine()->getGraphResourceHandler() }
				, m_result{ result }
				, m_cache{ cache }
			{
			}

			void doVisit3D( castor::String const & name
				, crg::ImageViewId viewId
				, VkImageLayout layout
				, TextureFactors const & factors )
			{
				auto info = viewId.data->info;
				info.image = VkImage( uint64_t( viewId.data->image.id ) );

				if ( factors.isSlice )
				{
					m_cache.emplace( info, factors.slice );
					m_result.emplace_back( name
						, viewId
						, layout
						, factors );
				}
				else
				{
					m_cache.emplace( info, 0u );
					m_result.emplace_back( name
						, viewId
						, layout
						, factors );
				}
			}

			void doVisit2DArray( castor::String const & name
				, crg::ImageViewId viewId
				, VkImageLayout layout
				, TextureFactors const & factors )
			{
				auto info = viewId.data->info;
				info.image = VkImage( uint64_t( viewId.data->image.id ) );

				if ( info.viewType == VK_IMAGE_VIEW_TYPE_2D_ARRAY
					|| info.viewType == VK_IMAGE_VIEW_TYPE_1D_ARRAY
					|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE
					|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY )
				{
					auto layerInfo = info;
					layerInfo.subresourceRange.layerCount = 1u;

					for ( uint32_t layerIdx = 0u; layerIdx < info.subresourceRange.layerCount; ++layerIdx )
					{
						auto layer = layerIdx + info.subresourceRange.baseArrayLayer;

						if ( auto layerViewId = m_handler.createViewId( crg::ImageViewData{ viewId.data->name + castor::string::toMbString( layer )
								, viewId.data->image
								, layerInfo.flags
								, layerInfo.viewType
								, layerInfo.format
								, layerInfo.subresourceRange } );
							doFilter( layerViewId, {} ) )
						{
							doVisit( name + castor::string::toString( layer )
								, layerViewId
								, layout
								, factors );
						}

						layerInfo.subresourceRange.baseArrayLayer++;
					}
				}
			}

			void doVisit( castor::String const & name
				, crg::ImageViewId viewId
				, VkImageLayout layout
				, TextureFactors const & factors )override
			{
				auto info = viewId.data->info;

				if ( ( info.viewType == VK_IMAGE_VIEW_TYPE_2D )
					&& ( info.subresourceRange.layerCount == 1u ) )
				{
					info.image = VkImage( uint64_t( viewId.data->image.id ) );
					m_cache.emplace( info, 0u );
					m_result.emplace_back( name
						, viewId
						, layout
						, factors );
				}
				else if ( info.viewType == VK_IMAGE_VIEW_TYPE_3D )
				{
					doVisit3D( name, viewId, layout, factors );
				}
				else
				{
					doVisit2DArray( name, viewId, layout, factors );
				}
			}

			bool doFilterArray( crg::ImageViewId const & viewId
				, TextureFactors const & factors )const
			{
				bool result = false;

				if ( auto & info = viewId.data->info;
					info.viewType == VK_IMAGE_VIEW_TYPE_2D_ARRAY
						|| info.viewType == VK_IMAGE_VIEW_TYPE_1D_ARRAY
						|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE
						|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY )
				{
					auto layerInfo = info;
					layerInfo.subresourceRange.layerCount = 1u;

					for ( uint32_t layerIdx = 0u; layerIdx < info.subresourceRange.layerCount; ++layerIdx )
					{
						auto layer = layerIdx + info.subresourceRange.baseArrayLayer;
						result = doFilter( m_handler.createViewId( crg::ImageViewData{ viewId.data->name + castor::string::toMbString( layer )
								, viewId.data->image
								, layerInfo.flags
								, layerInfo.viewType
								, layerInfo.format
								, layerInfo.subresourceRange } )
							, factors ) && result;
						layerInfo.subresourceRange.baseArrayLayer++;
					}
				}

				return result;
			}

			bool doFilter( crg::ImageViewId const & viewId
				, TextureFactors const & factors )const override
			{
				auto info = viewId.data->info;

				if ( info.viewType == VK_IMAGE_VIEW_TYPE_3D )
				{
					info.image = VkImage( uint64_t( viewId.data->image.id ) );
					return m_cache.end() == m_cache.find( { info, factors.slice } );
				}

				if ( info.subresourceRange.levelCount > 1u )
				{
					return false;
				}

				if ( info.viewType == VK_IMAGE_VIEW_TYPE_2D
					&& info.subresourceRange.layerCount == 1u )
				{
					info.image = VkImage( uint64_t( viewId.data->image.id ) );
					return m_cache.end() == m_cache.find( { info, 0u } );
				}

				return doFilterArray( viewId, factors );
			}

		private:
			crg::ResourceHandler & m_handler;
			IntermediateViewArray & m_result;
			ImageViewCache & m_cache;
		};

		static VkImageUsageFlags constexpr objectsUsageFlags = ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
			| VK_IMAGE_USAGE_SAMPLED_BIT
			| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
			| VK_IMAGE_USAGE_TRANSFER_DST_BIT
			| VK_IMAGE_USAGE_STORAGE_BIT );

		static CU_ImplementAttributeParserBlock( parserScene, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
				ScenePtrStrMap::iterator it = getRootContext( *blockContext )->mapScenes.find( name );

				if ( it != getRootContext( *blockContext )->mapScenes.end() )
				{
					blockContext->renderTarget->setScene( *it->second );
				}
				else
				{
					CU_ParsingError( cuT( "No scene found with name : [" ) + name + cuT( "]." ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCamera, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->renderTarget->getScene() )
				{
					auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );

					if ( auto camera = blockContext->renderTarget->getScene()->findCamera( name ) )
					{
						blockContext->renderTarget->setCamera( *camera );
					}
					else
					{
						CU_ParsingError( cuT( "Camera [" ) + name + cuT( "] was not found." ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "No scene initialised for this window, set scene before camera." ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSize, TargetContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->size );

				if ( blockContext->srgbPixelFormat != castor::PixelFormat::eUNDEFINED
					&& blockContext->hdrPixelFormat != castor::PixelFormat::eUNDEFINED )
				{
					bool allowHdr = blockContext->window
						? blockContext->window->window.allowHdr
						: true;
					blockContext->renderTarget = getEngine( *blockContext )->getRenderTargetCache().addNew( blockContext->targetType
						, blockContext->size
						, allowHdr ? blockContext->hdrPixelFormat : blockContext->srgbPixelFormat );
					blockContext->renderTarget->enableFullLoading( getRootContext( *blockContext )->enableFullLoading );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFormat, TargetContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->srgbPixelFormat );
				blockContext->hdrPixelFormat = blockContext->srgbPixelFormat;

				if ( blockContext->srgbPixelFormat < castor::PixelFormat::eD16_UNORM )
				{
					if ( blockContext->size != castor::Size{ 1u, 1u }
						&& blockContext->size != castor::Size{} )
					{
						bool allowHdr = blockContext->window
							? blockContext->window->window.allowHdr
							: true;
						blockContext->renderTarget = getEngine( *blockContext )->getRenderTargetCache().addNew( blockContext->targetType
							, blockContext->size
							, allowHdr ? blockContext->hdrPixelFormat : blockContext->srgbPixelFormat );
						blockContext->renderTarget->enableFullLoading( getRootContext( *blockContext )->enableFullLoading );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Wrong format for colour" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSRGBFormat, TargetContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->srgbPixelFormat );

				if ( blockContext->srgbPixelFormat < castor::PixelFormat::eD16_UNORM )
				{
					if ( blockContext->size != castor::Size{ 1u, 1u }
						&& blockContext->size != castor::Size{}
						&& blockContext->hdrPixelFormat != castor::PixelFormat::eUNDEFINED )
					{
						bool allowHdr = blockContext->window
							? blockContext->window->window.allowHdr
							: true;
						blockContext->renderTarget = getEngine( *blockContext )->getRenderTargetCache().addNew( blockContext->targetType
							, blockContext->size
							, allowHdr ? blockContext->hdrPixelFormat : blockContext->srgbPixelFormat );
						blockContext->renderTarget->enableFullLoading( getRootContext( *blockContext )->enableFullLoading );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Wrong format for colour" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserHDRFormat, TargetContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->hdrPixelFormat );

				if ( blockContext->hdrPixelFormat < castor::PixelFormat::eD16_UNORM )
				{
					if ( blockContext->size != castor::Size{ 1u, 1u }
						&& blockContext->size != castor::Size{}
						&& blockContext->srgbPixelFormat != castor::PixelFormat::eUNDEFINED )
					{
						bool allowHdr = blockContext->window
							? blockContext->window->window.allowHdr
							: true;
						blockContext->renderTarget = getEngine( *blockContext )->getRenderTargetCache().addNew( blockContext->targetType
							, blockContext->size
							, allowHdr ? blockContext->hdrPixelFormat : blockContext->srgbPixelFormat );
						blockContext->renderTarget->enableFullLoading( getRootContext( *blockContext )->enableFullLoading );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Wrong format for colour" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStereo, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float rIntraOcularDistance;
				params[0]->get( rIntraOcularDistance );

				if ( rIntraOcularDistance > 0 )
				{
					//! blockContext->renderTarget->setStereo( true );
					//! blockContext->renderTarget->setIntraOcularDistance( rIntraOcularDistance );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPostEffect, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				Parameters parameters;

				if ( params.size() > 1 )
				{
					parameters.parse( params[1]->get< castor::String >() );
				}

				castor::String name;
				auto effect = blockContext->renderTarget->getPostEffect( params[0]->get( name ) );

				if ( !effect )
				{
					CU_ParsingError( cuT( "PostEffect [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
				}
				else
				{
					effect->enable( true );
					effect->setParameters( castor::move( parameters ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserToneMapping, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->renderTarget->setToneMappingType( params[0]->get< castor::String >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFullLoading, TargetContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [enable] parameter." ) );
			}
			else
			{
				blockContext->renderTarget->enableFullLoading( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else
			{
				auto target = blockContext->renderTarget;
				log::info << "Loaded target [" << target->getName()
					<< ", FMT(" << castor::makeString( ashes::getName( VkFormat( target->getPixelFormat() ) ) ) << ")"
					<< ", DIM(" << target->getSize() << ")]" << std::endl;

				if ( blockContext->window )
				{
					blockContext->window->window.renderTarget = castor::move( blockContext->renderTarget );
				}
				else
				{
					blockContext->texture->renderTarget = castor::move( blockContext->renderTarget );
				}
			}
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	uint32_t RenderTarget::sm_uiCount = 0;

	RenderTarget::RenderTarget( Engine & engine
		, TargetType type
		, castor::Size const & size
		, castor::PixelFormat pixelFormat )
		: OwnedBy< Engine >{ engine }
		, m_device{ getOwner()->getRenderSystem()->getRenderDevice() }
		, m_type{ type }
		, m_size{ size }
		, m_safeBandedSize{ getSafeBandedSize( size ) }
		, m_pixelFormat{ VkFormat( pixelFormat ) }
		, m_initialised{ false }
		, m_resources{ getOwner()->getGraphResourceHandler() }
		, m_index{ ++sm_uiCount }
		, m_name{ cuT( "Target" ) + castor::string::toString( m_index ) }
		, m_toneMappingName{ ( castor::isFloatingPoint( m_pixelFormat )
			? castor::String{ cuT( "none" ) }
			: castor::String{ cuT( "linear" ) } ) }
		, m_graph{ m_resources.getHandler(), castor::toUtf8( m_name ) }
		, m_velocity{ m_device
			, m_resources
			, cuT( "Velocity" )
			, 0u
			, makeExtent3D( m_safeBandedSize )
			, 1u
			, 1u
			, VK_FORMAT_R16G16_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_STORAGE_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_srgbObjects{ Texture{ m_device
				, m_resources
				, cuT( "SRGBResult0" )
				, 0u
				, makeExtent3D( m_safeBandedSize )
				, 1u
				, 1u
				, getPixelFormat()
				, rendtgt::objectsUsageFlags
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
			, Texture{ m_device
				, m_resources
				, cuT( "SRGBResult1" )
				, 0u
				, makeExtent3D( m_safeBandedSize )
				, 1u
				, 1u
				, getPixelFormat()
				, rendtgt::objectsUsageFlags
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK } }
		, m_hdrObjects{ Texture{ m_device
				, m_resources
				, cuT( "HDRResult0" )
				, 0u
				, makeExtent3D( m_safeBandedSize )
				, 1u
				, 1u
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, rendtgt::objectsUsageFlags
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
			, Texture{ m_device
				, m_resources
				, cuT( "HDRResult1" )
				, 0u
				, makeExtent3D( m_safeBandedSize )
				, 1u
				, 1u
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, rendtgt::objectsUsageFlags
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK } }
		, m_overlays{ m_device
			, m_resources
			, cuT( "Overlays" )
			, 0u
			, makeExtent3D( m_size )
			, 1u
			, 1u
			, VK_FORMAT_R8G8B8A8_UNORM
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_combined{ m_device
			, m_resources
			, cuT( "Target" )
			, 0u
			, makeExtent3D( m_size )
			, 1u
			, 1u
			, getPixelFormat()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_cameraUbo{ m_device }
		, m_overlayPassDesc{ doCreateOverlayPass( nullptr, m_device ) }
	{
		m_graph.addInput( getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampledViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		m_graph.addOutput( m_combined.wholeViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );

		for ( auto const & entry : engine.getPostEffectFactory().listRegisteredTypes() )
		{
			auto effect = engine.getPostEffectFactory().create( entry.key
				, *this
				, *engine.getRenderSystem()
				, Parameters{} );
			effect->enable( false );

			if ( effect->isAfterToneMapping() )
			{
				m_srgbPostEffects.push_back( castor::move( effect ) );
			}
			else
			{
				m_hdrPostEffects.push_back( castor::move( effect ) );
			}
		}

		{
			auto queueData = m_device.graphicsData();
			auto fence = m_device->createFence();
			auto commandBuffer = queueData->commandPool->createCommandBuffer();
			commandBuffer->begin();

			for ( auto & texture : m_hdrObjects )
			{
				texture.create();
				commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, texture.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
			}

			for ( auto & texture : m_srgbObjects )
			{
				texture.create();
				commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, texture.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
			}

			commandBuffer->end();
			queueData->queue->submit( *commandBuffer, *fence );
			fence->wait( ashes::MaxTimeout );
		}
	}

	RenderTarget::~RenderTarget()noexcept
	{
		for ( auto & texture : m_srgbObjects )
		{
			texture.destroy();
		}

		for ( auto & texture : m_hdrObjects )
		{
			texture.destroy();
		}
	}

	uint32_t RenderTarget::countInitialisationSteps()const noexcept
	{
		uint32_t result = 1u; // render technique
		result += 1u; // Meshes
		result += 1u; // HDR post effects
		result += 1u; // SRGB post effects
		result += 1u; // compiling render graph
		result += 1u; // overlay renderer
		result += 1u; // other passes
		return result;
	}

	void RenderTarget::initialise( RenderDevice const & device
		, ProgressBar * progress )
	{
		if ( !m_initialised
			&& !m_initialising.exchange( true ) )
		{
			doInitialise( device, progress );
		}

		while ( m_initialising )
		{
			std::this_thread::sleep_for( 1_ms );
		}
	}

	void RenderTarget::initialise( OnInitialisedFunc const & onInitialised
		, ProgressBar * progress )
	{
		if ( !m_initialising.exchange( true )
			&& !m_initialised )
		{
			getEngine()->pushCpuJob( [this, progress]()
				{
					auto const & device = getEngine()->getRenderSystem()->getRenderDevice();
					auto queueWrapper = device.graphicsData();
					auto & queue = *queueWrapper;
					doInitialise( device, progress );
					m_onInitialised( *this, queue );
					m_onTargetInitialised.clear();
				} );
		}

		if ( m_initialising )
		{
			m_onTargetInitialised.push_back( m_onInitialised.connect( onInitialised ) );
		}

		if ( m_initialised )
		{
			auto const & device = getEngine()->getRenderSystem()->getRenderDevice();
			auto queueWrapper = device.graphicsData();
			auto & queue = *queueWrapper;
			onInitialised( *this, queue );
		}
	}

	void RenderTarget::cleanup( RenderDevice const & device )
	{
		if ( m_initialised.exchange( false ) )
		{
			m_signalReady.reset();
			m_overlayPass = {};
#if C3D_DebugTimers
			getEngine()->unregisterTimer( getName() + cuT( "/CPU Update" ), *m_cpuUpdateTimer );
			m_cpuUpdateTimer.reset();
			getEngine()->unregisterTimer( getName() + cuT( "/GPU Update" ), *m_gpuUpdateTimer );
			m_gpuUpdateTimer.reset();
#endif
			getEngine()->unregisterTimer( getName() + cuT( "/Overlays" ), *m_overlaysTimer );
			m_overlaysTimer.reset();
			m_intermediates.clear();
			getEngine()->unregisterTimer( castor::makeString( m_runnable->getName() + "/Graph" )
				, m_runnable->getTimer() );
			m_runnable.reset();
			m_debugDrawer.reset();
			m_combinePassSource = {};
			m_combinePass = {};

			for ( auto const & effect : m_srgbPostEffects )
			{
				effect->cleanup( device );
			}

			m_toneMapping.reset();

			for ( auto const & effect : m_hdrPostEffects )
			{
				effect->cleanup( device );
			}
		}

		m_overlays.destroy();
		m_velocity.destroy();
		m_combined.destroy();
		doCleanupTechnique();
		doCleanupCombineProgram();
		m_culler.reset();
		m_colourGradingUbo.reset();
		m_hdrConfigUbo.reset();
		m_frustumClusters.reset();
	}

	void RenderTarget::update( CpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

#if C3D_DebugTimers
		auto block( m_cpuUpdateTimer->start() );
#endif

		auto & camera = *getCamera();
		auto & scene = *getScene();
		updater.jitter = m_jitter;
		updater.scene = &scene;
		updater.camera = &camera;
		camera.resize( m_size );
		camera.update();

		auto & cache = scene.getMeshCache();
		{
			auto lock( castor::makeUniqueLock( cache ) );

			for ( auto const & [_, mesh] : cache )
			{
				mesh->update( updater );
			}
		}

		CU_Require( m_culler );
		m_culler->update( updater );
		m_renderTechnique->update( updater );
		auto jitter = updater.jitter;
		auto jitterProjSpace = jitter * 2.0f;
		jitterProjSpace[0] /= float( camera.getWidth() );
		jitterProjSpace[1] /= float( camera.getHeight() );
		m_cameraUbo.cpuUpdate( camera
			, updater.debugIndex
			, true
			, jitterProjSpace );

		m_overlayPass->update( updater );

		if ( m_frustumClusters )
		{
			m_frustumClusters->update( updater );

			if ( m_debugDrawer )
			{
				m_frustumClusters->updateDebug( *m_debugDrawer );
			}
		}

		m_hdrConfigUbo->cpuUpdate( getHdrConfig() );
		m_colourGradingUbo->cpuUpdate( getColourGradingConfig() );

		auto lastTarget = &doUpdatePostEffects( updater
			, m_hdrPostEffects
			, { &m_hdrObjects.front(), &m_hdrObjects.back() } );

		if ( m_toneMapping )
		{
			m_toneMapping->update( updater, lastTarget->sampledViewId );
		}
		else
		{
			m_hdrCopyPassIndex = ( lastTarget == m_hdrCopyPassSource ) ? 1u : 0u;
		}

		lastTarget = &doUpdatePostEffects( updater
			, m_srgbPostEffects
			, { &m_srgbObjects.front(), &m_srgbObjects.back() } );
		m_combinePassIndex = ( lastTarget == m_combinePassSource ) ? 1u : 0u;
	}

	void RenderTarget::update( GpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

#if C3D_DebugTimers
		auto block( m_gpuUpdateTimer->start() );
#endif

		auto & camera = *getCamera();
		auto & scene = *getScene();
		updater.jitter = m_jitter;
		updater.scene = &scene;
		updater.camera = &camera;

		m_renderTechnique->update( updater );
		m_overlayPass->update( updater );

		for ( auto const & effect : m_hdrPostEffects )
		{
			effect->update( updater );
		}

		for ( auto const & effect : m_srgbPostEffects )
		{
			effect->update( updater );
		}
	}

	void RenderTarget::upload( UploadData & uploader )
	{
		if ( m_initialised )
		{
			m_overlayPass->upload( uploader );
		}
	}

	crg::SemaphoreWaitArray RenderTarget::render( ashes::Queue const & queue
		, crg::SemaphoreWaitArray const & signalsToWait )
	{
		if ( !m_initialised )
		{
			return signalsToWait;
		}

		crg::SemaphoreWaitArray result{};

		if ( auto scene = getScene();
			m_initialised
				&& scene
				&& scene->isInitialised()
				&& getCamera() )
		{
			result = doRender( queue, signalsToWait );
		}

		return result;
	}

	ViewportType RenderTarget::getViewportType()const
	{
		auto camera = getCamera();
		return ( camera ? camera->getViewportType() : ViewportType::eCount );
	}

	void RenderTarget::setViewportType( ViewportType value )const
	{
		if ( auto camera = getCamera() )
		{
			camera->setViewportType( value );
		}
	}

	void RenderTarget::setCamera( Camera & camera )
	{
		auto myCamera = getCamera();

		if ( myCamera != &camera )
		{
			m_camera = &camera;
			m_camera->resize( m_size );

			if ( m_culler )
			{
				m_culler->resetCamera( getCamera() );
			}
			else
			{
				m_culler = castor::makeUniqueDerived< SceneCuller, FrustumCuller >( *getScene(), *getCamera() );
			}
		}
	}

	void RenderTarget::setScene( Scene & scene )
	{
		auto myScene = getScene();

		if ( myScene != &scene )
		{
			m_scene = &scene;
			m_graph.addInput( m_scene->getEnvironmentMap().getColourId().wholeViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
			m_culler.reset();
		}
	}

	void RenderTarget::setToneMappingType( castor::StringView name )
	{
		if ( !castor::isFloatingPoint( getPixelFormat() ) )
		{
			m_toneMappingName = name;

			if ( m_toneMapping )
			{
				getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
					, [this]( RenderDevice const &
						, QueueData const & )
					{
						if ( m_initialised )
						{
							m_toneMapping->updatePipeline( m_toneMappingName );
						}
						else
						{
							m_toneMapping->initialise( m_toneMappingName
								, m_hdrObjects.back().sampledViewId );
						}
					} ) );
			}
		}
	}

	PostEffectRPtr RenderTarget::getPostEffect( castor::String const & name )const
	{
		auto it = std::find_if( m_srgbPostEffects.begin()
			, m_srgbPostEffects.end()
			, [&name]( PostEffectUPtr const & lookup )
			{
				return lookup->getName() == name;
			} );

		if ( it != m_srgbPostEffects.end() )
		{
			return it->get();
		}

		it = std::find_if( m_hdrPostEffects.begin()
			, m_hdrPostEffects.end()
			, [&name]( PostEffectUPtr const & lookup )
			{
				return lookup->getName() == name;
			} );

		if ( it != m_hdrPostEffects.end() )
		{
			return it->get();
		}

		return nullptr;
	}

	HdrConfig const & RenderTarget::getHdrConfig()const
	{
		return getCamera()->getHdrConfig();
	}

	HdrConfig & RenderTarget::getHdrConfig()
	{
		return getCamera()->getHdrConfig();
	}

	ColourGradingConfig const & RenderTarget::getColourGradingConfig()const
	{
		return getCamera()->getColourGradingConfig();
	}

	ColourGradingConfig & RenderTarget::getColourGradingConfig()
	{
		return getCamera()->getColourGradingConfig();
	}

	ShadowMapLightTypeArray RenderTarget::getShadowMaps()const
	{
		if ( m_renderTechnique )
		{
			return m_renderTechnique->getShadowMaps();
		}

		return {};
	}

	ShadowBuffer * RenderTarget::getShadowBuffer()const
	{
		if ( m_renderTechnique && m_renderTechnique->hasShadowBuffer() )
		{
			return &m_renderTechnique->getShadowBuffer();
		}

		return nullptr;
	}

	TechniquePassVector RenderTarget::getCustomRenderPasses()const
	{
		if ( m_renderTechnique )
		{
			return m_renderTechnique->getCustomRenderPasses();
		}

		return {};
	}

	bool RenderTarget::hasIndirect()const noexcept
	{
		return getScene()->needsGlobalIllumination()
			|| isFullLoadingEnabled();
	}

	bool RenderTarget::hasSss()const noexcept
	{
		return getScene()->needsSubsurfaceScattering()
			|| isFullLoadingEnabled();
	}

	bool RenderTarget::areDebugTargetsEnabled()const noexcept
	{
		return getEngine()->areDebugTargetsEnabled();
	}

	SceneUbo const & RenderTarget::getSceneUbo()const noexcept
	{
		return m_scene->getUbo();
	}

	void RenderTarget::resetSemaphore()
	{
		m_signalFinished.clear();
	}

	crg::FramePass const & RenderTarget::createVertexTransformPass( crg::FramePassGroup & graph )const
	{
		return getScene()->getRenderNodes().createVertexTransformPass( graph );
	}

	void RenderTarget::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< TargetContext > targetCtx{ result, CSCNSection::eRenderTarget };

		targetCtx.addParser( cuT( "scene" ), rendtgt::parserScene, { makeParameter< ParameterType::eName >() } );
		targetCtx.addParser( cuT( "camera" ), rendtgt::parserCamera, { makeParameter< ParameterType::eName >() } );
		targetCtx.addParser( cuT( "size" ), rendtgt::parserSize, { makeParameter< ParameterType::eSize >() } );
		targetCtx.addParser( cuT( "format" ), rendtgt::parserFormat, { makeParameter< ParameterType::ePixelFormat >() } );
		targetCtx.addParser( cuT( "hdr_format" ), rendtgt::parserHDRFormat, { makeParameter< ParameterType::ePixelFormat >() } );
		targetCtx.addParser( cuT( "srgb_format" ), rendtgt::parserSRGBFormat, { makeParameter< ParameterType::ePixelFormat >() } );
		targetCtx.addParser( cuT( "stereo" ), rendtgt::parserStereo, { makeParameter< ParameterType::eFloat >() } );
		targetCtx.addParser( cuT( "postfx" ), rendtgt::parserPostEffect, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
		targetCtx.addParser( cuT( "tone_mapping" ), rendtgt::parserToneMapping, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
		targetCtx.addParser( cuT( "enable_full_loading" ), rendtgt::parserFullLoading, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		targetCtx.addPopParser( cuT( "}" ), rendtgt::parserEnd );
	}

	void RenderTarget::doInitialise( RenderDevice const & device
		, ProgressBar * progress )
	{
		setProgressBarGlobalTitle( progress
			, cuT( "Initialising: Render Target" ) );
		m_hdrConfigUbo = castor::make_unique< HdrConfigUbo >( device );
		m_colourGradingUbo = castor::make_unique< ColourGradingUbo >( device );
		m_culler = castor::makeUniqueDerived< SceneCuller, FrustumCuller >( *getScene(), *getCamera() );

		if ( m_clustersConfig.enabled || isFullLoadingEnabled() )
		{
			m_frustumClusters = castor::makeUnique< FrustumClusters >( device, *getCamera(), m_clustersConfig );
		}

		doInitCombineProgram();
		crg::FramePassArray passes;

		stepProgressBarGlobalStartLocal( progress
			, cuT( "Initialising: Meshes" )
			, getScene()->getMeshCache().getObjectCount() );
		auto & group = m_graph.createPassGroup( "Meshes" );

		for ( auto const & [name, mesh] : getScene()->getMeshCache() )
		{
			stepProgressBarLocal( progress, name );
			passes = mesh->record( m_resources
				, group
				, passes );
		}

		auto result = doInitialiseTechnique( device, progress, castor::move( passes ) );

		if ( !result )
		{
			log::error << "Couldn't initialise render technique, stopping here\n";
			return;
		}

		auto * previousPass = &m_renderTechnique->getLastPass();
		auto hdrSource = &m_hdrObjects.front();
		auto hdrTarget = &m_hdrObjects.back();

		if ( !m_hdrPostEffects.empty() )
		{
			stepProgressBarGlobalStartLocal( progress
				, cuT( "Creating: HDR Post effects" )
				, uint32_t( m_hdrPostEffects.size() ) );
			for ( auto const & effect : m_hdrPostEffects )
			{
				if ( result
					&& ( isFullLoadingEnabled() || effect->isEnabled() ) )
				{
					stepProgressBarLocal( progress, effect->getName() );
					result = effect->initialise( device
						, *hdrSource
						, *hdrTarget
						, *previousPass );
					castor::swap( hdrSource, hdrTarget );
					previousPass = &effect->getPass();
				}
			}
		}

		if ( result )
		{
			m_hdrLastPass = previousPass;
			stepProgressBarGlobalStartLocal( progress
				, cuT( "Creating: Tone Mapping" )
				, uint32_t( m_hdrPostEffects.size() ) );
			m_toneMapping = castor::makeUnique< ToneMapping >( *getEngine()
				, m_graph.getDefaultGroup()
				, crg::ImageViewIdArray{ hdrSource->sampledViewId, hdrTarget->sampledViewId }
				, m_srgbObjects.front().wholeViewId
				, *m_hdrLastPass
				, *m_hdrConfigUbo
				, *m_colourGradingUbo
				, progress );
			m_toneMapping->initialise( m_toneMappingName
				, m_hdrObjects.back().sampledViewId );
			previousPass = &m_toneMapping->getPass();
		}

		auto srgbSource = &m_srgbObjects.front();
		auto srgbTarget = &m_srgbObjects.back();

		if ( !m_srgbPostEffects.empty() )
		{
			stepProgressBarGlobalStartLocal( progress
				, cuT( "Creating: SRGB Post effects" )
				, uint32_t( m_srgbPostEffects.size() ) );
			for ( auto const & effect : m_srgbPostEffects )
			{
				if ( result
					&& ( isFullLoadingEnabled() || effect->isEnabled() ) )
				{
					stepProgressBarLocal( progress, effect->getName() );
					result = effect->initialise( device
						, *srgbSource
						, *srgbTarget
						, *previousPass );
					castor::swap( srgbSource, srgbTarget );
					previousPass = &effect->getPass();
				}
			}
		}

		stepProgressBarGlobalStartLocal( progress
			, cuT( "Creating: Other Passes" )
			, 2u );

		if ( result )
		{
			m_combinePassSource = srgbSource;
			m_debugDrawer = castor::makeUnique< DebugDrawer >( m_graph.getDefaultGroup()
				, previousPass
				, device
				, *this
				, crg::ImageViewIdArray{ srgbSource->sampledViewId, srgbTarget->sampledViewId }
				, m_renderTechnique->getDepth()
				, &m_combinePassIndex );
			previousPass = &m_debugDrawer->getLastPass();
			m_combinePass = &doCreateCombinePass( progress
				, crg::ImageViewIdArray{ srgbSource->sampledViewId, srgbTarget->sampledViewId } );
			m_combinePass->addDependency( *previousPass );

			stepProgressBarGlobalStartLocal( progress
				, cuT( "Compiling render graph" )
				, RenderTechnique::countInitialisationSteps() );
			m_runnable = m_graph.compile( device.makeContext() );
			getEngine()->registerTimer( castor::makeString( m_runnable->getName() + "/Graph" )
				, m_runnable->getTimer() );
			printGraph( *m_runnable );
			doListIntermediateViews( m_intermediates );
			m_debugConfig.resetImages();

			for ( auto const & intermediate : m_intermediates )
			{
				m_debugConfig.registerImage( intermediate.name );
			}

			m_overlays.create();
			m_velocity.create();
			m_combined.create();
			auto runnable = m_runnable.get();
			device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
				, [runnable, result, this]( RenderDevice const &
					, QueueData const & )
				{
					runnable->record();
					m_initialised = result;
				} ) );
		}

		auto mbName = castor::toUtf8( getName() );
		m_overlaysTimer = castor::makeUnique< FramePassTimer >( device.makeContext(), mbName + "/Overlays", crg::TimerScope::eUpdate );
		getEngine()->registerTimer( getName() + cuT( "/Overlays" ), *m_overlaysTimer );
#if C3D_DebugTimers
		m_cpuUpdateTimer = castor::makeUnique< FramePassTimer >( device.makeContext(), mbName + "/CPU Update", crg::TimerScope::eUpdate );
		getEngine()->registerTimer( getName() + cuT( "/CPU Update" ), *m_cpuUpdateTimer );
		m_gpuUpdateTimer = castor::makeUnique< FramePassTimer >( device.makeContext(), mbName + "/GPU Update", crg::TimerScope::eUpdate );
		getEngine()->registerTimer( getName() + cuT( "/GPU Update" ), *m_gpuUpdateTimer );
#endif
		m_signalReady = device->createSemaphore( castor::toUtf8( getName() ) + "Ready" );
		m_initialising = false;
	}

	crg::FramePass & RenderTarget::doCreateOverlayPass( ProgressBar * progress
		, RenderDevice const & device )
	{
		stepProgressBarLocal( progress, cuT( "Creating overlays pass" ) );
		auto & group = m_graph.createPassGroup( "Overlays" );
		auto & pass = group.createPass( "Overlays"
			, [this, progress, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising overlays pass" ) );
				auto result = castor::make_unique< OverlayPass >( framePass
					, context
					, graph
					, device
					, *m_scene
					, makeExtent2D( m_overlays.getExtent() )
					, m_overlays
					, *m_hdrConfigUbo
					, true );
				m_overlayPass = result.get();
				getOwner()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		pass.addOutputColourView( m_overlays.targetViewId );
		group.addGroupOutput( m_overlays.targetViewId );
		return pass;
	}

	crg::FramePass & RenderTarget::doCreateCombinePass( ProgressBar * progress
		, crg::ImageViewIdArray source )
	{
		stepProgressBarLocal( progress, cuT( "Creating combine pass" ) );
		auto & pass = m_graph.createPass( "Other/Combine"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising combine pass" ) );
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( m_combined.getExtent() ) )
					.texcoordConfig( {} )
					.passIndex( &m_combinePassIndex )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_combineStages ) )
					.build( framePass, context, graph, crg::ru::Config{ 2u } );
				getOwner()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( m_overlayPassDesc );
		pass.addSampledView( castor::move( source )
			, uint32_t( rendtgt::CombineIdx::Scene ) );
		pass.addSampledView( m_overlays.sampledViewId
			, uint32_t( rendtgt::CombineIdx::Overlays ) );
		m_hdrConfigUbo->createPassBinding( pass
			, uint32_t( rendtgt::CombineIdx::HdrConfig ) );
		pass.addOutputColourView( m_combined.targetViewId );
		return pass;
	}

	bool RenderTarget::doInitialiseTechnique( RenderDevice const & device
		, ProgressBar * progress
		, crg::FramePassArray previousPasses )
	{
		if ( !m_renderTechnique )
		{
			try
			{
				stepProgressBarGlobalStartLocal( progress
					, cuT( "Initialising: Render Technique" )
					, RenderTechnique::countInitialisationSteps() );
				m_renderTechnique = castor::makeUnique< RenderTechnique >( getName()
					, *this
					, device
					, m_hdrObjects.front()
					, m_hdrObjects.back()
					, castor::move( previousPasses )
					, progress
					, C3D_UseVisibilityBuffer != 0
					, C3D_UseWeightedBlendedRendering != 0 );
			}
			catch ( castor::Exception & exc )
			{
				log::error << cuT( "Couldn't load render technique: " ) << castor::makeString( exc.getFullDescription() ) << std::endl;
				throw;
			}
		}

		return true;
	}

	void RenderTarget::doCleanupTechnique()
	{
		m_renderTechnique.reset();
	}

	void RenderTarget::doInitCombineProgram()
	{
		auto const & renderSystem = *getEngine()->getRenderSystem();
		auto bandSize = double( castor3d::getSafeBandSize(m_size ) );
		auto bandedSize = castor3d::getSafeBandedExtent3D(m_size );
		auto bandRatioU = bandSize / bandedSize.width;
		auto bandRatioV = bandSize / bandedSize.height;
		castor::Point4f velocityMetrics{ bandRatioU
			, bandRatioV
			, float( 1.0 - 2.0 * bandRatioU )
			, float( 1.0 - 2.0 * bandRatioV ) };

		ProgramModule programModule{ getName() + cuT( "/Combine" ) };
		{
			sdw::TraditionalGraphicsWriter writer{ &getEngine()->getShaderAllocator() };

			auto c3d_mapScene = writer.declCombinedImg< FImg2DRgba8Unorm >( "c3d_mapScene", uint32_t( rendtgt::CombineIdx::Scene ), 0u );
			auto c3d_mapOverlays = writer.declCombinedImg< FImg2DRgba8Unorm >( "c3d_mapOverlays", uint32_t( rendtgt::CombineIdx::Overlays ), 0u );
			C3D_HdrConfig( writer, rendtgt::CombineIdx::HdrConfig, 0u );

			shader::Utils utils{ writer };

			auto getSafeBandedCoord = [&velocityMetrics]( sdw::Vec2 const & texcoord )
			{
				return vec2( texcoord.x() * velocityMetrics->z + velocityMetrics->x
					, texcoord.y() * velocityMetrics->w + velocityMetrics->y );
			};

			writer.implementEntryPointT< shader::PosUv2FT, shader::TwoUv2FT >( [this, &utils, &getSafeBandedCoord]( sdw::VertexInT< shader::PosUv2FT > const & in
				, sdw::VertexOutT< shader::TwoUv2FT > out )
				{
					out.uv0()= utils.topDownToBottomUp( in.uv() );
					out.uv1()= in.uv();

					if ( getTargetType() != TargetType::eWindow )
					{
						out.uv0().y() = 1.0_f - out.uv0().y();
						out.uv1().y() = 1.0_f - out.uv1().y();
					}

					out.uv0() = getSafeBandedCoord( out.uv0() );
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< shader::TwoUv2FT, shader::Colour4FT >( [&writer, &c3d_mapScene, &c3d_mapOverlays]( sdw::FragmentInT< shader::TwoUv2FT > const & in
				, sdw::FragmentOutT< shader::Colour4FT > const & out )
				{
					auto sceneColor = writer.declLocale( "sceneColor"
						, c3d_mapScene.lod( in.uv0(), 0.0_f ).rgb() );
					auto overlaysColor = writer.declLocale( "overlaysColor"
						, c3d_mapOverlays.lod( in.uv1(), 0.0_f ) );

					sceneColor *= 1.0_f - overlaysColor.a();
					out.colour() = vec4( sceneColor + overlaysColor.rgb(), 1.0_f );
				} );
			programModule.shader = writer.getBuilder().releaseShader();
		}

		m_combineStages = makeProgramStates( renderSystem.getRenderDevice(), programModule );
	}

	void RenderTarget::doCleanupCombineProgram()
	{
		m_combineStages.clear();
	}

	Texture const & RenderTarget::doUpdatePostEffects( CpuUpdater & updater
		, PostEffectArray const & effects
		, castor::Vector< Texture const * > const & images )const
	{
		Texture const * src = images.front();
		Texture const * dst = images.back();

		for ( auto & effect : effects )
		{
			if ( ( isFullLoadingEnabled() || effect->isEnabled() )
				&& effect->update( updater, *src ) )
			{
				castor::swap( src, dst );
			}
		}

		return *dst;
	}

	crg::SemaphoreWaitArray RenderTarget::doRender( ashes::Queue const & queue
		, crg::SemaphoreWaitArray signalsToWait )
	{
		auto scene = getScene();

		if ( m_type == TargetType::eWindow )
		{
			auto targetSemaphores = scene->getRenderTargetsSemaphores();
			signalsToWait.insert( signalsToWait.end()
				, targetSemaphores.begin()
				, targetSemaphores.end() );
		}

		// Compute all that is needed for the rendering of the scene.
		m_signalFinished = m_renderTechnique->preRender( signalsToWait
			, queue );

		// Then run the graph
		m_signalFinished = m_runnable->run( m_signalFinished
			, queue );

		return m_signalFinished;
	}

	void RenderTarget::doListIntermediateViews( IntermediateViewArray & result )const
	{
		result.emplace_back( cuT( "Target Result" )
			, m_combined
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.emplace_back( cuT( "Target SRGB Colour" )
			, m_srgbObjects.front()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		result.emplace_back( cuT( "Target HDR Colour" )
			, m_hdrObjects.front()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		result.emplace_back( cuT( "Target Overlays" )
			, m_overlays
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{ { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f } }.invert( true ) );
		result.emplace_back( cuT( "Target Velocity" )
			, m_velocity
			, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			, TextureFactors{}.invert( true ) );

		for ( auto & postEffect : m_hdrPostEffects )
		{
			if ( isFullLoadingEnabled() || postEffect->isEnabled() )
			{
				rendtgt::IntermediatesLister::submit( *getScene(), *postEffect, result );
			}
		}

		for ( auto & postEffect : m_srgbPostEffects )
		{
			if (isFullLoadingEnabled() || postEffect->isEnabled())
			{
				rendtgt::IntermediatesLister::submit( *getScene(), *postEffect, result );
			}
		}

		if ( m_renderTechnique )
		{
			rendtgt::IntermediatesLister::submit( *getScene(), *m_renderTechnique, result );
		}

		rendtgt::IntermediatesLister::submit( *getScene(), *getScene()->getBackground(), result );
	}

	castor::String getPrefix( TargetContext const & context )
	{
		return context.texture
			? getPrefix( *context.texture )
			: getPrefix( *context.window );
	}

	Engine * getEngine( TargetContext const & context )
	{
		return getEngine( *getRootContext( context ) );
	}

	RootContext * getRootContext( TargetContext const & context )
	{
		return context.window
			? context.window->root
			: context.texture->root;
	}

	void addDebugAabbs( RenderTarget const & target
		, ashes::VkDescriptorSetLayoutBindingArray const & bindings
		, ashes::WriteDescriptorSetArray const & writes
		, VkDeviceSize count
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest )
	{
		addDebugAabbs( target.getDebugDrawer(), bindings, writes, count, shader, enableDepthTest );
	}

	void addDebugDrawable( RenderTarget const & target
		, DebugVertexBuffers vertexBuffers
		, DebugIndexBuffer indexBuffer
		, ashes::VkVertexInputAttributeDescriptionArray const & vertexAttributes
		, ashes::VkVertexInputBindingDescriptionArray const & vertexBindings
		, ashes::VkDescriptorSetLayoutBindingArray const & bindings
		, ashes::WriteDescriptorSetArray const & writes
		, VkDeviceSize count
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest )
	{
		addDebugDrawable( target.getDebugDrawer()
			, std::move( vertexBuffers ), std::move( indexBuffer )
			, vertexAttributes, vertexBindings
			, bindings, writes, count, shader, enableDepthTest );
	}
}
