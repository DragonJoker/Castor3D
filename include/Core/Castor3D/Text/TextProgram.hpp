/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextShaderProgram_H___
#define ___C3D_TextShaderProgram_H___

#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ShaderProgram >
		: public TextWriterT< castor3d::ShaderProgram >
	{
	public:
		C3D_API explicit TextWriter( castor::String const & tabs );
		C3D_API bool operator()( castor3d::ShaderProgram const & program
			, castor::TextFile & file )override;
	};
}

#endif
