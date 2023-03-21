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

		void setTextMaterial( MaterialRPtr material )
		{
			m_textMaterial = material;
		}

		void setHighlightedBackgroundMaterial( MaterialRPtr material )
		{
			m_highlightedBackgroundMaterial = material;
		}

		void setHighlightedForegroundMaterial( MaterialRPtr material )
		{
			m_highlightedForegroundMaterial = material;
		}

		void setHighlightedTextMaterial( MaterialRPtr material )
		{
			m_highlightedTextMaterial = material;
		}

		void setPushedBackgroundMaterial( MaterialRPtr material )
		{
			m_pushedBackgroundMaterial = material;
		}

		void setPushedForegroundMaterial( MaterialRPtr material )
		{
			m_pushedForegroundMaterial = material;
		}

		void setPushedTextMaterial( MaterialRPtr material )
		{
			m_pushedTextMaterial = material;
		}

		void setDisabledBackgroundMaterial( MaterialRPtr material )
		{
			m_disabledBackgroundMaterial = material;
		}

		void setDisabledForegroundMaterial( MaterialRPtr material )
		{
			m_disabledForegroundMaterial = material;
		}

		void setDisabledTextMaterial( MaterialRPtr material )
		{
			m_disabledTextMaterial = material;
		}

		void setFont( castor::String const & value )
		{
			m_fontName = value;
		}

		MaterialRPtr getHighlightedBackgroundMaterial()const
		{
			return m_highlightedBackgroundMaterial;
		}

		MaterialRPtr getHighlightedForegroundMaterial()const
		{
			return m_highlightedForegroundMaterial;
		}

		MaterialRPtr getHighlightedTextMaterial()const
		{
			return m_highlightedTextMaterial;
		}

		MaterialRPtr getPushedBackgroundMaterial()const
		{
			return m_pushedBackgroundMaterial;
		}

		MaterialRPtr getPushedForegroundMaterial()const
		{
			return m_pushedForegroundMaterial;
		}

		MaterialRPtr getPushedTextMaterial()const
		{
			return m_pushedTextMaterial;
		}

		MaterialRPtr getDisabledBackgroundMaterial()const
		{
			return m_disabledBackgroundMaterial;
		}

		MaterialRPtr getDisabledForegroundMaterial()const
		{
			return m_disabledForegroundMaterial;
		}

		MaterialRPtr getDisabledTextMaterial()const
		{
			return m_disabledTextMaterial;
		}

		MaterialRPtr getTextMaterial()const
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
		MaterialRPtr m_textMaterial{};
		MaterialRPtr m_highlightedTextMaterial{};
		MaterialRPtr m_highlightedBackgroundMaterial{};
		MaterialRPtr m_highlightedForegroundMaterial{};
		MaterialRPtr m_pushedTextMaterial{};
		MaterialRPtr m_pushedBackgroundMaterial{};
		MaterialRPtr m_pushedForegroundMaterial{};
		MaterialRPtr m_disabledTextMaterial{};
		MaterialRPtr m_disabledBackgroundMaterial{};
		MaterialRPtr m_disabledForegroundMaterial{};
		castor::String m_fontName{};
	};
}

#endif
