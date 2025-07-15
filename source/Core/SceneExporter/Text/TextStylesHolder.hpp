/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextStylesHolder_H___
#define ___CSE_TextStylesHolder_H___

#include <Castor3D/Gui/Theme/StylePanel.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< StylesHolder >
		: public TextWriterT< StylesHolder >
	{
	public:
		explicit TextWriter( String const & tabs
			, Scene const * scene
			, String const & fontName );
		bool operator()( StylesHolder const & object
			, StringStream & file )override;

	private:
		Scene const * m_scene;
		String const & m_fontName;
	};
}

#endif
