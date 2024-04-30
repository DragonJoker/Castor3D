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
		ControlTreeItemProperty( bool editable
			, castor3d::Engine * engine );

		void setData( castor3d::Control & control
			, bool full
			, bool inLayout )noexcept
		{
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
		void doCreateControlProperties( wxPropertyGrid * grid, castor3d::ButtonCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, castor3d::ComboBoxCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, castor3d::EditCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, castor3d::ExpandablePanelCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, castor3d::FrameCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, castor3d::ListBoxCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, castor3d::PanelCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, castor3d::SliderCtrl & object );
		void doCreateControlProperties( wxPropertyGrid * grid, castor3d::StaticCtrl & object );

	private:
		castor3d::Control * m_control{};
		bool m_full{};
		bool m_inLayout{};
	};
}

#endif
