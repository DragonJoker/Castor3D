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
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnablePasses/BufferCopy.hpp>
#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>

CU_ImplementSmartPtr( c3d, Voxelizer )

namespace c3d
{
	//*********************************************************************************************

	namespace vxlsr
	{
		static Texture createTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, Extent3D const & size )
		{
			return Texture{ device
				, resources
				, name
				, { ImageCreateFlags::e2DArrayCompatible
					, size, 1u, getMipLevels( size, PixelFormat::eR16G16B16A16_SFLOAT )
					, PixelFormat::eR16G16B16A16_SFLOAT
					, ( ImageUsageFlags::eStorage
						| ImageUsageFlags::eTransferSrc
						| ImageUsageFlags::eTransferDst
						| ImageUsageFlags::eSampled ) }
				, { BorderColour::eFloatOpaqueBlack
					, ComparisonFunc::eNever }
				, false };
		}

		static BufferUPtrT< Voxel > createSsbo( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, uint32_t voxelGridSize )
		{
			return makeBuffer< Voxel >( device, resources
				, voxelGridSize * voxelGridSize * voxelGridSize
				, ( BufferUsageFlags::eStorageBuffer
					| BufferUsageFlags::eTransferSrc
					| BufferUsageFlags::eTransferDst )
				, MemoryPropertyFlags::eDeviceLocal
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
					, GetPipelineStateCallback( []() { return crg::getPipelineState( PipelineStageFlags::eTransfer ); } )
					, [this]( crg::RecordContext &, VkCommandBuffer cb, uint32_t i ) { doRecordInto( cb, i ); }
					, crg::defaultV< GetPassIndexCallback >
					, c3d::move( isEnabled ) } }
			{
			}

		protected:
			void doRecordInto( VkCommandBuffer commandBuffer
				, uint32_t index )const
			{
				for ( auto & [_, attach] : m_pass.outputs )
				{
					m_context.vkCmdFillBuffer( commandBuffer
						, m_graph.createBuffer( attach->buffer( index ).data->buffer )
						, attach->getBufferRange().offset
						, attach->getBufferRange().size
						, 0u );
				}
			}
		};
	}

	//*********************************************************************************************

	Voxelizer::Voxelizer( crg::ResourcesCache & resources
		, RenderDevice const & device
		, ProgressBar * progress
		, String const & prefix
		, Scene & scene
		, Camera & camera
		, VoxelizerUbo & voxelizerUbo
		, VctConfig const & voxelConfig )
		: m_engine{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_voxelConfig{ voxelConfig }
		, m_scene{ scene }
		, m_camera{ camera }
		, m_staticsCuller{ makeUniqueDerived< SceneCuller, DummyCuller >( m_scene, nullptr, true ) }
		, m_dynamicsCuller{ makeUniqueDerived< SceneCuller, DummyCuller >( m_scene, nullptr, false ) }
		, m_graph{ resources.getHandler(), toUtf8( prefix ) + "/Voxelizer" }
		, m_cameraUbo{ device }
		, m_renderUbo{ makeRawUnique< RenderUbo >( device ) }
		, m_firstBounce{ vxlsr::createTexture( device, resources, cuT( "VoxelizedSceneFirstBounce" ), { m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value() } ) }
		, m_secondaryBounce{ vxlsr::createTexture( device, resources, cuT( "VoxelizedSceneSecondaryBounce" ), { m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value() } ) }
		, m_staticsVoxels{ vxlsr::createSsbo( device, resources, cuT( "VoxelizedStaticSceneBuffer" ), m_voxelConfig.gridSize.value() ) }
		, m_dynamicsVoxels{ vxlsr::createSsbo( device, resources, cuT( "VoxelizedSceneBuffer" ), m_voxelConfig.gridSize.value() ) }
		, m_voxelizerUbo{ voxelizerUbo }
	{
		doCreateClearStaticsPass( progress );
		doCreateVoxelizePass( progress, *m_staticsVoxels, *m_staticsCuller, true );
		doCreateMergeStaticsPass( progress );
		doCreateVoxelizePass( progress, *m_dynamicsVoxels, *m_dynamicsCuller, false );
		doCreateVoxelToTexture( progress );
		doCreateVoxelMipGen( cuT( "FirstBounceMip" )
			, m_firstBounce
			, crg::RunnablePass::IsEnabledCallback( [this](){ return doEnableFirstBounceMipGen(); } )
			, progress );
		doCreateVoxelSecondaryBounce( progress );
		doCreateVoxelMipGen( cuT( "SecondaryBounceMip" )
			, m_secondaryBounce
			, crg::RunnablePass::IsEnabledCallback( [this](){ return doEnableSecondaryBounceMipGen(); } )
			, progress );
		m_runnable = m_graph.compile( m_device.makeContext() );
		m_scene.getEngine()->registerTimer( makeString( m_runnable->getName() + "/Graph" )
			, m_runnable->getTimer() );
		printGraph( *m_runnable );
		m_graph.addOutput( m_firstBounce.getWholeViewId()
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addOutput( m_secondaryBounce.getWholeViewId()
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
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
		m_scene.getEngine()->unregisterTimer( makeString( m_runnable->getName() + "/Graph" )
			, m_runnable->getTimer() );
		m_runnable.reset();
		m_firstBounce.destroy();
		m_secondaryBounce.destroy();
		m_dynamicsVoxels->destroy();
		m_dynamicsVoxels.reset();
		m_staticsVoxels->destroy();
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
			m_grid = Point4f{ 0.0f
				, 0.0f
				, 0.0f
				, voxelSize };
			static const Matrix4x4f identity{ []()
				{
					Matrix4x4f res;
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
			m_renderUbo->cpuUpdate( camera.getHdrConfig()
				, updater.renderSize, true
				, updater.debugIndex );
			m_cameraUbo.cpuUpdate( camera
				, identity
				, ortho
				, updater.jitter );
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
		visitor.visit( cuT( "Voxelisation First Bounce" )
			, m_firstBounce
			, ImageLayout::eShaderReadOnly
			, TextureFactors::tex3D( &m_grid ) );
		visitor.visit( cuT( "Voxelisation Secondary Bounce" )
			, m_secondaryBounce
			, ImageLayout::eShaderReadOnly
			, TextureFactors::tex3D( &m_grid ) );
		m_staticsVoxelizePass->accept( visitor );
		m_dynamicsVoxelizePass->accept( visitor );
		m_voxelToTexture->accept( visitor );
		m_voxelSecondaryBounce->accept( visitor );
	}

	SemaphoreWaitArray Voxelizer::render( SemaphoreWaitArray const & semaphore
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

	void Voxelizer::doCreateVoxelizePass( ProgressBar * progress
		, BufferT< Voxel > & outVoxels
		, SceneCuller & culler
		, bool isStatic )
	{
		MbString name = "NodesPass";

		if ( isStatic )
		{
			name = "Static" + name;
		}

		stepProgressBarLocal( progress, cuT( "Creating voxelize pass" ) );
		auto & oass = m_graph.createPass( name
			, [this, progress, isStatic, &outVoxels, &culler]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising voxelize pass" ) );
				auto res = makeRawUnique< VoxelizePass >( framePass, context, runnableGraph, m_device
					, m_cameraUbo, *m_renderUbo, m_scene.getUbo()
					, m_camera, culler, m_voxelizerUbo, outVoxels
					, m_voxelConfig, isStatic );
				if ( isStatic )
					m_staticsVoxelizePass = res.get();
				else
					m_dynamicsVoxelizePass = res.get();
				m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		MbString bufName = "Voxels";

		if ( isStatic )
		{
			bufName = "Static" + bufName;
		}

		outVoxels.setLastAttach( oass.addInOutStorage( *outVoxels.getLastAttach(), 0u ) );
	}

	void Voxelizer::doCreateClearStaticsPass( ProgressBar * progress )
	{
		stepProgressBarLocal( progress, cuT( "Creating clear static pass" ) );
		auto & result = m_graph.createPass( "StaticsClearPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising clear static pass" ) );
				auto res = makeRawUnique< vxlsr::VoxelsClear >( framePass
					, context
					, runnableGraph
					, crg::BufferCopy::IsEnabledCallback( [this]() { return doEnableClearStatic(); } ) );
				m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		m_staticsVoxels->setLastAttach( result.addOutputStorageBuffer( m_staticsVoxels->bufferViewId, 0u ) );
	}

	void Voxelizer::doCreateMergeStaticsPass( ProgressBar * progress )
	{
		stepProgressBarLocal( progress, cuT( "Creating copy static to dynamic pass" ) );
		auto & result = m_graph.createPass( "StaticsCopyPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising copy static to dynamic pass" ) );
				auto res = makeRawUnique< crg::BufferCopy >( framePass
					, context
					, runnableGraph
					, 0u
					, m_staticsVoxels->getSize()
					, crg::ru::Config{}
					, crg::BufferCopy::GetPassIndexCallback( []() { return 0u; } )
					, crg::BufferCopy::IsEnabledCallback( [this]() { return doEnableCopyStatic(); } ) );
				m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		result.addInputTransfer( *m_staticsVoxels->getLastAttach() );
		m_dynamicsVoxels->setLastAttach( result.addOutputTransferBuffer( m_dynamicsVoxels->bufferViewId ) );
	}

	void Voxelizer::doCreateVoxelToTexture( ProgressBar * progress )
	{
		m_firstBounce.create();
		stepProgressBarLocal( progress, cuT( "Creating voxel buffer to texture pass" ) );
		auto & result = m_graph.createPass( "VoxelBufferToTexture"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising voxel buffer to texture pass" ) );
				auto res = makeRawUnique< VoxelBufferToTexture >( framePass
					, context
					, runnableGraph
					, m_device
					, m_voxelConfig
					, crg::RunnablePass::IsEnabledCallback( [this]() { return doEnableVoxelToTexture(); } ) );
				m_voxelToTexture = res.get();
				m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		result.addInputStorage( *m_dynamicsVoxels->getLastAttach(), 0u );
		m_firstBounce.setLastAttach( result.addOutputStorageImage( m_firstBounce.getWholeViewId(), 1u ) );
	}

	void Voxelizer::doCreateVoxelMipGen( String const & name
		, Texture & view
		, crg::RunnablePass::IsEnabledCallback isEnabled
		, ProgressBar * progress )
	{
		stepProgressBarLocal( progress, cuT( "Creating voxel mipmap generation pass" ) );
		auto & result = m_graph.createPass( toUtf8( name )
			, [this, progress, enable = c3d::move( isEnabled )]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising voxel mipmap generation pass" ) );
				auto res = makeRawUnique< crg::GenerateMipmaps >( framePass
					, context
					, runnableGraph
					, ImageLayout::eShaderReadOnly
					, crg::ru::Config{}
					, crg::defaultV< crg::RunnablePass::GetPassIndexCallback >
					, enable );
				m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		view.setLastAttach( result.addInOutTransfer( *view.getLastAttach() ) );
	}

	void Voxelizer::doCreateVoxelSecondaryBounce( ProgressBar * progress )
	{
		m_secondaryBounce.create();
		stepProgressBarLocal( progress, cuT( "Creating voxel secondary bounce pass" ) );
		auto & result = m_graph.createPass( "SecondaryBounce"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising voxel secondary bounce pass" ) );
				auto res = makeRawUnique< VoxelSecondaryBounce >( framePass
					, context
					, runnableGraph
					, m_device
					, m_voxelConfig
					, crg::RunnablePass::IsEnabledCallback( [this]() { return doEnableSecondaryBounce(); } ) );
				m_voxelSecondaryBounce = res.get();
				m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		m_dynamicsVoxels->setLastAttach( result.addInOutStorage( *m_dynamicsVoxels->getLastAttach(), 0u ) );
		m_voxelizerUbo.createPassBinding( result, 1u );
		result.addInputSampled( *m_firstBounce.getSampledLastAttach(), 2u );
		m_secondaryBounce.setLastAttach( result.addOutputStorageImage( m_secondaryBounce.getWholeViewId(), 3u ) );
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
