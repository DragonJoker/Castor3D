#include "LightCache.hpp"

#include "SceneCache.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureImage.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< Light >::Name = cuT( "Light" );

	LightCache::LightCache( SceneNodeSPtr p_rootNode
							, SceneNodeSPtr p_rootCameraNode
							, SceneNodeSPtr p_rootObjectNode
							, SceneGetter && p_get
							, Producer && p_produce
							, Initialiser && p_initialise
							, Cleaner && p_clean
							, Merger && p_merge
							, Attacher && p_attach
							, Detacher && p_detach )
		: MyObjectCache{ p_rootNode
					   , p_rootCameraNode
					   , p_rootObjectNode
					   , std::move( p_get )
					   , std::move( p_produce )
					   , std::move( p_initialise )
					   , std::move( p_clean )
					   , std::move( p_merge )
					   , std::move( p_attach )
					   , std::move( p_detach ) }
		, m_lightsTexture{ std::make_shared< TextureUnit >( *GetEngine() ) }
	{
		m_initialise.m_typeSortedLights = &m_typeSortedLights;
		m_clean.m_typeSortedLights = &m_typeSortedLights;
	}

	LightCache::~LightCache()
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

	void LightCache::BindLights( ShaderProgram & p_program, FrameVariableBuffer & p_sceneBuffer )
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

	void LightCache::UnbindLights( ShaderProgram & p_program, FrameVariableBuffer & p_sceneBuffer )
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
