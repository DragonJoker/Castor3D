/*
See LICENSE file in root folder
*/
#ifndef ___CI_EditStyle_H___
#define ___CI_EditStyle_H___

#include "StyleControl.hpp"

namespace CastorGui
{
	/**
	\brief		Edit style
	*/
	class EditStyle
		: public ControlStyle
	{
	public:
		EditStyle( castor::String const & name
			, castor3d::Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ ControlType::eEdit
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

		void setTextMaterial( castor3d::MaterialRPtr material )
		{
			m_textMaterial = material;
		}

		castor::String const & getFontName()const
		{
			return m_fontName;
		}

		castor3d::MaterialRPtr getTextMaterial()const
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
		castor3d::MaterialRPtr m_textMaterial{};
	};
}

#endif
