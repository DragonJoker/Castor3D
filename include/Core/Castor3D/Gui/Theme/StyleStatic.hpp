/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StaticStyle_H___
#define ___C3D_StaticStyle_H___

#include "StyleControl.hpp"

namespace c3d
{
	class StaticStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eStatic;

		StaticStyle( String const & name
			, Engine & engine
			, String const & fontName )
			: StaticStyle{ name, nullptr, engine, fontName }
		{
		}

		StaticStyle( String const & name
			, Scene * scene
			, Engine & engine
			, String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine }
			, m_fontName{ fontName }
			, m_textMaterial{ getForegroundMaterial() }
		{
		}

		void setFont( StringView value )
		{
			m_fontName = value;
		}

		void setTextMaterial( MaterialObs value )
		{
			m_textMaterial = value;
		}

		String const & getFontName()const
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
		String m_fontName{};
		MaterialObs m_textMaterial{};
	};
}

#endif
