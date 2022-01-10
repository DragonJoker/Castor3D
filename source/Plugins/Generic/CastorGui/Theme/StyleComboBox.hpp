/*
See LICENSE file in root folder
*/
#ifndef ___CI_ComboBoxStyle_H___
#define ___CI_ComboBoxStyle_H___

#include "StyleButton.hpp"
#include "StyleListBox.hpp"

namespace CastorGui
{
	/**
	 *\brief		ComboBox style.
	*/
	class ComboBoxStyle
		: public ControlStyle
	{
	public:
		ComboBoxStyle( castor::String const & name
			, castor3d::Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ ControlType::eComboBox
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
