#include "GuiCommon/MaterialsList.hpp"

#include "GuiCommon/ImagesLoader.hpp"
#include "GuiCommon/MaterialTreeItemProperty.hpp"
#include "GuiCommon/PassTreeItemProperty.hpp"
#include "GuiCommon/PropertiesContainer.hpp"
#include "GuiCommon/RenderTargetTreeItemProperty.hpp"
#include "GuiCommon/TextureTreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass.hpp>
#include <Castor3D/Texture/TextureLayout.hpp>
#include <Castor3D/Texture/TextureUnit.hpp>

#include <CastorUtils/Log/Logger.hpp>

#include <wx/imaglist.h>

#include "GuiCommon/xpms/material.xpm"
#include "GuiCommon/xpms/material_sel.xpm"
#include "GuiCommon/xpms/pass.xpm"
#include "GuiCommon/xpms/pass_sel.xpm"
#include "GuiCommon/xpms/texture.xpm"
#include "GuiCommon/xpms/texture_sel.xpm"

#ifdef LoadImage
#	undef LoadImage
#	define LoadImage wxBitmap::LoadImage
#endif

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	MaterialsList::MaterialsList( PropertiesContainer * propertiesHolder
		, wxWindow * parent
		, wxPoint const & pos
		, wxSize const & size )
		: wxTreeCtrl( parent, wxID_ANY, pos, size, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxNO_BORDER )
		, m_propertiesHolder( propertiesHolder )
	{
		wxBusyCursor wait;
		ImagesLoader::addBitmap( eBMP_MATERIAL, material_xpm );
		ImagesLoader::addBitmap( eBMP_MATERIAL_SEL, material_sel_xpm );
		ImagesLoader::addBitmap( eBMP_PASS, pass_xpm );
		ImagesLoader::addBitmap( eBMP_PASS_SEL, pass_sel_xpm );
		ImagesLoader::addBitmap( eBMP_TEXTURE, texture_xpm );
		ImagesLoader::addBitmap( eBMP_TEXTURE_SEL, texture_sel_xpm );
		ImagesLoader::waitAsyncLoads();

		wxImage * icons[] =
		{
			ImagesLoader::getBitmap( eBMP_MATERIAL ),
			ImagesLoader::getBitmap( eBMP_MATERIAL_SEL ),
			ImagesLoader::getBitmap( eBMP_PASS ),
			ImagesLoader::getBitmap( eBMP_PASS_SEL ),
			ImagesLoader::getBitmap( eBMP_TEXTURE ),
			ImagesLoader::getBitmap( eBMP_TEXTURE_SEL ),
		};

		wxImageList * imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto image : icons )
		{
			int sizeOrig = image->GetWidth();

			if ( sizeOrig != GC_IMG_SIZE )
			{
				image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			imageList->Add( wxImage( *image ) );
		}

		AssignImageList( imageList );
	}

	MaterialsList::~MaterialsList()
	{
		DeleteAllItems();
	}

	void MaterialsList::loadMaterials( Engine * engine
		, Scene & scene )
	{
		m_engine = engine;
		m_scene = &scene;
		wxTreeItemId root = AddRoot( _( "Root" ), eBMP_SCENE, eBMP_SCENE_SEL );
		auto lock = castor::makeUniqueLock( m_engine->getMaterialCache() );

		for ( auto pair : m_engine->getMaterialCache() )
		{
			doAddMaterial( root, pair.second );
		}
	}

	void MaterialsList::unloadMaterials()
	{
		DeleteAllItems();
		m_scene = nullptr;
	}

	void MaterialsList::doAddMaterial( wxTreeItemId id
		, castor3d::MaterialSPtr material )
	{
		wxTreeItemId materialId = AppendItem( id
			, material->getName()
			, eBMP_MATERIAL - eBMP_MATERIAL
			, eBMP_MATERIAL_SEL - eBMP_MATERIAL
			, new MaterialTreeItemProperty( m_propertiesHolder->IsEditable()
				, material ) );
		uint32_t passIndex = 0;

		for ( auto pass : *material )
		{
			doAddPass( materialId, ++passIndex, pass );
		}
	}

	void MaterialsList::doAddPass( wxTreeItemId id
		, uint32_t index
		, castor3d::PassSPtr pass )
	{
		wxTreeItemId passId = AppendItem( id
			, wxString( _( "Pass " ) ) << index
			, eBMP_PASS - eBMP_MATERIAL
			, eBMP_PASS_SEL - eBMP_MATERIAL
			, new PassTreeItemProperty( m_propertiesHolder->IsEditable()
				, pass
				, *m_scene
				, this ) );
		uint32_t unitIndex = 0;

		for ( auto unit : *pass )
		{
			if ( unit->getConfiguration().environment == 0u )
			{
				doAddTexture( passId, ++unitIndex, unit, pass->getType() );
			}
		}
	}

	void MaterialsList::doAddTexture( wxTreeItemId id
		, uint32_t index
		, castor3d::TextureUnitSPtr texture
		, castor3d::MaterialType type )
	{
		wxTreeItemId unitId = AppendItem( id
			, wxString( _( "Texture Unit " ) ) << index
			, eBMP_TEXTURE - eBMP_MATERIAL
			, eBMP_TEXTURE_SEL - eBMP_MATERIAL
			, new TextureTreeItemProperty( m_propertiesHolder->IsEditable()
				, texture
				, type ) );

		if ( texture->getRenderTarget() )
		{
			RenderTargetSPtr target = texture->getRenderTarget();

			if ( target )
			{
				wxString name = _( "Render Target" );
				AppendItem( unitId
					, name
					, eBMP_RENDER_TARGET
					, eBMP_RENDER_TARGET_SEL
					, new RenderTargetTreeItemProperty( m_propertiesHolder->IsEditable()
						, target ) );
			}
		}
	}

	BEGIN_EVENT_TABLE( MaterialsList, wxTreeCtrl )
		EVT_CLOSE( MaterialsList::onClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, MaterialsList::onSelectItem )
		EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, MaterialsList::onMouseRButtonUp )
	END_EVENT_TABLE()

	void MaterialsList::onClose( wxCloseEvent & event )
	{
		DeleteAllItems();
		event.Skip();
	}

	void MaterialsList::onSelectItem( wxTreeEvent & event )
	{
		TreeItemProperty * data = reinterpret_cast< TreeItemProperty * >( event.GetClientObject() );
		m_propertiesHolder->setPropertyData( data );
		event.Skip();
	}

	void MaterialsList::onMouseRButtonUp( wxTreeEvent & event )
	{
		TreeItemProperty * data = reinterpret_cast< TreeItemProperty * >( event.GetClientObject() );
		wxPoint position = wxGetMousePosition();
		data->DisplayTreeItemMenu( this, position.x, position.y );
	}
}
