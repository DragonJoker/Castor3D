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
			: StaticStyle{ name, nullptr, engine, fontName }
		{
		}

		StaticStyle( castor::String const & name
			, Scene * scene
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine }
			, m_fontName{ fontName }
			, m_textMaterial{ getForegroundMaterial() }
		{
		}

		void setFont( castor::StringView value )
		{
			m_fontName = value;
		}

		void setTextMaterial( MaterialObs value )
		{
			m_textMaterial = value;
		}

		castor::String const & getFontName()const
		{
			return m_fontName;
		}

		MaterialObs getTextMaterial()const
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
		MaterialObs m_textMaterial{};
	};
}

#endif
