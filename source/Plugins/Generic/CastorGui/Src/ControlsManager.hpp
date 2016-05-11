/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#include <UserInputListener.hpp>

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
		, public Castor3D::UserInputListener
	{
	public:
		/**@name General */
		//@{

		/** Constructor
		 *\param[in]	p_engine		The engine
		*/
		C3D_CGui_API ControlsManager( Castor3D::Engine & p_engine );

		/** Destructor
		*/
		C3D_CGui_API virtual ~ControlsManager();

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

		//@}

	private:
		/** Initialises the base materials.
		*\return		true, hopefully :P
		*/
		bool DoInitialise();

		/** Cleans up the manager.
		*/
		void DoCleanup();

		/** Retrieves the first handler which can be targeted by mouse, at its position
		 *\param[in]	p_position		The mouse position
		*/
		Castor3D::EventHandlerSPtr DoGetMouseTargetableHandler( Castor::Position const & p_position )const;

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

	private:
		//!\~english  The mutex used to protect the controls array.	\~french Le mutex de protection du tableau de contrôles.
		mutable std::mutex m_mutexControls;
		//! The controls array
		mutable std::vector< ControlSPtr > m_controlsByZIndex;
		//! The controls map, sorted by ID
		std::map< uint32_t, ControlWPtr > m_controlsById;
		//! Tells the controls array has changed
		bool m_changed;
		//! The default font used by controls
		Castor::FontWPtr m_defaultFont;
	};
}

#endif
