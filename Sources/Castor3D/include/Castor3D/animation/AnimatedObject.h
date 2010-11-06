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
#ifndef ___C3D_AnimatedObject___
#define ___C3D_AnimatedObject___

#include "Module_Animation.h"

namespace Castor3D
{
	//! Class which represents the animated objects
	/*!
	Class to represent animated objects, lists all animations of an object.
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\todo Review all the animation system because it's not clear, not optimised, and not good enough to be validated.
	\todo Write and Read functions.
	*/
	class CS3D_API AnimatedObject
	{
	protected:
		AnimationPtrStrMap & m_animations;	//!< The animations list
		MovableObjectPtr m_object;			//!< The object affected by the animations

	public:
		/**
		 * Specified constructor, initialises the MovableObject and the animation list
		 *@param p_object : the MovableObject to affect
		 *@param p_animations : the animations list
		 */
		AnimatedObject( MovableObjectPtr p_object, 
						AnimationPtrStrMap & p_animations);
		/**
		 * Destructor, dummy, destroy everything yourself (as you created all by yourself :P)
		 */
		~AnimatedObject();
		/**
		 * Writes the animation in a file
		 *@param p_file : the file to write in
		 */
		bool Write( Castor::Utils::File & p_file)const;
		/**
		 * Reads the animation from a file
		 @param p_file : the file to read from
		 */
		bool Read( Castor::Utils::File & p_file);
		/**
		 * Updates the animations of the object, given the time since the last frame, the animation weight and the animation scale
		 *@param p_tslf : time elapsed since the last frame
		 *@param p_weight : the animation weight, default value is 1.0 to tell full weight
		 *@param p_scale : the time scale of the animation, value is 1.0 to tell normal scale
		 */
		void Update( real p_tslf, real p_weight = 1.0f, real p_scale = 1.0f);
		/**
		 * Starts the animation identified by the name
		 *@param p_name : the name of the animation to start
		 */
		void StartAnimation( const String & p_name);
		/**
		 * Stops the animation identified by the name
		 *@param p_name : the name of the animation to stop
		 */
		void StopAnimation( const String & p_name);
		/**
		 * Pauses the animation identified by the name
		 *@param p_name : the name of the animation to pause
		 */
		void PauseAnimation(  const String & p_name);
		/**
		 * Stops all the animations
		 */
		void StopAllAnimations();
		/**
		 * Pauses all the animations
		 */
		void PauseAllAnimations();

	public:
		/**@name Accessors */
		//@{
		/**
		 * Returns the MovableObject
		 *@return the MovableObject affected
		 */
		inline MovableObjectPtr	GetObject		()const { return m_object; }
		//@}
	};
}

#endif

