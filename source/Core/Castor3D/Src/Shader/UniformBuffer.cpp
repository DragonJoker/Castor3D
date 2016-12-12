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

	UniformSPtr UniformBuffer::CreateUniform( UniformType p_type, String const & p_name, uint32_t p_occurences )
	{
		UniformSPtr l_return;
		auto l_it = m_mapVariables.find( p_name );

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
		auto l_itMap = m_mapVariables.find( p_name );

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
		return DoInitialise();
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

	UniformSPtr UniformBuffer::DoCreateVariable( UniformType p_type, Castor::String const & p_name, uint32_t p_occurences )
	{
		UniformSPtr l_return;

		switch ( p_type )
		{
		case UniformType::eInt:
			l_return = std::make_shared< Uniform1i >( p_occurences );
			break;

		case UniformType::eUInt:
			l_return = std::make_shared< Uniform1ui >( p_occurences );
			break;

		case UniformType::eFloat:
			l_return = std::make_shared< Uniform1f >( p_occurences );
			break;

		case UniformType::eDouble:
			l_return = std::make_shared< Uniform1d >( p_occurences );
			break;

		case UniformType::eSampler:
			l_return = std::make_shared< Uniform1i >( p_occurences );
			break;

		case UniformType::eVec2i:
			l_return = std::make_shared< Uniform2i >( p_occurences );
			break;

		case UniformType::eVec3i:
			l_return = std::make_shared< Uniform3i >( p_occurences );
			break;

		case UniformType::eVec4i:
			l_return = std::make_shared< Uniform4i >( p_occurences );
			break;

		case UniformType::eVec2ui:
			l_return = std::make_shared< Uniform2ui >( p_occurences );
			break;

		case UniformType::eVec3ui:
			l_return = std::make_shared< Uniform3ui >( p_occurences );
			break;

		case UniformType::eVec4ui:
			l_return = std::make_shared< Uniform4ui >( p_occurences );
			break;

		case UniformType::eVec2f:
			l_return = std::make_shared< Uniform2f >( p_occurences );
			break;

		case UniformType::eVec3f:
			l_return = std::make_shared< Uniform3f >( p_occurences );
			break;

		case UniformType::eVec4f:
			l_return = std::make_shared< Uniform4f >( p_occurences );
			break;

		case UniformType::eVec2d:
			l_return = std::make_shared< Uniform2d >( p_occurences );
			break;

		case UniformType::eVec3d:
			l_return = std::make_shared< Uniform3d >( p_occurences );
			break;

		case UniformType::eVec4d:
			l_return = std::make_shared< Uniform4d >( p_occurences );
			break;

		case UniformType::eMat2x2f:
			l_return = std::make_shared< Uniform2x2f >( p_occurences );
			break;

		case UniformType::eMat2x3f:
			l_return = std::make_shared< Uniform2x3f >( p_occurences );
			break;

		case UniformType::eMat2x4f:
			l_return = std::make_shared< Uniform2x4f >( p_occurences );
			break;

		case UniformType::eMat3x2f:
			l_return = std::make_shared< Uniform3x2f >( p_occurences );
			break;

		case UniformType::eMat3x3f:
			l_return = std::make_shared< Uniform3x3f >( p_occurences );
			break;

		case UniformType::eMat3x4f:
			l_return = std::make_shared< Uniform3x4f >( p_occurences );
			break;

		case UniformType::eMat4x2f:
			l_return = std::make_shared< Uniform4x2f >( p_occurences );
			break;

		case UniformType::eMat4x3f:
			l_return = std::make_shared< Uniform4x3f >( p_occurences );
			break;

		case UniformType::eMat4x4f:
			l_return = std::make_shared< Uniform4x4f >( p_occurences );
			break;

		case UniformType::eMat2x2d:
			l_return = std::make_shared< Uniform2x2d >( p_occurences );
			break;

		case UniformType::eMat2x3d:
			l_return = std::make_shared< Uniform2x3d >( p_occurences );
			break;

		case UniformType::eMat2x4d:
			l_return = std::make_shared< Uniform2x4d >( p_occurences );
			break;

		case UniformType::eMat3x2d:
			l_return = std::make_shared< Uniform3x2d >( p_occurences );
			break;

		case UniformType::eMat3x3d:
			l_return = std::make_shared< Uniform3x3d >( p_occurences );
			break;

		case UniformType::eMat3x4d:
			l_return = std::make_shared< Uniform3x4d >( p_occurences );
			break;

		case UniformType::eMat4x2d:
			l_return = std::make_shared< Uniform4x2d >( p_occurences );
			break;

		case UniformType::eMat4x3d:
			l_return = std::make_shared< Uniform4x3d >( p_occurences );
			break;

		case UniformType::eMat4x4d:
			l_return = std::make_shared< Uniform4x4d >( p_occurences );
			break;
		}

		if ( l_return )
		{
			l_return->SetName( p_name );
		}

		return l_return;
	}

	//*************************************************************************************************
}
