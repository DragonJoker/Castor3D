/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PanelStyle_H___
#define ___C3D_PanelStyle_H___

#include "StyleControl.hpp"

namespace castor3d
{
	class PanelStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::ePanel;

		PanelStyle( castor::String const & name
			, Engine & engine )
			: ControlStyle{ Type
				, name
				, engine }
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
