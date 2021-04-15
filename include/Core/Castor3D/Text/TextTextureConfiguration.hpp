/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextTextureConfiguration_H___
#define ___C3D_TextTextureConfiguration_H___
#pragma once

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::TextureConfiguration >
		: public TextWriterT< castor3d::TextureConfiguration >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs
			, castor3d::MaterialType type );
		C3D_API bool operator()( castor3d::TextureConfiguration const & object
			, StringStream & file )override;

	private:
		castor3d::MaterialType m_type;
	};
}

#endif
