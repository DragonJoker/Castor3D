/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTextureConfiguration_H___
#define ___CSE_TextTextureConfiguration_H___
#pragma once

#include <Castor3D/Material/Texture/TextureConfiguration.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< TextureConfiguration >
		: public TextWriterT< TextureConfiguration >
	{
	public:
		explicit TextWriter( String const & tabs
			, Engine const & engine );
		bool operator()( TextureConfiguration const & object
			, StringStream & file )override;

	private:
		Engine const & m_engine;
	};
}

#endif
