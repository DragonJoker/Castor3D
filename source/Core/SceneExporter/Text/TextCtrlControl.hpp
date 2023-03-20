/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextControl_H___
#define ___CSE_TextControl_H___

#include <Castor3D/Gui/Controls/CtrlControl.hpp>
#include <Castor3D/Gui/Theme/StyleControl.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Control >
		: public TextWriterT< castor3d::Control >
	{
	public:
		explicit TextWriter( String const & tabs
			, bool customStyle = false );
		bool operator()( castor3d::Control const & overlay
			, castor::StringStream & file )override;

	private:
		bool m_customStyle;
	};

	template<>
	class TextWriter< castor3d::ControlStyle >
		: public TextWriterT< castor3d::ControlStyle >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::ControlStyle const & overlay
			, castor::StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
