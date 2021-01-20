#include "Castor3D/Render/Technique/Voxelize/Voxelizer.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ashespp/RenderPass/FrameBuffer.hpp>

CU_ImplementCUSmartPtr( castor3d, Voxelizer );

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
				, 1u
				, 1u
				, VK_SAMPLE_COUNT_1_BIT
				, VK_IMAGE_TILING_OPTIMAL
				, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT };
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
	}

	//*********************************************************************************************

	Voxelizer::Voxelizer( Engine & engine
		, RenderDevice const & device
		, uint32_t voxelGridSize
		, Scene & scene
		, SceneCuller & culler
		, ashes::ImageView colourView )
		: m_engine{ engine }
		, m_matrixUbo{ engine }
		, m_voxelGridSize{ voxelGridSize }
		, m_result{ createTexture( engine, device, "VoxelizedScene", { voxelGridSize, voxelGridSize, voxelGridSize } ) }
		, m_voxelizerUbo{ device.uboPools->getBuffer< VoxelizerUboConfiguration >( 0u ) }
		, m_voxelizePass{ engine
			, device
			, m_matrixUbo
			, culler
			, m_voxelizerUbo
			, m_result
			, voxelGridSize }
		, m_voxelRenderer{ device
			, m_voxelizerUbo
			, m_result
			, colourView
			, voxelGridSize }
	{
	}

	Voxelizer::~Voxelizer()
	{
	}

	void Voxelizer::update( CpuUpdater & updater )
	{
		m_voxelizePass.update( updater );
		auto & camera = m_voxelizePass.getCuller().getCamera();
		auto & voxelData = m_voxelizerUbo.getData();
		auto aabb = camera.getScene()->getBoundingBox();
		auto cellSize = std::max( std::max( aabb.getDimensions()->x
			, aabb.getDimensions()->y )
			, aabb.getDimensions()->z ) / m_voxelGridSize;

		auto const f = 0.05f;
		voxelData.center->x = floorf( camera.getParent()->getDerivedPosition()->x * f ) / f;
		voxelData.center->y = floorf( camera.getParent()->getDerivedPosition()->y * f ) / f;
		voxelData.center->z = floorf( camera.getParent()->getDerivedPosition()->z * f ) / f;
		voxelData.size = cellSize;
		voxelData.sizeInverse = 1.0f / voxelData.size;
		voxelData.resolution = float( m_voxelGridSize );
		voxelData.resolutionInverse = 1.0f / voxelData.resolution;

		voxelData.transform = ( camera.getProjection() * camera.getView() );
	}

	void Voxelizer::update( GpuUpdater & updater )
	{
		m_voxelizePass.update( updater );
	}

	ashes::Semaphore const & Voxelizer::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;
		result = &m_voxelizePass.render( device, *result );
		return *result;
	}

	ashes::Semaphore const & Voxelizer::debug( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;
		result = &m_voxelRenderer.render( device, *result );
		return *result;
	}

	void Voxelizer::accept( RenderTechniqueVisitor & visitor )
	{
		m_voxelizePass.accept( visitor );
	}
}
