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
#ifndef ___C3D_AnimationManager___
#define ___C3D_AnimationManager___

#include "../Prerequisites.h"
#include "AnimatedObjectGroup.h"

namespace Castor3D
{
	//! AnimatedObjectGroups handles
	/*!
	The class which handles all the AnimatedObjectGroups
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API AnimationManager : public Textable, public Serialisable, public Castor::Templates::Manager<AnimatedObjectGroup>
	{
	private:
		friend class Castor::Templates::Manager<AnimatedObjectGroup>;

	private:
		Utils::PreciseTimer m_timer;	//!< A timer, usefull for animation handling
		double m_dTimeSinceLastFrame;	//!< The time since the last frame
		Scene * m_pScene;

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor, initialises the timer
		 */
		AnimationManager( Scene * p_pScene);
		/**
		 * Destructor, cleans up the list
		 */
		~AnimationManager();
		//@}
		/**
		 * Creates an AnimatedObjectGroup with the name given as a parameter, adds it to the list and returns it
		 *@param p_strName : the name of the AnimatedObjectGroup to create
		 *@return the created AnimatedObjectGroup
		 */
		AnimatedObjectGroupPtr CreateAnimatedObjectGroup( const String & p_strName);
		/**
		 * Refreshes all the AnimatedObjectGroup
		 */
		void Update();

		/**@name Inherited methods from Textable */
		//@{
		virtual bool Write( File & p_file)const;
		virtual bool Read( File & p_file);
		//@}
		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const;
		virtual bool Load( File & p_file);
		//@}
	};
}

#endif
