/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextFrameCtrl_H___
#define ___CSE_TextFrameCtrl_H___

#include <Castor3D/Gui/Controls/CtrlFrame.hpp>
#include <Castor3D/Gui/Theme/StyleFrame.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::FrameCtrl >
		: public TextWriterT< castor3d::FrameCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::FrameCtrl const & overlay
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< castor3d::FrameStyle >
		: public TextWriterT< castor3d::FrameStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( castor3d::FrameStyle const & overlay
			, castor::StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
