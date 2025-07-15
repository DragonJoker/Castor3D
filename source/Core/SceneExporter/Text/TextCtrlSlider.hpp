/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSliderCtrl_H___
#define ___CSE_TextSliderCtrl_H___

#include <Castor3D/Gui/Controls/CtrlSlider.hpp>
#include <Castor3D/Gui/Theme/StyleSlider.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< SliderCtrl >
		: public TextWriterT< SliderCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( SliderCtrl const & overlay
			, StringStream & file )override;

	private:
		String m_prefix;
	};

	template<>
	class TextWriter< SliderStyle >
		: public TextWriterT< SliderStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( SliderStyle const & overlay
			, StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
