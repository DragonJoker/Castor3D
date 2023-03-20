/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ExpandablePanelStyle_H___
#define ___C3D_ExpandablePanelStyle_H___

#include "StyleButton.hpp"
#include "StylePanel.hpp"
#include "StyleStatic.hpp"

namespace castor3d
{
	class ExpandablePanelStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eExpandablePanel;

		ExpandablePanelStyle( castor::String const & name
			, Scene * scene
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine }
			, m_headerStyle{ name + "/Header", scene, engine }
			, m_expandStyle{ name + "/Expand", scene, engine, fontName }
			, m_contentStyle{ name + "/Content", scene, engine }
		{
		}

		ExpandablePanelStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ExpandablePanelStyle{ name, nullptr, engine, fontName }
		{
		}

		PanelStyle const & getHeaderStyle()const
		{
			return m_headerStyle;
		}

		ButtonStyle const & getExpandStyle()const
		{
			return m_expandStyle;
		}

		PanelStyle const & getContentStyle()const
		{
			return m_contentStyle;
		}

		PanelStyle & getHeaderStyle()
		{
			return m_headerStyle;
		}

		ButtonStyle & getExpandStyle()
		{
			return m_expandStyle;
		}

		PanelStyle & getContentStyle()
		{
			return m_contentStyle;
		}

	private:
		void doUpdateBackgroundMaterial()override
		{
		}

		void doUpdateForegroundMaterial()override
		{
		}

	private:
		PanelStyle m_headerStyle;
		ButtonStyle m_expandStyle;
		PanelStyle m_contentStyle;
	};
}

#endif
