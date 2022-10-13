/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTextureConfiguration_H___
#define ___CSE_TextTextureConfiguration_H___
#pragma once

#include <Castor3D/Material/Texture/TextureConfiguration.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::TextureConfiguration >
		: public TextWriterT< castor3d::TextureConfiguration >
	{
	public:
		explicit TextWriter( String const & tabs
			, castor3d::TextureUnit const & parent
			, bool isPbr );
		bool operator()( castor3d::TextureConfiguration const & object
			, StringStream & file )override;

	private:
		castor3d::TextureUnit const & m_parent;
		bool m_isPbr;
	};
}

#endif
