/*
See LICENSE file in root folder
*/
#ifndef ___CI_ButtonStyle_H___
#define ___CI_ButtonStyle_H___

#include "StyleControl.hpp"

namespace CastorGui
{
	/**
	\brief		Button style.
	*/
	class ButtonStyle
		: public ControlStyle
	{
	public:
		ButtonStyle( castor::String const & name
			, castor3d::Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ ControlType::eButton
				, name
				, engine }
			, m_textMaterial{ getForegroundMaterial() }
			, m_highlightedTextMaterial{ doCreateMaterial( getTextMaterial(), -0.1f, "_THG" ) }
			, m_highlightedBackgroundMaterial{ doCreateMaterial( getBackgroundMaterial(), 0.1f, "_BHG" ) }
			, m_highlightedForegroundMaterial{ doCreateMaterial( getForegroundMaterial(), -0.1f, "_FHG" ) }
			, m_pushedTextMaterial{ doCreateMaterial( getTextMaterial(), -0.1f, "_TPU" ) }
			, m_pushedBackgroundMaterial{ doCreateMaterial( getBackgroundMaterial(), 0.1f, "_BPU" ) }
			, m_pushedForegroundMaterial{ doCreateMaterial( getForegroundMaterial(), -0.1f, "_FPU" ) }
			, m_fontName{ fontName }
		{
		}

		void setTextMaterial( castor3d::MaterialRPtr material )
		{
			m_textMaterial = material;
		}

		void setHighlightedBackgroundMaterial( castor3d::MaterialRPtr material )
		{
			m_highlightedBackgroundMaterial = material;
		}

		void setHighlightedForegroundMaterial( castor3d::MaterialRPtr material )
		{
			m_highlightedForegroundMaterial = material;
		}

		void setHighlightedTextMaterial( castor3d::MaterialRPtr material )
		{
			m_highlightedTextMaterial = material;
		}

		void setPushedBackgroundMaterial( castor3d::MaterialRPtr material )
		{
			m_pushedBackgroundMaterial = material;
		}

		void setPushedForegroundMaterial( castor3d::MaterialRPtr material )
		{
			m_pushedForegroundMaterial = material;
		}

		void setPushedTextMaterial( castor3d::MaterialRPtr material )
		{
			m_pushedTextMaterial = material;
		}

		void setFont( castor::String const & value )
		{
			m_fontName = value;
		}

		castor3d::MaterialRPtr getHighlightedBackgroundMaterial()const
		{
			return m_highlightedBackgroundMaterial;
		}

		castor3d::MaterialRPtr getHighlightedForegroundMaterial()const
		{
			return m_highlightedForegroundMaterial;
		}

		castor3d::MaterialRPtr getHighlightedTextMaterial()const
		{
			return m_highlightedTextMaterial;
		}

		castor3d::MaterialRPtr getPushedBackgroundMaterial()const
		{
			return m_pushedBackgroundMaterial;
		}

		castor3d::MaterialRPtr getPushedForegroundMaterial()const
		{
			return m_pushedForegroundMaterial;
		}

		castor3d::MaterialRPtr getPushedTextMaterial()const
		{
			return m_pushedTextMaterial;
		}

		castor3d::MaterialRPtr getTextMaterial()const
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
		castor3d::MaterialRPtr m_textMaterial{};
		castor3d::MaterialRPtr m_highlightedTextMaterial{};
		castor3d::MaterialRPtr m_highlightedBackgroundMaterial{};
		castor3d::MaterialRPtr m_highlightedForegroundMaterial{};
		castor3d::MaterialRPtr m_pushedTextMaterial{};
		castor3d::MaterialRPtr m_pushedBackgroundMaterial{};
		castor3d::MaterialRPtr m_pushedForegroundMaterial{};
		castor::String m_fontName{};
	};
}

#endif
