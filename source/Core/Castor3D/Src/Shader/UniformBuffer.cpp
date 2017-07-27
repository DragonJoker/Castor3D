#include "UniformBuffer.hpp"

#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBufferBinding.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		String WriteFlags( ShaderTypeFlags const & flags )
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

			String result;
			String sep;

			for ( auto it : Names )
			{
				if ( CheckFlag( flags, it.first ) )
				{
					result += sep + it.second;
					sep = cuT( " | " );
				}
			}

			return result;
		}
	}

	//*************************************************************************************************

	UniformBuffer::TextWriter::TextWriter( String const & tabs )
		: Castor::TextWriter< UniformBuffer >{ tabs }
	{
	}

	bool UniformBuffer::TextWriter::operator()( UniformBuffer const & object, TextFile & file )
	{
		bool result = file.WriteText( m_tabs + cuT( "constants_buffer \"" ) + object.GetName() + cuT( "\"\n" ) ) > 0
						&& file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		CheckError( result, "Frame variable buffer" );
		auto tabs = m_tabs + cuT( "\t" );

		if ( result )
		{
			for ( auto & variable : object )
			{
				if ( result )
				{
					result = file.WriteText( tabs + cuT( "variable \"" ) + variable->GetName() + cuT( "\"\n" ) ) > 0
						&& file.WriteText( tabs + cuT( "{\n" ) ) > 0;
					CheckError( result, "Frame variable buffer variable name" );
				}

				if ( result )
				{
					result = file.WriteText( tabs + cuT( "\tcount " ) + string::to_string( variable->GetOccCount() ) + cuT( "\n" ) ) > 0;
					CheckError( result, "Frame variable buffer variable occurences" );
				}

				if ( result )
				{
					result = file.WriteText( tabs + cuT( "\ttype " ) + variable->GetFullTypeName() + cuT( "\n" ) ) > 0;
					CheckError( result, "Frame variable buffer variable type name" );
				}

				if ( result )
				{
					result = file.WriteText( tabs + cuT( "\tvalue " ) + variable->GetStrValue() + cuT( "\n" ) ) > 0;
					CheckError( result, "Frame variable buffer variable value" );
				}

				if ( result )
				{
					result = file.WriteText( tabs + cuT( "}\n" ) ) > 0;
					CheckError( result, "Frame variable buffer variable end" );
				}
			}
		}

		if ( result )
		{
			result = file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
			CheckError( result, "Frame variable buffer end" );
		}

		return result;
	}

	//*************************************************************************************************

	UniformBuffer::UniformBuffer( String const & name
		, RenderSystem & renderSystem
		, uint32_t bindingPoint )
		: OwnedBy< RenderSystem >{ renderSystem }
		, Named{ name }
		, m_bindingPoint{ bindingPoint }
	{
	}

	UniformBuffer::~UniformBuffer()
	{
	}

	UniformSPtr UniformBuffer::CreateUniform( UniformType type
		, String const & name
		, uint32_t occurences )
	{
		UniformSPtr result;
		auto it = m_mapVariables.find( name );

		if ( it == m_mapVariables.end() )
		{
			result = DoCreateVariable( type, name, occurences );

			if ( result )
			{
				m_mapVariables.insert( std::make_pair( name, result ) );
				m_listVariables.push_back( result );
			}
		}
		else
		{
			result = it->second.lock();
		}

		return result;
	}

	void UniformBuffer::RemoveUniform( String const & name )
	{
		auto itMap = m_mapVariables.find( name );

		if ( itMap != m_mapVariables.end() )
		{
			m_mapVariables.erase( itMap );
		}

		auto itList = std::find_if( m_listVariables.begin(), m_listVariables.end(), [&name]( UniformSPtr p_variable )
		{
			return name == p_variable->GetName();
		} );

		if ( itList != m_listVariables.end() )
		{
			m_listVariables.erase( itList );
		}
	}

	void UniformBuffer::Cleanup()
	{
		if ( m_storage )
		{
			m_storage->Destroy();
			m_storage.reset();
		}

		m_bindings.clear();
		m_mapVariables.clear();
		m_listVariables.clear();
	}

	void UniformBuffer::BindTo( uint32_t index )const
	{
		REQUIRE( m_storage );
		m_storage->SetBindingPoint( index );
	}

	uint32_t UniformBuffer::GetBindingPoint()const
	{
		REQUIRE( m_storage );
		return m_storage->GetBindingPoint();
	}

	UniformBufferBinding & UniformBuffer::CreateBinding( ShaderProgram & program )
	{
		auto it = m_bindings.find( &program );

		if ( it == m_bindings.end() )
		{
			auto binding = GetRenderSystem()->CreateUniformBufferBinding( *this, program );

			if ( !m_storage && binding->GetSize() > 0 )
			{
				DoInitialise( *binding );
				ENSURE( m_storage );

				if ( m_storage )
				{
					m_storage->SetBindingPoint( m_bindingPoint );
				}
			}

			it = m_bindings.emplace( &program, std::move( binding ) ).first;
		}

		return *it->second;
	}

	UniformBufferBinding & UniformBuffer::GetBinding( ShaderProgram & program )const
	{
		REQUIRE( m_bindings.find( &program ) != m_bindings.end() );
		return *m_bindings.find( &program )->second;
	}

	void UniformBuffer::Update()const
	{
		REQUIRE( m_storage );
		bool changed = m_listVariables.end() != std::find_if( m_listVariables.begin()
			, m_listVariables.end()
			, []( UniformSPtr p_variable )
			{
				return p_variable->IsChanged();
			} );

		if ( changed )
		{
			m_storage->Upload( 0u, uint32_t( m_buffer.size() ), m_buffer.data() );

			for ( auto & variable : m_listVariables )
			{
				variable->SetChanged( false );
			}
		}
	}

	void UniformBuffer::DoInitialise( UniformBufferBinding const & binding )
	{
		m_buffer.resize( binding.GetSize() );

		for ( auto & variable : *this )
		{
			auto it = std::find_if( binding.begin()
				, binding.end()
				, [&variable]( auto & p_info )
				{
					return variable->GetName() == p_info.m_name;
				} );

			if ( it != binding.end() )
			{
				REQUIRE( variable->size() <= m_buffer.size() - ( it->m_offset ) );
				variable->link( &m_buffer[it->m_offset], it->m_stride < 0 ? 0u : uint32_t( it->m_stride ) );
				++it;
			}
			else
			{
				Logger::LogWarning( cuT( "The variable [" ) + variable->GetName() + cuT( "] was not found in the binding." ) );
			}
		}

		if ( !m_storage )
		{
			m_storage = GetRenderSystem()->CreateBuffer( BufferType::eUniform );
			m_storage->Create();
		}

		m_storage->InitialiseStorage( uint32_t( m_buffer.size() )
			, BufferAccessType::eDynamic
			, BufferAccessNature::eDraw );
		m_storage->Upload( 0u
			, uint32_t( m_buffer.size() )
			, m_buffer.data() );

		for ( auto & variable : *this )
		{
			variable->SetChanged( false );
		}
	}

	UniformSPtr UniformBuffer::DoCreateVariable( UniformType type
		, Castor::String const & name
		, uint32_t occurences )
	{
		UniformSPtr result;

		switch ( type )
		{
		case UniformType::eInt:
			result = std::make_shared< Uniform1i >( occurences );
			break;

		case UniformType::eUInt:
			result = std::make_shared< Uniform1ui >( occurences );
			break;

		case UniformType::eFloat:
			result = std::make_shared< Uniform1f >( occurences );
			break;

		case UniformType::eDouble:
			result = std::make_shared< Uniform1d >( occurences );
			break;

		case UniformType::eSampler:
			result = std::make_shared< Uniform1i >( occurences );
			break;

		case UniformType::eVec2i:
			result = std::make_shared< Uniform2i >( occurences );
			break;

		case UniformType::eVec3i:
			result = std::make_shared< Uniform3i >( occurences );
			break;

		case UniformType::eVec4i:
			result = std::make_shared< Uniform4i >( occurences );
			break;

		case UniformType::eVec2ui:
			result = std::make_shared< Uniform2ui >( occurences );
			break;

		case UniformType::eVec3ui:
			result = std::make_shared< Uniform3ui >( occurences );
			break;

		case UniformType::eVec4ui:
			result = std::make_shared< Uniform4ui >( occurences );
			break;

		case UniformType::eVec2f:
			result = std::make_shared< Uniform2f >( occurences );
			break;

		case UniformType::eVec3f:
			result = std::make_shared< Uniform3f >( occurences );
			break;

		case UniformType::eVec4f:
			result = std::make_shared< Uniform4f >( occurences );
			break;

		case UniformType::eVec2d:
			result = std::make_shared< Uniform2d >( occurences );
			break;

		case UniformType::eVec3d:
			result = std::make_shared< Uniform3d >( occurences );
			break;

		case UniformType::eVec4d:
			result = std::make_shared< Uniform4d >( occurences );
			break;

		case UniformType::eMat2x2f:
			result = std::make_shared< Uniform2x2f >( occurences );
			break;

		case UniformType::eMat2x3f:
			result = std::make_shared< Uniform2x3f >( occurences );
			break;

		case UniformType::eMat2x4f:
			result = std::make_shared< Uniform2x4f >( occurences );
			break;

		case UniformType::eMat3x2f:
			result = std::make_shared< Uniform3x2f >( occurences );
			break;

		case UniformType::eMat3x3f:
			result = std::make_shared< Uniform3x3f >( occurences );
			break;

		case UniformType::eMat3x4f:
			result = std::make_shared< Uniform3x4f >( occurences );
			break;

		case UniformType::eMat4x2f:
			result = std::make_shared< Uniform4x2f >( occurences );
			break;

		case UniformType::eMat4x3f:
			result = std::make_shared< Uniform4x3f >( occurences );
			break;

		case UniformType::eMat4x4f:
			result = std::make_shared< Uniform4x4f >( occurences );
			break;

		case UniformType::eMat2x2d:
			result = std::make_shared< Uniform2x2d >( occurences );
			break;

		case UniformType::eMat2x3d:
			result = std::make_shared< Uniform2x3d >( occurences );
			break;

		case UniformType::eMat2x4d:
			result = std::make_shared< Uniform2x4d >( occurences );
			break;

		case UniformType::eMat3x2d:
			result = std::make_shared< Uniform3x2d >( occurences );
			break;

		case UniformType::eMat3x3d:
			result = std::make_shared< Uniform3x3d >( occurences );
			break;

		case UniformType::eMat3x4d:
			result = std::make_shared< Uniform3x4d >( occurences );
			break;

		case UniformType::eMat4x2d:
			result = std::make_shared< Uniform4x2d >( occurences );
			break;

		case UniformType::eMat4x3d:
			result = std::make_shared< Uniform4x3d >( occurences );
			break;

		case UniformType::eMat4x4d:
			result = std::make_shared< Uniform4x4d >( occurences );
			break;
		}

		if ( result )
		{
			result->SetName( name );
		}

		return result;
	}

	//*************************************************************************************************
}
