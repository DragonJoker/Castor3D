#include "LightManager.hpp"

#include "DynamicTexture.hpp"
#include "FrameVariableBuffer.hpp"
#include "RenderSystem.hpp"
#include "SceneManager.hpp"
#include "ShaderProgram.hpp"
#include "TextureUnit.hpp"

using namespace Castor;

namespace Castor3D
{
	LightManager::LightManager( Scene & p_owner, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
		: ObjectManager< Castor::String, Light >( p_owner, p_rootNode, p_rootCameraNode, p_rootObjectNode )
		, m_lightFactory( GetEngine()->GetSceneManager().GetFactory() )
		, m_lightsTexture( std::make_shared< TextureUnit >( *GetEngine() ) )
	{
		m_lightsData = PxBufferBase::create( Size( 1000, 1 ), ePIXEL_FORMAT_ARGB32F );
		DynamicTextureSPtr l_texture = GetEngine()->GetRenderSystem()->CreateDynamicTexture( eACCESS_TYPE_WRITE, eACCESS_TYPE_READ );
		l_texture->SetType( eTEXTURE_TYPE_BUFFER );
		l_texture->SetImage( m_lightsData );
		SamplerSPtr l_sampler = GetEngine()->GetLightsSampler();
		m_lightsTexture->SetAutoMipmaps( false );
		m_lightsTexture->SetSampler( l_sampler );
		m_lightsTexture->SetTexture( l_texture );
		GetEngine()->PostEvent( MakeInitialiseEvent( *m_lightsTexture ) );
	}

	LightManager::~LightManager()
	{
		m_lightsData.reset();
		m_lightsTexture.reset();
	}

	void LightManager::Cleanup()
	{
		GetEngine()->PostEvent( MakeCleanupEvent( *m_lightsTexture ) );
		ObjectManager< Castor::String, Light >::Cleanup();
	}

	void LightManager::Insert( String const & p_name, LightSPtr p_element )
	{
		ObjectManager< Castor::String, Light >::Insert( p_name, p_element );
		DoAddLight( p_element );
	}

	void LightManager::Remove( String const & p_name )
	{
		auto l_lock = make_unique_lock( m_elements );

		if ( m_elements.has( p_name ) )
		{
			auto l_element = m_elements.find( p_name );
			ElementDetacher< Light >::Detach( *l_element );
			m_elements.erase( p_name );
			auto l_itMap = m_typeSortedLights.find( l_element->GetLightType() );

			if ( l_itMap != m_typeSortedLights.end() )
			{
				auto l_it = std::find( l_itMap->second.begin(), l_itMap->second.end(), l_element );

				if ( l_it != l_itMap->second.end() )
				{
					l_itMap->second.erase( l_it );
				}
			}

			GetScene()->SetChanged();
		}
	}

	void LightManager::BindLights( ShaderProgramBase & p_program, FrameVariableBuffer & p_sceneBuffer )
	{
		GetEngine()->GetRenderSystem()->RenderAmbientLight( GetScene()->GetAmbientLight(), p_sceneBuffer );
		OneTextureFrameVariableSPtr l_lights = p_program.FindFrameVariable( ShaderProgramBase::Lights, eSHADER_TYPE_PIXEL );
		Point4iFrameVariableSPtr l_lightsCount;
		p_sceneBuffer.GetVariable( ShaderProgramBase::LightsCount, l_lightsCount );

		if ( l_lights && l_lightsCount )
		{
			l_lights->SetValue( m_lightsTexture->GetTexture().get() );
			int l_index = 0;

			for ( auto && l_it : m_typeSortedLights )
			{
				l_lightsCount->GetValue( 0 )[l_it.first] += l_it.second.size();

				for ( auto l_light : l_it.second )
				{
					l_light->Bind( *m_lightsData, l_index++ );
				}
			}

			m_lightsTexture->UploadImage( false );
			m_lightsTexture->Bind();
		}
	}

	void LightManager::UnbindLights( ShaderProgramBase & p_program, FrameVariableBuffer & p_sceneBuffer )
	{
		Point4iFrameVariableSPtr l_lightsCount;
		p_sceneBuffer.GetVariable( ShaderProgramBase::LightsCount, l_lightsCount );

		if ( l_lightsCount )
		{
			m_lightsTexture->Unbind();
			int l_index = 0;

			for ( auto && l_it : m_typeSortedLights )
			{
				l_lightsCount->GetValue( 0 )[l_it.first] -= l_it.second.size();
			}
		}
	}

	void LightManager::DoAddLight( LightSPtr p_light )
	{
		auto l_it = m_typeSortedLights.insert( std::make_pair( p_light->GetLightType(), LightsArray() ) ).first;
		bool l_found = std::binary_search( l_it->second.begin(), l_it->second.end(), p_light );

		if ( !l_found )
		{
			l_it->second.push_back( p_light );
		}
	}
}
