/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextStaticCtrl_H___
#define ___CSE_TextStaticCtrl_H___

#include <Castor3D/Gui/Controls/CtrlStatic.hpp>
#include <Castor3D/Gui/Theme/StyleStatic.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< StaticCtrl >
		: public TextWriterT< StaticCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( StaticCtrl const & overlay
			, StringStream & file )override;

	private:
		String m_prefix;
	};

	template<>
	class TextWriter< StaticStyle >
		: public TextWriterT< StaticStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( StaticStyle const & overlay
			, StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
