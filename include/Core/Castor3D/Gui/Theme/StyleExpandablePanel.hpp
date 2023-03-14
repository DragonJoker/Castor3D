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
		ExpandablePanelStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ ControlType::eExpandablePanel
				, name
				, engine }
			, m_headerStyle{ name + "/Header", engine }
			, m_expandStyle{ name + "/Expand", engine, fontName }
			, m_panelStyle{ name + "/Panel", engine }
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

		PanelStyle const & getPanelStyle()const
		{
			return m_panelStyle;
		}

		PanelStyle & getHeaderStyle()
		{
			return m_headerStyle;
		}

		ButtonStyle & getExpandStyle()
		{
			return m_expandStyle;
		}

		PanelStyle & getPanelStyle()
		{
			return m_panelStyle;
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
		PanelStyle m_panelStyle;
	};
}

#endif
