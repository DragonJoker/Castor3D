/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EditStyle_H___
#define ___C3D_EditStyle_H___

#include "StyleControl.hpp"

namespace castor3d
{
	class EditStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eEdit;

		EditStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, engine
				, MouseCursor::eText }
			, m_fontName{ fontName }
			, m_textMaterial{ getForegroundMaterial() }
		{
		}

		void setFont( castor::String const & value )
		{
			m_fontName = value;
		}

		void setTextMaterial( MaterialRPtr material )
		{
			m_textMaterial = material;
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
