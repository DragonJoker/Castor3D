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
#ifndef ___C3D_AnimatedObjectGroup___
#define ___C3D_AnimatedObjectGroup___

#include "../Prerequisites.h"

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
	class C3D_API AnimatedObjectGroup : public Textable, public Serialisable, public MemoryTraced<AnimatedObjectGroup>
	{
	protected:
		real m_rCurrentTime;						//!< The current time index
		String m_strName;							//!< The animated object group name
		AnimationPtrStrMap m_mapAnimations;			//!< The list of animations
		AnimatedObjectPtrStrMap m_mapObjects;		//!< The list of AnimatedObjects
		AnimationPtrStrMap m_mapPlayingAnimations;	//!< The map of currently playing animations
		Scene * m_pScene;

	public:
		/**
		 * Constructor of the group, intialises it with the name given as a parameter
		 *@param p_strName : the group name
		 */
		AnimatedObjectGroup( Scene * p_pScene, Manager<AnimatedObjectGroup> * p_pManager, const String & p_strName);
		/**
		 * Destructor, deletes the objects and the animations
		 */
		~AnimatedObjectGroup();
		/**
		 * Creates an AnimatedObject from the MovableObject given as a parameter, adds it to the list
		 *@param p_pObject : the MovableObject from which to create an AnimatedObject
		 *@return the created and added AnimatedObject
		 */
		AnimatedObjectPtr AddObject( MovableObjectPtr p_pObject);
		/**
		 * Adds the animation to the list
		 *@param p_pAnimation : the animation to add
		 */
		void AddAnimation( AnimationPtr p_pAnimation);
		/**
		 * Update all the animations
		 *@param p_rTslf : time since the last frame
		 */
		void Update( real p_rTslf);
		/**
		 * Starts an animation, given its name
		 *@param p_strName : the animation name
		 */
		void StartAnimation( const String & p_strName);
		/**
		 * Stops an animation, given its name
		 *@param p_strName : the animation name
		 */
		void StopAnimation( const String & p_strName);
		/**
		 * Pause an animation, given its name
		 *@param p_strName : the animation name
		 */
		void PauseAnimation(  const String & p_strName);
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

		/**@name Inherited methods from Textable */
		//@{
		virtual bool Write( File & p_file)const;
		virtual bool Read( File & p_file) { return false; }
		//@}
		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const;
		virtual bool Load( File & p_file);
		//@}

	public:
		/**@name Accessors */
		//@{
		/**
		 * Returns the name of the group
		 *@return the name of the group
		 */
		inline const String & GetName() { return m_strName; }
		//@}
	};
}

#endif

