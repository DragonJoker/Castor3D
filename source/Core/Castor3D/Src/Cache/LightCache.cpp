#include "LightCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureImage.hpp"

using namespace Castor;

namespace Castor3D
{
	template<> const String ObjectCacheTraits< Light, String >::Name = cuT( "Light" );

	namespace
	{
		struct LightInitialiser
		{
			LightInitialiser( LightsMap & p_typeSortedLights )
				: m_typeSortedLights{ p_typeSortedLights }
			{
			}

			inline void operator()( LightSPtr p_element )
			{
				auto l_index = size_t( p_element->GetLightType() );
				auto l_it = std::find( m_typeSortedLights[l_index].begin()
					, m_typeSortedLights[l_index].end()
					, p_element );

				if ( l_it == m_typeSortedLights[l_index].end() )
				{
					m_typeSortedLights[l_index].push_back( p_element );
				}
			}

			LightsMap & m_typeSortedLights;
		};

		struct LightCleaner
		{
			LightCleaner( LightsMap & p_typeSortedLights )
				: m_typeSortedLights{ p_typeSortedLights }
			{
			}

			inline void operator()( LightSPtr p_element )
			{
				auto l_index = size_t( p_element->GetLightType() );
				auto l_it = std::find( m_typeSortedLights[l_index].begin()
					, m_typeSortedLights[l_index].end()
					, p_element );

				if ( l_it != m_typeSortedLights[l_index].end() )
				{
					m_typeSortedLights[l_index].erase( l_it );
				}
			}

			LightsMap & m_typeSortedLights;
		};
	}

	ObjectCache< Light, Castor::String >::ObjectCache( Engine & p_engine
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
		: MyObjectCache( p_engine
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
		, m_lightsTexture{ *GetEngine() }
	{
	}

	ObjectCache< Light, Castor::String >::~ObjectCache()
	{
	}

	void ObjectCache< Light, Castor::String >::Initialise()
	{
		auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eBuffer
			, AccessType::eWrite
			, AccessType::eRead
			, PixelFormat::eRGBA32F
			, Size( 1000, 1 ) );
		l_texture->GetImage().InitialiseSource();
		SamplerSPtr l_sampler = GetEngine()->GetLightsSampler();
		m_lightsTexture.SetAutoMipmaps( false );
		m_lightsTexture.SetSampler( l_sampler );
		m_lightsTexture.SetTexture( l_texture );
		m_lightsTexture.SetIndex( Pass::LightBufferIndex );
		m_scene.GetListener().PostEvent( MakeInitialiseEvent( m_lightsTexture ) );
		m_lightsBuffer = l_texture->GetImage().GetBuffer();
	}

	void ObjectCache< Light, Castor::String >::Cleanup()
	{
		m_scene.GetListener().PostEvent( MakeCleanupEvent( m_lightsTexture ) );
		MyObjectCache::Cleanup();
	}

	void ObjectCache< Light, Castor::String >::UpdateLights()const
	{
		auto l_layout = m_lightsTexture.GetTexture();

		if ( l_layout )
		{
			auto const & l_image = l_layout->GetImage();
			int l_index = 0;

			for ( auto l_lights : m_typeSortedLights )
			{
				for ( auto l_light : l_lights )
				{
					l_light->Bind( *m_lightsBuffer, l_index++ );
				}
			}

			auto l_locked = l_layout->Lock( AccessType::eWrite );

			if ( l_locked )
			{
				memcpy( l_locked, m_lightsBuffer->const_ptr(), m_lightsBuffer->size() );
			}

			l_layout->Unlock( true );
		}
	}

	void ObjectCache< Light, Castor::String >::BindLights()const
	{
		m_lightsTexture.Bind();
	}

	void ObjectCache< Light, Castor::String >::UnbindLights()const
	{
		m_lightsTexture.Unbind();
	}
}
