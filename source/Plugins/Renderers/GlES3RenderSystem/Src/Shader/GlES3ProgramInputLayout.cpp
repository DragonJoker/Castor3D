#include "Shader/GlES3ProgramInputLayout.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3RenderSystem.hpp"
#include "Shader/GlES3ShaderProgram.hpp"
#include "Buffer/GlES3Buffer.hpp"

#include <Shader/AtomicCounterBuffer.hpp>
#include <Shader/ShaderStorageBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GlES3Render
{
	GlES3ProgramInputLayout::GlES3ProgramInputLayout( OpenGlES3 & p_gl, RenderSystem & p_renderSystem )
		: ProgramInputLayout( p_renderSystem )
		, Holder( p_gl )
	{
	}

	GlES3ProgramInputLayout::~GlES3ProgramInputLayout()
	{
	}

	bool GlES3ProgramInputLayout::Initialise( ShaderProgram const & p_program )
	{
		auto l_attributes = DoListAttributes( p_program );
		bool l_return = true;

		for ( auto & l_element : l_attributes )
		{
			DoAddAttribute( l_element.second.m_name, l_element.second.m_dataType );
		}

		DoListOther( p_program );
		return l_return;
	}

	void GlES3ProgramInputLayout::Cleanup()
	{
	}

	std::multimap< int, BufferElementDeclaration > GlES3ProgramInputLayout::DoListAttributes( ShaderProgram const & p_program )
	{
		std::multimap< int, BufferElementDeclaration > l_attributes;
		GlES3ShaderProgram const & l_program = static_cast< GlES3ShaderProgram const & >( p_program );

		int l_count = 0;
		GetOpenGlES3().GetProgramiv( l_program.GetGlES3Name(), GlES3ShaderStatus::eActiveAttributeMaxLength, &l_count );
		std::vector< char > l_buffer( l_count );
		GetOpenGlES3().GetProgramiv( l_program.GetGlES3Name(), GlES3ShaderStatus::eActiveAttributes, &l_count );

		for ( int i = 0; i < l_count; ++i )
		{
			int l_arraySize = 0;
			uint32_t l_type = 0;
			int l_actualLength = 0;
			GetOpenGlES3().GetActiveAttrib( l_program.GetGlES3Name(), i, uint32_t( l_buffer.size() ), &l_actualLength, &l_arraySize, &l_type, l_buffer.data() );
			std::string l_name( l_buffer.data(), l_actualLength );
			Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( l_name ) );
			ElementType l_elemType = GetOpenGlES3().Get( GlslAttributeType( l_type ) );
			int l_loc = GetOpenGlES3().GetAttribLocation( l_program.GetGlES3Name(), l_buffer.data() );
			string::to_lower_case( l_name );

			if ( l_elemType == ElementType::eVec4 && l_name == string::string_cast< char >( ShaderProgram::Position ) )
			{
				l_elemType = ElementType::eVec3;
			}

			l_attributes.insert( { l_loc, BufferElementDeclaration{ l_buffer.data(), 0, l_elemType } } );
		}

		return l_attributes;
	}

	void GlES3ProgramInputLayout::DoListOther( Castor3D::ShaderProgram const & p_program )
	{
	}
}
