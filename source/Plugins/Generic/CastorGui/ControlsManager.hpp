/*
See LICENSE file in root folder
*/
#ifndef ___CI_CONTROLS_CACHE_H___
#define ___CI_CONTROLS_CACHE_H___

#include "CastorGui/CastorGuiPrerequisites.hpp"
#include "CastorGui/Theme/Theme.hpp"

#include <Castor3D/Event/UserInput/UserInputListener.hpp>
#include <Castor3D/Event/UserInput/EventHandler.hpp>

#include <CastorUtils/Graphics/Position.hpp>

namespace CastorGui
{
	/**
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

		//@}
		/**@name Style management */
		//@{

		/**
		*\brief
		*	Creates a theme if it doesn't exist, returns the existing one if it exists
		*\param[in] name
		*	The theme name
		*\return
		*	The theme.
		*/
		C3D_CGui_API ThemeRPtr createTheme( castor::String const & name );
		/**
		*\return
		*	The theme for given name (\p nullptr if it doesn't exist).
		*/
		C3D_CGui_API ThemeRPtr getTheme( castor::String const & name );

		//@}
		/**@name Controls management */
		//@{

		/**
		*\brief
		*	Creates the given control's overlays and binds its callbacks to appropriate events
		*\param[in] control
		*	The control
		*/
		C3D_CGui_API void create( ControlSPtr control );
		/**
		*\brief
		*	Destroys the given control's overlays and unbinds its callbacks from appropriate events.
		*\param[in] control
		*	The control.
		*/
		C3D_CGui_API void destroy( ControlSPtr control );
		/**
		*\brief
		*	adds a control that can an event target
		*\param[in] control
		*	The control
		*/
		C3D_CGui_API void addControl( ControlSPtr control );
		/**
		*\brief
		*	Removes a control
		*\param[in] id
		*	The control ID
		*/
		C3D_CGui_API void removeControl( uint32_t id );
		/**
		*\brief
		*	Retrieves a control
		*\param[in] id
		*	The control ID
		*\return
		*	The contro
		*/
		C3D_CGui_API ControlSPtr getControl( uint32_t id );

		//@}

	private:
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( ButtonCtrl & control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( ComboBoxCtrl & control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( EditCtrl & control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( ListBoxCtrl & control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( SliderCtrl & control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ButtonCtrl & control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ComboBoxCtrl & control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( EditCtrl & control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ListBoxCtrl & control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( SliderCtrl & control );

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
		*\brief
		*	Retrieves the first handler which can be targeted by mouse, at its position
		*\param[in] position
		*	The mouse position
		*/
		castor3d::EventHandler * doGetMouseTargetableHandler( castor::Position const & position )const override;
		/**
		*\brief
		*	Updates the z-index ordered controls array
		*/
		void doUpdate()const;
		/**
		*\brief
		*	Removes a control
		*\param[in] id
		*	The control ID
		*/
		void doRemoveControl( uint32_t id );
		/**
		*\return
		*	The controls by z-index.
		*/
		std::vector< Control * > doGetControlsByZIndex()const;
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
		mutable std::mutex m_mutexControlsByZIndex;
		mutable std::vector< Control *  > m_controlsByZIndex;
		mutable std::mutex m_mutexControlsById;
		std::map< uint32_t, ControlWPtr > m_controlsById;
		bool m_changed;
		std::map< castor::String, ThemeUPtr > m_themes;
		std::map< Control const *, OnButtonEventConnection > m_onButtonClicks;
		std::map< Control const *, OnComboEventConnection > m_onComboSelects;
		std::map< Control const *, OnEditEventConnection > m_onEditUpdates;
		std::map< Control const *, OnListEventConnection > m_onListSelects;
		std::map< Control const *, OnSliderEventConnection > m_onSliderTracks;
		std::map< Control const *, OnSliderEventConnection > m_onSliderReleases;
	};
}

#endif
