#include "Castor3D/Render/RenderTarget.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Cache/TechniqueCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/PostEffect/PostEffect.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"
#include "Castor3D/Render/ToTexture/Texture3DTo2D.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, RenderTarget )

using namespace castor;

namespace castor3d
{
	namespace
	{
		enum CombineIdx
		{
			CombineLhsIdx = 0u,
			CombineRhsIdx = 1u,
		};

		class IntermediatesLister
			: public RenderTechniqueVisitor
		{
			struct VkImageViewCreateInfoComp
			{
				bool operator()( VkImageViewCreateInfo const & lhs
					, VkImageViewCreateInfo const & rhs )const
				{
					return ( lhs.image < rhs.image
						|| ( lhs.image == rhs.image
							&& ( lhs.subresourceRange.baseArrayLayer < rhs.subresourceRange.baseArrayLayer
								|| ( lhs.subresourceRange.baseArrayLayer == rhs.subresourceRange.baseArrayLayer
									&& ( lhs.subresourceRange.baseMipLevel < rhs.subresourceRange.baseMipLevel
										|| ( lhs.subresourceRange.baseMipLevel == rhs.subresourceRange.baseMipLevel
											&& ( lhs.subresourceRange.layerCount < rhs.subresourceRange.layerCount
												|| ( lhs.subresourceRange.layerCount == rhs.subresourceRange.layerCount
													&& ( lhs.subresourceRange.levelCount < rhs.subresourceRange.levelCount
														|| ( lhs.subresourceRange.levelCount == rhs.subresourceRange.levelCount
															&& ( lhs.viewType < rhs.viewType
																|| ( lhs.viewType == rhs.viewType
																	&& ( lhs.format < rhs.format ) ) ) ) ) ) ) ) ) ) ) ) );

				}
			};

		public:
			static void submit( RenderTechnique & technique
				, IntermediateViewArray & intermediates )
			{
				std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > cache;

				PipelineFlags flags{};
				IntermediatesLister visOpaque{ flags, *technique.getRenderTarget().getScene(), cache, intermediates };
				technique.accept( visOpaque );

				flags.passFlags |= PassFlag::eAlphaBlending;
				IntermediatesLister visTransparent{ flags, *technique.getRenderTarget().getScene(), cache, intermediates };
				technique.accept( visTransparent );
			}

			static void submit( Scene const & scene
				, PostEffect & effect
				, IntermediateViewArray & intermediates )
			{
				std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > cache;

				PipelineFlags flags{};
				IntermediatesLister vis{ flags, scene, cache, intermediates };
				effect.accept( vis );
			}

		private:
			IntermediatesLister( PipelineFlags const & flags
				, Scene const & scene
				, std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > & cache
				, IntermediateViewArray & result )
				: RenderTechniqueVisitor{ flags, scene, { true, false } }
				, m_handler{ scene.getEngine()->getGraphResourceHandler() }
				, m_result{ result }
				, m_cache{ cache }
			{
			}

			void doVisitArray( castor::String const & name
				, crg::ImageViewId viewId
				, VkImageLayout layout
				, TextureFactors const & factors )
			{
				auto & info = viewId.data->info;

				if ( ( info.viewType == VK_IMAGE_VIEW_TYPE_2D_ARRAY
					|| info.viewType == VK_IMAGE_VIEW_TYPE_1D_ARRAY
					|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE
					|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY ) )
				{
					auto layerInfo = info;
					layerInfo.subresourceRange.layerCount = 1u;

					for ( uint32_t layerIdx = 0u; layerIdx < info.subresourceRange.layerCount; ++layerIdx )
					{
						auto layer = layerIdx + info.subresourceRange.baseArrayLayer;
						auto layerViewId = m_handler.createViewId( crg::ImageViewData{ viewId.data->name + std::to_string( layer )
							, viewId.data->image
							, layerInfo.flags
							, layerInfo.viewType
							, layerInfo.format
							, layerInfo.subresourceRange } );

						if ( doFilter( layerViewId ) )
						{
							doVisit( name + std::to_string( layer )
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

				if ( ( info.viewType == VK_IMAGE_VIEW_TYPE_3D )
					|| ( ( info.viewType == VK_IMAGE_VIEW_TYPE_2D )
						&& ( info.subresourceRange.layerCount == 1u ) ) )
				{
					info.image = VkImage( uint64_t( viewId.data->image.id ) );
					m_cache.insert( info );
					m_result.emplace_back( name
						, viewId
						, layout
						, factors );
				}
				else
				{
					doVisitArray( name, viewId, layout, factors );
				}
			}

			bool doFilterArray( crg::ImageViewId const & viewId )const
			{
				bool result = false;
				auto & info = viewId.data->info;

				if ( ( info.viewType == VK_IMAGE_VIEW_TYPE_2D_ARRAY
					|| info.viewType == VK_IMAGE_VIEW_TYPE_1D_ARRAY
					|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE
					|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY ) )
				{
					auto layerInfo = info;
					layerInfo.subresourceRange.layerCount = 1u;

					for ( uint32_t layerIdx = 0u; layerIdx < info.subresourceRange.layerCount; ++layerIdx )
					{
						auto layer = layerIdx + info.subresourceRange.baseArrayLayer;
						result = doFilter( m_handler.createViewId( crg::ImageViewData{ viewId.data->name + std::to_string( layer )
							, viewId.data->image
							, layerInfo.flags
							, layerInfo.viewType
							, layerInfo.format
							, layerInfo.subresourceRange } ) ) && result;
						layerInfo.subresourceRange.baseArrayLayer++;
					}
				}

				return result;
			}

			bool doFilter( crg::ImageViewId const & viewId )const override
			{
				auto info = viewId.data->info;

				if ( info.subresourceRange.levelCount > 1u
					&& info.viewType != VK_IMAGE_VIEW_TYPE_3D )
				{
					return false;
				}

				if ( ( info.viewType == VK_IMAGE_VIEW_TYPE_3D )
					|| ( ( info.viewType == VK_IMAGE_VIEW_TYPE_2D )
						&& ( info.subresourceRange.layerCount == 1u ) ) )
				{
					info.image = VkImage( uint64_t( viewId.data->image.id ) );
					return m_cache.end() == m_cache.find( info );
				}

				return doFilterArray( viewId );
			}

		private:
			crg::ResourceHandler & m_handler;
			IntermediateViewArray & m_result;
			std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > & m_cache;
		};
	}

	//*********************************************************************************************

	uint32_t RenderTarget::sm_uiCount = 0;
	const castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & engine
		, TargetType type
		, castor::Size const & size
		, castor::PixelFormat pixelFormat )
		: OwnedBy< Engine >{ engine }
		, m_type{ type }
		, m_size{ size }
		, m_safeBandedSize{ getSafeBandedSize( size ) }
		, m_pixelFormat{ VkFormat( pixelFormat ) }
		, m_initialised{ false }
		, m_renderTechnique{}
		, m_index{ ++sm_uiCount }
		, m_name{ cuT( "Target" ) + string::toString( m_index ) }
		, m_graph{ getOwner()->getGraphResourceHandler(), m_name }
		, m_velocity{ *getOwner()->getRenderSystem()->getMainRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Velocity"
			, 0u
			, makeExtent3D( m_safeBandedSize )
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_objects{ *getOwner()->getRenderSystem()->getMainRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Scene"
			, 0u
			, makeExtent3D( m_safeBandedSize )
			, 1u
			, 1u
			, getPixelFormat()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_overlays{ *getOwner()->getRenderSystem()->getMainRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Overlays"
			, 0u
			, makeExtent3D( m_size )
			, 1u
			, 1u
			, VK_FORMAT_R8G8B8A8_UNORM
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_combined{ *getOwner()->getRenderSystem()->getMainRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Target"
			, 0u
			, makeExtent3D( m_size )
			, 1u
			, 1u
			, getPixelFormat()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_combinePass{ doCreateCombinePass() }
	{
		SamplerSPtr sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + getName() + cuT( "Linear" ) );
		sampler->setMinFilter( VK_FILTER_LINEAR );
		sampler->setMagFilter( VK_FILTER_LINEAR );

		sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + getName() + cuT( "Nearest" ) );
		sampler->setMinFilter( VK_FILTER_NEAREST );
		sampler->setMagFilter( VK_FILTER_NEAREST );
	}

	RenderTarget::~RenderTarget()
	{
	}

	void RenderTarget::initialise( RenderDevice const & device )
	{
		if ( !m_initialised )
		{
			auto & renderSystem = device.renderSystem;

			m_hdrConfigUbo = std::make_unique< HdrConfigUbo >( device );

			m_culler = std::make_unique< FrustumCuller >( *getScene(), *getCamera() );
			doInitCombineProgram();
			m_initialised = doInitialiseTechnique( device );

			m_overlaysTimer = std::make_shared< RenderPassTimer >( device, cuT( "Overlays" ), cuT( "Overlays" ) );
			auto * previousPass = &m_renderTechnique->getLastPass();

			if ( !m_hdrPostEffects.empty() )
			{
				auto const * sourceView = &m_renderTechnique->getResultImgView();

				for ( auto effect : m_hdrPostEffects )
				{
					if ( m_initialised )
					{
						m_initialised = effect->initialise( device
							, *sourceView
							, *previousPass );
						previousPass = &effect->getPass();
						sourceView = &effect->getResult();
					}
				}

				if ( m_initialised )
				{
					previousPass = &doInitialiseCopyCommands( device
						, "HDR"
						, *sourceView
						, m_renderTechnique->getResultImgView()
						, *previousPass );
				}
			}

			if ( m_initialised )
			{
				m_hdrLastPass = previousPass;
				m_toneMapping = std::make_shared< ToneMapping >( *getEngine()
					, device
					, m_size
					, m_graph
					, m_renderTechnique->getResultImgView()
					, m_objects.wholeViewId
					, *m_hdrLastPass
					, *m_hdrConfigUbo
					, Parameters{} );
				m_toneMapping->initialise( cuT( "linear" ) );
				previousPass = &m_toneMapping->getPass();
			}

			if ( !m_srgbPostEffects.empty() ) 
			{
				auto const * sourceView = &m_objects.wholeViewId;

				for ( auto effect : m_srgbPostEffects )
				{
					if ( m_initialised )
					{
						m_initialised = effect->initialise( device
							, *sourceView
							, *previousPass );
						previousPass = &effect->getPass();
						sourceView = &effect->getResult();
					}
				}

				if ( m_initialised )
				{
					previousPass = &doInitialiseCopyCommands( device
						, "SRGB"
						, *sourceView
						, m_objects.wholeViewId
						, *previousPass );
				}
			}

			if ( m_initialised )
			{
				m_combinePass.addDependency( *previousPass );
				m_runnable = m_graph.compile( device.makeContext() );

				m_objects.create();
				m_overlays.create();
				m_velocity.create();
				m_combined.create();
				m_runnable->record();
			}

			m_overlayRenderer = std::make_shared< OverlayRenderer >( device
				, m_overlays );

			m_signalReady = device->createSemaphore( getName() + "Ready" );
		}
	}

	void RenderTarget::cleanup( RenderDevice const & device )
	{
		if ( m_initialised )
		{
			m_initialised = false;
			m_culler.reset();

			m_signalReady.reset();
			m_runnable.reset();

			m_overlayRenderer.reset();

			for ( auto effect : m_srgbPostEffects )
			{
				effect->cleanup( device );
			}

			m_toneMapping.reset();

			for ( auto effect : m_hdrPostEffects )
			{
				effect->cleanup( device );
			}

			m_hdrPostEffects.clear();
			m_srgbPostEffects.clear();

			m_overlaysTimer.reset();

			m_hdrConfigUbo.reset();
			m_renderTechnique.reset();
			getEngine()->getRenderTechniqueCache().remove( getName() + cuT( "Technique" ) );
		}
	}

	void RenderTarget::update( CpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

		auto & camera = *getCamera();
		updater.camera = getCamera();
		camera.resize( m_size );
		camera.update();

		CU_Require( m_culler );
		m_culler->compute();

		m_hdrConfigUbo->cpuUpdate( getHdrConfig() );

		for ( auto effect : m_hdrPostEffects )
		{
			effect->update( updater );
		}

		for ( auto effect : m_srgbPostEffects )
		{
			effect->update( updater );
		}
	}

	void RenderTarget::update( GpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

		updater.jitter = m_jitter;
		updater.scene = getScene();
		updater.camera = getCamera();

		updater.scene->update( updater );

		m_renderTechnique->update( updater );
		m_overlayRenderer->update( updater );

		for ( auto effect : m_hdrPostEffects )
		{
			effect->update( updater );
		}

		for ( auto effect : m_srgbPostEffects )
		{
			effect->update( updater );
		}
	}

	void RenderTarget::render( RenderDevice const & device
		, RenderInfo & info )
	{
		if ( !m_initialised )
		{
			return;
		}

		SceneSPtr scene = getScene();

		if ( scene )
		{
			if ( m_initialised && scene->isInitialised() )
			{
				CameraSPtr camera = getCamera();

				if ( camera )
				{
					getEngine()->getRenderSystem()->pushScene( scene.get() );
					scene->getGeometryCache().fillInfo( info );
					doRender( device, info, getCamera() );
					getEngine()->getRenderSystem()->popScene();
				}
			}
		}
	}

	ViewportType RenderTarget::getViewportType()const
	{
		return ( getCamera() ? getCamera()->getViewportType() : ViewportType::eCount );
	}

	void RenderTarget::setViewportType( ViewportType value )
	{
		if ( getCamera() )
		{
			getCamera()->setViewportType( value );
		}
	}

	void RenderTarget::setCamera( CameraSPtr camera )
	{
		auto myCamera = getCamera();

		if ( myCamera != camera )
		{
			m_camera = camera;
			camera->resize( m_size );
			m_culler = std::make_unique< FrustumCuller >( *getScene(), *getCamera() );
		}
	}

	void RenderTarget::setScene( SceneSPtr scene )
	{
		auto myScene = getScene();

		if ( myScene != scene )
		{
			m_scene = scene;
			m_culler.reset();
		}
	}

	void RenderTarget::setToneMappingType( String const & name
		, Parameters const & parameters )
	{
		getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, name, parameters]( RenderDevice const & device )
			{
				if ( m_initialised )
				{
					m_toneMapping->updatePipeline( name );
				}
				else
				{
					m_toneMapping->initialise( name );
				}
			} ) );
	}

	void RenderTarget::addPostEffect( PostEffectSPtr effect )
	{
		if ( effect->isAfterToneMapping() )
		{
			m_srgbPostEffects.push_back( effect );
		}
		else
		{
			m_hdrPostEffects.push_back( effect );
		}
	}

	HdrConfig const & RenderTarget::getHdrConfig()const
	{
		return getCamera()->getHdrConfig();
	}

	HdrConfig & RenderTarget::getHdrConfig()
	{
		return getCamera()->getHdrConfig();
	}

	void RenderTarget::setExposure( float value )
	{
		getCamera()->setExposure( value );
	}

	void RenderTarget::setGamma( float value )
	{
		getCamera()->setGamma( value );
	}

	void RenderTarget::addTechniqueParameters( Parameters const & parameters )
	{
		m_techniqueParameters.add( parameters );
	}

	void RenderTarget::listIntermediateViews( IntermediateViewArray & result )const
	{
		result.emplace_back( "Target Result"
			, m_combined
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.emplace_back( "Target Colour"
			, m_objects
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		result.emplace_back( "Target Overlays"
			, m_overlays
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		result.emplace_back( "Target Velocity"
			, m_velocity
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );

		for ( auto & postEffect : m_hdrPostEffects )
		{
			IntermediatesLister::submit( *getScene(), *postEffect, result );
		}

		for ( auto & postEffect : m_srgbPostEffects )
		{
			IntermediatesLister::submit( *getScene(), *postEffect, result );
		}

		if ( auto technique = getTechnique() )
		{
			IntermediatesLister::submit( *technique, result );
		}
	}

	void RenderTarget::resetSemaphore()
	{
		m_signalFinished.semaphore = nullptr;
		m_signalFinished.dstStageMask = 0u;
	}

	crg::FramePass & RenderTarget::doCreateCombinePass()
	{
		auto & result = m_graph.createPass( "CombinePass"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				return crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( m_combined.getExtent() ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_combineStages ) )
					.build( pass, context, graph );
			} );
		result.addSampledView( m_objects.sampledViewId
			, CombineLhsIdx
			, {} );
		result.addSampledView( m_overlays.sampledViewId
			, CombineRhsIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addOutputColourView( m_combined.targetViewId );
		return result;
	}

	bool RenderTarget::doInitialiseTechnique( RenderDevice const & device )
	{
		if ( !m_renderTechnique )
		{
			try
			{
				auto name = getName() + cuT( "Technique" );
				m_renderTechnique = getEngine()->getRenderTechniqueCache().add( name
					, std::make_shared< RenderTechnique >( name
						, *this
						, device
						, m_techniqueParameters
						, m_ssaoConfig ) );
			}
			catch ( Exception & p_exc )
			{
				log::error << cuT( "Couldn't load render technique: " ) << string::stringCast< xchar >( p_exc.getFullDescription() ) << std::endl;
				throw;
			}
		}

		return true;
	}

	crg::FramePass const & RenderTarget::doInitialiseCopyCommands( RenderDevice const & device
		, castor::String const & name
		, crg::ImageViewId const & source
		, crg::ImageViewId const & target
		, crg::FramePass const & previousPass )
	{
		if ( source != target )
		{
			auto & pass = m_graph.createPass( getName() + name + "Copy"
				, [this, &source, &target]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					return std::make_unique< crg::ImageCopy >( pass
						, context
						, graph
						, getSafeBandedExtent3D( m_size ) );
				} );
			pass.addDependency( previousPass );
			pass.addTransferInputView( source );
			pass.addTransferOutputView( target );
			return pass;
		}

		return previousPass;
	}

	void RenderTarget::doInitCombineProgram()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto bandSize = double( castor3d::getSafeBandSize(m_size ) );
		auto bandedSize = castor3d::getSafeBandedExtent3D(m_size );
		auto bandRatioU = bandSize / bandedSize.width;
		auto bandRatioV = bandSize / bandedSize.height;
		Point4f velocityMetrics{ bandRatioU
			, bandRatioV
			, float( 1.0 - 2.0 * bandRatioU )
			, float( 1.0 - 2.0 * bandRatioV ) };

		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_textureLhs = writer.declOutput< Vec2 >( "vtx_textureLhs", CombineLhsIdx );
			auto vtx_textureRhs = writer.declOutput< Vec2 >( "vtx_textureRhs", CombineRhsIdx );
			auto out = writer.getOut();

			shader::Utils utils{ writer, *getEngine() };
			utils.declareInvertVec2Y();

			auto getSafeBandedCoord = [&]( Vec2 const & texcoord )
			{
				return vec2( texcoord.x() * velocityMetrics->z + velocityMetrics->x
					, texcoord.y() * velocityMetrics->w + velocityMetrics->y );
			};

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_textureLhs = utils.topDownToBottomUp( uv );
					vtx_textureRhs = uv;

					if ( getTargetType() != TargetType::eWindow )
					{
						vtx_textureLhs.y() = 1.0_f - vtx_textureLhs.y();
						vtx_textureRhs.y() = 1.0_f - vtx_textureRhs.y();
					}

					vtx_textureLhs = getSafeBandedCoord( vtx_textureLhs );
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			m_combineVtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapLhs = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLhs", CombineLhsIdx, 0u );
			auto c3d_mapRhs = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapRhs", CombineRhsIdx, 0u );

			auto vtx_textureLhs = writer.declInput< Vec2 >( "vtx_textureLhs", CombineLhsIdx );
			auto vtx_textureRhs = writer.declInput< Vec2 >( "vtx_textureRhs", CombineRhsIdx );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto lhsColor = writer.declLocale( "lhsColor"
						, c3d_mapLhs.sample( vtx_textureLhs ) );
					auto rhsColor = writer.declLocale( "rhsColor"
						, c3d_mapRhs.sample( vtx_textureRhs ) );
					lhsColor.rgb() *= 1.0_f - rhsColor.a();
					pxl_fragColor = vec4( lhsColor.rgb() + rhsColor.rgb(), 1.0_f );
				} );
			m_combinePxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		m_combineStages.push_back( makeShaderState( *renderSystem.getMainRenderDevice(), m_combineVtx ) );
		m_combineStages.push_back( makeShaderState( *renderSystem.getMainRenderDevice(), m_combinePxl ) );
	}

	void RenderTarget::doRender( RenderDevice const & device
		, RenderInfo & info
		, CameraSPtr camera )
	{
		SceneSPtr scene = getScene();
		crg::SemaphoreWaitArray signalsToWait;

		if ( m_type == TargetType::eWindow )
		{
			signalsToWait = scene->getRenderTargetsSemaphores();
		}

		// Render overlays.
		m_signalFinished = doRenderOverlays( device
			, signalsToWait );

		m_signalFinished = m_renderTechnique->preRender( m_signalFinished );

		// Then run the graph
		m_signalFinished = m_runnable->run( m_signalFinished
			, *device.graphicsQueue );
	}

	crg::SemaphoreWait RenderTarget::doRenderOverlays( RenderDevice const & device
		, crg::SemaphoreWaitArray const & toWait )
	{
		crg::SemaphoreWait result;
		auto timerBlock = m_overlaysTimer->start();
		using LockType = std::unique_lock< OverlayCache >;
		LockType lock{ castor::makeUniqueLock( getEngine()->getOverlayCache() ) };
		m_overlayRenderer->beginPrepare( *m_overlaysTimer, toWait );
		auto preparer = m_overlayRenderer->getPreparer( device );

		for ( auto category : getEngine()->getOverlayCache() )
		{
			SceneSPtr scene = category->getOverlay().getScene();

			if ( category->getOverlay().isVisible()
				&& ( ( !scene && m_type == TargetType::eWindow )
					|| scene.get() == getScene().get() ) )
			{
				category->update( *m_overlayRenderer );
				category->accept( preparer );
			}
		}

		m_overlayRenderer->endPrepare( *m_overlaysTimer );
		return m_overlayRenderer->render( *m_overlaysTimer );
	}
}
