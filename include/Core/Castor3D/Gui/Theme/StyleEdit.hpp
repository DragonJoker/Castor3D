/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EditStyle_H___
#define ___C3D_EditStyle_H___

#include "StyleControl.hpp"
#include "StyleScrollable.hpp"

namespace castor3d
{
	class EditStyle
		: public ControlStyle
		, public ScrollableStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eEdit;

		EditStyle( castor::String const & name
			, Scene * scene
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine
				, MouseCursor::eText }
			, m_fontName{ fontName }
			, m_textMaterial{ getForegroundMaterial() }
			, m_selectionMaterial{ getEngine().findMaterial( cuT( "AlphaLightBlue" ) ) }
		{
		}

		EditStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: EditStyle{ name, nullptr, engine, fontName }
		{
		}

		void setFont( castor::StringView value )
		{
			m_fontName = value;
		}

		void setTextMaterial( MaterialObs material )
		{
			m_textMaterial = material;
		}

		void setSelectionMaterial( MaterialObs material )
		{
			m_selectionMaterial = material;
		}

		castor::String const & getFontName()const
		{
			return m_fontName;
		}

		MaterialObs getTextMaterial()const
		{
			return m_textMaterial;
		}

		MaterialObs getSelectionMaterial()const
		{
			return m_selectionMaterial;
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
		MaterialObs m_selectionMaterial{};
	};
}

#endif
