/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ExpandablePanelCtrl_H___
#define ___C3D_ExpandablePanelCtrl_H___

#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlPanel.hpp"
#include "Castor3D/Gui/Theme/StyleExpandablePanel.hpp"

namespace castor3d
{
	class ExpandablePanelCtrl
		: public Control
	{
	public:
		/** Constructor
		*\param[in]	scene	The parent scene (nullptr for global).
		*\param[in]	name	The control name.
		*\param[in]	style	The control style.
		*\param[in]	parent	The parent control, if any.
		*/
		C3D_API ExpandablePanelCtrl( SceneRPtr scene
			, castor::String const & name
			, ExpandablePanelStyle * style
			, ControlRPtr parent );

		/** Constructor
		*\param[in]	scene			The parent scene (nullptr for global).
		*\param[in]	name			The control name.
		*\param[in]	style			The control style.
		*\param[in]	parent			The parent control, if any.
		*\param[in]	position		The position.
		*\param[in]	size			The size.
		*\param[in]	headerHeight	The header height.
		*\param[in]	expanded		\p true to create expanded, \p false to create collapsed.
		*\param[in]	flags			The configuration flags.
		*\param[in]	visible			Initial visibility status.
		*/
		C3D_API ExpandablePanelCtrl( SceneRPtr scene
			, castor::String const & name
			, ExpandablePanelStyle * style
			, ControlRPtr parent
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t headerHeight
			, bool expanded
			, ControlFlagType flags = 0
			, bool visible = true );

		C3D_API ~ExpandablePanelCtrl()noexcept override;

		C3D_API void setExpandCaption( castor::U32String v );
		C3D_API void setRetractCaption( castor::U32String v );

		/** Connects a function to an event
		*\param[in]	event		The event type
		*\param[in]	function	The function
		*\return	The internal function index, to be able to disconnect it
		*/
		OnExpandablePanelEventConnection connect( ExpandablePanelEvent event
			, OnExpandablePanelEventFunction function )
		{
			return m_signals[size_t( event )].connect( function );
		}

		/** \return	The expandable panel style.
		*/
		ExpandablePanelStyle const & getStyle()const noexcept
		{
			return static_cast< ExpandablePanelStyle const & >( getBaseStyle() );
		}

		auto getHeader()const noexcept
		{
			return m_header;
		}

		auto getContent()const noexcept
		{
			return m_content;
		}

		auto getExpand()const noexcept
		{
			return m_expand;
		}

		auto & getExpandCaption()const noexcept
		{
			return m_expandCaption;
		}

		auto & getRetractCaption()const noexcept
		{
			return m_retractCaption;
		}

		bool isExpanded()const
		{
			return m_expanded;
		}

		C3D_API static ControlType constexpr Type{ ControlType::eExpandablePanel };

	private:
		ExpandablePanelStyle & getStyle()
		{
			return static_cast< ExpandablePanelStyle & >( getBaseStyle() );
		}

		/** @copydoc Control::doCreate
		*/
		void doCreate()override;

		/** @copydoc Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc Control::doSetPosition
		*/
		void doSetPosition( castor::Position const & value )override;

		/** @copydoc Control::doSetSize
		*/
		void doSetSize( castor::Size const & value )override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doCatchesMouseEvents
		*/
		bool doCatchesMouseEvents()const override;

		/** @copydoc Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** Switch the panel visible status.
		*/
		void doSwitchExpand();

		/** Update position of children.
		*/
		void doUpdatePositions();

		/** Update size of children.
		*/
		void doUpdateSizes();

	private:
		uint32_t m_headerHeight;
		castor::U32String m_expandCaption{ U"+" };
		castor::U32String m_retractCaption{ U"-" };
		PanelCtrlRPtr m_header;
		ButtonCtrlRPtr m_expand;
		PanelCtrlRPtr m_content;
		OnExpandablePanelEvent m_signals[size_t( ExpandablePanelEvent::eCount )];
		OnButtonEventConnection m_expandClickedConnection;
		bool m_expanded{ true };
	};
}

#endif
