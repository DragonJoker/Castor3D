/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextStylesHolder_H___
#define ___CSE_TextStylesHolder_H___

#include <Castor3D/Gui/Theme/StylePanel.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::StylesHolder >
		: public TextWriterT< castor3d::StylesHolder >
	{
	public:
		explicit TextWriter( String const & tabs
			, castor3d::Scene const * scene
			, String const & fontName );
		bool operator()( castor3d::StylesHolder const & object
			, castor::StringStream & file )override;

	private:
		castor3d::Scene const * m_scene;
		String const & m_fontName;
	};
}

#endif
