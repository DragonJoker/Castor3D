/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextEditCtrl_H___
#define ___CSE_TextEditCtrl_H___

#include <Castor3D/Gui/Controls/CtrlEdit.hpp>
#include <Castor3D/Gui/Theme/StyleEdit.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< EditCtrl >
		: public TextWriterT< EditCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( EditCtrl const & overlay
			, StringStream & file )override;
	};

	template<>
	class TextWriter< EditStyle >
		: public TextWriterT< EditStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( EditStyle const & overlay
			, StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
