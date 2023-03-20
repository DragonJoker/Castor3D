/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextButtonCtrl_H___
#define ___CSE_TextButtonCtrl_H___

#include <Castor3D/Gui/Controls/CtrlButton.hpp>
#include <Castor3D/Gui/Theme/StyleButton.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ButtonCtrl >
		: public TextWriterT< castor3d::ButtonCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::ButtonCtrl const & overlay
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< castor3d::ButtonStyle >
		: public TextWriterT< castor3d::ButtonStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( castor3d::ButtonStyle const & overlay
			, castor::StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
