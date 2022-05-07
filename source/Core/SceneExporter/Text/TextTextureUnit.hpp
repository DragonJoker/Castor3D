/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTextureUnit_H___
#define ___CSE_TextTextureUnit_H___
#pragma once

#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::TextureUnit >
		: public TextWriterT< castor3d::TextureUnit >
	{
	public:
		explicit TextWriter( String const & tabs
			, castor3d::PassTypeID type
			, bool isPbr
			, Path const & folder
			, String subFolder );
		bool operator()( castor3d::TextureUnit const & unit
			, StringStream & file )override;

	private:
		castor3d::PassTypeID m_type;
		bool m_isPbr;
		Path m_folder;
		String m_subFolder;
	};
}

#endif
