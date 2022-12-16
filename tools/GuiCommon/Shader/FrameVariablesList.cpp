#include "GuiCommon/Shader/FrameVariablesList.hpp"

#include "GuiCommon/System/ImagesLoader.hpp"
#include "GuiCommon/Properties/TreeItems/FrameVariableBufferTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/FrameVariableTreeItemProperty.hpp"
#include "GuiCommon/Properties/PropertiesContainer.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <wx/imaglist.h>

namespace fbvx
{
#include "GuiCommon/xpms/frame_variable.xpm"
#include "GuiCommon/xpms/frame_variable_sel.xpm"
#include "GuiCommon/xpms/frame_variable_buffer.xpm"
#include "GuiCommon/xpms/frame_variable_buffer_sel.xpm"
}

#ifdef LoadImage
#	undef LoadImage
#	define LoadImage wxBitmap::LoadImage
#endif

namespace GuiCommon
{
	namespace fbv
	{
		typedef enum eID
		{
			eID_FRAME_VARIABLE,
			eID_FRAME_VARIABLE_SEL,
			eID_FRAME_VARIABLE_BUFFER,
			eID_FRAME_VARIABLE_BUFFER_SEL,
		}	eID;
	}

	FrameVariablesList::FrameVariablesList( castor3d::Engine * engine
		, PropertiesContainer * propertiesHolder
		, wxWindow * parent
		, wxPoint const & ptPos
		, wxSize const & size )
		: wxTreeCtrl( parent, wxID_ANY, ptPos, size, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxNO_BORDER )
		, m_engine{ engine }
		, m_propertiesHolder( propertiesHolder )
	{
		wxBusyCursor wait;
		ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE, fbvx::frame_variable_xpm );
		ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE_SEL, fbvx::frame_variable_sel_xpm );
		ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE_BUFFER, fbvx::frame_variable_buffer_xpm );
		ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL, fbvx::frame_variable_buffer_sel_xpm );
		ImagesLoader::waitAsyncLoads();

		wxImage * icons[] =
		{
			ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE ),
			ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE_SEL ),
			ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE_BUFFER ),
			ImagesLoader::getBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL ),
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

	FrameVariablesList::~FrameVariablesList()
	{
		unloadVariables();
	}

	void FrameVariablesList::loadVariables( VkShaderStageFlagBits stage
		, std::vector< UniformBufferValues > & ubos )
	{
		wxTreeItemId root = AddRoot( _( "Root" ) );

		for ( auto & ubo : ubos )
		{
			if ( castor::checkFlag( ubo.stages, stage ) )
			{
				doAddBuffer( root, ubo );
			}
		}
	}

	void FrameVariablesList::unloadVariables()
	{
		DeleteAllItems();
	}

	void FrameVariablesList::doAddBuffer( wxTreeItemId id
		, UniformBufferValues & buffer )
	{
		wxTreeItemId bufferId = AppendItem( id, buffer.name
			, fbv::eID_FRAME_VARIABLE_BUFFER
			, fbv::eID_FRAME_VARIABLE_BUFFER_SEL
			, new FrameVariableBufferTreeItemProperty( m_engine
				, m_propertiesHolder->isEditable()
				, buffer ) );

		for ( auto & uniform : buffer.uniforms )
		{
			doAddVariable( bufferId, *uniform );
		}
	}

	void FrameVariablesList::doAddVariable( wxTreeItemId id
		, UniformValueBase & uniform )
	{
		AppendItem( id
			, uniform.getName()
			, fbv::eID_FRAME_VARIABLE
			, fbv::eID_FRAME_VARIABLE_SEL
			, new FrameVariableTreeItemProperty( m_engine
				, m_propertiesHolder->isEditable()
				, uniform ) );
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( FrameVariablesList, wxTreeCtrl )
		EVT_CLOSE( FrameVariablesList::onClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, FrameVariablesList::onSelectItem )
		EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, FrameVariablesList::onMouseRButtonUp )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void FrameVariablesList::onClose( wxCloseEvent & event )
	{
		unloadVariables();
		event.Skip();
	}

	void FrameVariablesList::onSelectItem( wxTreeEvent & event )
	{
		TreeItemProperty * data = static_cast< TreeItemProperty * >( event.GetClientObject() );
		m_propertiesHolder->setPropertyData( data );
		event.Skip();
	}

	void FrameVariablesList::onMouseRButtonUp( wxTreeEvent & event )
	{
	}
}
