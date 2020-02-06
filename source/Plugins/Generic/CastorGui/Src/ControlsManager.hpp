﻿/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CI_CONTROLS_MANAGER_H___
#define ___CI_CONTROLS_MANAGER_H___

#include "CastorGuiPrerequisites.hpp"

#include <Position.hpp>
#include <FrameListener.hpp>

#include "EventHandler.hpp"

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
		, public Castor3D::FrameListener
	{
	public:
		/**@name General */
		//@{

		/** Constructor
		 *\param[in]	p_engine		The engine
		*/
		C3D_CGui_API ControlsManager( Castor3D::Engine * p_engine );

		/** Destructor
		*/
		C3D_CGui_API virtual ~ControlsManager();

		/** Initialises the base materials.
		 *\return		true, hopefully :P
		*/
		C3D_CGui_API bool Initialise();

		/** Cleans up the manager.
		*/
		C3D_CGui_API void Cleanup();

		/** Handles all queued events
		*/
		C3D_CGui_API void ProcessEvents();

		/** Retrieves the current mouse position
		 *\return		The value
		*/
		Castor::Position const & GetMousePosition()const
		{
			return m_mouse.m_position;
		}

		/** Retrieves the default font used by controls
		 *\return		The font
		*/
		Castor::FontSPtr GetDefaultFont()const
		{
			return m_defaultFont.lock();
		}

		/** Sets the default font used by controls
		 *\param[in]	p_font	The font
		*/
		void SetDefaultFont( Castor::FontSPtr p_font )
		{
			m_defaultFont = p_font;
		}

		//@}
		/**@name Mouse events */
		//@{

		/** Fires a mouse move event.
		 *\param[in]	p_position	The mouse position.
		 *\return		\p true if the event is processed by a control.
		 */
		C3D_CGui_API bool FireMouseMove( Castor::Position const & p_position );

		/** Fires a mouse pushed event.
		 *\param[in]	p_button	The mouse button.
		 *\return		\p true if the event is processed by a control.
		 */
		C3D_CGui_API bool FireMouseButtonPushed( eMOUSE_BUTTON p_button );

		/** Fires a mouse released event.
		 *\param[in]	p_button	The mouse button.
		 *\return		\p true if the event is processed by a control.
		 */
		C3D_CGui_API bool FireMouseButtonReleased( eMOUSE_BUTTON p_button );

		/** Fires a mouse wheel event.
		 *\param[in]	p_offsets	The mouse wheel offsets (x and y).
		 *\return		\p true if the event is processed by a control.
		 */
		C3D_CGui_API bool FireMouseWheel( Castor::Position const & p_offsets );

		//@}
		/**@name Keyboard events */
		//@{

		/** Fires a keyboard key down event.
		 *\param[in]	p_key		The key.
		 *\param[in]	p_ctrl		Tells if the Ctrl key is down.
		 *\param[in]	p_alt		Tells if the Alt key is down.
		 *\param[in]	p_shift		Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a control.
		 */
		C3D_CGui_API bool FireKeyDown( eKEYBOARD_KEY p_key, bool p_ctrl, bool p_alt, bool p_shift );

		/** Fires a keyboard key up event.
		 *\param[in]	p_key		The key.
		 *\param[in]	p_ctrl		Tells if the Ctrl key is down.
		 *\param[in]	p_alt		Tells if the Alt key is down.
		 *\param[in]	p_shift		Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a control.
		*/
		C3D_CGui_API bool FireKeyUp( eKEYBOARD_KEY p_key, bool p_ctrl, bool p_alt, bool p_shift );

		/** Fires a printable key event.
		 *\param[in]	p_key		The key.
		 *\param[in]	p_char		The character coorresponding to the key, displayable as is.
		 *\return		\p true if the event is processed by a control.
		*/
		C3D_CGui_API bool FireChar( eKEYBOARD_KEY p_key, Castor::String const & p_char );

		//@}
		/**@name Controls management */
		//@{

		/** Creates the given control's overlays and binds its callbacks to appropriate events
		 *\param[in]	p_control		The control
		*/
		C3D_CGui_API void Create( ControlSPtr p_control );

		/** Destroys the given control's overlays and unbinds its callbacks from appropriate events.
		 *\param[in]	p_control		The control.
		*/
		C3D_CGui_API void Destroy( ControlSPtr p_control );

		/** Adds a control that can an event target
		 *\param[in]	p_control		The control
		*/
		C3D_CGui_API void AddControl( ControlSPtr p_control );

		/** Removes a control
		 *\param[in]	p_id		The control ID
		*/
		C3D_CGui_API void RemoveControl( uint32_t p_id );

		/** Retrieves a control
		 *\param[in]	p_id		The control ID
		 *\return		The contro
		*/
		C3D_CGui_API ControlSPtr GetControl( uint32_t p_id );

		/** Retrieves the active control
		 *\return		The control
		*/
		inline ControlSPtr GetActiveControl()const
		{
			return m_activeControl.lock();
		}

		/** Retrieves the focused control
		 *\return		The control
		*/
		inline ControlSPtr GetFocusedControl()const
		{
			return m_lastMouseTarget.lock();
		}

		//@}

	private:
		/** Retrieves the first control which can be targeted by mouse, at its position
		 *\param[in]	p_position		The mouse position
		*/
		ControlSPtr DoGetMouseTargetableControl( Castor::Position const & p_position )const;

		/** Updates the z-index ordered controls array
		*/
		void DoUpdate()const;

		/** Removes a control
		 *\param[in]	p_id		The control ID
		*/
		void DoRemoveControl( uint32_t p_id );
		/**
		 *\copydoc		Castor3D::FrameListener::DoFlush
		 */
		virtual void DoFlush();

		/** Retrieve the controls in a thread-safe way
		 *\return		The controls array
		*/
		std::vector< ControlSPtr > DoGetControls()const
		{
			std::unique_lock< std::mutex > l_lock( m_mutexControls );
			return m_controls;
		}

	private:
		//! The engine
		Castor3D::Engine * m_engine;
		//! The current mouse state
		MouseState m_mouse;
		//! The current keyboard state
		KeyboardState m_keyboard;
		//! The currently active control
		ControlWPtr m_activeControl;
		//! The currently active control
		ControlWPtr m_lastMouseTarget;
		//! The mutex used to protect the controls array.
		mutable std::mutex m_mutexControls;
		//! The controls array
		std::vector< ControlSPtr > m_controls;
		//! The controls array
		mutable std::vector< ControlSPtr > m_controlsByZIndex;
		//! The controls map, sorted by ID
		std::map< uint32_t, ControlWPtr > m_controlsById;
		//! Tells the controls array has changed
		bool m_changed;
		//! Tells if the manager needs to be refreshed or not.
		bool m_enabled;
		//! The default font used by controls
		Castor::FontWPtr m_defaultFont;
	};
}

#endif
