/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextButtonCtrl_H___
#define ___CSE_TextButtonCtrl_H___

#include <Castor3D/Gui/Controls/CtrlButton.hpp>
#include <Castor3D/Gui/Theme/StyleButton.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< ButtonCtrl >
		: public TextWriterT< ButtonCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( ButtonCtrl const & overlay
			, StringStream & file )override;
	};

	template<>
	class TextWriter< ButtonStyle >
		: public TextWriterT< ButtonStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( ButtonStyle const & overlay
			, StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
