/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CI_CONTROLS_CACHE_H___
#define ___CI_CONTROLS_CACHE_H___

#include "CastorGuiPrerequisites.hpp"

#include <Graphics/Position.hpp>
#include <Event/UserInput/UserInputListener.hpp>

#include <Event/UserInput/EventHandler.hpp>

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
		, public Castor3D::UserInputListener
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
		*\param[in] p_engine
		*	The engine
		*/
		C3D_CGui_API ControlsManager( Castor3D::Engine & p_engine );
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
		Castor::FontSPtr GetDefaultFont()const
		{
			return m_defaultFont.lock();
		}
		/**
		*\brief
		*	Sets the default font used by controls
		*\param[in] p_font
		*	The font
		*/
		void SetDefaultFont( Castor::FontSPtr p_font )
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
		C3D_CGui_API void Create( ControlSPtr p_control );
		/**
		*\brief
		*	Destroys the given control's overlays and unbinds its callbacks from appropriate events.
		*\param[in] p_control
		*	The control.
		*/
		C3D_CGui_API void Destroy( ControlSPtr p_control );
		/**
		*\brief
		*	Adds a control that can an event target
		*\param[in] p_control
		*	The control
		*/
		C3D_CGui_API void AddControl( ControlSPtr p_control );
		/**
		*\brief
		*	Removes a control
		*\param[in] p_id
		*	The control ID
		*/
		C3D_CGui_API void RemoveControl( uint32_t p_id );
		/**
		*\brief
		*	Retrieves a control
		*\param[in] p_id
		*	The control ID
		*\return
		*	The contro
		*/
		C3D_CGui_API ControlSPtr GetControl( uint32_t p_id );

		//@}

	private:
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void ConnectEvents( ButtonCtrl & p_control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void ConnectEvents( ComboBoxCtrl & p_control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void ConnectEvents( EditCtrl & p_control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void ConnectEvents( ListBoxCtrl & p_control );
		/**
		*\brief
		*	Connects the manager to given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void ConnectEvents( SliderCtrl & p_control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void DisconnectEvents( ButtonCtrl & p_control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void DisconnectEvents( ComboBoxCtrl & p_control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void DisconnectEvents( EditCtrl & p_control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void DisconnectEvents( ListBoxCtrl & p_control );
		/**
		*\brief
		*	Disconnects the manager from given control's events.
		*\param[in] p_control
		*	The control.
		*/
		void DisconnectEvents( SliderCtrl & p_control );

	private:
		/**
		 *copydoc		Castor3D::UserInputListener::DoInitialise
		 */
		bool DoInitialise()override;
		/**
		 *copydoc		Castor3D::UserInputListener::DoCleanup
		 */
		void DoCleanup()override;
		/**
		 *copydoc		Castor3D::UserInputListener::FireMaterialEvent
		 */
		bool FireMaterialEvent( Castor::String const & p_overlay, Castor::String const & p_material )override;
		
		/**
		 *copydoc		Castor3D::UserInputListener::FireTextEvent
		 */
		bool FireTextEvent( Castor::String const & p_overlay, Castor::String const & p_caption )override;
		/**
		*\brief
		*	Retrieves the first handler which can be targeted by mouse, at its position
		*\param[in] p_position
		*	The mouse position
		*/
		Castor3D::EventHandlerSPtr DoGetMouseTargetableHandler( Castor::Position const & p_position )const;
		/**
		*\brief
		*	Updates the z-index ordered controls array
		*/
		void DoUpdate()const;
		/**
		*\brief
		*	Removes a control
		*\param[in] p_id
		*	The control ID
		*/
		void DoRemoveControl( uint32_t p_id );
		/**
		 *\copydoc		Castor3D::FrameListener::DoFlush
		 */
		virtual void DoFlush();

	private:
		//! The mutex used to protect the controls array.
		mutable std::mutex m_mutexControls;
		//! The controls array
		mutable std::vector< ControlSPtr > m_controlsByZIndex;
		//! The controls map, sorted by ID
		std::map< uint32_t, ControlWPtr > m_controlsById;
		//! Tells the controls array has changed
		bool m_changed;
		//! The default font used by controls
		Castor::FontWPtr m_defaultFont;
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
