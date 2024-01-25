/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ListBoxStyle_H___
#define ___C3D_ListBoxStyle_H___

#include "StyleStatic.hpp"

namespace castor3d
{
	class ListBoxStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eListBox;

		ListBoxStyle( castor::String const & name
			, Scene * scene
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine }
			, m_itemStyle{ name + cuT( "/Item" ), scene, engine, fontName }
			, m_selectedItemStyle{ name + cuT( "/SelectedItem" ), scene, engine, fontName }
			, m_highlightedItemStyle{ name + cuT( "/HighlightedItem" ), scene, engine, fontName }
		{
		}

		ListBoxStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ListBoxStyle{ name, nullptr, engine, fontName }
		{
		}

		StaticStyle const & getItemStyle()const
		{
			return m_itemStyle;
		}

		StaticStyle const & getSelectedItemStyle()const
		{
			return m_selectedItemStyle;
		}

		StaticStyle const & getHighlightedItemStyle()const
		{
			return m_highlightedItemStyle;
		}

		StaticStyle & getItemStyle()
		{
			return m_itemStyle;
		}

		StaticStyle & getSelectedItemStyle()
		{
			return m_selectedItemStyle;
		}

		StaticStyle & getHighlightedItemStyle()
		{
			return m_highlightedItemStyle;
		}

	private:
		castor::HdrRgbColour doGetHighlightedColour( castor::HdrRgbColour colour )const noexcept
		{
			colour.red() = std::min( 1.0f, colour.red() / 2.0f );
			colour.green() = std::min( 1.0f, colour.green() / 2.0f );
			colour.blue() = std::min( 1.0f, colour.blue() / 2.0f );
			return colour;
		}

		void doUpdateBackgroundMaterial()override
		{
			m_itemStyle.setBackgroundMaterial( getBackgroundMaterial() );
			m_highlightedItemStyle.setBackgroundMaterial( createMaterial( getEngine()
				, getBackgroundMaterial()->getName() + cuT( "_Highlight" )
				, doGetHighlightedColour( getMaterialColour( *getBackgroundMaterial()->getPass( 0 ) ) ) ) );
		}

		void doUpdateForegroundMaterial()override
		{
			m_itemStyle.setForegroundMaterial( getForegroundMaterial() );
		}

	private:
		StaticStyle m_itemStyle;
		StaticStyle m_selectedItemStyle;
		StaticStyle m_highlightedItemStyle;
	};
}

#endif
