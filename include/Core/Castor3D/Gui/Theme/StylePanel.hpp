/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PanelStyle_H___
#define ___C3D_PanelStyle_H___

#include "StyleControl.hpp"
#include "StylesHolder.hpp"

namespace castor3d
{
	class PanelStyle
		: public ControlStyle
		, public StylesHolder
	{
	public:
		static ControlType constexpr Type = ControlType::ePanel;

		PanelStyle( castor::String const & name
			, Engine & engine )
			: PanelStyle{ name, nullptr, engine }
		{
		}

		PanelStyle( castor::String const & name
			, Scene * scene
			, Engine & engine )
			: ControlStyle{ Type, name, scene, engine }
			, StylesHolder{ name, engine }
		{
		}

	private:
		void doUpdateBackgroundMaterial()override
		{
		}

		void doUpdateForegroundMaterial()override
		{
		}
	};
}

#endif
