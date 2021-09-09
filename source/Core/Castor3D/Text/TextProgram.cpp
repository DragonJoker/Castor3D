#include "Castor3D/Text/TextProgram.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< ShaderProgram >::TextWriter( String const & tabs )
		: TextWriterT< ShaderProgram >{ tabs }
	{
	}

	bool TextWriter< ShaderProgram >::operator()( ShaderProgram const & shaderProgram
		, StringStream & file )
	{
		bool result = false;
		bool hasFile = false;

		for ( auto shaderFile : shaderProgram.m_files )
		{
			hasFile |= !shaderFile.second.empty();
		}

		if ( hasFile )
		{
			if ( auto block{ beginBlock( file, "cs_shader_program" ) } )
			{
				// TODO : Implement castor3d::ShaderModule::TextWriter (to support at least SPIR-V external shaders).

				//for ( auto & module : shaderProgram.m_modules )
				//{
				//	result = ShaderModule::TextWriter( tabs, module.first )( module.second, file );
				//}
			}
		}
		else
		{
			result = true;
		}

		return result;
	}
}
