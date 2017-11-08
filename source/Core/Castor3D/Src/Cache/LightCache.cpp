#include "LightCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureImage.hpp"

using namespace castor;

namespace castor3d
{
	template<> const String ObjectCacheTraits< Light, String >::Name = cuT( "Light" );

	namespace
	{
		struct LightInitialiser
		{
			explicit LightInitialiser( LightsMap & p_typeSortedLights )
				: m_typeSortedLights{ p_typeSortedLights }
			{
			}

			inline void operator()( LightSPtr p_element )
			{
				auto index = size_t( p_element->getLightType() );
				auto it = std::find( m_typeSortedLights[index].begin()
					, m_typeSortedLights[index].end()
					, p_element );

				if ( it == m_typeSortedLights[index].end() )
				{
					m_typeSortedLights[index].push_back( p_element );
				}
			}

			LightsMap & m_typeSortedLights;
		};

		struct LightCleaner
		{
			explicit LightCleaner( LightsMap & p_typeSortedLights )
				: m_typeSortedLights{ p_typeSortedLights }
			{
			}

			inline void operator()( LightSPtr p_element )
			{
				auto index = size_t( p_element->getLightType() );
				auto it = std::find( m_typeSortedLights[index].begin()
					, m_typeSortedLights[index].end()
					, p_element );

				if ( it != m_typeSortedLights[index].end() )
				{
					m_typeSortedLights[index].erase( it );
				}
			}

			LightsMap & m_typeSortedLights;
		};
	}

	ObjectCache< Light, castor::String >::ObjectCache( Engine & engine
		, Scene & p_scene
		, SceneNodeSPtr p_rootNode
		, SceneNodeSPtr p_rootCameraNode
		, SceneNodeSPtr p_rootObjectNode
		, Producer && p_produce
		, Initialiser && p_initialise
		, Cleaner && p_clean
		, Merger && p_merge
		, Attacher && p_attach
		, Detacher && p_detach )
		: MyObjectCache( engine
			, p_scene
			, p_rootNode
			, p_rootCameraNode
			, p_rootObjectNode
			, std::move( p_produce )
			, std::bind( LightInitialiser{ m_typeSortedLights }, std::placeholders::_1 )
			, std::bind( LightCleaner{ m_typeSortedLights }, std::placeholders::_1 )
			, std::move( p_merge )
			, std::move( p_attach )
			, std::move( p_detach ) )
		, m_lightsTexture{ *getEngine() }
	{
	}

	ObjectCache< Light, castor::String >::~ObjectCache()
	{
	}

	void ObjectCache< Light, castor::String >::initialise()
	{
		auto texture = getEngine()->getRenderSystem()->createTexture( TextureType::eBuffer
			, AccessType::eWrite
			, AccessType::eRead
			, PixelFormat::eRGBA32F
			, Size( 1000, 1 ) );
		texture->getImage().initialiseSource();
		SamplerSPtr sampler = getEngine()->getLightsSampler();
		m_lightsTexture.setAutoMipmaps( false );
		m_lightsTexture.setSampler( sampler );
		m_lightsTexture.setTexture( texture );
		m_lightsTexture.setIndex( LightBufferIndex );
		m_scene.getListener().postEvent( makeInitialiseEvent( m_lightsTexture ) );
		m_lightsBuffer = texture->getImage().getBuffer();
	}

	void ObjectCache< Light, castor::String >::cleanup()
	{
		m_scene.getListener().postEvent( makeCleanupEvent( m_lightsTexture ) );
		MyObjectCache::cleanup();
	}

	void ObjectCache< Light, castor::String >::updateLights( Camera const & camera )const
	{
		auto layout = m_lightsTexture.getTexture();

		if ( layout )
		{
			auto const & image = layout->getImage();
			int index = 0;

			for ( auto lights : m_typeSortedLights )
			{
				for ( auto light : lights )
				{
					if ( light->getLightType() == LightType::eDirectional
						|| camera.isVisible( light->getCubeBox(), light->getParent()->getDerivedTransformationMatrix() ) )
					{
						light->bind( *m_lightsBuffer, index++ );
					}
				}
			}

			auto locked = layout->lock( AccessType::eWrite );

			if ( locked )
			{
				memcpy( locked, m_lightsBuffer->constPtr(), m_lightsBuffer->size() );
			}

			layout->unlock( true );
		}
	}

	void ObjectCache< Light, castor::String >::bindLights()const
	{
		m_lightsTexture.bind();
	}

	void ObjectCache< Light, castor::String >::unbindLights()const
	{
		m_lightsTexture.unbind();
	}
}
