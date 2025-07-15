/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTextureData_H___
#define ___CSE_TextTextureData_H___
#pragma once

#include <Castor3D/Material/Texture/TextureSourceInfo.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< TextureData >
		: public TextWriterT< TextureData >
	{
	public:
		explicit TextWriter( String const & tabs
			, Engine const & engine
			, Path const & folder
			, String subFolder );
		bool operator()( TextureData const & unit
			, StringStream & file )override;

	private:
		Engine const & m_engine;
		Path m_folder;
		String m_subFolder;
	};
}

#endif
