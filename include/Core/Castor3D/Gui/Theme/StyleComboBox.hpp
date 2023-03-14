/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComboBoxStyle_H___
#define ___C3D_ComboBoxStyle_H___

#include "StyleButton.hpp"
#include "StyleListBox.hpp"

namespace castor3d
{
	class ComboBoxStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eComboBox;

		ComboBoxStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, engine }
			, m_buttonStyle{ name + "/Button", engine, fontName }
			, m_listBoxStyle{ name + "/List", engine, fontName }
		{
		}

		ButtonStyle const & getButtonStyle()const
		{
			return m_buttonStyle;
		}

		ListBoxStyle const & getListBoxStyle()const
		{
			return m_listBoxStyle;
		}

		ButtonStyle & getButtonStyle()
		{
			return m_buttonStyle;
		}

		ListBoxStyle & getListBoxStyle()
		{
			return m_listBoxStyle;
		}

	private:
		void doUpdateBackgroundMaterial()override
		{
			m_listBoxStyle.setBackgroundMaterial( getBackgroundMaterial() );
		}

		void doUpdateForegroundMaterial()override
		{
			m_buttonStyle.setForegroundMaterial( getForegroundMaterial() );
			m_listBoxStyle.setForegroundMaterial( getForegroundMaterial() );
		}

	private:
		ButtonStyle m_buttonStyle;
		ListBoxStyle m_listBoxStyle;
	};
}

#endif
