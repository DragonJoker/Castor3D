#include "Castor3D/Render/Technique/Voxelize/Voxelizer.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
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

CU_ImplementCUSmartPtr( castor3d, Voxelizer )

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		TextureUnit createTexture( Engine & engine
			, RenderDevice const & device
			, String const & name
			, VkExtent3D const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			ashes::ImageCreateInfo image{ VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT
				, VK_IMAGE_TYPE_3D
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, size
				, getMipLevels( size )
				, 1u
				, VK_SAMPLE_COUNT_1_BIT
				, VK_IMAGE_TILING_OPTIMAL
				, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT };
			auto layout = std::make_shared< TextureLayout >( renderSystem
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			TextureUnit result{ engine };
			result.setSampler( createSampler( engine, name, VK_FILTER_NEAREST, nullptr ) );
			result.setTexture( layout );
			result.initialise( device );
			return result;
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

	Voxelizer::Voxelizer( Engine & engine
		, RenderDevice const & device
		, Scene & scene
		, Camera & camera
		, ashes::ImageView colourView
		, MatrixUbo & matrixUbo
		, VoxelizerUbo & voxelizerUbo
		, VoxelSceneData const & voxelConfig )
		: m_engine{ engine }
		, m_voxelConfig{ voxelConfig }
		, m_culler{ scene, &camera }
		, m_matrixUbo{ engine }
		, m_firstBounce{ createTexture( engine, device, "VoxelizedSceneFirstBounce", { m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value() } ) }
		, m_secondaryBounce{ createTexture( engine, device, "VoxelizedSceneSecondaryBounce", { m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value(), m_voxelConfig.gridSize.value() } ) }
		, m_voxels{ createSsbo( engine, device, "VoxelizedSceneBuffer", m_voxelConfig.gridSize.value() ) }
		, m_voxelizerUbo{ voxelizerUbo }
		, m_voxelizePass{ castor::makeUnique< VoxelizePass >( engine
			, device
			, m_matrixUbo
			, m_culler
			, m_voxelizerUbo
			, *m_voxels
			, m_voxelConfig ) }
		, m_voxelToTexture{ castor::makeUnique< VoxelBufferToTexture >( device
			, m_voxelConfig
			, *m_voxels
			, m_firstBounce ) }
		, m_voxelSecondaryBounce{ castor::makeUnique< VoxelSecondaryBounce >( device
			, m_voxelConfig
			, *m_voxels
			, m_voxelizerUbo
			, m_firstBounce
			, m_secondaryBounce ) }
	{
	}

	Voxelizer::~Voxelizer()
	{
	}

	void Voxelizer::update( CpuUpdater & updater )
	{
		auto & camera = *updater.camera;
		auto & aabb = camera.getScene()->getBoundingBox();
		auto max = std::max( aabb.getDimensions()->x, std::max( aabb.getDimensions()->y, aabb.getDimensions()->z ) );
		auto cellSize = 2.0f * m_voxelConfig.gridSize.value() / max;
		auto voxelSize = ( cellSize * m_voxelConfig.voxelSizeFactor );
		float f = 0.05f / voxelSize;
		m_grid = castor::Point4f{ 0.0f
			, 0.0f
			, 0.0f
			, voxelSize };
		m_voxelizePass->update( updater );
		m_voxelizerUbo.cpuUpdate( m_voxelConfig
			, voxelSize
			, m_voxelConfig.gridSize.value() );
	}

	void Voxelizer::update( GpuUpdater & updater )
	{
		m_voxelizePass->update( updater );
	}

	ashes::Semaphore const & Voxelizer::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;
		result = &m_voxelizePass->render( device, *result );
		result = &m_voxelToTexture->render( device, *result );

		if ( m_voxelConfig.enableSecondaryBounce )
		{
			result = &m_voxelSecondaryBounce->render( device, *result );
		}

		return *result;
	}

	void Voxelizer::accept( RenderTechniqueVisitor & visitor )
	{
		m_voxelizePass->accept( visitor );
	}

	void Voxelizer::listIntermediates( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Voxelisation Result"
			, m_firstBounce.getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors::tex3D( &m_grid ) );
		visitor.visit( "Voxelisation SecondaryBounce"
			, m_secondaryBounce.getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors::tex3D( &m_grid ) );
	}
}
