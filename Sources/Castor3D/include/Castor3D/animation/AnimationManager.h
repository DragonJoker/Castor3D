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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_AnimationManager___
#define ___C3D_AnimationManager___

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
	class CS3D_API AnimationManager : public General::Templates::Manager<AnimatedObjectGroup>, public General::Theory::AutoSingleton<AnimationManager>
	{
	private:
		//! A timer, usefull for animation handling
		PreciseTimer m_timer;
		//! The time since the last frame
		double m_timeSinceLastFrame;

	public:
		/**
		 * Constructor, initialises the timer
		 */
		AnimationManager();
		/**
		 * Destructor, cleans up the list
		 */
		~AnimationManager();
		/**
		 * Creates an AnimatedObjectGroup with the name given as a parameter, adds it to the list and returns it
		 *@param p_name : the name of the AnimatedObjectGroup to create
		 *@return the created AnimatedObjectGroup
		 */
		AnimatedObjectGroup * CreateAnimatedObjectGroup( const String & p_name);
		/**
		 * Refreshes all the AnimatedObjectGroup
		 */
		void Update();
		/**
		 * Writes the animation manager in a file
		 *@param p_file : the file to write in
		 */
		bool Write( General::Utils::FileIO & p_file)const;
		/**
		 * Reads the animation manager from a file
		 *@param p_file : the file to read from
		 */
		bool Read( General::Utils::FileIO & p_file);
	};
}

#endif
