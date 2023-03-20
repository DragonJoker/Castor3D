/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSliderCtrl_H___
#define ___CSE_TextSliderCtrl_H___

#include <Castor3D/Gui/Controls/CtrlSlider.hpp>
#include <Castor3D/Gui/Theme/StyleSlider.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SliderCtrl >
		: public TextWriterT< castor3d::SliderCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::SliderCtrl const & overlay
			, castor::StringStream & file )override;

	private:
		String m_prefix;
	};

	template<>
	class TextWriter< castor3d::SliderStyle >
		: public TextWriterT< castor3d::SliderStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( castor3d::SliderStyle const & overlay
			, castor::StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
