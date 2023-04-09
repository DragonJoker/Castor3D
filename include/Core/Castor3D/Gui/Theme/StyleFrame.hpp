/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrameStyle_H___
#define ___C3D_FrameStyle_H___

#include "StylePanel.hpp"
#include "StyleStatic.hpp"

namespace castor3d
{
	class FrameStyle
		: public ControlStyle
	{
		friend class FrameCtrl;

	public:
		static ControlType constexpr Type = ControlType::eFrame;

		FrameStyle( castor::String const & name
			, Scene * scene
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine }
			, m_headerStyle{ name + "/Header", scene, engine, fontName }
			, m_contentStyle{ name + "/Content", scene, engine }
		{
			m_headerStyle.setBackgroundInvisible( true );
			m_contentStyle.setBackgroundInvisible( true );
			m_contentStyle.setForegroundInvisible( true );
		}

		FrameStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: FrameStyle{ name, nullptr, engine, fontName }
		{
		}

		void setHeaderFont( castor::String const & value )
		{
			m_headerStyle.setFont( value );
		}

		void setHeaderTextMaterial( MaterialObs value )
		{
			m_headerStyle.setTextMaterial( value );
		}

		castor::String const & getHeaderFontName()const
		{
			return m_headerStyle.getFontName();
		}

		MaterialObs getHeaderTextMaterial()const
		{
			return m_headerStyle.getTextMaterial();
		}

	private:
		StaticStyle const & getHeaderStyle()const
		{
			return m_headerStyle;
		}

		PanelStyle const & getContentStyle()const
		{
			return m_contentStyle;
		}

		StaticStyle & getHeaderStyle()
		{
			return m_headerStyle;
		}

		PanelStyle & getContentStyle()
		{
			return m_contentStyle;
		}

		void doUpdateBackgroundMaterial()override
		{
		}

		void doUpdateForegroundMaterial()override
		{
			m_headerStyle.setForegroundMaterial( getForegroundMaterial() );
		}

	private:
		StaticStyle m_headerStyle;
		PanelStyle m_contentStyle;
	};
}

#endif
