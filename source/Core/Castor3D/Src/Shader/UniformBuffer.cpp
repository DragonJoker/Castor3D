#include "UniformBuffer.hpp"
#include "Uniform.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		String WriteFlags( FlagCombination< ShaderTypeFlag > const & p_flags )
		{
			static std::map< ShaderTypeFlag, String > const Names
			{
				{ ShaderTypeFlag::eVertex, cuT( "vertex" ) },
				{ ShaderTypeFlag::eHull, cuT( "hull" ) },
				{ ShaderTypeFlag::eDomain, cuT( "domain" ) },
				{ ShaderTypeFlag::eGeometry, cuT( "geometry" ) },
				{ ShaderTypeFlag::ePixel, cuT( "pixel" ) },
				{ ShaderTypeFlag::eCompute, cuT( "compute" ) }
			};

			String l_return;
			String l_sep;

			for ( auto it : Names )
			{
				if ( CheckFlag( p_flags, it.first ) )
				{
					l_return += l_sep + it.second;
					l_sep = cuT( " | " );
				}
			}

			return l_return;
		}
	}

	//*************************************************************************************************

	UniformBuffer::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< UniformBuffer >{ p_tabs }
	{
	}

	bool UniformBuffer::TextWriter::operator()( UniformBuffer const & p_object, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( m_tabs + cuT( "constants_buffer \"" ) + p_object.GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		CheckError( l_return, "Frame variable buffer" );
		auto l_tabs = m_tabs + cuT( "\t" );

		if ( l_return )
		{
			l_return = p_file.WriteText( l_tabs + cuT( "shaders " ) + WriteFlags( p_object.m_flags ) + cuT( "\n" ) ) > 0;
			CheckError( l_return, "Frame variable buffer shaders" );
		}

		if ( l_return )
		{
			for ( auto & l_variable : p_object )
			{
				if ( l_return )
				{
					l_return = p_file.WriteText( l_tabs + cuT( "variable \"" ) + l_variable->GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( l_tabs + cuT( "{\n" ) ) > 0;
					CheckError( l_return, "Frame variable buffer variable name" );
				}

				if ( l_return )
				{
					l_return = p_file.WriteText( l_tabs + cuT( "\tcount " ) + string::to_string( l_variable->GetOccCount() ) + cuT( "\n" ) ) > 0;
					CheckError( l_return, "Frame variable buffer variable occurences" );
				}

				if ( l_return )
				{
					l_return = p_file.WriteText( l_tabs + cuT( "\ttype " ) + l_variable->GetFullTypeName() + cuT( "\n" ) ) > 0;
					CheckError( l_return, "Frame variable buffer variable type name" );
				}

				if ( l_return )
				{
					l_return = p_file.WriteText( l_tabs + cuT( "\tvalue " ) + l_variable->GetStrValue() + cuT( "\n" ) ) > 0;
					CheckError( l_return, "Frame variable buffer variable value" );
				}

				if ( l_return )
				{
					l_return = p_file.WriteText( l_tabs + cuT( "}\n" ) ) > 0;
					CheckError( l_return, "Frame variable buffer variable end" );
				}
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
			CheckError( l_return, "Frame variable buffer end" );
		}

		return l_return;
	}

	//*************************************************************************************************

	uint32_t UniformBuffer::sm_uiCount = 0;

	UniformBuffer::UniformBuffer(
		String const & p_name,
		ShaderProgram & p_program,
		FlagCombination< ShaderTypeFlag > const & p_flags,
		RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_name{ p_name }
		, m_index{ sm_uiCount++ }
		, m_program{ p_program }
		, m_flags{ p_flags }
	{
	}

	UniformBuffer::~UniformBuffer()
	{
	}

	UniformSPtr UniformBuffer::CreateVariable( UniformType p_type, String const & p_name, uint32_t p_occurences )
	{
		UniformSPtr l_return;
		UniformPtrStrMapConstIt l_it = m_mapVariables.find( p_name );

		if ( l_it == m_mapVariables.end() )
		{
			l_return = DoCreateVariable( p_type, p_name, p_occurences );

			if ( l_return )
			{
				m_mapVariables.insert( std::make_pair( p_name, l_return ) );
				m_listVariables.push_back( l_return );
			}
		}
		else
		{
			l_return = l_it->second.lock();
		}

		return l_return;
	}

	void UniformBuffer::RemoveVariable( String const & p_name )
	{
		UniformPtrStrMapConstIt l_itMap = m_mapVariables.find( p_name );

		if ( l_itMap != m_mapVariables.end() )
		{
			m_mapVariables.erase( l_itMap );
		}

		auto l_itList = std::find_if( m_listVariables.begin(), m_listVariables.end(), [&p_name]( UniformSPtr p_variable )
		{
			return p_name == p_variable->GetName();
		} );

		if ( l_itList != m_listVariables.end() )
		{
			m_listVariables.erase( l_itList );
		}
	}

	bool UniformBuffer::Initialise()
	{
		bool l_return = true;

		if ( !DoInitialise() )
		{
			uint32_t l_totalSize = 0;

			for ( auto l_variable : m_listVariables )
			{
				if ( l_variable->Initialise() )
				{
					l_totalSize += l_variable->size();
					m_listInitialised.push_back( l_variable );
				}
			}

			m_buffer.resize( l_totalSize );
			uint8_t * l_buffer = m_buffer.data();

			for ( auto l_variable : m_listInitialised )
			{
				l_variable->link( l_buffer );
				l_buffer += l_variable->size();
			}
		}

		return l_return;
	}

	void UniformBuffer::Cleanup()
	{
		DoCleanup();
		m_mapVariables.clear();
		m_listVariables.clear();
	}

	void UniformBuffer::BindTo( uint32_t p_index )
	{
		DoBindTo( p_index );
	}

	void UniformBuffer::Update()
	{
		DoUpdate();
	}

	//*************************************************************************************************
}
