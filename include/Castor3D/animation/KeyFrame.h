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
	class C3D_API MovingObject : public Serialisable, public MemoryTraced<MovingObject>
	{
	private:
		Point3r m_ptTranslate;		//!< The wanted translation of the object
		Point3r m_ptScale;			//!< The wanted scale of the object
		Quaternion m_qRotate;		//!< The wanted rotation of the object
		MovableObjectPtr m_pObject;	//!< the object affected by the animations
		Scene * m_pScene;

	public:
		/**
		 * Default constructor, dummy
		 */
		MovingObject( Scene * p_pScene);
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

		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const;
		virtual bool Load( File & p_file);
		//@}

	public:
		/**@name Accessors */
		//@{
		inline void SetObject		( MovableObjectPtr p_pObject)		{ m_pObject = p_pObject; }
		inline void SetTranslate	( const Point3r & p_ptTranslate)	{ m_ptTranslate = p_ptTranslate; }
		inline void SetScale		( const Point3r & p_ptScale)		{ m_ptScale = p_ptScale; }
		inline void SetRotate		( const Quaternion & p_qRotate)		{ m_qRotate = p_qRotate; }
		inline MovableObjectPtr		GetObject		()const { return m_pObject; }
		inline const Quaternion	&	GetRotate		()const { return m_qRotate; }
		inline const Point3r	&	GetScale		()const { return m_ptScale; }
		inline const Point3r	&	GetTranslate	()const { return m_ptTranslate; }
		//@}
	};

	//! The class which manages key frames
	/*!
	Key frames are the frames are the frames where the animation must be at a precise state
	*/
	class C3D_API KeyFrame : public Serialisable, public MemoryTraced<KeyFrame>
	{
	protected:
		MovingObjectPtrStrMap m_mapToMove;	//!< The list of objects to move
		real m_rTo;							//!< The end time index
		real m_rFrom;						//!< The start time index
		Scene * m_pScene;

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor, tells from which time index the animation starts and to which time index it ends
		 *@param p_rFrom : [in] When the animation starts
		 *@param p_rTo : [in] When the animation ends
		 */
		KeyFrame( Scene * p_pScene, real p_rFrom=0, real p_rTo=0);
		/**
		 * Copy constructor
		 *@param p_keyFrame : [in] The KeyFrame to copy
		 */
		KeyFrame( const KeyFrame & p_keyFrame);
		/**
		 * Destructor, deletes all its objects
		 */
		~KeyFrame();
		//@}

		/**
		 * Updates the animation, given the time and the weight
		 *@param p_rTime : [in] The current time index
		 *@param p_rWeight : [in] The animation weight
		 */
		void Update( real p_rTime, real p_rWeight);
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

		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const;
		virtual bool Load( File & p_file);
		//@}

		/**@name Accessors */
		//@{
		inline real GetTo()const { return m_rTo; }
		inline real GetFrom()const { return m_rFrom; }
		//@}
	};
}

#endif

