/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PanelStyle_H___
#define ___C3D_PanelStyle_H___

#include "StyleControl.hpp"
#include "StyleScrollable.hpp"
#include "StylesHolder.hpp"

namespace c3d
{
	class PanelStyle
		: public ControlStyle
		, public StylesHolder
		, public ScrollableStyle
	{
	public:
		static ControlType constexpr Type = ControlType::ePanel;

		PanelStyle( String const & name
			, Engine & engine )
			: PanelStyle{ name, nullptr, engine }
		{
		}

		PanelStyle( String const & name
			, Scene * scene
			, Engine & engine )
			: ControlStyle{ Type, name, scene, engine }
			, StylesHolder{ name, engine }
		{
		}

	private:
		ControlStyleRPtr doCreate( String const & cloneName )const override;

		void doCopyInto( ControlStyle & copy )const override
		{
			auto & target = static_cast< PanelStyle & >( copy );
			copyScrollableInto( target );
		}

		void doUpdateBackgroundMaterial()override
		{
		}

		void doUpdateForegroundMaterial()override
		{
		}
	};
}

#endif
