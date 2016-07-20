#include "LightCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Shader/FrameVariableBuffer.hpp"
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
				auto l_it = m_typeSortedLights.insert( { p_element->GetLightType(), LightsArray() } ).first;
				bool l_found = std::binary_search( l_it->second.begin(), l_it->second.end(), p_element );

				if ( !l_found )
				{
					l_it->second.push_back( p_element );
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
				auto l_itMap = m_typeSortedLights.find( p_element->GetLightType() );

				if ( l_itMap != m_typeSortedLights.end() )
				{
					auto l_it = std::find( l_itMap->second.begin(), l_itMap->second.end(), p_element );

					if ( l_it != l_itMap->second.end() )
					{
						l_itMap->second.erase( l_it );
					}
				}
			}

			LightsMap & m_typeSortedLights;
		};
	}

	LightCache::ObjectCache( Engine & p_engine
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
		, m_lightsTexture{ std::make_shared< TextureUnit >( *GetEngine() ) }
	{
	}

	LightCache::~ObjectCache()
	{
		m_lightsTexture.reset();
	}

	void LightCache::Initialise()
	{
		auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::Buffer, eACCESS_TYPE_WRITE, eACCESS_TYPE_READ );
		l_texture->GetImage().SetSource( Size( 1000, 1 ), ePIXEL_FORMAT_ARGB32F );
		SamplerSPtr l_sampler = GetEngine()->GetLightsSampler();
		m_lightsTexture->SetAutoMipmaps( false );
		m_lightsTexture->SetSampler( l_sampler );
		m_lightsTexture->SetTexture( l_texture );
		GetEngine()->PostEvent( MakeInitialiseEvent( *m_lightsTexture ) );
	}

	void LightCache::Cleanup()
	{
		GetEngine()->PostEvent( MakeCleanupEvent( *m_lightsTexture ) );
		MyObjectCache::Cleanup();
	}

	void LightCache::BindLights( ShaderProgram const & p_program, FrameVariableBuffer const & p_sceneBuffer )const
	{
		GetEngine()->GetRenderSystem()->RenderAmbientLight( GetScene()->GetAmbientLight(), p_sceneBuffer );
		OneIntFrameVariableSPtr l_lights = p_program.FindFrameVariable< OneIntFrameVariable >( ShaderProgram::Lights, eSHADER_TYPE_PIXEL );
		Point4iFrameVariableSPtr l_lightsCount;
		p_sceneBuffer.GetVariable( ShaderProgram::LightsCount, l_lightsCount );

		if ( l_lights && l_lightsCount )
		{
			auto l_layout = m_lightsTexture->GetTexture();

			if ( l_layout )
			{
				auto const & l_image = l_layout->GetImage();
				auto l_buffer = l_image.GetBuffer();
				l_lights->SetValue( m_lightsTexture->GetIndex() );
				int l_index = 0;

				for ( auto l_it : m_typeSortedLights )
				{
					l_lightsCount->GetValue( 0 )[l_it.first] += uint32_t( l_it.second.size() );

					for ( auto l_light : l_it.second )
					{
						l_light->Bind( *l_buffer, l_index++ );
					}
				}

				auto l_locked = l_layout->GetImage().Lock( eACCESS_TYPE_WRITE );

				if ( l_locked )
				{
					memcpy( l_locked, l_image.GetBuffer()->const_ptr(), l_image.GetBuffer()->size() );
				}

				l_layout->GetImage().Unlock( true );
			}

			m_lightsTexture->Bind();
		}
	}

	void LightCache::UnbindLights( ShaderProgram const & p_program, FrameVariableBuffer const & p_sceneBuffer )const
	{
		Point4iFrameVariableSPtr l_lightsCount;
		p_sceneBuffer.GetVariable( ShaderProgram::LightsCount, l_lightsCount );

		if ( l_lightsCount )
		{
			m_lightsTexture->Unbind();
			int l_index = 0;

			for ( auto l_it : m_typeSortedLights )
			{
				l_lightsCount->GetValue( 0 )[l_it.first] -= uint32_t( l_it.second.size() );
			}
		}
	}
}
