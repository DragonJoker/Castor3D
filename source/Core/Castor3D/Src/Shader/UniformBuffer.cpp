#include "UniformBuffer.hpp"

#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBufferBinding.hpp"

#include "Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
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
				if ( checkFlag( flags, it.first ) )
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
		: castor::TextWriter< UniformBuffer >{ tabs }
	{
	}

	bool UniformBuffer::TextWriter::operator()( UniformBuffer const & object, TextFile & file )
	{
		bool result = file.writeText( m_tabs + cuT( "constants_buffer \"" ) + object.getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		checkError( result, "Frame variable buffer" );
		auto tabs = m_tabs + cuT( "\t" );

		if ( result )
		{
			for ( auto & variable : object )
			{
				if ( result )
				{
					result = file.writeText( tabs + cuT( "variable \"" ) + variable->getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( tabs + cuT( "{\n" ) ) > 0;
					checkError( result, "Frame variable buffer variable name" );
				}

				if ( result )
				{
					result = file.writeText( tabs + cuT( "\tcount " ) + string::toString( variable->getOccCount() ) + cuT( "\n" ) ) > 0;
					checkError( result, "Frame variable buffer variable occurences" );
				}

				if ( result )
				{
					result = file.writeText( tabs + cuT( "\ttype " ) + variable->getFullTypeName() + cuT( "\n" ) ) > 0;
					checkError( result, "Frame variable buffer variable type name" );
				}

				if ( result )
				{
					result = file.writeText( tabs + cuT( "\tvalue " ) + variable->getStrValue() + cuT( "\n" ) ) > 0;
					checkError( result, "Frame variable buffer variable value" );
				}

				if ( result )
				{
					result = file.writeText( tabs + cuT( "}\n" ) ) > 0;
					checkError( result, "Frame variable buffer variable end" );
				}
			}
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			checkError( result, "Frame variable buffer end" );
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

	UniformSPtr UniformBuffer::createUniform( UniformType type
		, String const & name
		, uint32_t occurences )
	{
		UniformSPtr result;
		auto it = m_mapVariables.find( name );

		if ( it == m_mapVariables.end() )
		{
			result = doCreateVariable( type, name, occurences );

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

	void UniformBuffer::removeUniform( String const & name )
	{
		auto itMap = m_mapVariables.find( name );

		if ( itMap != m_mapVariables.end() )
		{
			m_mapVariables.erase( itMap );
		}

		auto itList = std::find_if( m_listVariables.begin(), m_listVariables.end(), [&name]( UniformSPtr p_variable )
		{
			return name == p_variable->getName();
		} );

		if ( itList != m_listVariables.end() )
		{
			m_listVariables.erase( itList );
		}
	}

	void UniformBuffer::cleanup()
	{
		if ( m_storage )
		{
			m_storage->destroy();
			m_storage.reset();
		}

		m_bindings.clear();
		m_mapVariables.clear();
		m_listVariables.clear();
	}

	void UniformBuffer::bindTo( uint32_t index )const
	{
		REQUIRE( m_storage );
		m_storage->setBindingPoint( index );
	}

	uint32_t UniformBuffer::getBindingPoint()const
	{
		REQUIRE( m_storage );
		return m_storage->getBindingPoint();
	}

	UniformBufferBinding & UniformBuffer::createBinding( ShaderProgram & program )
	{
		auto it = m_bindings.find( &program );

		if ( it == m_bindings.end() )
		{
			auto binding = getRenderSystem()->createUniformBufferBinding( *this, program );

			if ( !m_storage && binding->getSize() > 0 )
			{
				doInitialise( *binding );
				ENSURE( m_storage );

				if ( m_storage )
				{
					m_storage->setBindingPoint( m_bindingPoint );
				}
			}

			it = m_bindings.emplace( &program, std::move( binding ) ).first;
		}

		return *it->second;
	}

	UniformBufferBinding & UniformBuffer::getBinding( ShaderProgram & program )const
	{
		REQUIRE( m_bindings.find( &program ) != m_bindings.end() );
		return *m_bindings.find( &program )->second;
	}

	void UniformBuffer::update()const
	{
		REQUIRE( m_storage );
		bool changed = m_listVariables.end() != std::find_if( m_listVariables.begin()
			, m_listVariables.end()
			, []( UniformSPtr p_variable )
			{
				return p_variable->isChanged();
			} );

		if ( changed )
		{
			m_storage->upload( 0u, uint32_t( m_buffer.size() ), m_buffer.data() );

			for ( auto & variable : m_listVariables )
			{
				variable->setChanged( false );
			}
		}
	}

	void UniformBuffer::doInitialise( UniformBufferBinding const & binding )
	{
		m_buffer.resize( binding.getSize() );

		for ( auto & variable : *this )
		{
			auto it = std::find_if( binding.begin()
				, binding.end()
				, [&variable]( auto & p_info )
				{
					return variable->getName() == p_info.m_name;
				} );

			if ( it != binding.end() )
			{
				REQUIRE( variable->size() <= m_buffer.size() - ( it->m_offset ) );
				variable->link( &m_buffer[it->m_offset], it->m_stride < 0 ? 0u : uint32_t( it->m_stride ) );
				++it;
			}
			else
			{
				Logger::logWarning( cuT( "The variable [" ) + variable->getName() + cuT( "] was not found in the binding." ) );
			}
		}

		if ( !m_storage )
		{
			auto buffer = getRenderSystem()->getBuffer( BufferType::eUniform
				, uint32_t( m_buffer.size() )
				, BufferAccessType::eDynamic
				, BufferAccessNature::eDraw );
			m_storage = std::move( buffer.buffer );
			m_offset = buffer.offset;
			m_storage->create();
		}

		m_storage->upload( m_offset
			, uint32_t( m_buffer.size() )
			, m_buffer.data() );

		for ( auto & variable : *this )
		{
			variable->setChanged( false );
		}
	}

	UniformSPtr UniformBuffer::doCreateVariable( UniformType type
		, castor::String const & name
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
			result->setName( name );
		}

		return result;
	}

	//*************************************************************************************************
}
