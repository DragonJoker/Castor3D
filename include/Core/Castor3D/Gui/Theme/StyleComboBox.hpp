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
			, Scene * scene
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine }
			, m_buttonStyle{ name + cuT( "/Expand" ), scene, engine, fontName }
			, m_listBoxStyle{ name + cuT( "/Elements" ), scene, engine, fontName }
		{
		}

		ComboBoxStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ComboBoxStyle{ name, nullptr, engine, fontName }
		{
		}

		ButtonStyle const & getExpandStyle()const
		{
			return m_buttonStyle;
		}

		ListBoxStyle const & getElementsStyle()const
		{
			return m_listBoxStyle;
		}

		ButtonStyle & getExpandStyle()
		{
			return m_buttonStyle;
		}

		ListBoxStyle & getElementsStyle()
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
