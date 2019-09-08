#include "Castor3D/Cache/LightCache.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/InitialiseEvent.hpp"
#include "Castor3D/Event/Frame/CleanupEvent.hpp"
#include "Castor3D/Material/Pass.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Castor3DPrerequisites.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureView.hpp"

#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/Image.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static String const C3D_UniqueDirectionalLight = cuT( "Only one directional light is allowed." );
	}

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
				auto & device = getCurrentRenderDevice( *getScene() );
				m_textureBuffer = makeBuffer< castor::Point4f >( device
					, uint32_t( m_lightsBuffer.size() )
					, VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, "LightsBuffer" );
				m_textureView = device->createBufferView( m_textureBuffer->getBuffer()
					, VK_FORMAT_R32G32B32A32_SFLOAT
					, 0u
					, uint32_t( m_lightsBuffer.size() * sizeof( Point4f ) ) );
				setDebugObjectName( device, *m_textureView, "LightsBufferView" );
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
			if ( doCheckUniqueDirectionalLight( element->getLightType() ) )
			{
				auto lock = castor::makeUniqueLock( m_elements );

				if ( m_elements.has( name ) )
				{
					doReportDuplicate( name );
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
		}
		else
		{
			doReportNull();
		}

		return result;
	}

	ObjectCache< Light, castor::String >::ElementPtr ObjectCache< Light, String >::add( Key const & name, SceneNodeSPtr parent, LightType type )
	{
		auto lock = castor::makeUniqueLock( m_elements );
		ElementPtr result;

		if ( !m_elements.has( name ) )
		{
			if ( doCheckUniqueDirectionalLight( type ) )
			{
				result = m_produce( name, parent, type );
				m_initialise( result );
				m_elements.insert( name, result );
				m_attach( result, parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
				doReportCreation( name );
				m_dirtyLights.emplace_back( result.get() );
				m_connections.emplace( result.get()
					, result->onChanged.connect( [this]( Light & light )
						{
							onLightChanged( light );
						} ) );
				onChanged();
			}
		}
		else
		{
			result = m_elements.find( name );
			doReportDuplicate( name );
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
		uint32_t index = 0;
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
					index += shader::MaxLightComponentsCount;
				}
			}
		}

		if ( index )
		{
			if ( auto * locked = m_textureBuffer->lock( 0u
				, index
				, 0u ) )
			{
				std::copy( m_lightsBuffer.begin(), m_lightsBuffer.begin() + index, locked );
				m_textureBuffer->flush( 0u, index );
				m_textureBuffer->unlock();
			}
		}
	}

	void ObjectCache< Light, castor::String >::onLightChanged( Light & light )
	{
		m_dirtyLights.emplace_back( &light );
	}

	bool ObjectCache< Light, castor::String >::doCheckUniqueDirectionalLight( LightType toAdd )
	{
		bool result = toAdd != LightType::eDirectional
			|| getLightsCount( LightType::eDirectional ) == 0u;

		if ( !result )
		{
			castor::Logger::logError( C3D_UniqueDirectionalLight );
		}

		return result;
	}
}
