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
#include "Texture/TextureView.hpp"

#include <Design/ArrayView.hpp>

using namespace castor;

namespace castor3d
{
	template<> const String ObjectCacheTraits< Light, String >::Name = cuT( "Light" );

	namespace
	{
		class LightInitialiser
		{
		public:
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

		private:
			LightsMap & m_typeSortedLights;
		};

		class LightCleaner
		{
		public:
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

		private:
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
		m_dirtyLights.clear();
		m_connections.clear();
		MyObjectCache::cleanup();
	}

	ObjectCache< Light, castor::String >::ElementPtr ObjectCache< Light, castor::String >::add( Key const & p_name, ElementPtr p_element )
	{
		ElementPtr result{ p_element };

		if ( p_element )
		{
			auto lock = castor::makeUniqueLock( m_elements );

			if ( m_elements.has( p_name ) )
			{
				castor::Logger::logWarning( castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << getObjectTypeName() << cuT( ": " ) << p_name );
				result = m_elements.find( p_name );
				m_dirtyLights.emplace_back( result.get() );
				m_connections.emplace( result.get()
					, result->onChanged.connect( std::bind( &ObjectCache< Light, String >::onLightChanged
						, this
						, std::placeholders::_1 ) ) );
			}
			else
			{
				m_elements.insert( p_name, p_element );
				onChanged();
			}
		}
		else
		{
			castor::Logger::logWarning( castor::StringStream() << WARNING_CACHE_NULL_OBJECT << getObjectTypeName() << cuT( ": " ) );
		}

		return result;
	}

	ObjectCache< Light, castor::String >::ElementPtr ObjectCache< Light, String >::add( Key const & p_name, SceneNodeSPtr p_parent, LightType p_type )
	{
		auto lock = castor::makeUniqueLock( m_elements );
		ElementPtr result;

		if ( !m_elements.has( p_name ) )
		{
			result = m_produce( p_name, p_parent, p_type );
			m_initialise( result );
			m_elements.insert( p_name, result );
			m_attach( result, p_parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
			castor::Logger::logDebug( castor::StringStream() << INFO_CACHE_CREATED_OBJECT << getObjectTypeName() << cuT( ": " ) << p_name );
			m_dirtyLights.emplace_back( result.get() );
			m_connections.emplace( result.get()
				, result->onChanged.connect( std::bind( &ObjectCache< Light, String >::onLightChanged
					, this
					, std::placeholders::_1 ) ) );
			onChanged();
		}
		else
		{
			result = m_elements.find( p_name );
			castor::Logger::logWarning( castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << getObjectTypeName() << cuT( ": " ) << p_name );
		}

		return result;
	}

	void ObjectCache< Light, castor::String >::remove( Key const & p_name )
	{
		auto lock = castor::makeUniqueLock( m_elements );

		if ( m_elements.has( p_name ) )
		{
			auto element = m_elements.find( p_name );
			m_detach( element );
			m_connections.erase( element.get() );
			m_elements.erase( p_name );
			onChanged();
		}
	}

	void ObjectCache< Light, castor::String >::update()
	{
		if ( !m_dirtyLights.empty() )
		{
			LightsRefArray dirty;
			std::swap( m_dirtyLights, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin()
				, end
				, []( Light * light )
				{
					light->update();
				} );
		}
	}

	void ObjectCache< Light, castor::String >::updateLightsTexture( Camera const & camera )const
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
						|| camera.isVisible( light->getBoundingBox()
							, light->getParent()->getDerivedTransformationMatrix() ) )
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

	void ObjectCache< Light, castor::String >::onLightChanged( Light & light )
	{
		m_dirtyLights.emplace_back( &light );
	}
}
