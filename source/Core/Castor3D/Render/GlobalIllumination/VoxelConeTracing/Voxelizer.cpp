#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/Voxelizer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelBufferToTexture.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizePass.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSecondaryBounce.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnablePasses/BufferCopy.hpp>
#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>

CU_ImplementSmartPtr( castor3d, Voxelizer )

namespace castor3d
{
	//*********************************************************************************************

	namespace vxlsr
	{
		static Texture createTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkExtent3D const & size )
		{
			return Texture{ device
				, resources
				, name
				, VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT
				, size
				, 1u
				, getMipLevels( size, VK_FORMAT_R16G16B16A16_SFLOAT )
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, ( VK_IMAGE_USAGE_STORAGE_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT )
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK
				, VK_COMPARE_OP_NEVER
				, false };
		}

		static ashes::BufferPtr< Voxel > createSsbo( RenderDevice const & device
			, castor::String const & name
			, uint32_t voxelGridSize )
		{
			return castor3d::makeBuffer< Voxel >( device
				, voxelGridSize * voxelGridSize * voxelGridSize
				, ( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					| VK_BUFFER_USAGE_TRANSFER_SRC_BIT
					| VK_BUFFER_USAGE_TRANSFER_DST_BIT )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
		}

		class VoxelsClear
			: public crg::RunnablePass
		{
		public:
			VoxelsClear( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, IsEnabledCallback isEnabled )
				: crg::RunnablePass{ pass
				, context
				, graph
				, { crg::defaultV< InitialiseCallback >
					, GetPipelineStateCallback( []() { return crg::getPipelineState( VK_PIPELINE_STAGE_TRANSFER_BIT ); } )
					, [this]( crg::RecordContext &, VkCommandBuffer cb, uint32_t i ) { doRecordInto( cb, i ); }
					, crg::defaultV< GetPassIndexCallback >
					, std::move( isEnabled ) } }
			{
			}

		protected:
			void doRecordInto( VkCommandBuffer commandBuffer
				, uint32_t index )const
			{
				for ( auto & attach : m_pass.buffers )
				{
					m_context.vkCmdFillBuffer( commandBuffer
						, attach.buffer.buffer.buffer( index )
						, attach.buffer.range.offset
						, attach.buffer.range.size
						, 0u );
				}
			}
		};
	}

	//*********************************************************************************************

	Voxelizer::Voxelizer( crg::ResourcesCache & resources
		, RenderDevice const & device
		, ProgressBar * progress
		, castor::String const & prefix
		, Scene & scene
		, Camera & camera
		, VoxelizerUbo & voxelizerUbo
		, VctConfig const & voxelConfig
		, crg::FramePassArray const & previousPasses )
		: m_engine{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_voxelConfig{ voxelConfig }
		, m_scene{ scene }
		, m_camera{ camera }
		, m_staticsCuller{ castor::makeUniqueDerived< SceneCuller, DummyCuller >( m_scene, nullptr, true ) }
		, m_dynamicsCuller{ castor::makeUniqueDerived< SceneCuller, DummyCuller >( m_scene, nullptr, false ) }
		, m_graph{ resources.getHandler(), prefix + "/Voxelizer" }
		, m_cameraUbo{ device }
		, m_sceneUbo{ device }
		, m_firstBounce{ vxlsr::createTexture( device, resources, "VoxelizedSceneFirstBounce", { m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value() } ) }
		, m_secondaryBounce{ vxlsr::createTexture( device, resources, "VoxelizedSceneSecondaryBounce", { m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value() } ) }
		, m_staticsVoxels{ vxlsr::createSsbo( device, "VoxelizedStaticSceneBuffer", m_voxelConfig.gridSize.value() ) }
		, m_dynamicsVoxels{ vxlsr::createSsbo( device, "VoxelizedSceneBuffer", m_voxelConfig.gridSize.value() ) }
		, m_voxelizerUbo{ voxelizerUbo }
		, m_clearStatics{ doCreateClearStaticsPass( previousPasses, progress ) }
		, m_staticsVoxelizePassDesc{ doCreateVoxelizePass( { &m_clearStatics }, progress, *m_staticsVoxels, *m_staticsCuller, true ) }
		, m_mergeStaticsDesc{ doCreateMergeStaticsPass( m_staticsVoxelizePassDesc, progress ) }
		, m_dynamicsVoxelizePassDesc{ doCreateVoxelizePass( { &m_mergeStaticsDesc }, progress, *m_dynamicsVoxels, *m_dynamicsCuller, false ) }
		, m_voxelToTextureDesc{ doCreateVoxelToTexture( m_dynamicsVoxelizePassDesc, progress ) }
		, m_voxelMipGen{ doCreateVoxelMipGen( m_voxelToTextureDesc
			, "FirstBounceMip"
			, m_firstBounce.wholeViewId
			, crg::RunnablePass::IsEnabledCallback( [this](){ return doEnableFirstBounceMipGen(); } )
			, progress ) }
		, m_voxelSecondaryBounceDesc{ doCreateVoxelSecondaryBounce( m_voxelMipGen, progress ) }
		, m_voxelSecondaryMipGen{ doCreateVoxelMipGen( m_voxelSecondaryBounceDesc
			, "SecondaryBounceMip"
			, m_secondaryBounce.wholeViewId
			, crg::RunnablePass::IsEnabledCallback( [this](){ return doEnableSecondaryBounceMipGen(); } )
			, progress ) }
		, m_runnable{ m_graph.compile( m_device.makeContext() ) }
	{
		m_scene.getEngine()->registerTimer( m_runnable->getName() + "/Graph"
			, m_runnable->getTimer() );
		printGraph( *m_runnable );
		m_graph.addOutput( m_firstBounce.wholeViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		m_graph.addOutput( m_secondaryBounce.wholeViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		auto runnable = m_runnable.get();
		m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
			, [runnable]( RenderDevice const &
				, QueueData const & )
			{
				runnable->record();
			} ) );
	}

	Voxelizer::~Voxelizer()noexcept
	{
		m_scene.getEngine()->unregisterTimer( m_runnable->getName() + "/Graph"
			, m_runnable->getTimer() );
		m_runnable.reset();
		m_firstBounce.destroy();
		m_secondaryBounce.destroy();
		m_dynamicsVoxels.reset();
		m_staticsVoxels.reset();
	}

	void Voxelizer::update( CpuUpdater & updater )
	{
		if ( m_staticsVoxelizePass )
		{
			auto const & camera = *updater.camera;
			auto const & scene = *updater.scene;
			auto & aabb = scene.getBoundingBox();
			auto max = std::max( aabb.getDimensions()->x, std::max( aabb.getDimensions()->y, aabb.getDimensions()->z ) );
			auto cellSize = float( m_voxelConfig.gridSize.value() ) / max;
			auto voxelSize = ( cellSize * m_voxelConfig.voxelSizeFactor );
			m_grid = castor::Point4f{ 0.0f
				, 0.0f
				, 0.0f
				, voxelSize };
			static const castor::Matrix4x4f identity{ []()
				{
					castor::Matrix4x4f res;
					res.setIdentity();
					return res;
				}() };
			//Orthograhic projection
			auto const & sceneBoundingBox = scene.getBoundingBox();
			auto ortho = m_device.renderSystem.getOrtho( sceneBoundingBox.getMin()->x
				, sceneBoundingBox.getMax()->x
				, sceneBoundingBox.getMin()->y
				, sceneBoundingBox.getMax()->y
				, -1.0f * sceneBoundingBox.getMin()->z
				, -1.0f * sceneBoundingBox.getMax()->z );
			auto jitterProjSpace = updater.jitter * 2.0f;
			jitterProjSpace[0] /= float( camera.getWidth() );
			jitterProjSpace[1] /= float( camera.getHeight() );
			m_cameraUbo.cpuUpdate( camera
				, identity
				, ortho
				, updater.debugIndex
				, true
				, jitterProjSpace );
			m_sceneUbo.cpuUpdate( scene );
			m_voxelizerUbo.cpuUpdate( m_voxelConfig
				, voxelSize
				, m_voxelConfig.gridSize.value() );

			m_staticsCuller->update( updater );
			m_dynamicsCuller->update( updater );
			m_staticsVoxelizePass->update( updater );
			m_dynamicsVoxelizePass->update( updater );
		}
	}

	void Voxelizer::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Voxelisation First Bounce"
			, m_firstBounce
			, m_graph.getFinalLayoutState( m_firstBounce.wholeViewId ).layout
			, TextureFactors::tex3D( &m_grid ) );
		visitor.visit( "Voxelisation Secondary Bounce"
			, m_secondaryBounce
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors::tex3D( &m_grid ) );
		m_staticsVoxelizePass->accept( visitor );
		m_dynamicsVoxelizePass->accept( visitor );
		m_voxelToTexture->accept( visitor );
		m_voxelSecondaryBounce->accept( visitor );
	}

	crg::SemaphoreWaitArray Voxelizer::render( crg::SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )
	{
		auto result = m_runnable->run( semaphore, queue );

		if ( m_staticsVoxelizePass->isPassEnabled() )
		{
			m_staticsVoxelizePass->setUpToDate();
		}

		if ( m_dynamicsVoxelizePass->isPassEnabled() )
		{
			m_dynamicsVoxelizePass->setUpToDate();
		}

		return result;
	}

	uint32_t Voxelizer::countInitialisationSteps()noexcept
	{
		uint32_t result = 0u;
		result += 1;// clear static pass
		result += 1;// voxelize static pass
		result += 1;// copy static to dynamic pass
		result += 1;// voxelize dynamic pass
		result += 1;// voxel buffer to texture pass
		result += 1;// primary voxel mipmap generation pass
		result += 1;// voxel secondary bounce pass
		result += 1;// secondary voxel mipmap generation pass
		return result;
	}

	crg::FramePass & Voxelizer::doCreateVoxelizePass( crg::FramePassArray const & previousPasses
		, ProgressBar * progress
		, ashes::Buffer< Voxel > const & outVoxels
		, SceneCuller & culler
		, bool isStatic )
	{
		std::string name = "NodesPass";

		if ( isStatic )
		{
			name = "Static" + name;
		}

		stepProgressBarLocal( progress, "Creating voxelize pass" );
		auto & result = m_graph.createPass( name
			, [this, progress, isStatic, &outVoxels, &culler]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising voxelize pass" );
				auto res = std::make_unique< VoxelizePass >( framePass
					, context
					, runnableGraph
					, m_device
					, m_cameraUbo
					, m_sceneUbo
					, m_camera
					, culler
					, m_voxelizerUbo
					, outVoxels
					, m_voxelConfig
					, isStatic );

				if ( isStatic )
				{
					m_staticsVoxelizePass = res.get();
				}
				else
				{
					m_dynamicsVoxelizePass = res.get();
				}

				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );

		if ( !previousPasses.empty() )
		{
			result.addDependencies( previousPasses );
		}

		std::string bufName = "Voxels";

		if ( isStatic )
		{
			bufName = "Static" + bufName;
		}

		result.addInOutStorageBuffer( { outVoxels.getBuffer(), bufName }
			, 0u
			, 0u
			, outVoxels.getBuffer().getSize() );
		return result;
	}

	crg::FramePass & Voxelizer::doCreateClearStaticsPass( crg::FramePassArray const & previousPasses
		, ProgressBar * progress )
	{
		stepProgressBarLocal( progress, "Creating clear static pass" );
		auto & result = m_graph.createPass( "StaticsClearPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising clear static pass" );
				auto res = std::make_unique< vxlsr::VoxelsClear >( framePass
					, context
					, runnableGraph
					, crg::BufferCopy::IsEnabledCallback( [this]() { return doEnableClearStatic(); } ) );
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		result.addOutputStorageBuffer( { m_staticsVoxels->getBuffer(), "StaticsVoxels" }
			, 0u
			, 0u
			, m_staticsVoxels->getBuffer().getSize() );
		return result;
	}

	crg::FramePass & Voxelizer::doCreateMergeStaticsPass( crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		stepProgressBarLocal( progress, "Creating copy static to dynamic pass" );
		auto & result = m_graph.createPass( "StaticsCopyPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising copy static to dynamic pass" );
				auto res = std::make_unique< crg::BufferCopy >( framePass
					, context
					, runnableGraph
					, 0u
					, m_staticsVoxels->getBuffer().getSize()
					, crg::ru::Config{}
					, crg::BufferCopy::GetPassIndexCallback( []() { return 0u; } )
					, crg::BufferCopy::IsEnabledCallback( [this]() { return doEnableCopyStatic(); } ) );
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( previousPass );
		result.addInputStorageBuffer( { m_staticsVoxels->getBuffer(), "StaticsVoxels" }
			, 0u
			, 0u
			, m_staticsVoxels->getBuffer().getSize() );
		result.addOutputStorageBuffer( { m_dynamicsVoxels->getBuffer(), "Voxels" }
			, 0u
			, 0u
			, m_dynamicsVoxels->getBuffer().getSize() );
		return result;
	}

	crg::FramePass & Voxelizer::doCreateVoxelToTexture( crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		m_firstBounce.create();
		stepProgressBarLocal( progress, "Creating voxel buffer to texture pass" );
		auto & result = m_graph.createPass( "VoxelBufferToTexture"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising voxel buffer to texture pass" );
				auto res = std::make_unique< VoxelBufferToTexture >( framePass
					, context
					, runnableGraph
					, m_device
					, m_voxelConfig
					, crg::RunnablePass::IsEnabledCallback( [this]() { return doEnableVoxelToTexture(); } ) );
				m_voxelToTexture = res.get();
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( previousPass );
		result.addInputStorageBuffer( { m_dynamicsVoxels->getBuffer(), "Voxels" }
			, 0u
			, 0u
			, m_dynamicsVoxels->getBuffer().getSize() );
		result.addOutputStorageView( m_firstBounce.wholeViewId
			, 1u );
		return result;
	}

	crg::FramePass & Voxelizer::doCreateVoxelMipGen( crg::FramePass const & previousPass
		, std::string const & name
		, crg::ImageViewId const & view
		, crg::RunnablePass::IsEnabledCallback isEnabled
		, ProgressBar * progress )
	{
		stepProgressBarLocal( progress, "Creating voxel mipmap generation pass" );
		auto & result = m_graph.createPass( name
			, [this, progress, enable = std::move( isEnabled )]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising voxel mipmap generation pass" );
				auto res = std::make_unique< crg::GenerateMipmaps >( framePass
					, context
					, runnableGraph
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, crg::ru::Config{}
					, crg::defaultV< crg::RunnablePass::GetPassIndexCallback >
					, enable );
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( previousPass );
		result.addTransferInOutView( view );
		return result;
	}

	crg::FramePass & Voxelizer::doCreateVoxelSecondaryBounce( crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		m_secondaryBounce.create();
		stepProgressBarLocal( progress, "Creating voxel secondary bounce pass" );
		auto & result = m_graph.createPass( "SecondaryBounce"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising voxel secondary bounce pass" );
				auto res = std::make_unique< VoxelSecondaryBounce >( framePass
					, context
					, runnableGraph
					, m_device
					, m_voxelConfig
					, crg::RunnablePass::IsEnabledCallback( [this]() { return doEnableSecondaryBounce(); } ) );
				m_voxelSecondaryBounce = res.get();
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( previousPass );
		result.addInOutStorageBuffer( { m_dynamicsVoxels->getBuffer(), "Voxels" }
			, 0u
			, 0u
			, m_dynamicsVoxels->getBuffer().getSize() );
		m_voxelizerUbo.createPassBinding( result
			, 1u );
		result.addSampledView( m_firstBounce.wholeViewId
			, 2u );
		result.addOutputStorageView( m_secondaryBounce.wholeViewId
			, 3u );
		return result;
	}

	bool Voxelizer::doEnableClearStatic()const
	{
		return m_voxelConfig.enabled
			&& m_staticsVoxelizePass->isPassEnabled();
	}

	bool Voxelizer::doEnableCopyStatic()const
	{
		return m_voxelConfig.enabled
			&& ( !m_dynamicsVoxelizePass->hasNodes()
				|| m_dynamicsVoxelizePass->isPassEnabled() );
	}

	bool Voxelizer::doEnableVoxelToTexture()const
	{
		return m_voxelConfig.enabled
			&& ( m_dynamicsVoxelizePass->isPassEnabled()
				|| m_staticsVoxelizePass->isPassEnabled() );
	}

	bool Voxelizer::doEnableSecondaryBounce()const
	{
		return m_voxelConfig.enabled
			&& m_voxelConfig.enableSecondaryBounce
			&& ( m_dynamicsVoxelizePass->isPassEnabled()
				|| m_staticsVoxelizePass->isPassEnabled() );
	}

	bool Voxelizer::doEnableFirstBounceMipGen()const
	{
		return m_voxelConfig.enabled
			&& ( m_dynamicsVoxelizePass->isPassEnabled()
				|| m_staticsVoxelizePass->isPassEnabled() );
	}

	bool Voxelizer::doEnableSecondaryBounceMipGen()const
	{
		return m_voxelConfig.enabled
			&& m_voxelConfig.enableSecondaryBounce
			&& ( m_dynamicsVoxelizePass->isPassEnabled()
				|| m_staticsVoxelizePass->isPassEnabled() );
	}
}
