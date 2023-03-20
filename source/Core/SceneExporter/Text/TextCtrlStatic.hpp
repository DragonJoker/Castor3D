/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextStaticCtrl_H___
#define ___CSE_TextStaticCtrl_H___

#include <Castor3D/Gui/Controls/CtrlStatic.hpp>
#include <Castor3D/Gui/Theme/StyleStatic.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::StaticCtrl >
		: public TextWriterT< castor3d::StaticCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::StaticCtrl const & overlay
			, castor::StringStream & file )override;

	private:
		String m_prefix;
	};

	template<>
	class TextWriter< castor3d::StaticStyle >
		: public TextWriterT< castor3d::StaticStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( castor3d::StaticStyle const & overlay
			, castor::StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
