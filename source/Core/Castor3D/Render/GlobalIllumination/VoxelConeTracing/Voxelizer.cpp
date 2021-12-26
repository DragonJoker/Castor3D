#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/Voxelizer.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelBufferToTexture.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizePass.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSecondaryBounce.hpp"
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
#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>

CU_ImplementCUSmartPtr( castor3d, Voxelizer )

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		Texture createTexture( RenderDevice const & device
			, crg::ResourceHandler & handler
			, String const & name
			, VkExtent3D const & size )
		{
			return Texture{ device
				, handler
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
				, false };
		}

		ashes::BufferPtr< Voxel > createSsbo( Engine & engine
			, RenderDevice const & device
			, String const & name
			, uint32_t voxelGridSize )
		{
			return castor3d::makeBuffer< Voxel >( device
				, voxelGridSize * voxelGridSize * voxelGridSize
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
		}
	}

	//*********************************************************************************************

	Voxelizer::Voxelizer( crg::ResourceHandler & handler
		, RenderDevice const & device
		, ProgressBar * progress
		, Scene & scene
		, Camera & camera
		, MatrixUbo & matrixUbo
		, VoxelizerUbo & voxelizerUbo
		, VoxelSceneData const & voxelConfig )
		: m_engine{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_voxelConfig{ voxelConfig }
		, m_graph{ handler, "Voxelizer" }
		, m_culler{ scene, &camera }
		, m_matrixUbo{ device }
		, m_firstBounce{ createTexture( device, handler, "VoxelizedSceneFirstBounce", { m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value() } ) }
		, m_secondaryBounce{ createTexture( device, handler, "VoxelizedSceneSecondaryBounce", { m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value() } ) }
		, m_voxels{ createSsbo( m_engine, device, "VoxelizedSceneBuffer", m_voxelConfig.gridSize.value() ) }
		, m_voxelizerUbo{ voxelizerUbo }
		, m_voxelizePassDesc{ doCreateVoxelizePass( progress ) }
		, m_voxelToTextureDesc{ doCreateVoxelToTexture( m_voxelizePassDesc, progress ) }
		, m_voxelMipGen{ doCreateVoxelMipGen( m_voxelToTextureDesc
			, "FirstBounceMip"
			, m_firstBounce.wholeViewId
			, crg::RunnablePass::IsEnabledCallback( [&voxelConfig](){ return voxelConfig.enabled; } )
			, progress ) }
		, m_voxelSecondaryBounceDesc{ doCreateVoxelSecondaryBounce( m_voxelMipGen, progress ) }
		, m_voxelSecondaryMipGen{ doCreateVoxelMipGen( m_voxelSecondaryBounceDesc
			, "SecondaryBounceMip"
			, m_secondaryBounce.wholeViewId
			, crg::RunnablePass::IsEnabledCallback( [&voxelConfig](){ return voxelConfig.enabled && voxelConfig.enableSecondaryBounce; } )
			, progress ) }
		, m_runnable{ m_graph.compile( m_device.makeContext() ) }
	{
		m_firstBounce.create();
		m_secondaryBounce.create();
		auto runnable = m_runnable.get();
		m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [runnable]( RenderDevice const &
				, QueueData const & )
			{
				runnable->record();
			} ) );
	}

	Voxelizer::~Voxelizer()
	{
		m_runnable.reset();
		m_firstBounce.destroy();
		m_secondaryBounce.destroy();
		m_voxels.reset();
		m_engine.getSamplerCache().remove( "VoxelizedSceneSecondaryBounce" );
		m_engine.getSamplerCache().remove( "VoxelizedSceneFirstBounce" );
	}

	void Voxelizer::update( CpuUpdater & updater )
	{
		if ( m_voxelizePass )
		{
			auto & camera = *updater.camera;
			auto & aabb = camera.getScene()->getBoundingBox();
			auto max = std::max( aabb.getDimensions()->x, std::max( aabb.getDimensions()->y, aabb.getDimensions()->z ) );
			auto cellSize = float( m_voxelConfig.gridSize.value() ) / max;
			auto voxelSize = ( cellSize * m_voxelConfig.voxelSizeFactor );
			m_grid = castor::Point4f{ 0.0f
				, 0.0f
				, 0.0f
				, voxelSize };
			m_voxelizePass->update( updater );
			m_voxelizerUbo.cpuUpdate( m_voxelConfig
				, voxelSize
				, m_voxelConfig.gridSize.value() );
		}
	}

	void Voxelizer::update( GpuUpdater & updater )
	{
		if ( m_voxelizePass )
		{
			m_voxelizePass->update( updater );
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
			, m_graph.getFinalLayoutState( m_secondaryBounce.wholeViewId ).layout
			, TextureFactors::tex3D( &m_grid ) );
		m_voxelizePass->accept( visitor );
		m_voxelToTexture->accept( visitor );
		m_voxelSecondaryBounce->accept( visitor );
	}

	crg::SemaphoreWaitArray Voxelizer::render( crg::SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )
	{
		return m_runnable->run( semaphore, queue );
	}

	crg::FramePass & Voxelizer::doCreateVoxelizePass( ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating voxelize pass" );
		auto & result = m_graph.createPass( "VoxelizePass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising voxelize pass" );
				auto res = std::make_unique< VoxelizePass >( framePass
					, context
					, runnableGraph
					, m_device
					, m_matrixUbo
					, m_culler
					, m_voxelizerUbo
					, *m_voxels
					, m_voxelConfig );
				m_voxelizePass = res.get();
				m_device.renderSystem.getEngine()->registerTimer( runnableGraph.getName() + "/Voxelizer"
					, res->getTimer() );
				return res;
			} );
		result.addOutputStorageBuffer( { m_voxels->getBuffer(), "Voxels" }
			, 0u
			, 0u
			, m_voxels->getBuffer().getSize() );
		return result;
	}

	crg::FramePass & Voxelizer::doCreateVoxelToTexture( crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating voxel buffer to texture pass" );
		auto & result = m_graph.createPass( "VoxelBufferToTexture"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising voxel buffer to texture pass" );
				auto res = std::make_unique< VoxelBufferToTexture >( framePass
					, context
					, runnableGraph
					, m_device
					, m_voxelConfig );
				m_voxelToTexture = res.get();
				m_device.renderSystem.getEngine()->registerTimer( runnableGraph.getName() + "/Voxelizer"
					, res->getTimer() );
				return res;
			} );
		result.addDependency( previousPass );
		result.addInputStorageBuffer( { m_voxels->getBuffer(), "Voxels" }
			, 0u
			, 0u
			, m_voxels->getBuffer().getSize() );
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
		stepProgressBar( progress, "Creating voxel mipmap generation pass" );
		auto & result = m_graph.createPass( name
			, [this, progress, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising voxel mipmap generation pass" );
				auto res = std::make_unique< crg::GenerateMipmaps >( framePass
					, context
					, runnableGraph
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, crg::ru::Config{}
					, crg::defaultV< crg::RunnablePass::GetPassIndexCallback >
					, isEnabled );
				m_device.renderSystem.getEngine()->registerTimer( runnableGraph.getName() + "/Voxelizer"
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
		stepProgressBar( progress, "Creating voxel secondary bounce pass" );
		auto & result = m_graph.createPass( "VoxelSecondaryBounce"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising voxel secondary bounce pass" );
				auto res = std::make_unique< VoxelSecondaryBounce >( framePass
					, context
					, runnableGraph
					, m_device
					, m_voxelConfig );
				m_voxelSecondaryBounce = res.get();
				m_device.renderSystem.getEngine()->registerTimer( runnableGraph.getName() + "/Voxelizer"
					, res->getTimer() );
				return res;
			} );
		result.addDependency( previousPass );
		result.addInOutStorageBuffer( { m_voxels->getBuffer(), "Voxels" }
			, 0u
			, 0u
			, m_voxels->getBuffer().getSize() );
		m_voxelizerUbo.createPassBinding( result
			, 1u );
		result.addSampledView( m_firstBounce.wholeViewId
			, 2u );
		result.addOutputStorageView( m_secondaryBounce.wholeViewId
			, 3u );
		return result;
	}
}
