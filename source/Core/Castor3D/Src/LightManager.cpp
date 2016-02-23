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
	const String ManagedObjectNamer< Light >::Name = cuT( "Light" );

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

	void LightManager::BindLights( ShaderProgram & p_program, FrameVariableBuffer & p_sceneBuffer )
	{
		GetEngine()->GetRenderSystem()->RenderAmbientLight( GetScene()->GetAmbientLight(), p_sceneBuffer );
		OneIntFrameVariableSPtr l_lights = p_program.FindFrameVariable( ShaderProgram::Lights, eSHADER_TYPE_PIXEL );
		Point4iFrameVariableSPtr l_lightsCount;
		p_sceneBuffer.GetVariable( ShaderProgram::LightsCount, l_lightsCount );

		if ( l_lights && l_lightsCount )
		{
			l_lights->SetValue( m_lightsTexture->GetIndex() );
			int l_index = 0;

			for ( auto && l_it : m_typeSortedLights )
			{
				l_lightsCount->GetValue( 0 )[l_it.first] += uint32_t( l_it.second.size() );

				for ( auto l_light : l_it.second )
				{
					l_light->Bind( *m_lightsData, l_index++ );
				}
			}

			m_lightsTexture->UploadImage( false );
			m_lightsTexture->Bind();
		}
	}

	void LightManager::UnbindLights( ShaderProgram & p_program, FrameVariableBuffer & p_sceneBuffer )
	{
		Point4iFrameVariableSPtr l_lightsCount;
		p_sceneBuffer.GetVariable( ShaderProgram::LightsCount, l_lightsCount );

		if ( l_lightsCount )
		{
			m_lightsTexture->Unbind();
			int l_index = 0;

			for ( auto && l_it : m_typeSortedLights )
			{
				l_lightsCount->GetValue( 0 )[l_it.first] -= uint32_t( l_it.second.size() );
			}
		}
	}

	LightSPtr LightManager::Create( Castor::String const & p_name, SceneNodeSPtr p_parent, eLIGHT_TYPE p_eLightType )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		LightSPtr l_return;

		if ( !m_elements.has( p_name ) )
		{
			l_return = std::make_shared< Light >( p_name, *this->GetScene(), p_parent, m_lightFactory, p_eLightType );
			m_elements.insert( p_name, l_return );
			ElementAttacher< Light >::Attach( l_return, p_parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
			DoAddLight( l_return );
			Castor::Logger::LogInfo( Castor::StringStream() << INFO_MANAGER_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
			GetScene()->SetChanged();
		}
		else
		{
			l_return = m_elements.find( p_name );
			Castor::Logger::LogWarning( Castor::StringStream() << WARNING_MANAGER_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
		}

		return l_return;
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
