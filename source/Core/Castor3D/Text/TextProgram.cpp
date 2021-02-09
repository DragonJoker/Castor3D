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
		, TextFile & file )
	{
		bool result = false;
		bool hasFile = false;

		for ( auto file : shaderProgram.m_files )
		{
			hasFile |= !file.second.empty();
		}

		if ( hasFile )
		{
			if ( auto block = beginBlock( "cs_shader_program", file ) )
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
