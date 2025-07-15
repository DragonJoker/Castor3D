/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextComboBoxCtrl_H___
#define ___CSE_TextComboBoxCtrl_H___

#include <Castor3D/Gui/Controls/CtrlComboBox.hpp>
#include <Castor3D/Gui/Theme/StyleComboBox.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< ComboBoxCtrl >
		: public TextWriterT< ComboBoxCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( ComboBoxCtrl const & overlay
			, StringStream & file )override;

	private:
		String m_prefix;
	};

	template<>
	class TextWriter< ComboBoxStyle >
		: public TextWriterT< ComboBoxStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( ComboBoxStyle const & overlay
			, StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
