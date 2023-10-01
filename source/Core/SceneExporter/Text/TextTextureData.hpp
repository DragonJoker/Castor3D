/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTextureData_H___
#define ___CSE_TextTextureData_H___
#pragma once

#include <Castor3D/Material/Texture/TextureSourceInfo.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::TextureData >
		: public TextWriterT< castor3d::TextureData >
	{
	public:
		explicit TextWriter( String const & tabs
			, castor3d::Engine const & engine
			, Path const & folder
			, String subFolder );
		bool operator()( castor3d::TextureData const & unit
			, StringStream & file )override;

	private:
		castor3d::Engine const & m_engine;
		Path m_folder;
		String m_subFolder;
	};
}

#endif
