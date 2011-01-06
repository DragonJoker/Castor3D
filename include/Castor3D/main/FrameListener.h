/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___C3D_FrameListener___
#define ___C3D_FrameListener___

#include "../Prerequisites.h"

namespace Castor3D
{
	//! Frame event class
	/*!
	The interface which represents a frame event. Basically a frame event as a FrameEventType to know when it must be applied.
	It can be applied, so the function must be implemented by children classes
	\author Sylvain DOREMUS
	\version 0.1
	\date 03/03/2010
	*/
	class C3D_API FrameEvent : public MemoryTraced<FrameEvent>
	{
	public:
		//! Frame Event Type enumeration
		/*!
		Describes the different types of frame event : pre-render, queue-render, post-render
		*/
		typedef enum
		{
			ePreRender,		//!< This kind of event happens before any render
			eQueueRender,		//!< This kind of event happens after the render queue
			ePostRender,		//!< This kind of event happens after the buffer switch
		}
		eTYPE;

	protected:
		eTYPE m_type;	//!< The type of the event

	public:
		/**
		 * Constructor
		 */
		FrameEvent( eTYPE p_type)
			:	m_type( p_type)
		{}
		/**
		 * Destructor
		 */
		virtual ~FrameEvent(){}
		/**
		 * Applies the event, must be implemented by children classes
		 */
		virtual bool Apply()=0;
	public:
		/**@name Accessors */
		//@{
		/**
		 * returns the event's type
		 */
		inline eTYPE GetType() { return m_type; }
		//@}
	};

	//! User event synchronisation class
	/*!
	The manager of the frame events. It can add frame events and applies them at the wanted times
	\author Sylvain DOREMUS
	\version 0.1
	\date 03/03/2010
	*/
	class C3D_API FrameListener : public MemoryTraced<FrameListener>
	{
	protected:
		FrameEventArray m_preEvents;		//!< The events happening at pre render
		FrameEventArray m_queueEvents;		//!< The events happening at queue render
		FrameEventArray m_postEvents;		//!< The events happening at post render
		MultiThreading::RecursiveMutex m_mutex;

	public:
		/**
		 * Constructor
		 */
		FrameListener(){}
		/**
		 * Constructor
		 */
		~FrameListener()
		{
			m_postEvents.clear();
			m_preEvents.clear();
			m_queueEvents.clear();
		}
		/**
		 * Puts an event in the corresponding array
		 *@param p_event : [in] The event to put
		 */
		void PostEvent( FrameEventPtr p_event);
		/**
		 * Applies all events of a given type, then discards them
		 *@param p_type : [in] The type of event to fire
		 */
		bool FireEvents( FrameEvent::eTYPE p_type);
	};
}

#endif