/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTextureUnit_H___
#define ___CSE_TextTextureUnit_H___
#pragma once

#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< TextureUnit >
		: public TextWriterT< TextureUnit >
	{
	public:
		explicit TextWriter( String const & tabs
			, Path const & folder
			, String subFolder );
		bool operator()( TextureUnit const & unit
			, StringStream & file )override;

	private:
		Path m_folder;
		String m_subFolder;
	};
}

#endif
