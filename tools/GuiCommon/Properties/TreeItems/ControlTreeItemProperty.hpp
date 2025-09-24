/*
See LICENSE file in root folder
*/
#ifndef ___GC_ControlTreeItemProperty_H___
#define ___GC_ControlTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Gui/GuiModule.hpp>

namespace GuiCommon
{
	class ControlTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	engine		The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	engine		Le moteur.
		 */
		ControlTreeItemProperty( ImagesLoader & imagesLoader
			, bool editable
			, c3d::Engine * engine );

		void setData( c3d::Control & control
			, bool full
			, bool inLayout )noexcept
		{
			clearProperties();
			m_control = &control;
			m_full = full;
			m_inLayout = inLayout;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		void doCreateControlProperties( wxPropertyGrid * grid, c3d::ButtonCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, c3d::ComboBoxCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, c3d::EditCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, c3d::ExpandablePanelCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, c3d::FrameCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, c3d::ListBoxCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, c3d::PanelCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, c3d::SliderCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, c3d::StaticCtrl & object );

	private:
		c3d::Control * m_control{};
		bool m_full{};
		bool m_inLayout{};
	};
}

#endif
