#include "Castor3D/Render/Technique/Voxelize/Voxelizer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"

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
			ashes::ImageCreateInfo image
			{
				VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT,
				VK_IMAGE_TYPE_3D,
				VK_FORMAT_R8_UNORM,
				size,
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			};
			auto layout = std::make_shared< TextureLayout >( renderSystem
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			layout->initialise( device );
			TextureUnit result{ engine };
			result.setSampler( createSampler( engine, name, VK_FILTER_NEAREST, nullptr ) );
			result.setTexture( layout );
			return result;
		}
	}

	//*********************************************************************************************

	Voxelizer::Voxelizer( Engine & engine
		, RenderDevice const & device
		, VkExtent3D const & resultDimensions
		, Scene & scene
		, SceneCuller & culler
		, ashes::ImageView colourView )
		: m_engine{ engine }
		, m_matrixUbo{ engine }
		, m_size{ resultDimensions }
		, m_result{ createTexture( engine, device, "VoxelizerResult", resultDimensions ) }
		, m_voxelizePass{ engine
			, m_matrixUbo
			, culler
			, m_result.getTexture()
			, colourView }
	{
		m_voxelizePass.initialise( device, { resultDimensions.width, resultDimensions.height } );
	}

	Voxelizer::~Voxelizer()
	{
	}

	void Voxelizer::update( CpuUpdater & updater )
	{
		m_voxelizePass.update( updater );
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

	void Voxelizer::accept( RenderTechniqueVisitor & visitor )
	{
		m_voxelizePass.accept( visitor );
	}
}
