#include "Material.hpp"
#include "Pass.hpp"
#include "SceneFileParser.hpp"
#include "Sampler.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	Material::BinaryParser::BinaryParser( Path const & p_path, Engine * p_engine )
		: Castor3D::BinaryParser< Material >( p_path )
		, m_engine( p_engine )
	{
	}

	bool Material::BinaryParser::Parse( Material & p_obj, BinaryChunk & p_chunk )const
	{
		p_obj.Cleanup();
		bool l_return = true;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_NAME:
					l_return = DoParseChunk( l_name, l_chunk );

					if ( l_return )
					{
						p_obj.m_name = l_name;
					}

					break;

				case eCHUNK_TYPE_MATERIAL_PASS:
					PassSPtr l_pass = p_obj.CreatePass();
					l_return = Pass::BinaryParser( m_path ).Parse( *l_pass, l_chunk );
					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		return l_return;
	}

	bool Material::BinaryParser::Fill( Material const & p_obj, BinaryChunk & p_chunk )const
	{
		BinaryChunk  l_chunk( eCHUNK_TYPE_MATERIAL );
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		if ( l_return )
		{
			auto l_it = p_obj.begin();
			Pass::BinaryParser l_parser( m_path );

			while ( l_return && l_it != p_obj.end() )
			{
				l_return = l_parser.Fill( const_cast< Pass const & >( *( *l_it ) ), l_chunk );
				++l_it;
			}
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	Material::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		:	Loader< Material, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool Material::TextLoader::operator()( Material const & p_material, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "material \"" ) + p_material.GetName() + cuT( "\"\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "{\n" ) ) > 0;
		}

		bool l_first = true;

		for ( auto && l_pass : p_material )
		{
			if ( l_first )
			{
				l_first = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			l_return = Pass::TextLoader()( *l_pass, p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*********************************************************************************************

	const Castor::String Material::DefaultMaterialName = cuT( "DefaultMaterial" );

	Material::Material( Engine & p_engine, String const & p_name )
		: Resource<Material>( p_name )
		, OwnedBy< Engine >( p_engine )
	{
	}

	Material::~Material()
	{
	}

	void Material::Initialise()
	{
		Logger::LogDebug( cuT( "Initialising material [" ) + GetName() + cuT( "]" ) );

		for ( auto && l_pass : m_passes )
		{
			l_pass->Initialise();
		}
	}

	void Material::Cleanup()
	{
		for ( auto && l_pass : m_passes )
		{
			l_pass->Cleanup();
		}
	}

	void Material::Render()
	{
		uint8_t l_byIndex = 0;
		uint8_t l_byCount = uint8_t( m_passes.size() );

		for ( auto && l_pass : m_passes )
		{
			l_pass->Render( l_byIndex++, l_byCount );
		}
	}

	void Material::Render2D()
	{
		uint8_t l_byIndex = 0;
		uint8_t l_byCount = uint8_t( m_passes.size() );

		for ( auto && l_pass : m_passes )
		{
			l_pass->Render2D( l_byIndex++, l_byCount );
		}
	}

	void Material::EndRender()
	{
		for ( auto && l_pass : m_passes )
		{
			l_pass->EndRender();
		}
	}

	PassSPtr Material::CreatePass()
	{
		PassSPtr l_newPass = std::make_shared< Pass >( *GetOwner(), shared_from_this() );
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
		bool l_return = true;
		uint32_t l_count = GetPassCount();

		for ( uint32_t i = 0; i < l_count && l_return; i++ )
		{
			l_return = m_passes[i]->HasAlphaBlending();
		}

		return l_return;
	}
}
