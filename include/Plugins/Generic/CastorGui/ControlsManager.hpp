/*
See LICENSE file in root folder
*/
#ifndef ___CI_CONTROLS_CACHE_H___
#define ___CI_CONTROLS_CACHE_H___

#include "CastorGui/CastorGuiPrerequisites.hpp"

#include <Castor3D/Event/UserInput/UserInputListener.hpp>
#include <Castor3D/Event/UserInput/EventHandler.hpp>

#include <CastorUtils/Graphics/Position.hpp>

namespace CastorGui
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		02/03/201
	*\version	0.1.
	*\brief		Class used to to manage the controls: events and all GUI related stuff
	*/
	class ControlsManager
		: public std::enable_shared_from_this< ControlsManager >
		, public castor3d::UserInputListener
	{
		friend class ButtonCtrl;
		friend class ComboBoxCtrl;
		friend class EditCtrl;
		friend class ListBoxCtrl;
		friend class SliderCtrl;

	public:
		/**@name General */
		//@{

		/**
		*\brief
		*	Constructor
		*\param[in] engine
		*	The engine
		*/
		C3D_CGui_API explicit ControlsManager( castor3d::Engine & engine );
		/**
		*\brief
		*	Destructor
		*/
		C3D_CGui_API virtual ~ControlsManager();
		/**
		*\brief
		*	Retrieves the default font used by controls
		*\return
		*	The font
		*/
		castor::FontSPtr getDefaultFont()const
		{
			return m_defaultFont.lock();
		}
		/**
		*\brief
		*	sets the default font used by controls
		*\param[in] p_font
		*	The font
		*/
		void setDefaultFont( castor::FontSPtr p_font )
		{
			m_defaultFont = p_font;
		}

		//@}
		/**@name Controls management */
		//@{

		/**
		*\brief
		*	Creates the given control's overlays and binds its callbacks to appropriate events
		*\param[in] p_control
		*	The control
		*/
		C3D_CGui_API void create( ControlSPtr p_control );
		/**
		*\brief
		*	Destroys the given control's overlays and unbinds its callbacks from appropriate events.
		*\param[in] p_control
		*	The control.
		*/
		C3D_CGui_API void destroy( ControlSPtr p_control );
		/**
		*\brief
		*	adds a control that can an event target
		*\param[in] p_control
		*	The control
		*/
		C3D_CGui_API void addControl( ControlSPtr p_control );
		/**
		*\brief
		*	Removes a control
		*\param[in] p_id
		*	The control ID
		*/
		C3D_CGui_API void removeControl( uint32_t p_id );
		/**
		*\brief
		*	Retrieves a control
		*\param[in] p_id
		*	The control ID
		*\return
		*	The contro
		*/
		C3D_CGui_API ControlSPtr getControl( uint32_t p_id );

		//@}

	private:
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void connectEvents( ButtonCtrl & p_control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void connectEvents( ComboBoxCtrl & p_control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void connectEvents( EditCtrl & p_control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void connectEvents( ListBoxCtrl & p_control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void connectEvents( SliderCtrl & p_control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void disconnectEvents( ButtonCtrl & p_control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void disconnectEvents( ComboBoxCtrl & p_control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void disconnectEvents( EditCtrl & p_control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void disconnectEvents( ListBoxCtrl & p_control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void disconnectEvents( SliderCtrl & p_control );

	private:
		/**
		 *copydoc		castor3d::UserInputListener::doInitialise
		 */
		bool doInitialise()override;
		/**
		 *copydoc		castor3d::UserInputListener::doCleanup
		 */
		void doCleanup()override;
		/**
		 *copydoc		castor3d::UserInputListener::fireMaterialEvent
		 */
		bool fireMaterialEvent( castor::String const & p_overlay, castor::String const & p_material )override;
		
		/**
		 *copydoc		castor3d::UserInputListener::fireTextEvent
		 */
		bool fireTextEvent( castor::String const & p_overlay, castor::String const & p_caption )override;
		/**
		*\brief
		*	Retrieves the first handler which can be targeted by mouse, at its position
		*\param[in] p_position
		*	The mouse position
		*/
		castor3d::EventHandlerSPtr doGetMouseTargetableHandler( castor::Position const & p_position )const;
		/**
		*\brief
		*	Updates the z-index ordered controls array
		*/
		void doUpdate()const;
		/**
		*\brief
		*	Removes a control
		*\param[in] p_id
		*	The control ID
		*/
		void doRemoveControl( uint32_t p_id );
		/**
		*\return
		*	The controls by z-index.
		*/
		std::vector< ControlSPtr > doGetControlsByZIndex()const;
		/**
		*\return
		*	The controls by ID.
		*/
		std::map< uint32_t, ControlWPtr > doGetControlsById()const;
		/**
		 *\copydoc		castor3d::FrameListener::doFlush
		 */
		virtual void doFlush();

	private:
		//! The mutex used to protect the controls by z-index.
		mutable std::mutex m_mutexControlsByZIndex;
		//! The controls array
		mutable std::vector< ControlSPtr > m_controlsByZIndex;
		//! The mutex used to protect the controls by ID.
		mutable std::mutex m_mutexControlsById;
		//! The controls map, sorted by ID
		std::map< uint32_t, ControlWPtr > m_controlsById;
		//! Tells the controls array has changed
		bool m_changed;
		//! The default font used by controls
		castor::FontWPtr m_defaultFont;
		//! The button click event connections.
		std::map< Control const *, OnButtonEventConnection > m_onButtonClicks;
		//! The combo-box item selected event connections.
		std::map< Control const *, OnComboEventConnection > m_onComboSelects;
		//! The text updated event connections.
		std::map< Control const *, OnEditEventConnection > m_onEditUpdates;
		//! The list-box item selected event connections.
		std::map< Control const *, OnListEventConnection > m_onListSelects;
		//! The slider thumb track event connections.
		std::map< Control const *, OnSliderEventConnection > m_onSliderTracks;
		//! The slider thumb release event connections.
		std::map< Control const *, OnSliderEventConnection > m_onSliderReleases;
	};
}

#endif
