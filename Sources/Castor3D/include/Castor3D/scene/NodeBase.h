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
#ifndef ___C3D_SceneNode___
#define ___C3D_SceneNode___

#include "Module_Scene.h"
#include "../geometry/Module_Geometry.h"
#include "../render_system/Renderable.h"

namespace Castor3D
{
	//! The scene node handler class
	/*!
	A scene node is a parent for nearly every object in a scene : geometry, camera...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API NodeBase
	{
	public:
		static int s_nbNodes;											//!< The total number of scene nodes

		typedef enum
		{
			eBase,
			eLight,
			eCamera,
			eGeometry
		}
		eTYPE;

	protected:
		String m_name;													//!< The node's name
		bool m_displayable;												//!< Tells if it is displayable. A node is displayable if his parent is either displayable or the root node
		bool m_visible;													//!< The visible status. If a node is hidden, all objects attached to it are hidden
		Quaternion m_orientation;										//!< The relative orientation of the node
		Point3r m_position;												//!< The relative position of the node
		Point3r m_scale;												//!< The relative scale transform value of the node
		Quaternion m_derivedOrientation;								//!< The absolute orientation of the node
		Point3r m_derivedPosition;										//!< The absolute position of the node
		Point3r m_derivedScale;											//!< The absolute scale transform value of the node
		Matrix4x4r m_matrix;											//!< The rotation matrix

		NodeBase * m_parent;											//!< This node's parent
		C3DMap( String, NodeBase *) m_childs;							//!< This node's childs		
		C3DMap( String, MovableObject *) m_attachedObjects;				//!< This node's attached geometries

	public:
		/**
		* Constructor
		*@param p_name : [in] The node's name. If empty the name is "SceneNode<s_nbSceneNodes>"
		*/
		NodeBase( const String & p_name = C3DEmptyString);
		/**
		 * Destructor
		 */
		virtual ~NodeBase();
		/**
		 * Draws the geometries and the child's ones in the display mode given
		 *@param p_displayMode : [in] The mode in which the display must be done
		 */
		virtual void Render( eDRAW_TYPE p_displayMode);
		/**
		 * Attaches a MovableObject to the node
		 *@param p_geometry : [in] The geometry to attach
		 */
		void AttachObject( MovableObject * p_pObject);
		/**
		 * Detaches a MovableObject from the node
		 *@param p_geometry : [in] The geometry to detach
		 */
		void DetachObject( MovableObject * p_pObject);
		/**
		 * Sets the parent
		 *@param p_parent : [in] The new parent
		 */
		void AttachTo( NodeBase * p_parent);
		/**
		 * Detaches the node from it's parent
		 */
		void Detach();
		/**
		 *@param p_name : [in] The name of the node
		 *@return true if one of my child has the given name
		 */
		bool HasChild( const String & p_name);
		/**
		 * Add the given node to my childs if it isn't already
		 *@param p_child : [in] The node to add
		 */
		void AddChild( NodeBase * p_child);
		/**
		 * Detaches a child from my child's list, if it is one of my childs
		 *@param p_child : [in] The child to detach
		 */
		void DetachChild( NodeBase * p_child);
		/**
		 * Detaches a child from my child's list, if it is one of my childs
		 *@param p_childName : [in] The name of the child to detach
		 */
		void DetachChild( const String & p_childName);
		/**
		 * Detaches all my childs
		 */
		void DetachAllChilds();
		/**
		 * Rotates around Y axis
		 *@param p_angle : [in] the rotation angle
		 */
		void Yaw( const Angle & p_angle);
		/**
		 * Rotates around Z axis
		 *@param p_angle : [in] the rotation angle
		 */
		void Pitch( const Angle & p_angle);
		/**
		 * Rotates around X axis
		 *@param p_angle : [in] the rotation angle
		 */
		void Roll( const Angle & p_angle);
		/**
		 * Rotate the node with the given orientation
		 *@param p_quat : [in] The orientation to add to current one
		 */
		void Rotate( real x, real y, real z, real w);
		/**
		 * Rotate the node with the given orientation
		 *@param p_quat : [in] The orientation to add to current one
		 */
		void Rotate( const Quaternion & p_quat);
		/**
		 * Rotate the node with the given orientation
		 *@param p_quat : [in] The orientation to add to current one
		 */
		void Rotate( const Point3r & p_quat, const Angle & p_angle);
		/**
		 * Rotate the node with the given orientation
		 *@param p_quat : [in] The orientation to add to current one
		 */
		void Rotate( real * p_quat);
		/**
		 * Translates the node
		 *@param p_t : [in] The translation value
		 */
		void Translate( real x, real y, real z);
		/**
		 * Translates the node
		 *@param p_t : [in] The translation value
		 */
		void Translate( const Point3r & p_t);
		/**
		 * Translates the node
		 *@param p_t : [in] The translation value
		 */
		void Translate( real * p_pCoords);
		/**
		 * Scales the node
		 *@param p_s : [in] The scale value
		 */
		void Scale( real x, real y, real z);
		/**
		 * Scales the node
		 *@param p_s : [in] The scale value
		 */
		void Scale( const Point3r & p_s);
		/**
		 * Scales the node
		 *@param p_s : [in] The scale value
		 */
		void Scale( real * p_pCoords);
		/**
		 * Sets the orientation
		 *@param p_orientation : [in] The new orientation
		 */
		void SetOrientation	( const Quaternion & p_orientation);
		/**
		 * Sets the orientation
		 *@param p_orientation : [in] The new orientation
		 */
		void SetOrientation	( const Point3r & p_axis, const Angle & p_angle);
		/**
		 * Sets the orientation
		 *@param p_orientation : [in] The new orientation
		 */
		void SetOrientation	( real x, real y, real z, real w);
		/**
		 * Sets the orientation
		 *@param p_orientation : [in] The new orientation
		 */
		void SetOrientation	( real * p_quat);
		/**
		 * Sets the relative position from a Point3r
		 *@param p_position : [in] The new position
		 */
		void SetPosition	( const Point3r & p_position);
		/**
		 * Sets the relative position from a three floats
		 *@param x, y, z : [in] The new position values
		 */
		void SetPosition	( real x, real y, real z);
		/**
		 * Sets the relative position from a three floats
		 *@param x, y, z : [in] The new position values
		 */
		void SetPosition	( real * p_pCoords);
		/**
		 * Sets the relative scale from a Point3r
		 *@param p_scale : [in] The new scale
		 */
		void SetScale		( const Point3r & p_scale);
		/**
		 * Sets the relative scale from a three floats
		 *@param x, y, z : [in] The new scale values
		 */
		void SetScale		( real x, real y, real z);
		/**
		 * Sets the relative scale from a three floats
		 *@param x, y, z : [in] The new scale values
		 */
		void SetScale		( real * p_pCoords);
		/**
		 * Writes the node  and it's childs recursively in a file
		 *@param p_pFile : [in] The file to write in
		 *@return true if successful, false if not
		 */
		virtual bool Write( Castor::Utils::File & p_pFile)const;
		virtual bool Read( Castor::Utils::File & p_pFile){ return true; }
		/**
		 * Writes the node in a file
		 *@param p_pFile : [in] The file to write in
		 *@return true if successful, false if not
		 */
		bool WriteOne( Castor::Utils::File & p_pFile)const;

	public:
		virtual eTYPE							GetType					()const										{ return eBase; }
		inline const Point3r				&	GetPosition				()const										{ return m_position; }
		inline Point3r						&	GetPosition				()											{ return m_position; }
		inline Point3r							GetDerivedPosition		()const										{ return m_derivedPosition; }
		inline const Quaternion				&	GetOrientation			()const										{ return m_orientation; }
		inline Quaternion						GetDerivedOrientation	()const										{ return m_derivedOrientation; }
		inline const Point3r				&	GetScale				()const										{ return m_scale; }
		inline Point3r							GetDerivedScale			()const										{ return m_derivedScale; }
		inline void								GetAxisAngle			( Point3r & p_axis, Angle & p_angle)		{ m_orientation.ToAxisAngle( p_axis, p_angle); }
		inline bool								IsVisible				()const										{ return m_visible; }
		inline bool								IsDisplayable			()const										{ return m_displayable; }
		inline NodeBase						*	GetParent				()const										{ return m_parent; }
		inline String							GetName					()const										{ return m_name; }
		inline C3DMap( String, NodeBase *)		GetChilds				()const										{ return m_childs; }
		inline C3DMap( String, MovableObject *)	GetAttachedObjects		()const										{ return m_attachedObjects; }
		inline NodeBase *						GetChild				( const String & p_name)					{ return (m_childs.find( p_name) != m_childs.end() ? m_childs.find( p_name)->second : NULL); }
		inline const Matrix4x4r &				GetRotationMatrix		()const										{ return m_matrix; }

		inline void SetVisible		( bool p_visible)						{ m_visible = p_visible; }
		inline void SetName			( const String & p_name)				{ m_name = p_name; }
	};
}

#endif