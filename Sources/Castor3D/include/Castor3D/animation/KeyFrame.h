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
	class CS3D_API MovingObject
	{
	private:
		Point3r m_translate;		//! The wanted translation of the object
		Point3r m_scale;			//! The wanted scale of the object
		Quaternion m_rotate;		//! The wanted rotation of the object
		MovableObjectPtr m_object;	//! the object affected by the animations

	public:
		/**
		 * Default constructor, dummy
		 */
		MovingObject();
		/**
		 * Specified constructor, initialises the transformations
		 *@param p_object : the object to affect
		 *@param p_translate : the wanted object translation
		 *@param p_scale : the wanted object scale
		 *@param p_rotate : the wanted object rotation
		 */
		MovingObject( MovableObjectPtr p_object,
					  const Point3r & p_translate, const Point3r & p_scale,
					  const Quaternion & p_rotate);
		/**
		 * Destructor, dummy, destroy the movable object yourself
		 */
		~MovingObject();
		/**
		 * Updates the movable object, applies the transformations multiplied by the percentage and the weight given as parameters
		 *@param p_percent : percentage between 0.0 and 1.0
		 *@param p_weight : weight of the animation, determine how much to apply the transformations
		 */
		void Update( real p_percent, real p_weight);

	public:
		/**@name Accessors */
		//@{
		/**
		 * Sets the movable object to animate
		 *@param p_object : the new movable object
		 */
		inline void SetObject		( MovableObjectPtr p_object)	{ m_object = p_object; }
		/**
		 * Sets the translation to apply to the movable object
		 *@param p_translate : the new translation
		 */
		inline void SetTranslate	( const Point3r & p_translate)	{ m_translate = p_translate; }
		/**
		 * Sets the scale to apply to the movable object
		 *@param p_scale : the new scale
		 */
		inline void SetScale		( const Point3r & p_scale)		{ m_scale = p_scale; }
		/**
		 * Sets the rotation to apply to the movable object
		 *@param p_rotate : the new rotation
		 */
		inline void SetRotate		( const Quaternion & p_rotate)	{ m_rotate = p_rotate; }
		/**
		 * Gives the movable object
		 *@return the current movable object
		 */
		inline MovableObjectPtr GetObject ()const { return m_object; }
		//@}
	};

	//! The class which manages key frames
	/*!
	Key frames are the frames are the frames where the animation must be at a precise state
	*/
	class CS3D_API KeyFrame
	{
	protected:
		//! The list of objects to move
		MovingObjectPtrStrMap m_toMove;
		//! The end time index
		real m_to;
		//! The start time index
		real m_from;

	public:
		/**
		 * Constructor, tells from which time index the animation starts and to which time index it ends
		 *@param p_from : when the animation starts
		 *@param p_to : when the animation ends
		 */
		KeyFrame( real p_from, real p_to);
		/**
		 * Destructor, deletes all its objects
		 */
		~KeyFrame();
		/**
		 * Updates the animation, given the time and the weight
		 *@param p_time : the current time index
		 *@param p_weight : the animation weight
		 */
		void Update( real p_time, real p_weight);
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
		 *@param p_object : the movable object to add
		 *@param p_translate : the wanted translation
		 *@param p_scale : the wanted scale
		 *@param p_rotate : the wanted rotation
		 */
		void AddMovingObject( MovableObjectPtr p_object, const Point3r & p_translate,
							  const Point3r & p_scale, const Quaternion & p_rotate);
		/**
		 * Removes a moving object, determined by its name, deletes the MovingObject, not the MovableObject
		 *@param p_name : the name of the object to remove
		 */
		void RemoveMovingObject( const String & p_name);
	public:
		/**@name Accessors */
		//@{
		/**
		 * Returns the end time index
		 *@return The end time index
		 */
		inline real GetTo()const { return m_to; }
		//@}
	};
}

#endif

