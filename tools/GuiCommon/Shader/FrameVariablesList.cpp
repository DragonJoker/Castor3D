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
		enum class eID
		{
			FrameVariable,
			FrameVariableSelected,
			FrameVariableBuffer,
			FrameVariableBufferSelected,
		};
	}

	FrameVariablesList::FrameVariablesList( c3d::Engine * engine
		, ImagesLoader & imagesLoader
		, PropertiesContainer * propertiesHolder
		, wxWindow * parent
		, wxPoint const & ptPos
		, wxSize const & size )
		: wxTreeCtrl( parent, wxID_ANY, ptPos, size, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxNO_BORDER )
		, m_engine{ engine }
		, m_imagesLoader{ imagesLoader }
		, m_propertiesHolder( propertiesHolder )
	{
		wxBusyCursor wait;
		imagesLoader.addBitmapT( eBMP::eFrameVariable, fbvx::frame_variable_xpm );
		imagesLoader.addBitmapT( eBMP::eFrameVariableSelected, fbvx::frame_variable_sel_xpm );
		imagesLoader.addBitmapT( eBMP::eFrameVariableBuffer, fbvx::frame_variable_buffer_xpm );
		imagesLoader.addBitmapT( eBMP::eFrameVariableBufferSelected, fbvx::frame_variable_buffer_sel_xpm );
		imagesLoader.waitAsyncLoads();

		c3d::Array< wxImage *, 4u > icons{ imagesLoader.getBitmapT( eBMP::eFrameVariable )
			, imagesLoader.getBitmapT( eBMP::eFrameVariableSelected )
			, imagesLoader.getBitmapT( eBMP::eFrameVariableBuffer )
			, imagesLoader.getBitmapT( eBMP::eFrameVariableBufferSelected ) };
		auto imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto image : icons )
		{
			int sizeOrig = image->GetWidth();

			if ( sizeOrig != GC_IMG_SIZE )
			{
				image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			imageList->Add( *image );
		}

		wxTreeCtrl::AssignImageList( imageList );
	}

	FrameVariablesList::~FrameVariablesList()
	{
		unloadVariables();
	}

	void FrameVariablesList::loadVariables( VkShaderStageFlagBits stage
		, c3d::Vector< UniformBufferValues > & ubos )
	{
		wxTreeItemId root = AddRoot( _( "Root" ) );

		for ( auto & ubo : ubos )
		{
			if ( c3d::checkFlag( ubo.stages, stage ) )
			{
				doAddBuffer( root, ubo );
			}
		}
	}

	void FrameVariablesList::unloadVariables()
	{
		wxTreeCtrl::DeleteAllItems();
	}

	void FrameVariablesList::doAddBuffer( wxTreeItemId id
		, UniformBufferValues & buffer )
	{
		wxTreeItemId bufferId = AppendItem( id, buffer.name
			, int( fbv::eID::FrameVariableBuffer )
			, int( fbv::eID::FrameVariableBufferSelected )
			, new FrameVariableBufferTreeItemProperty( m_engine
				, m_imagesLoader
				, m_propertiesHolder->isEditable()
				, buffer ) );

		for ( auto const & uniform : buffer.uniforms )
		{
			doAddVariable( bufferId, *uniform );
		}
	}

	void FrameVariablesList::doAddVariable( wxTreeItemId id
		, UniformValueBase & uniform )
	{
		AppendItem( id
			, uniform.getName()
			, int( fbv::eID::FrameVariable )
			, int( fbv::eID::FrameVariableSelected )
			, new FrameVariableTreeItemProperty( m_engine
				, m_imagesLoader
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
		auto data = static_cast< TreeItemProperty * >( event.GetClientObject() );
		m_propertiesHolder->setPropertyData( data );
		event.Skip();
	}

	void FrameVariablesList::onMouseRButtonUp( wxTreeEvent & event )
	{
		event.Skip( false );
	}
}
