#include "TreeItemProperty.hpp"

#include <Engine.hpp>
#include <MaterialManager.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GuiCommon
{
	wxTreeItemProperty::wxTreeItemProperty( ePROPERTY_DATA_TYPE p_type )
		: wxTreeItemData()
		, m_type( p_type )
	{
	}

	wxTreeItemProperty::~wxTreeItemProperty()
	{
	}

	wxEnumProperty * wxTreeItemProperty::DoCreateMaterialProperty( wxString const & p_name, Engine * p_engine )
	{
		wxEnumProperty * l_material = new wxEnumProperty( p_name );
		MaterialManager & l_manager = p_engine->GetMaterialManager();
		wxPGChoices l_choices;
		l_manager.lock();

		for ( auto && l_pair : l_manager )
		{
			l_choices.Add( l_pair.first );
		}

		l_material->SetChoices( l_choices );
		l_manager.unlock();
		return l_material;
	}
}
