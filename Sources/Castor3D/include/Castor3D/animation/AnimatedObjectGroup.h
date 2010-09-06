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
#ifndef ___C3D_AnimatedObjectGroup___
#define ___C3D_AnimatedObjectGroup___

namespace Castor3D
{
	//! Class which represents the animated object groups
	/*!
	Class to represent animated object groups, id est the list of animated objects with the same animations
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\todo Review all the animation system because it's not clear, not optimised, and not good enough to be validated.
	\todo Write and Read functions.
	*/
	class CS3D_API AnimatedObjectGroup
	{
	protected:
		//! The name of the group
		String m_name;
		//! The list of animations
		AnimationStrMap m_animations;
		//! The list of AnimatedObjects
		AnimatedObjectStrMap m_objects;
		//! The current time index
		float m_currentTime;
		//! The name of currently playing animations
		AnimationStrMap * m_playingAnimations;

	public:
		/**
		 * Constructor of the group, intialises it with the name given as a parameter
		 *@param p_name : the group name
		 */
		AnimatedObjectGroup( const String & p_name);
		/**
		 * Destructor, deletes the objects and the animations
		 */
		~AnimatedObjectGroup();
		/**
		 * Creates an AnimatedObject from the MovableObject given as a parameter, adds it to the list
		 *@param p_object : the MovableObject from which to create an AnimatedObject
		 *@return the created and added AnimatedObject
		 */
		AnimatedObject * AddObject( MovableObject * p_object);
		/**
		 * Adds the animation to the list
		 *@param p_animation : the animation to add
		 */
		void AddAnimation( Animation * p_animation);
		/**
		 * Writes the group in a file
		 *@param p_file : the file to write in
		 */
		bool Write( General::Utils::FileIO & p_file)const;
		/**
		 * Reads the group from a file
		 @param p_file : the file to read from
		 */
		bool Read( General::Utils::FileIO & p_file);
		/**
		 * Update all the animations
		 *@param p_tslf : time since the last frame
		 */
		void Update( float p_tslf);
		/**
		 * Starts an animation, given its name
		 *@param p_name : the animation name
		 */
		void StartAnimation( const String & p_name);
		/**
		 * Stops an animation, given its name
		 *@param p_name : the animation name
		 */
		void StopAnimation( const String & p_name);
		/**
		 * Pause an animation, given its name
		 *@param p_name : the animation name
		 */
		void PauseAnimation(  const String & p_name);
		/**
		 * Starts all the animations
		 */
		void StartAllAnimations();
		/**
		 * Stops all the animations
		 */
		void StopAllAnimations();
		/**
		 * Pauses all the animations
		 */
		void PauseAllAnimations();
		/**
		 * Returns the name of this group
		 *@return the name of this group
		 */
		inline const String & GetName() { return m_name; }
	};
}

#endif

