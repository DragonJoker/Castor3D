#include "Material.hpp"
#include "Pass.hpp"
#include "SceneFileParser.hpp"
#include "Sampler.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	Material::BinaryParser::BinaryParser( Path const & p_path, Engine * p_engine )
		:	Castor3D::BinaryParser< Material >( p_path )
		,	m_engine( p_engine )
	{
	}

	bool Material::BinaryParser::Parse( Material & p_obj, BinaryChunk & p_chunk )const
	{
		p_obj.Cleanup();
		bool l_bReturn = true;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_NAME:
					l_bReturn = DoParseChunk( l_name, l_chunk );

					if ( l_bReturn )
					{
						p_obj.m_name = l_name;
					}

					break;

				case eCHUNK_TYPE_MATERIAL_PASS:
					PassSPtr l_pass = p_obj.CreatePass();
					l_bReturn = Pass::BinaryParser( m_path ).Parse( *l_pass, l_chunk );
					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	bool Material::BinaryParser::Fill( Material const & p_obj, BinaryChunk & p_chunk )const
	{
		BinaryChunk  l_chunk( eCHUNK_TYPE_MATERIAL );
		bool l_bReturn = true;

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		if ( l_bReturn )
		{
			auto l_it = p_obj.begin();
			Pass::BinaryParser l_parser( m_path );

			while ( l_bReturn && l_it != p_obj.end() )
			{
				l_bReturn = l_parser.Fill( const_cast< Pass const & >( *( *l_it ) ), l_chunk );
				++l_it;
			}
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	Material::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Material, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool Material::TextLoader::operator()( Material const & p_material, TextFile & p_file )
	{
		bool l_bReturn = p_file.WriteText( cuT( "material \"" ) + p_material.GetName() + cuT( "\"\n" ) ) > 0;

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "{\n" ) ) > 0;
		}

		bool l_bFirst = true;

		for ( auto && l_pass : p_material )
		{
			if ( l_bFirst )
			{
				l_bFirst = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			l_bReturn = Pass::TextLoader()( *l_pass, p_file );
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "}\n" ) ) > 0;
		}

		return l_bReturn;
	}

	//*********************************************************************************************

	const Castor::String Material::DefaultMaterialName = cuT( "DefaultMaterial" );

	Material::Material( Engine * p_pEngine, String const & p_name )
		:	Resource<Material>( p_name )
		,	m_pEngine( p_pEngine )
	{
	}

	Material::~Material()
	{
	}

	void Material::Initialise()
	{
		Logger::LogDebug( cuT( "Initialising material [" ) + GetName() + cuT( "]" ) );
		std::for_each( m_passes.begin(), m_passes.end(), [&]( PassSPtr p_pPass )
		{
			p_pPass->Initialise();
		} );
	}

	void Material::Cleanup()
	{
		std::for_each( m_passes.begin(), m_passes.end(), [&]( PassSPtr p_pPass )
		{
			p_pPass->Cleanup();
		} );
	}

	void Material::Render()
	{
		uint8_t l_byIndex = 0;
		uint8_t l_byCount = uint8_t( m_passes.size() );
		std::for_each( m_passes.begin(), m_passes.end(), [&]( PassSPtr p_pPass )
		{
			p_pPass->Render( l_byIndex++, l_byCount );
		} );
	}

	void Material::Render2D()
	{
		uint8_t l_byIndex = 0;
		uint8_t l_byCount = uint8_t( m_passes.size() );
		std::for_each( m_passes.begin(), m_passes.end(), [&]( PassSPtr p_pPass )
		{
			p_pPass->Render2D( l_byIndex++, l_byCount );
		} );
	}

	void Material::EndRender()
	{
		std::for_each( m_passes.begin(), m_passes.end(), [&]( PassSPtr p_pPass )
		{
			p_pPass->EndRender();
		} );
	}

	PassSPtr Material::CreatePass()
	{
		PassSPtr l_newPass = std::make_shared< Pass >( m_pEngine, shared_from_this() );
		m_passes.push_back( l_newPass );
		return l_newPass;
	}

	const PassSPtr Material::GetPass( uint32_t p_index )const
	{
		CASTOR_ASSERT( p_index < m_passes.size() );
		return m_passes[p_index];
	}

	PassSPtr Material::GetPass( uint32_t p_index )
	{
		CASTOR_ASSERT( p_index < m_passes.size() );
		return m_passes[p_index];
	}

	void Material::DestroyPass( uint32_t p_index )
	{
		CASTOR_ASSERT( p_index < m_passes.size() );
		m_passes.erase( m_passes.begin() + p_index );
	}

	bool Material::HasAlphaBlending()const
	{
		bool l_bReturn = true;
		uint32_t l_uiCount = GetPassCount();

		for ( uint32_t i = 0; i < l_uiCount && l_bReturn; i++ )
		{
			l_bReturn = m_passes[i]->HasAlphaBlending();
		}

		return l_bReturn;
	}
}
