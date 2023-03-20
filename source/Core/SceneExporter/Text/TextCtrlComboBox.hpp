/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextComboBoxCtrl_H___
#define ___CSE_TextComboBoxCtrl_H___

#include <Castor3D/Gui/Controls/CtrlComboBox.hpp>
#include <Castor3D/Gui/Theme/StyleComboBox.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ComboBoxCtrl >
		: public TextWriterT< castor3d::ComboBoxCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::ComboBoxCtrl const & overlay
			, castor::StringStream & file )override;

	private:
		String m_prefix;
	};

	template<>
	class TextWriter< castor3d::ComboBoxStyle >
		: public TextWriterT< castor3d::ComboBoxStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( castor3d::ComboBoxStyle const & overlay
			, castor::StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
