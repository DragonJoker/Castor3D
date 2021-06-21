#include "Castor3D/Render/Technique/Voxelize/Voxelizer.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelBufferToTexture.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelizePass.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelSceneData.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelSecondaryBounce.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

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
		, m_voxelizePassDesc{ doCreateVoxelizePass() }
		, m_voxelToTextureDesc{ doCreateVoxelToTexture( m_voxelizePassDesc ) }
		, m_voxelMipGen{ doCreateVoxelMipGen( m_voxelToTextureDesc, "FirstBounceMip", m_firstBounce.wholeViewId ) }
		, m_voxelSecondaryBounceDesc{ doCreateVoxelSecondaryBounce( m_voxelMipGen ) }
		, m_voxelSecondaryMipGen{ doCreateVoxelMipGen( m_voxelSecondaryBounceDesc, "SecondaryBounceMip", m_secondaryBounce.wholeViewId ) }
		, m_runnable{ m_graph.compile( m_device.makeContext() ) }
	{
		m_firstBounce.create();
		m_secondaryBounce.create();
		m_runnable->record();
	}

	Voxelizer::~Voxelizer()
	{
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
			auto cellSize = m_voxelConfig.gridSize.value() / max;
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
		m_voxelizePass->accept( visitor );
	}

	crg::SemaphoreWait Voxelizer::render( crg::SemaphoreWait const & semaphore )
	{
		return m_runnable->run( semaphore, *m_device.graphicsQueue );
	}

	void Voxelizer::listIntermediates( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Voxelisation Result"
			, m_firstBounce
			, m_graph.getFinalLayout( m_firstBounce.wholeViewId ).layout
			, TextureFactors::tex3D( &m_grid ) );
		visitor.visit( "Voxelisation SecondaryBounce"
			, m_secondaryBounce
			, m_graph.getFinalLayout( m_secondaryBounce.wholeViewId ).layout
			, TextureFactors::tex3D( &m_grid ) );
	}

	crg::FramePass & Voxelizer::doCreateVoxelizePass()
	{
		auto & result = m_graph.createPass( "VoxelizePass"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< VoxelizePass >( pass
					, context
					, graph
					, m_device
					, m_matrixUbo
					, m_culler
					, m_voxelizerUbo
					, *m_voxels
					, m_voxelConfig );
				m_voxelizePass = result.get();
				m_device.renderSystem.getEngine()->registerTimer( "Voxelizer"
					, result->getTimer() );
				return result;
			} );
		result.addOutputStorageBuffer( { m_voxels->getBuffer(), "Voxels" }
			, 0u
			, 0u
			, m_voxels->getBuffer().getSize() );
		return result;
	}

	crg::FramePass & Voxelizer::doCreateVoxelToTexture( crg::FramePass const & previousPass )
	{
		auto & result = m_graph.createPass( "VoxelBufferToTexture"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< VoxelBufferToTexture >( pass
					, context
					, graph
					, m_device
					, m_voxelConfig );
				m_voxelToTexture = result.get();
				m_device.renderSystem.getEngine()->registerTimer( "Voxelizer"
					, result->getTimer() );
				return result;
			} );
		result.addDependency( previousPass );
		result.addInputStorageBuffer( { m_voxels->getBuffer(), "Voxels" }
			, 0u
			, 0u
			, m_voxels->getBuffer().getSize() );
		result.addOutputStorageView( m_firstBounce.wholeViewId
			, 1u
			, VK_IMAGE_LAYOUT_UNDEFINED );
		return result;
	}

	crg::FramePass & Voxelizer::doCreateVoxelMipGen( crg::FramePass const & previousPass
		, std::string const & name
		, crg::ImageViewId const & view )
	{
		auto & result = m_graph.createPass( name
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< crg::GenerateMipmaps >( pass
					, context
					, graph
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
				m_device.renderSystem.getEngine()->registerTimer( "Voxelizer"
					, result->getTimer() );
				return result;
			} );
		result.addDependency( previousPass );
		result.addTransferInOutView( view );
		return result;
	}

	crg::FramePass & Voxelizer::doCreateVoxelSecondaryBounce( crg::FramePass const & previousPass )
	{
		auto & result = m_graph.createPass( "VoxelSecondaryBounce"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< VoxelSecondaryBounce >( pass
					, context
					, graph
					, m_device
					, m_voxelConfig );
				m_voxelSecondaryBounce = result.get();
				m_device.renderSystem.getEngine()->registerTimer( "Voxelizer"
					, result->getTimer() );
				return result;
			} );
		result.addDependency( previousPass );
		result.addInOutStorageBuffer( { m_voxels->getBuffer(), "Voxels" }
			, 0u
			, 0u
			, m_voxels->getBuffer().getSize() );
		m_voxelizerUbo.createPassBinding( result
			, 1u );
		result.addSampledView( m_firstBounce.wholeViewId
			, 2u
			, VK_IMAGE_LAYOUT_UNDEFINED );
		result.addOutputStorageView( m_secondaryBounce.wholeViewId
			, 3u
			, VK_IMAGE_LAYOUT_UNDEFINED );
		return result;
	}
}
