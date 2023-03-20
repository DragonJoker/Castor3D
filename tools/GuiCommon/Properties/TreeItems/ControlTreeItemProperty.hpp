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
		 *\param[in]	control		The target control.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	control		Le contrôle cible.
		 */
		ControlTreeItemProperty( bool editable
			, castor3d::Control & control
			, bool full
			, bool inLayout );

		inline castor3d::Control & getControl()const noexcept
		{
			return m_control;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )override;

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
		castor3d::Control & m_control;
		bool m_full;
		bool m_inLayout;
	};
}

#endif
