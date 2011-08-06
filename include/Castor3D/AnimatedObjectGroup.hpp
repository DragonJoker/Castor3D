/*
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
#ifndef ___C3D_AnimatedObjectGroup___
#define ___C3D_AnimatedObjectGroup___

#include "Prerequisites.hpp"
#include <CastorUtils/Loader.hpp>

namespace Castor
{	namespace Resources
{
	//! AnimatedObjectGroup loader
	/*!
	Loads and saves animated object groups from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::AnimatedObjectGroup>
	{
	private:
		static Castor3D::Scene * m_pScene;

	public:
		/**
		 * Loads an animated object group from a binary file
		 *@param p_file : [in] the file to save the animated object group in
		 *@param p_group : [in] the animated object group to save
		 *@param p_pScene : [in] the scene which holds the animated object group
		 */
		static bool Load( Castor3D::AnimatedObjectGroup & p_group, Utils::File & p_file, Castor3D::Scene * p_pScene);
		/**
		 * Writes an animated object group into a text file
		 *@param p_file : [in] the file to save the animated object group in
		 *@param p_group : [in] the animated object group to save
		 */
		static bool Write( const Castor3D::AnimatedObjectGroup & p_group, Utils::File & p_file);

	private:
		static bool Load( Castor3D::AnimatedObjectGroup & p_group, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	//! Class which represents the animated object groups
	/*!
	Class to represent animated object groups, id est the list of animated objects with the same animations
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\todo Review all the animation system because it's not clear, not optimised, and not good enough to be validated.
	*/
	class C3D_API AnimatedObjectGroup : public Serialisable, public MemoryTraced<AnimatedObjectGroup>
	{
	protected:
		friend class Factory<AnimatedObjectGroup>;

		String m_strName;							//!< The animated object group name
		AnimationPtrStrMap m_mapAnimations;			//!< The list of animations
		AnimatedObjectPtrStrMap m_mapObjects;		//!< The list of AnimatedObjects
		AnimationPtrStrMap m_mapPlayingAnimations;	//!< The map of currently playing animations
		Scene * m_pScene;
		Utils::PreciseTimer m_timer;				//!< A timer, usefull for animation handling

	protected:
		/**
		 * Constructor
		 */
		AnimatedObjectGroup();
		/**
		 * Constructor of the group, intialises it with the name given as a parameter
		 *@param p_strName : the group name
		 */
		AnimatedObjectGroup( Scene * p_pScene, String const & p_strName);
		DECLARE_SERIALISE_MAP()

	public:
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
		 * Adds an AnimatedObject to this group.
		 * This function assumes the object has been created in order to be put in this group (with the good scene and animations map)
		 */
		bool AddObject( AnimatedObjectPtr p_pObject);
		/**
		 * Adds the animation to the list
		 *@param p_pAnimation : the animation to add
		 */
		void AddAnimation( AnimationPtr p_pAnimation);
		/**
		 * Update all the animations
		 */
		void Update();
		/**
		 * Starts an animation, given its name
		 *@param p_strName : the animation name
		 */
		void StartAnimation( String const & p_strName);
		/**
		 * Stops an animation, given its name
		 *@param p_strName : the animation name
		 */
		void StopAnimation( String const & p_strName);
		/**
		 * Pause an animation, given its name
		 *@param p_strName : the animation name
		 */
		void PauseAnimation(  String const & p_strName);
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

	public:
		/**@name Accessors */
		//@{
		/**
		 * Returns the name of the group
		 *@return the name of the group
		 */
		inline const String	&							GetName				()const	{ return m_strName; }
		inline size_t									GetAnimationCount	()const	{ return m_mapAnimations.size(); }
		inline size_t									GetObjectCount		()const	{ return m_mapObjects.size(); }
		inline AnimationPtrStrMap &						GetAnimations		()		{ return m_mapAnimations; }
		inline AnimationPtrStrMap::const_iterator		AnimationsBegin		()const	{ return m_mapAnimations.begin(); }
		inline AnimationPtrStrMap::const_iterator		AnimationsEnd		()const	{ return m_mapAnimations.end(); }
		inline AnimatedObjectPtrStrMap::iterator		ObjectsBegin		()		{ return m_mapObjects.begin(); }
		inline AnimatedObjectPtrStrMap::const_iterator	ObjectsBegin		()const	{ return m_mapObjects.begin(); }
		inline AnimatedObjectPtrStrMap::const_iterator	ObjectsEnd			()const	{ return m_mapObjects.end(); }
		inline Scene *									GetScene			()const	{ return m_pScene; }
		//@}
	};
}

#endif

