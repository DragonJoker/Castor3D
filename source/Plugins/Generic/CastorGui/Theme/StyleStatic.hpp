/*
See LICENSE file in root folder
*/
#ifndef ___CI_StaticStyle_H___
#define ___CI_StaticStyle_H___

#include "StyleControl.hpp"

namespace CastorGui
{
	/**
	 *\brief	Static style.
	*/
	class StaticStyle
		: public ControlStyle
	{
	public:
		StaticStyle( castor::String const & name
			, castor3d::Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ ControlType::eStatic
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

		void setTextMaterial( castor3d::MaterialRPtr value )
		{
			m_textMaterial = value;
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
