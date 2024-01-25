/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ScrollBarStyle_H___
#define ___C3D_ScrollBarStyle_H___

#include "StyleButton.hpp"
#include "StylePanel.hpp"

namespace castor3d
{
	class ScrollBarStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eScrollBar;

		ScrollBarStyle( castor::String const & name
			, Scene * scene
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine
				, MouseCursor::eHand }
			, m_barStyle{ name + cuT( "/Bar" ), scene, engine }
			, m_thumbStyle{ name + cuT( "/Thumb" ), scene, engine }
			, m_beginStyle{ name + cuT( "/Begin" ), scene, engine, fontName }
			, m_endStyle{ name + cuT( "/End" ), scene, engine, fontName }
		{
		}

		ScrollBarStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ScrollBarStyle{ name, nullptr, engine, fontName }
		{
		}

		PanelStyle const & getBarStyle()const
		{
			return m_barStyle;
		}

		PanelStyle const & getThumbStyle()const
		{
			return m_thumbStyle;
		}

		ButtonStyle const & getBeginStyle()const
		{
			return m_beginStyle;
		}

		ButtonStyle const & getEndStyle()const
		{
			return m_endStyle;
		}

		PanelStyle & getBarStyle()
		{
			return m_barStyle;
		}

		PanelStyle & getThumbStyle()
		{
			return m_thumbStyle;
		}

		ButtonStyle & getBeginStyle()
		{
			return m_beginStyle;
		}

		ButtonStyle & getEndStyle()
		{
			return m_endStyle;
		}

	private:
		void doUpdateBackgroundMaterial()override
		{
		}

		void doUpdateForegroundMaterial()override
		{
		}

	private:
		PanelStyle m_barStyle;
		PanelStyle m_thumbStyle;
		ButtonStyle m_beginStyle;
		ButtonStyle m_endStyle;
	};
}

#endif
