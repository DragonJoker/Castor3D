/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ProgressStyle_H___
#define ___C3D_ProgressStyle_H___

#include "StyleControl.hpp"
#include "StylePanel.hpp"
#include "StyleStatic.hpp"

namespace castor3d
{
	class ProgressStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eProgress;

		ProgressStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ProgressStyle{ name, nullptr, engine, fontName }
		{
		}

		ProgressStyle( castor::String const & name
			, Scene * scene
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type, name, scene, engine }
			, m_titleFontName{ fontName }
			, m_titleMaterial{ getEngine().findMaterial( cuT( "White" ) ) }
			, m_containerStyle{ name + "/Container", scene, engine }
			, m_progressStyle{ name + "/Progress", scene, engine }
			, m_textFontName{ fontName }
			, m_textMaterial{ m_titleMaterial }
		{
		}

		castor::String const & getTitleFontName()const
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

		castor::String const & getTextFontName()const
		{
			return m_textFontName;
		}

		MaterialObs getTextMaterial()const
		{
			return m_textMaterial;
		}

		void setTitleFontName( castor::String value )
		{
			m_titleFontName = std::move( value );
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

		void setTextFontName( castor::String value )
		{
			m_textFontName = std::move( value );
		}

		void setTextMaterial( MaterialObs value )
		{
			m_textMaterial = value;
		}

	private:
		void doUpdateBackgroundMaterial()override
		{
		}

		void doUpdateForegroundMaterial()override
		{
		}

	private:
		castor::String m_titleFontName{};
		MaterialObs m_titleMaterial{};
		PanelStyle m_containerStyle;
		PanelStyle m_progressStyle;
		castor::String m_textFontName{};
		MaterialObs m_textMaterial{};
	};
}

#endif
