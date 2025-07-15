/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextFrameCtrl_H___
#define ___CSE_TextFrameCtrl_H___

#include <Castor3D/Gui/Controls/CtrlFrame.hpp>
#include <Castor3D/Gui/Theme/StyleFrame.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< FrameCtrl >
		: public TextWriterT< FrameCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( FrameCtrl const & overlay
			, StringStream & file )override;
	};

	template<>
	class TextWriter< FrameStyle >
		: public TextWriterT< FrameStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( FrameStyle const & overlay
			, StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
