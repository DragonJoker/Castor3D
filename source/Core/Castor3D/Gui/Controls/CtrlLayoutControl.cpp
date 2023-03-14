#include "Castor3D/Gui/Controls/CtrlLayoutControl.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Layout/Layout.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementCUSmartPtr( castor3d, LayoutControl )

namespace castor3d
{
	LayoutControl::LayoutControl( ControlType type
		, SceneRPtr scene
		, castor::String const & name
		, ControlStyleRPtr style
		, ControlRPtr parent
		, castor::Position const & position
		, castor::Size const & size
		, uint64_t flags
		, bool visible )
		: Control{ type
			, scene
			, name
			, style
			, parent
			, position
			, size
			, flags
			, visible }
	{
	}

	void LayoutControl::setLayout( LayoutUPtr layout )
	{
		m_layout = std::move( layout );
	}

	void LayoutControl::doSetBackgroundBorders( castor::Point4ui const & value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetBackgroundBorders( value );
	}

	void LayoutControl::doSetPosition( castor::Position const & value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetPosition( value );
	}

	void LayoutControl::doSetSize( castor::Size const & value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetSize( value );
	}

	void LayoutControl::doSetCaption( castor::String const & caption )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetCaption( caption );
	}

	void LayoutControl::doSetVisible( bool value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetVisible( value );
	}
}
