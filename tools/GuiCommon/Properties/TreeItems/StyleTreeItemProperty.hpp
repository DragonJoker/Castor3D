/*
See LICENSE file in root folder
*/
#ifndef ___GC_StyleTreeItemProperty_H___
#define ___GC_StyleTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Gui/GuiModule.hpp>

namespace GuiCommon
{
	class StyleTreeItemProperty
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
		StyleTreeItemProperty( bool editable
			, c3d::Engine * engine );

		void setData( c3d::ControlStyle & data )noexcept
		{
			clearProperties();
			m_style = &data;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		void doCreateStyleProperties( wxPropertyGrid * grid, c3d::ButtonStyle & object );
		void doCreateStyleProperties( wxPropertyGrid * grid, c3d::ComboBoxStyle & object );
		void doCreateStyleProperties( wxPropertyGrid * grid, c3d::EditStyle & object );
		void doCreateStyleProperties( wxPropertyGrid * grid, c3d::ExpandablePanelStyle & object );
		void doCreateStyleProperties( wxPropertyGrid * grid, c3d::FrameStyle & object );
		void doCreateStyleProperties( wxPropertyGrid * grid, c3d::ListBoxStyle & object );
		void doCreateStyleProperties( wxPropertyGrid * grid, c3d::PanelStyle & object );
		void doCreateStyleProperties( wxPropertyGrid * grid, c3d::SliderStyle & object );
		void doCreateStyleProperties( wxPropertyGrid * grid, c3d::StaticStyle & object );

	private:
		c3d::ControlStyle * m_style{};
		wxArrayString m_materials;
		wxArrayString m_fonts;
	};
}

#endif
