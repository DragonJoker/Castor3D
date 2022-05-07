/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextShaderProgram_H___
#define ___CSE_TextShaderProgram_H___

#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ShaderProgram >
		: public TextWriterT< castor3d::ShaderProgram >
	{
	public:
		explicit TextWriter( castor::String const & tabs );
		bool operator()( castor3d::ShaderProgram const & program
			, castor::StringStream & file )override;
	};
}

#endif
