/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ProgressStyle_H___
#define ___C3D_ProgressStyle_H___

#include "StyleControl.hpp"
#include "StylePanel.hpp"
#include "StyleStatic.hpp"

namespace c3d
{
	class ProgressStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eProgress;

		ProgressStyle( String const & name
			, Engine & engine
			, String const & fontName )
			: ProgressStyle{ name, nullptr, engine, fontName }
		{
		}

		ProgressStyle( String const & name
			, Scene * scene
			, Engine & engine
			, String const & fontName )
			: ControlStyle{ Type, name, scene, engine }
			, m_titleFontName{ fontName }
			, m_titleMaterial{ getEngine().findMaterial( cuT( "White" ) ) }
			, m_containerStyle{ name + cuT( "/Container" ), scene, engine }
			, m_progressStyle{ name + cuT( "/Progress" ), scene, engine }
			, m_labelStyle{ name + cuT( "/Label" ), scene, engine, fontName }
		{
			setTextMaterial( m_titleMaterial );
			m_labelStyle.setBackgroundInvisible( true );
		}

		String const & getTitleFontName()const
		{
			return m_titleFontName;
		}

		MaterialObs getTitleMaterial()const
		{
			return m_titleMaterial;
		}

		PanelStyle const & getContainerStyle()const
		{
			return m_containerStyle;
		}

		PanelStyle const & getProgressStyle()const
		{
			return m_progressStyle;
		}

		StaticStyle const & getLabelStyle()const
		{
			return m_labelStyle;
		}

		String const & getTextFontName()const
		{
			return m_labelStyle.getFontName();
		}

		MaterialObs getTextMaterial()const
		{
			return m_labelStyle.getForegroundMaterial();
		}

		void setTitleFontName( StringView value )
		{
			m_titleFontName = c3d::move( value );
		}

		void setTitleMaterial( MaterialObs value )
		{
			m_titleMaterial = value;
		}

		PanelStyle & getContainerStyle()
		{
			return m_containerStyle;
		}

		PanelStyle & getProgressStyle()
		{
			return m_progressStyle;
		}

		StaticStyle & getLabelStyle()
		{
			return m_labelStyle;
		}

		void setTextFontName( String value )
		{
			m_labelStyle.setFont( value );
		}

		void setTextMaterial( MaterialObs value )
		{
			m_labelStyle.setTextMaterial( value );
		}

	private:
		ControlStyleRPtr doCreate( String const & cloneName )const override;

		void doCopyInto( ControlStyle & copy )const override
		{
			auto & target = static_cast< ProgressStyle & >( copy );
			target.m_titleFontName = m_titleFontName;
			target.m_titleMaterial = m_titleMaterial;
			m_containerStyle.copyInto( target.m_containerStyle );
			m_progressStyle.copyInto( target.m_progressStyle );
			m_labelStyle.copyInto( target.m_labelStyle );
		}

		void doUpdateBackgroundMaterial()override
		{
		}

		void doUpdateForegroundMaterial()override
		{
		}

	private:
		String m_titleFontName{};
		MaterialObs m_titleMaterial{};
		PanelStyle m_containerStyle;
		PanelStyle m_progressStyle;
		StaticStyle m_labelStyle;
	};
}

#endif
