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
		PanelStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ ControlType::ePanel
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
