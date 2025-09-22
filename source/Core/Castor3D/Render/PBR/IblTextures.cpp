#include "Castor3D/Render/PBR/IblTextures.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( c3d, IblTextures )

namespace c3d
{
	namespace ibltex
	{
		static SamplerObs doCreateSampler( Engine & engine )
		{
			auto name = cuT( "IblTexturesBRDF" );
			auto result = engine.tryFindSampler( name );

			if ( !result )
			{
				auto created = engine.createSampler( name, engine );
				created->setMinFilter( FilterMode::eLinear );
				created->setMagFilter( FilterMode::eLinear );
				created->setWrapS( WrapMode::eClampToEdge );
				created->setWrapT( WrapMode::eClampToEdge );
				created->setWrapR( WrapMode::eClampToEdge );
				created->setSerialisable( false );
				result = engine.addSampler( name, created, false );
			}

			result->initialise( engine.getRenderSystem()->getRenderDevice() );
			return result;
		}
	}

	//************************************************************************************************

	IblTextures::IblTextures( Scene & scene
		, RenderDevice const & device
		, Texture const & source
		, Texture const & brdf
		, SamplerObs sampler )
		: OwnedBy< Scene >{ scene }
		, m_brdf{ brdf }
		, m_sampler{ ibltex::doCreateSampler( *scene.getEngine() ) }
		, m_radianceComputer{ *scene.getEngine()
			, device
			, Size{ RadianceMapSize, RadianceMapSize }
			, source }
		, m_environmentPrefilter{ *scene.getEngine()
			, device
			, Size{ PrefilteredEnvironmentMapSize, PrefilteredEnvironmentMapSize }
			, source
			, sampler
			, false }
		, m_environmentSheenPrefilter{ *scene.getEngine()
			, device
			, Size{ PrefilteredEnvironmentMapSize, PrefilteredEnvironmentMapSize }
			, source
			, sampler
			, true }
	{
	}

	void IblTextures::update( QueueData const & queueData )const
	{
		m_radianceComputer.render( queueData );
		m_environmentPrefilter.render( queueData );
		m_environmentSheenPrefilter.render( queueData );
	}

	SemaphoreWaitArray IblTextures::update( SemaphoreWaitArray signalsToWait
		, ashes::Queue const & queue )const
	{
		signalsToWait = m_radianceComputer.render( signalsToWait, queue );
		signalsToWait = m_environmentPrefilter.render( signalsToWait, queue );
		signalsToWait = m_environmentSheenPrefilter.render( signalsToWait, queue );
		return signalsToWait;
	}
}
