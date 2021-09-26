#include "GuiCommon/System/MaterialsList.hpp"

#include "GuiCommon/System/ImagesLoader.hpp"
#include "GuiCommon/Properties/PropertiesContainer.hpp"
#include "GuiCommon/Properties/TreeItems/MaterialTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/PassTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/RenderTargetTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/TextureTreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Render/RenderTarget.hpp>

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

			imageList->Add( *image );
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
		auto lock( castor::makeUniqueLock( m_engine->getMaterialCache() ) );

		for ( auto & pair : m_engine->getMaterialCache() )
		{
			addMaterial( this
				, *m_scene
				, m_propertiesHolder->isEditable()
				, root
				, pair.second.get()
				, eBMP_MATERIAL );
		}
	}

	void MaterialsList::unloadMaterials()
	{
		DeleteAllItems();
		m_scene = nullptr;
	}

	void MaterialsList::addMaterial( wxTreeCtrl * treeCtrl
		, castor3d::Scene & scene
		, bool editable
		, wxTreeItemId id
		, castor3d::MaterialRPtr material
		, uint32_t iconOffset )
	{
		wxTreeItemId materialId = treeCtrl->AppendItem( id
			, material->getName()
			, int( eBMP_MATERIAL - iconOffset )
			, int( eBMP_MATERIAL_SEL - iconOffset )
			, new MaterialTreeItemProperty( editable, *material ) );
		uint32_t passIndex = 0;

		for ( auto pass : *material )
		{
			doAddPass( treeCtrl
				, scene
				, editable
				, materialId
				, ++passIndex
				, pass
				, iconOffset );
		}
	}

	void MaterialsList::doAddPass( wxTreeCtrl * treeCtrl
		, castor3d::Scene & scene
		, bool editable
		, wxTreeItemId id
		, uint32_t index
		, castor3d::PassSPtr pass
		, uint32_t iconOffset )
	{
		wxTreeItemId passId = treeCtrl->AppendItem( id
			, wxString( _( "Pass " ) ) << index
			, int( eBMP_PASS - iconOffset )
			, int( eBMP_PASS_SEL - iconOffset )
			, new PassTreeItemProperty( editable
				, pass
				, scene
				, treeCtrl ) );
		uint32_t unitIndex = 0;

		for ( auto unit : *pass )
		{
			doAddTexture( treeCtrl
				, editable
				, passId
				, ++unitIndex
				, *pass
				, unit
				, iconOffset );
		}
	}

	void MaterialsList::doAddTexture( wxTreeCtrl * treeCtrl
		, bool editable
		, wxTreeItemId id
		, uint32_t index
		, castor3d::Pass & pass
		, castor3d::TextureUnitSPtr texture
		, uint32_t iconOffset )
	{
		wxTreeItemId unitId = treeCtrl->AppendItem( id
			, wxString( _( "Texture Unit " ) ) << index
			, int( eBMP_TEXTURE - iconOffset )
			, int( eBMP_TEXTURE_SEL - iconOffset )
			, new TextureTreeItemProperty{ editable
				, pass
				, texture } );
		RenderTargetSPtr target = texture->getRenderTarget();

		if ( target )
		{
			AppendRenderTarget( treeCtrl
				, editable
				, unitId
				, *target );
		}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( MaterialsList, wxTreeCtrl )
		EVT_CLOSE( MaterialsList::onClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, MaterialsList::onSelectItem )
		EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, MaterialsList::onMouseRButtonUp )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void MaterialsList::onClose( wxCloseEvent & event )
	{
		DeleteAllItems();
		event.Skip();
	}

	void MaterialsList::onSelectItem( wxTreeEvent & event )
	{
		TreeItemProperty * data = static_cast< TreeItemProperty * >( event.GetClientObject() );
		m_propertiesHolder->setPropertyData( data );
		event.Skip();
	}

	void MaterialsList::onMouseRButtonUp( wxTreeEvent & event )
	{
		TreeItemProperty * data = static_cast< TreeItemProperty * >( event.GetClientObject() );
		wxPoint position = wxGetMousePosition();
		data->DisplayTreeItemMenu( this, position.x, position.y );
	}
}
