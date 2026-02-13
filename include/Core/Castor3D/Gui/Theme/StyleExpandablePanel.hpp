/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ExpandablePanelStyle_H___
#define ___C3D_ExpandablePanelStyle_H___

#include "StyleButton.hpp"
#include "StylePanel.hpp"
#include "StyleStatic.hpp"

namespace c3d
{
	class ExpandablePanelStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eExpandablePanel;

		ExpandablePanelStyle( String const & name
			, Scene * scene
			, Engine & engine
			, String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine }
			, m_headerStyle{ name + cuT( "/Header" ), scene, engine }
			, m_expandStyle{ name + cuT( "/Expand" ), scene, engine, fontName }
			, m_contentStyle{ name + cuT( "/Content" ), scene, engine }
		{
		}

		ExpandablePanelStyle( String const & name
			, Engine & engine
			, String const & fontName )
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
		ControlStyleRPtr doCreate( String const & cloneName )const override;

		void doCopyInto( ControlStyle & copy )const override
		{
			auto & target = static_cast< ExpandablePanelStyle & >( copy );
			m_headerStyle.copyInto( target.m_headerStyle );
			m_expandStyle.copyInto( target.m_expandStyle );
			m_contentStyle.copyInto( target.m_contentStyle );
		}

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
