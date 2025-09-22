/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextShaderProgram_H___
#define ___CSE_TextShaderProgram_H___

#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< ShaderProgram >
		: public TextWriterT< ShaderProgram >
	{
	public:
		explicit TextWriter( String const & tabs, Path const & folder );
		bool operator()( ShaderProgram const & program
			, StringStream & file )override;

	private:
		Path m_folder;
	};
}

#endif
