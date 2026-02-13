/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ButtonStyle_H___
#define ___C3D_ButtonStyle_H___

#include "StyleControl.hpp"

namespace c3d
{
	class ButtonStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eButton;

		ButtonStyle( String const & name
			, Scene * scene
			, Engine & engine
			, String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine
				, MouseCursor::eHand }
			, m_textMaterial{ getForegroundMaterial() }
			, m_highlightedTextMaterial{ doCreateMaterial( getTextMaterial(), -0.1f, cuT( "_THG" ) ) }
			, m_highlightedBackgroundMaterial{ doCreateMaterial( getBackgroundMaterial(), 0.1f, cuT( "_BHG" ) ) }
			, m_highlightedForegroundMaterial{ doCreateMaterial( getForegroundMaterial(), -0.1f, cuT( "_FHG" ) ) }
			, m_pushedTextMaterial{ doCreateMaterial( getTextMaterial(), -0.1f, cuT( "_TPU" ) ) }
			, m_pushedBackgroundMaterial{ doCreateMaterial( getBackgroundMaterial(), 0.1f, cuT( "_BPU" ) ) }
			, m_pushedForegroundMaterial{ doCreateMaterial( getForegroundMaterial(), -0.1f, cuT( "_FPU" ) ) }
			, m_disabledTextMaterial{ doCreateMaterial( getTextMaterial(), -0.2f, cuT( "_TDS" ) ) }
			, m_disabledBackgroundMaterial{ doCreateMaterial( getBackgroundMaterial(), +0.2f, cuT( "_BDS" ) ) }
			, m_disabledForegroundMaterial{ doCreateMaterial( getForegroundMaterial(), -0.2f, cuT( "_FDS" ) ) }
			, m_fontName{ fontName }
		{
		}

		ButtonStyle( String const & name
			, Engine & engine
			, String const & fontName )
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

		void setFont( StringView value )
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

		String const & getFontName()const
		{
			return m_fontName;
		}

	private:
		ControlStyleRPtr doCreate( String const & cloneName )const override;

		void doCopyInto( ControlStyle & copy )const override
		{
			auto & target = static_cast< ButtonStyle & >( copy );
			target.m_textMaterial = m_textMaterial;
			target.m_highlightedBackgroundMaterial = m_highlightedBackgroundMaterial;
			target.m_highlightedForegroundMaterial = m_highlightedForegroundMaterial;
			target.m_highlightedTextMaterial = m_highlightedTextMaterial;
			target.m_pushedBackgroundMaterial = m_pushedBackgroundMaterial;
			target.m_pushedForegroundMaterial = m_pushedForegroundMaterial;
			target.m_pushedTextMaterial = m_pushedTextMaterial;
			target.m_disabledBackgroundMaterial = m_disabledBackgroundMaterial;
			target.m_disabledForegroundMaterial = m_disabledForegroundMaterial;
			target.m_disabledTextMaterial = m_disabledTextMaterial;
			target.m_fontName = m_fontName;
		}

		void doUpdateBackgroundMaterial()override
		{
			m_highlightedBackgroundMaterial = doCreateMaterial( getBackgroundMaterial(), 0.1f, cuT( "_BHG" ) );
		}

		void doUpdateForegroundMaterial()override
		{
			m_highlightedForegroundMaterial = doCreateMaterial( getForegroundMaterial(), -0.1f, cuT( "_FHG" ) );
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
		String m_fontName{};
	};
}

#endif
