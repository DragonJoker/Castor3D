/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StaticStyle_H___
#define ___C3D_StaticStyle_H___

#include "StyleControl.hpp"

namespace castor3d
{
	class StaticStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eStatic;

		StaticStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, engine }
			, m_fontName{ fontName }
			, m_textMaterial{ getForegroundMaterial() }
		{
		}

		void setFont( castor::String const & value )
		{
			m_fontName = value;
		}

		void setTextMaterial( MaterialRPtr value )
		{
			m_textMaterial = value;
		}

		castor::String const & getFontName()const
		{
			return m_fontName;
		}

		MaterialRPtr getTextMaterial()const
		{
			return m_textMaterial;
		}

	private:
		void doUpdateBackgroundMaterial()override
		{
		}

		void doUpdateForegroundMaterial()override
		{
		}

	private:
		castor::String m_fontName{};
		MaterialRPtr m_textMaterial{};
	};
}

#endif
