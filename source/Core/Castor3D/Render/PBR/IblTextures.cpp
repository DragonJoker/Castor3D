#include "Castor3D/Render/PBR/IblTextures.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Image.hpp>

#include <RenderGraph/ResourceHandler.hpp>

namespace castor3d
{
	namespace ibltex
	{
		static SamplerResPtr doCreateSampler( Engine & engine
			, RenderDevice const & device )
		{
			auto name = cuT( "IblTexturesBRDF" );
			auto result = engine.tryFindSampler( name );

			if ( !result.lock() )
			{
				auto created = engine.createSampler( name, engine );
				created->setMinFilter( VK_FILTER_LINEAR );
				created->setMagFilter( VK_FILTER_LINEAR );
				created->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result = engine.addSampler( name, created, false );
			}

			result.lock()->initialise( engine.getRenderSystem()->getRenderDevice() );
			return result;
		}
	}

	//************************************************************************************************

	IblTextures::IblTextures( Scene & scene
		, RenderDevice const & device
		, Texture const & source
		, Texture const & brdf
		, SamplerResPtr sampler )
		: OwnedBy< Scene >{ scene }
		, m_brdf{ brdf }
		, m_sampler{ ibltex::doCreateSampler( *scene.getEngine(), device ) }
		, m_radianceComputer{ *scene.getEngine()
			, device
			, castor::Size{ RadianceMapSize, RadianceMapSize }
			, source }
		, m_environmentPrefilter{ *scene.getEngine()
			, device
			, castor::Size{ PrefilteredEnvironmentMapSize, PrefilteredEnvironmentMapSize }
			, source
			, sampler
			, false }
		, m_environmentSheenPrefilter{ *scene.getEngine()
			, device
			, castor::Size{ PrefilteredEnvironmentMapSize, PrefilteredEnvironmentMapSize }
			, source
			, sampler
			, true }
	{
	}

	IblTextures::~IblTextures()
	{
	}

	void IblTextures::update( QueueData const & queueData )
	{
		m_radianceComputer.render( queueData );
		m_environmentPrefilter.render( queueData );
		m_environmentSheenPrefilter.render( queueData );
	}

	crg::SemaphoreWaitArray IblTextures::update( crg::SemaphoreWaitArray signalsToWait
		, ashes::Queue const & queue )const
	{
		signalsToWait = m_radianceComputer.render( signalsToWait, queue );
		signalsToWait = m_environmentPrefilter.render( signalsToWait, queue );
		signalsToWait = m_environmentSheenPrefilter.render( signalsToWait, queue );
		return signalsToWait;
	}
}
