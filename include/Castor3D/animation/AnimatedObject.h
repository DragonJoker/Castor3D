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

#include "../Prerequisites.h"

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
	class C3D_API AnimatedObject : public Serialisable, public MemoryTraced<AnimatedObject>
	{
	protected:
		AnimationPtrStrMap & m_mapAnimations;	//!< The animations list
		MovableObjectPtr m_pObject;				//!< The object affected by the animations
		Scene * m_pScene;

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		AnimatedObject( Scene * p_pScene, AnimationPtrStrMap & p_mapAnimations);
		/**
		 * Specified constructor, initialises the MovableObject and the animation list
		 *@param p_pObject : the MovableObject to affect
		 *@param p_mapAnimations : the animations list
		 */
		AnimatedObject( Scene * p_pScene, MovableObjectPtr p_pObject, 
						AnimationPtrStrMap & p_mapAnimations);
		/**
		 * Destructor, dummy, destroy everything yourself (as you created all by yourself :P)
		 */
		~AnimatedObject();
		//@}
		/**
		 * Updates the animations of the object, given the time since the last frame, the animation weight and the animation scale
		 *@param p_rTslf : time elapsed since the last frame
		 *@param p_rWeight : the animation weight, default value is 1.0 to tell full weight
		 *@param p_rScale : the time scale of the animation, value is 1.0 to tell normal scale
		 */
		void Update( real p_rTslf, real p_rWeight = 1.0f, real p_rScale = 1.0f);
		/**
		 * Starts the animation identified by the name
		 *@param p_strName : the name of the animation to start
		 */
		void StartAnimation( const String & p_strName);
		/**
		 * Stops the animation identified by the name
		 *@param p_strName : the name of the animation to stop
		 */
		void StopAnimation( const String & p_strName);
		/**
		 * Pauses the animation identified by the name
		 *@param p_strName : the name of the animation to pause
		 */
		void PauseAnimation(  const String & p_strName);
		/**
		 * Stops all the animations
		 */
		void StopAllAnimations();
		/**
		 * Pauses all the animations
		 */
		void PauseAllAnimations();

		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const;
		virtual bool Load( File & p_file);
		//@}

		/**@name Accessors */
		//@{
		inline MovableObjectPtr	GetObject		()const { return m_pObject; }
		//@}
	};
}

#endif

