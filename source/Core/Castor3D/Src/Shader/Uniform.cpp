#include "Uniform.hpp"


using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	const Castor::String UniformTyper< UniformType::eBool >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eVec2b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eVec3b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eVec4b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eMat2x2b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eMat2x3b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eMat2x4b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eMat3x2b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eMat3x3b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eMat3x4b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eMat4x2b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eMat4x3b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eMat4x4b >::data_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eInt >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eVec2i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eVec3i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eVec4i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eMat2x2i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eMat2x3i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eMat2x4i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eMat3x2i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eMat3x3i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eMat3x4i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eMat4x2i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eMat4x3i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eMat4x4i >::data_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eUInt >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eVec2ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eVec3ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eVec4ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eMat2x2ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eMat2x3ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eMat2x4ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eMat3x2ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eMat3x3ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eMat3x4ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eMat4x2ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eMat4x3ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eMat4x4ui >::data_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eFloat >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eVec2f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eVec3f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eVec4f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eMat2x2f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eMat2x3f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eMat2x4f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eMat3x2f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eMat3x3f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eMat3x4f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eMat4x2f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eMat4x3f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eMat4x4f >::data_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eDouble >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eVec2d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eVec3d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eVec4d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eMat2x2d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eMat2x3d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eMat2x4d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eMat3x2d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eMat3x3d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eMat3x4d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eMat4x2d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eMat4x3d >::data_type_name = cuT( "double" );
	const Castor::String UniformTyper< UniformType::eMat4x4d >::data_type_name = cuT( "double" );

	//*************************************************************************************************

	const Castor::String UniformTyper< UniformType::eBool >::full_type_name = cuT( "bool" );
	const Castor::String UniformTyper< UniformType::eInt >::full_type_name = cuT( "int" );
	const Castor::String UniformTyper< UniformType::eUInt >::full_type_name = cuT( "uint" );
	const Castor::String UniformTyper< UniformType::eFloat >::full_type_name = cuT( "float" );
	const Castor::String UniformTyper< UniformType::eDouble >::full_type_name = cuT( "double" );

	//*************************************************************************************************

	const Castor::String UniformTyper< UniformType::eVec2b >::full_type_name = cuT( "vec2b" );
	const Castor::String UniformTyper< UniformType::eVec3b >::full_type_name = cuT( "vec3b" );
	const Castor::String UniformTyper< UniformType::eVec4b >::full_type_name = cuT( "vec4b" );
	const Castor::String UniformTyper< UniformType::eVec2i >::full_type_name = cuT( "vec2i" );
	const Castor::String UniformTyper< UniformType::eVec3i >::full_type_name = cuT( "vec3i" );
	const Castor::String UniformTyper< UniformType::eVec4i >::full_type_name = cuT( "vec4i" );
	const Castor::String UniformTyper< UniformType::eVec2ui >::full_type_name = cuT( "vec2ui" );
	const Castor::String UniformTyper< UniformType::eVec3ui >::full_type_name = cuT( "vec3ui" );
	const Castor::String UniformTyper< UniformType::eVec4ui >::full_type_name = cuT( "vec4ui" );
	const Castor::String UniformTyper< UniformType::eVec2f >::full_type_name = cuT( "vec2f" );
	const Castor::String UniformTyper< UniformType::eVec3f >::full_type_name = cuT( "vec3f" );
	const Castor::String UniformTyper< UniformType::eVec4f >::full_type_name = cuT( "vec4f" );
	const Castor::String UniformTyper< UniformType::eVec2d >::full_type_name = cuT( "vec2d" );
	const Castor::String UniformTyper< UniformType::eVec3d >::full_type_name = cuT( "vec3d" );
	const Castor::String UniformTyper< UniformType::eVec4d >::full_type_name = cuT( "vec4d" );

	//*************************************************************************************************

	const Castor::String UniformTyper< UniformType::eMat2x2b >::full_type_name = cuT( "mat2x2b" );
	const Castor::String UniformTyper< UniformType::eMat2x3b >::full_type_name = cuT( "mat2x3b" );
	const Castor::String UniformTyper< UniformType::eMat2x4b >::full_type_name = cuT( "mat2x4b" );
	const Castor::String UniformTyper< UniformType::eMat3x2b >::full_type_name = cuT( "mat3x2b" );
	const Castor::String UniformTyper< UniformType::eMat3x3b >::full_type_name = cuT( "mat3x3b" );
	const Castor::String UniformTyper< UniformType::eMat3x4b >::full_type_name = cuT( "mat3x4b" );
	const Castor::String UniformTyper< UniformType::eMat4x2b >::full_type_name = cuT( "mat4x2b" );
	const Castor::String UniformTyper< UniformType::eMat4x3b >::full_type_name = cuT( "mat4x3b" );
	const Castor::String UniformTyper< UniformType::eMat4x4b >::full_type_name = cuT( "mat4x4b" );
	const Castor::String UniformTyper< UniformType::eMat2x2i >::full_type_name = cuT( "mat2x2i" );
	const Castor::String UniformTyper< UniformType::eMat2x3i >::full_type_name = cuT( "mat2x3i" );
	const Castor::String UniformTyper< UniformType::eMat2x4i >::full_type_name = cuT( "mat2x4i" );
	const Castor::String UniformTyper< UniformType::eMat3x2i >::full_type_name = cuT( "mat3x2i" );
	const Castor::String UniformTyper< UniformType::eMat3x3i >::full_type_name = cuT( "mat3x3i" );
	const Castor::String UniformTyper< UniformType::eMat3x4i >::full_type_name = cuT( "mat3x4i" );
	const Castor::String UniformTyper< UniformType::eMat4x2i >::full_type_name = cuT( "mat4x2i" );
	const Castor::String UniformTyper< UniformType::eMat4x3i >::full_type_name = cuT( "mat4x3i" );
	const Castor::String UniformTyper< UniformType::eMat4x4i >::full_type_name = cuT( "mat4x4i" );
	const Castor::String UniformTyper< UniformType::eMat2x2ui >::full_type_name = cuT( "mat2x2ui" );
	const Castor::String UniformTyper< UniformType::eMat2x3ui >::full_type_name = cuT( "mat2x3ui" );
	const Castor::String UniformTyper< UniformType::eMat2x4ui >::full_type_name = cuT( "mat2x4ui" );
	const Castor::String UniformTyper< UniformType::eMat3x2ui >::full_type_name = cuT( "mat3x2ui" );
	const Castor::String UniformTyper< UniformType::eMat3x3ui >::full_type_name = cuT( "mat3x3ui" );
	const Castor::String UniformTyper< UniformType::eMat3x4ui >::full_type_name = cuT( "mat3x4ui" );
	const Castor::String UniformTyper< UniformType::eMat4x2ui >::full_type_name = cuT( "mat4x2ui" );
	const Castor::String UniformTyper< UniformType::eMat4x3ui >::full_type_name = cuT( "mat4x3ui" );
	const Castor::String UniformTyper< UniformType::eMat4x4ui >::full_type_name = cuT( "mat4x4ui" );
	const Castor::String UniformTyper< UniformType::eMat2x2f >::full_type_name = cuT( "mat2x2f" );
	const Castor::String UniformTyper< UniformType::eMat2x3f >::full_type_name = cuT( "mat2x3f" );
	const Castor::String UniformTyper< UniformType::eMat2x4f >::full_type_name = cuT( "mat2x4f" );
	const Castor::String UniformTyper< UniformType::eMat3x2f >::full_type_name = cuT( "mat3x2f" );
	const Castor::String UniformTyper< UniformType::eMat3x3f >::full_type_name = cuT( "mat3x3f" );
	const Castor::String UniformTyper< UniformType::eMat3x4f >::full_type_name = cuT( "mat3x4f" );
	const Castor::String UniformTyper< UniformType::eMat4x2f >::full_type_name = cuT( "mat4x2f" );
	const Castor::String UniformTyper< UniformType::eMat4x3f >::full_type_name = cuT( "mat4x3f" );
	const Castor::String UniformTyper< UniformType::eMat4x4f >::full_type_name = cuT( "mat4x4f" );
	const Castor::String UniformTyper< UniformType::eMat2x2d >::full_type_name = cuT( "mat2x2d" );
	const Castor::String UniformTyper< UniformType::eMat2x3d >::full_type_name = cuT( "mat2x3d" );
	const Castor::String UniformTyper< UniformType::eMat2x4d >::full_type_name = cuT( "mat2x4d" );
	const Castor::String UniformTyper< UniformType::eMat3x2d >::full_type_name = cuT( "mat3x2d" );
	const Castor::String UniformTyper< UniformType::eMat3x3d >::full_type_name = cuT( "mat3x3d" );
	const Castor::String UniformTyper< UniformType::eMat3x4d >::full_type_name = cuT( "mat3x4d" );
	const Castor::String UniformTyper< UniformType::eMat4x2d >::full_type_name = cuT( "mat4x2d" );
	const Castor::String UniformTyper< UniformType::eMat4x3d >::full_type_name = cuT( "mat4x3d" );
	const Castor::String UniformTyper< UniformType::eMat4x4d >::full_type_name = cuT( "mat4x4d" );

	//*************************************************************************************************

	Uniform::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Uniform >{ p_tabs }
	{
	}

	bool Uniform::TextWriter::operator()( Uniform const & p_variable, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "variable\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttype " ) + p_variable.GetFullTypeName() + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Uniform >::CheckError( l_return, "Uniform type" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tname " ) + p_variable.GetName() + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Uniform >::CheckError( l_return, "Uniform name" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tvalue " ) + p_variable.GetStrValue() + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Uniform >::CheckError( l_return, "Uniform value" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	Uniform::Uniform( uint32_t p_occurences )
		: m_changed( true )
		, m_occurences( p_occurences )
	{
	}

	Uniform::~Uniform()
	{
	}

	void Uniform::SetStrValue( String const & p_value, uint32_t p_index )
	{
		REQUIRE( p_index < m_occurences );
		DoSetStrValue( p_value, p_index );
		m_changed = true;
	}

	String Uniform::GetStrValue( uint32_t p_index )const
	{
		REQUIRE( p_index < m_occurences );
		return DoGetStrValue( p_index );
	}

	void Uniform::SetStrValues( String const & p_value )
	{
		StringArray l_values = string::split( p_value, cuT( "|" ) );
		REQUIRE( l_values.size() == m_occurences );
		uint32_t l_index = 0u;

		for ( auto l_value : l_values )
		{
			if ( l_index < m_occurences )
			{
				SetStrValue( l_value, l_index++ );
			}
		}

		m_changed = true;
	}

	String Uniform::GetStrValues()const
	{
		String l_return;
		String l_separator;

		for ( uint32_t i = 0u; i < m_occurences; ++i )
		{
			l_return += l_separator + GetStrValue( i );
			l_separator = "|";
		}

		return l_return;
	}
}
