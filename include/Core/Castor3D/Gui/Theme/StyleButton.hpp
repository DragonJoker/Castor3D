/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ButtonStyle_H___
#define ___C3D_ButtonStyle_H___

#include "StyleControl.hpp"

namespace castor3d
{
	class ButtonStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eButton;

		ButtonStyle( castor::String const & name
			, Scene * scene
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine
				, MouseCursor::eHand }
			, m_textMaterial{ getForegroundMaterial() }
			, m_highlightedTextMaterial{ doCreateMaterial( getTextMaterial(), -0.1f, "_THG" ) }
			, m_highlightedBackgroundMaterial{ doCreateMaterial( getBackgroundMaterial(), 0.1f, "_BHG" ) }
			, m_highlightedForegroundMaterial{ doCreateMaterial( getForegroundMaterial(), -0.1f, "_FHG" ) }
			, m_pushedTextMaterial{ doCreateMaterial( getTextMaterial(), -0.1f, "_TPU" ) }
			, m_pushedBackgroundMaterial{ doCreateMaterial( getBackgroundMaterial(), 0.1f, "_BPU" ) }
			, m_pushedForegroundMaterial{ doCreateMaterial( getForegroundMaterial(), -0.1f, "_FPU" ) }
			, m_disabledTextMaterial{ doCreateMaterial( getTextMaterial(), -0.2f, "_TDS" ) }
			, m_disabledBackgroundMaterial{ doCreateMaterial( getBackgroundMaterial(), +0.2f, "_BDS" ) }
			, m_disabledForegroundMaterial{ doCreateMaterial( getForegroundMaterial(), -0.2f, "_FDS" ) }
			, m_fontName{ fontName }
		{
		}

		ButtonStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ButtonStyle{ name, nullptr, engine, fontName }
		{
		}

		void setTextMaterial( MaterialObs material )
		{
			m_textMaterial = material;
		}

		void setHighlightedBackgroundMaterial( MaterialObs material )
		{
			m_highlightedBackgroundMaterial = material;
		}

		void setHighlightedForegroundMaterial( MaterialObs material )
		{
			m_highlightedForegroundMaterial = material;
		}

		void setHighlightedTextMaterial( MaterialObs material )
		{
			m_highlightedTextMaterial = material;
		}

		void setPushedBackgroundMaterial( MaterialObs material )
		{
			m_pushedBackgroundMaterial = material;
		}

		void setPushedForegroundMaterial( MaterialObs material )
		{
			m_pushedForegroundMaterial = material;
		}

		void setPushedTextMaterial( MaterialObs material )
		{
			m_pushedTextMaterial = material;
		}

		void setDisabledBackgroundMaterial( MaterialObs material )
		{
			m_disabledBackgroundMaterial = material;
		}

		void setDisabledForegroundMaterial( MaterialObs material )
		{
			m_disabledForegroundMaterial = material;
		}

		void setDisabledTextMaterial( MaterialObs material )
		{
			m_disabledTextMaterial = material;
		}

		void setFont( castor::StringView value )
		{
			m_fontName = value;
		}

		MaterialObs getHighlightedBackgroundMaterial()const
		{
			return m_highlightedBackgroundMaterial;
		}

		MaterialObs getHighlightedForegroundMaterial()const
		{
			return m_highlightedForegroundMaterial;
		}

		MaterialObs getHighlightedTextMaterial()const
		{
			return m_highlightedTextMaterial;
		}

		MaterialObs getPushedBackgroundMaterial()const
		{
			return m_pushedBackgroundMaterial;
		}

		MaterialObs getPushedForegroundMaterial()const
		{
			return m_pushedForegroundMaterial;
		}

		MaterialObs getPushedTextMaterial()const
		{
			return m_pushedTextMaterial;
		}

		MaterialObs getDisabledBackgroundMaterial()const
		{
			return m_disabledBackgroundMaterial;
		}

		MaterialObs getDisabledForegroundMaterial()const
		{
			return m_disabledForegroundMaterial;
		}

		MaterialObs getDisabledTextMaterial()const
		{
			return m_disabledTextMaterial;
		}

		MaterialObs getTextMaterial()const
		{
			return m_textMaterial;
		}

		castor::String const & getFontName()const
		{
			return m_fontName;
		}

	private:
		void doUpdateBackgroundMaterial()override
		{
			m_highlightedBackgroundMaterial = doCreateMaterial( getBackgroundMaterial(), 0.1f, "_BHG" );
		}

		void doUpdateForegroundMaterial()override
		{
			m_highlightedForegroundMaterial = doCreateMaterial( getForegroundMaterial(), -0.1f, "_FHG" );
		}

	private:
		MaterialObs m_textMaterial{};
		MaterialObs m_highlightedTextMaterial{};
		MaterialObs m_highlightedBackgroundMaterial{};
		MaterialObs m_highlightedForegroundMaterial{};
		MaterialObs m_pushedTextMaterial{};
		MaterialObs m_pushedBackgroundMaterial{};
		MaterialObs m_pushedForegroundMaterial{};
		MaterialObs m_disabledTextMaterial{};
		MaterialObs m_disabledBackgroundMaterial{};
		MaterialObs m_disabledForegroundMaterial{};
		castor::String m_fontName{};
	};
}

#endif
