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
#ifndef ___C3D_KeyFrame___
#define ___C3D_KeyFrame___

#include "../Prerequisites.h"

namespace Castor3D
{
	//! Class which represents the moving objects
	/*!
	Class to represent moving and so animated objects.
	Manages translation, scale, rotation of the object
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\todo Review all the animation system because it's not clear, not optimised, and not good enough to be validated
	\todo Write and Read functions.
	*/
	class C3D_API MovingObject : public MemoryTraced<MovingObject>
	{
	private:
		Point3r m_ptTranslate;		//!< The wanted translation of the object
		Point3r m_ptScale;			//!< The wanted scale of the object
		Quaternion m_qRotate;		//!< The wanted rotation of the object
		MovableObjectPtr m_pObject;	//!< the object affected by the animations

	public:
		/**
		 * Default constructor, dummy
		 */
		MovingObject();
		/**
		 * Specified constructor, initialises the transformations
		 *@param p_pObject : [in] The object to affect
		 *@param p_ptTranslate : [in] The wanted object translation
		 *@param p_ptScale : [in] The wanted object scale
		 *@param p_qRotate : [in] The wanted object rotation
		 */
		MovingObject( MovableObjectPtr p_pObject,
					  const Point3r & p_ptTranslate, const Point3r & p_ptScale,
					  const Quaternion & p_qRotate);
		/**
		 * Destructor, dummy, destroy the movable object yourself
		 */
		~MovingObject();
		/**
		 * Updates the movable object, applies the transformations multiplied by the percentage and the weight given as parameters
		 *@param p_rPercent : [in] Percentage between 0.0 and 1.0
		 *@param p_rWeight : [in] Weight of the animation, determine how much to apply the transformations
		 */
		void Update( real p_rPercent, real p_rWeight);

	public:
		/**@name Accessors */
		//@{
		/**
		 * Sets the movable object to animate
		 *@param p_pObject : the new movable object
		 */
		inline void SetObject		( MovableObjectPtr p_pObject)	{ m_pObject = p_pObject; }
		/**
		 * Sets the translation to apply to the movable object
		 *@param p_ptTranslate : the new translation
		 */
		inline void SetTranslate	( const Point3r & p_ptTranslate)	{ m_ptTranslate = p_ptTranslate; }
		/**
		 * Sets the scale to apply to the movable object
		 *@param p_ptScale : the new scale
		 */
		inline void SetScale		( const Point3r & p_ptScale)		{ m_ptScale = p_ptScale; }
		/**
		 * Sets the rotation to apply to the movable object
		 *@param p_qRotate : the new rotation
		 */
		inline void SetRotate		( const Quaternion & p_qRotate)	{ m_qRotate = p_qRotate; }
		/**
		 * Gives the movable object
		 *@return the current movable object
		 */
		inline MovableObjectPtr GetObject ()const { return m_pObject; }
		//@}
	};

	//! The class which manages key frames
	/*!
	Key frames are the frames are the frames where the animation must be at a precise state
	*/
	class C3D_API KeyFrame : public MemoryTraced<KeyFrame>
	{
	protected:
		MovingObjectPtrStrMap m_mapToMove;	//!< The list of objects to move
		real m_rTo;							//!< The end time index
		real m_rFrom;						//!< The start time index

	public:
		/**
		 * Constructor, tells from which time index the animation starts and to which time index it ends
		 *@param p_rFrom : [in] When the animation starts
		 *@param p_rTo : [in] When the animation ends
		 */
		KeyFrame( real p_rFrom, real p_rTo);
		/**
		 * Destructor, deletes all its objects
		 */
		~KeyFrame();
		/**
		 * Updates the animation, given the time and the weight
		 *@param p_rTime : [in] The current time index
		 *@param p_rWeight : [in] The animation weight
		 */
		void Update( real p_rTime, real p_rWeight);
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
		 * Adds a moving object, with its wanted tranformations
		 *@param p_pObject : [in] The movable object to add
		 *@param p_ptTranslate : [in] The wanted translation
		 *@param p_ptScale : [in] The wanted scale
		 *@param p_qRotate : [in] The wanted rotation
		 */
		void AddMovingObject( MovableObjectPtr p_pObject, const Point3r & p_ptTranslate,
							  const Point3r & p_ptScale, const Quaternion & p_qRotate);
		/**
		 * Removes a moving object, determined by its name, deletes the MovingObject, not the MovableObject
		 *@param p_strName : [in] The name of the object to remove
		 */
		void RemoveMovingObject( const String & p_strName);
	public:
		/**@name Accessors */
		//@{
		/**
		 * Returns the end time index
		 *@return The end time index
		 */
		inline real GetTo()const { return m_rTo; }
		//@}
	};
}

#endif

