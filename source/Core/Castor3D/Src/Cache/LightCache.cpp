#include "LightCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Castor3DPrerequisites.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureView.hpp"

#include <Core/Device.hpp>
#include <Image/Texture.hpp>

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
			explicit LightInitialiser( LightsMap & typeSortedLights )
				: m_typeSortedLights{ typeSortedLights }
			{
			}

			inline void operator()( LightSPtr element )
			{
				auto index = size_t( element->getLightType() );
				auto it = std::find( m_typeSortedLights[index].begin()
					, m_typeSortedLights[index].end()
					, element );

				if ( it == m_typeSortedLights[index].end() )
				{
					m_typeSortedLights[index].push_back( element );
				}
			}

		private:
			LightsMap & m_typeSortedLights;
		};

		class LightCleaner
		{
		public:
			explicit LightCleaner( LightsMap & typeSortedLights )
				: m_typeSortedLights{ typeSortedLights }
			{
			}

			inline void operator()( LightSPtr element )
			{
				auto index = size_t( element->getLightType() );
				auto it = std::find( m_typeSortedLights[index].begin()
					, m_typeSortedLights[index].end()
					, element );

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
		, Scene & scene
		, SceneNodeSPtr rootNode
		, SceneNodeSPtr rootCameraNode
		, SceneNodeSPtr rootObjectNode
		, Producer && produce
		, Initialiser && initialise
		, Cleaner && clean
		, Merger && merge
		, Attacher && attach
		, Detacher && detach )
		: MyObjectCache( engine
			, scene
			, rootNode
			, rootCameraNode
			, rootObjectNode
			, std::move( produce )
			, std::bind( LightInitialiser{ m_typeSortedLights }, std::placeholders::_1 )
			, std::bind( LightCleaner{ m_typeSortedLights }, std::placeholders::_1 )
			, std::move( merge )
			, std::move( attach )
			, std::move( detach ) )
	{
	}

	ObjectCache< Light, castor::String >::~ObjectCache()
	{
	}

	void ObjectCache< Light, castor::String >::initialise()
	{
		m_lightsBuffer.resize( 300u * shader::MaxLightComponentsCount );
		m_scene.getListener().postEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				auto & device = getCurrentDevice( *getScene() );
				m_textureBuffer = renderer::makeBuffer< castor::Point4f >( device
					, uint32_t( m_lightsBuffer.size() )
					, renderer::BufferTarget::eUniformTexelBuffer | renderer::BufferTarget::eTransferDst
					, renderer::MemoryPropertyFlag::eHostVisible );
				m_textureView = device.createBufferView( m_textureBuffer->getBuffer()
					, renderer::Format::eR32G32B32A32_SFLOAT
					, 0u
					, uint32_t( m_lightsBuffer.size() * sizeof( Point4f ) ) );
			} ) );
	}

	void ObjectCache< Light, castor::String >::cleanup()
	{
		m_scene.getListener().postEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				m_textureView.reset();
				m_textureBuffer.reset();
			} ) );
		m_dirtyLights.clear();
		m_connections.clear();
		MyObjectCache::cleanup();
	}

	ObjectCache< Light, castor::String >::ElementPtr ObjectCache< Light, castor::String >::add( Key const & name, ElementPtr element )
	{
		ElementPtr result{ element };

		if ( element )
		{
			auto lock = castor::makeUniqueLock( m_elements );

			if ( m_elements.has( name ) )
			{
				castor::Logger::logWarning( castor::makeStringStream() << WARNING_CACHE_DUPLICATE_OBJECT << getObjectTypeName() << cuT( ": " ) << name );
				result = m_elements.find( name );
				m_dirtyLights.emplace_back( result.get() );
				m_connections.emplace( result.get()
					, result->onChanged.connect( [this]( Light & light )
						{
							onLightChanged( light );
						} ) );
			}
			else
			{
				m_elements.insert( name, element );
				onChanged();
			}
		}
		else
		{
			castor::Logger::logWarning( castor::makeStringStream() << WARNING_CACHE_NULL_OBJECT << getObjectTypeName() << cuT( ": " ) );
		}

		return result;
	}

	ObjectCache< Light, castor::String >::ElementPtr ObjectCache< Light, String >::add( Key const & name, SceneNodeSPtr parent, LightType type )
	{
		auto lock = castor::makeUniqueLock( m_elements );
		ElementPtr result;

		if ( !m_elements.has( name ) )
		{
			result = m_produce( name, parent, type );
			m_initialise( result );
			m_elements.insert( name, result );
			m_attach( result, parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
			castor::Logger::logDebug( castor::makeStringStream() << INFO_CACHE_CREATED_OBJECT << getObjectTypeName() << cuT( ": " ) << name );
			m_dirtyLights.emplace_back( result.get() );
			m_connections.emplace( result.get()
				, result->onChanged.connect( [this]( Light & light )
					{
						onLightChanged( light );
					} ) );
			onChanged();
		}
		else
		{
			result = m_elements.find( name );
			castor::Logger::logWarning( castor::makeStringStream() << WARNING_CACHE_DUPLICATE_OBJECT << getObjectTypeName() << cuT( ": " ) << name );
		}

		return result;
	}

	void ObjectCache< Light, castor::String >::remove( Key const & name )
	{
		auto lock = castor::makeUniqueLock( m_elements );

		if ( m_elements.has( name ) )
		{
			auto element = m_elements.find( name );
			m_detach( element );
			m_connections.erase( element.get() );
			m_elements.erase( name );
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
		int index = 0;
		Point4f * data = m_lightsBuffer.data();

		for ( auto lights : m_typeSortedLights )
		{
			for ( auto light : lights )
			{
				if ( light->getLightType() == LightType::eDirectional
					|| camera.isVisible( light->getBoundingBox()
						, light->getParent()->getDerivedTransformationMatrix() ) )
				{
					light->bind( data );
					data += shader::MaxLightComponentsCount;
				}
			}
		}

		if ( auto * locked = m_textureBuffer->lock( 0u
			, m_textureBuffer->getCount()
			, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateBuffer ) )
		{
			std::copy( m_lightsBuffer.begin(), m_lightsBuffer.end(), locked );
			m_textureBuffer->flush( 0u, m_textureBuffer->getCount() );
			m_textureBuffer->unlock();
		}
	}

	void ObjectCache< Light, castor::String >::onLightChanged( Light & light )
	{
		m_dirtyLights.emplace_back( &light );
	}
}
