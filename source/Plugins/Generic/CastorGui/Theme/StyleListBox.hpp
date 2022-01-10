/*
See LICENSE file in root folder
*/
#ifndef ___CI_ListBoxStyle_H___
#define ___CI_ListBoxStyle_H___

#include "StyleStatic.hpp"

namespace CastorGui
{
	/**
	*\brief		ListBox style
	*/
	class ListBoxStyle
		: public ControlStyle
	{
	public:
		ListBoxStyle( castor::String const & name
			, castor3d::Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ ControlType::eListBox
				, name
				, engine }
			, m_itemStyle{ name + "/Item", engine, fontName }
			, m_selectedItemStyle{ name + "/SelectedItem", engine, fontName }
			, m_highlightedItemStyle{ name + "/HighlightedItem", engine, fontName }
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
		castor::RgbColour doGetHighlightedColour( castor::RgbColour colour )
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
				, getBackgroundMaterial()->getName() + "_Highlight"
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
