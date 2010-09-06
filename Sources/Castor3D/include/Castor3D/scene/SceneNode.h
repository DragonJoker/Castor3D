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
#ifndef ___C3D_SceneNode___
#define ___C3D_SceneNode___

#include "Module_Scene.h"
#include "../geometry/Module_Geometry.h"

namespace Castor3D
{
	//! The scene node handler class
	/*!
	A scene node is a parent for nearly every object in a scene : geometry, camera...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API SceneNode
	{
	public:
		static int s_nbSceneNodes;						//!< The total number of scene nodes

	private:
		String m_name;									//!< The node's name
		bool m_displayable;								//!< Tells if it is displayable. A node is displayable if his parent is either displayable or the root node
		bool m_visible;									//!< The visible status. If a node is hidden, all objects attached to it are hidden
		Quaternion m_orientation;						//!< The relative orientation of this node
		Vector3f m_position;							//!< The realtive position of this node
		Vector3f m_scale;								//!< The relative scale transform value of this node
		Quaternion m_derivedOrientation;				//!< The absolute orientation of this node
		Vector3f m_derivedPosition;						//!< The absolute position of this node
		Vector3f m_derivedScale;						//!< The absolute scale transform value of this node
		float * m_matrix;								//!< The rotation matrix

		SceneNode * m_parent;							//!< This node's parent
		SceneNodeStrMap m_childs;						//!< This node's childs
		SceneNodeStrMap::iterator m_childsBegin;		//!< Iterator on the begin of the child's map
		SceneNodeStrMap::iterator m_childsEnd;			//!< Iterator on the end of the child's map
		
		MovableObjectStrMap m_attachedGeometries;		//!< This node's attached geometries
		MovableObjectStrMap::iterator m_geometriesBegin;//!< Iterator on the begin of the geometries map
		MovableObjectStrMap::iterator m_geometriesEnd;	//!< Iterator on the end of the geometries map

		SceneNodeRenderer * m_renderer;					//!< The node renderer

	public:
		/**
		* Constructor
		*@param p_renderer : [in] The scene node renderer, may be OpenGL or Direct3D
		*@param p_name : [in] The node's name. If empty the name is "SceneNode<s_nbSceneNodes>"
		*/
		SceneNode( SceneNodeRenderer * p_renderer, const String & p_name = C3DEmptyString);
		/**
		 * Destructor
		 */
		~SceneNode();
		/**
		 * Draws the geometries and the child's ones in the display mode given
		 *@param p_displayMode : [in] The mode in which the display must be done
		 */
		void Draw( DrawType p_displayMode);
		/**
		 * Attaches a MovableObject to this node
		 *@param p_geometry : [in] The geometry to attach
		 */
		void AttachGeometry( MovableObject * p_geometry);
		/**
		 * Detaches a MovableObject from this node
		 *@param p_geometry : [in] The geometry to detach
		 */
		void DetachGeometry( MovableObject * p_geometry);
		/**
		 * Sets the parent
		 *@param p_parent : [in] The new parent
		 */
		void AttachTo( SceneNode * p_parent);
		/**
		 * Detaches this node from it's parent
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
		void AddChild( SceneNode * p_child);
		/**
		 * Detaches a child from my child's list, if it is one of my childs
		 *@param p_child : [in] The child to detach
		 */
		void DetachChild( SceneNode * p_child);
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
		 * Destroy a child from my child's list, if it is one of my childs
		 *@param p_child : [in] The child to destroy
		 */
		void DestroyChild( SceneNode * p_child);
		/**
		 * Destroy a child from my child's list, if it is one of my childs
		 *@param p_childName : [in] The name of the child to destroy
		 */
		void DestroyChild( const String & p_childName);
		/**
		 * Destroys all my childs
		 */
		void DestroyAllChilds();
		/**
		* Destroys all my childs, and all their childs, recursively
		*/
		void DestroyAllChildsRecursive();
		/**
		 * Rotates around Y axis
		 *@param p_angle : [in] the rotation angle
		 */
		void Yaw( float p_angle);
		/**
		 * Rotates around Z axis
		 *@param p_angle : [in] the rotation angle
		 */
		void Pitch( float p_angle);
		/**
		 * Rotates around X axis
		 *@param p_angle : [in] the rotation angle
		 */
		void Roll( float p_angle);
		/**
		 * Rotate the node with the given orientation
		 *@param p_quat : [in] The orientation to add to current one
		 */
		void Rotate( const Quaternion & p_quat);
		/**
		 * Translates the node
		 *@param p_t : [in] The translation value
		 */
		void Translate( const Vector3f & p_t);
		/**
		 * Creates the vertex buffer of attached geometries
		 *@param p_nm : [in] The normals mode (face or vertex)
		 *@param p_showNormals : [in] Tells if we show normals (of faces or vertex)
		 *@param p_nbFaces : [in/out] The faces number
		 *@param p_nbVertex : [in/out] The vertex number
		 */
		void CreateList( NormalsMode p_nm, bool p_showNormals, 
						 size_t & p_nbFaces, size_t & p_nbVertex)const;
		/**
		 * Writes the node  and it's childs recursively in a file
		 *@param p_pFile : [in] The file to write in
		 *@return true if successful, false if not
		 */
		bool Write( General::Utils::FileIO * p_pFile)const;
		/**
		 * Writes this node in a file
		 *@param p_pFile : [in] The file to write in
		 *@return true if successful, false if not
		 */
		bool WriteOne( General::Utils::FileIO * p_pFile)const;
		/**
		 * Computes the rotation matrix and returns it
		 *@return The rotation matrix
		 */
		float *	Get4x4RotationMatrix();
		/**
		* Applies the scene node transformations, for rendering
		*/
		void ApplyTransformations();
		/**
		* Removes the scene node transformations, for rendering
		*/
		void RemoveTransformations();
		/**
		 * Sets the orientation
		 *@param p_orientation : [in] The new orientation
		 */
		void SetOrientation	( const Quaternion & p_orientation);
		/**
		 * Sets the relative position from a Vector3f
		 *@param p_position : [in] The new position
		 */
		void SetPosition	( const Vector3f & p_position);
		/**
		 * Sets the relative position from a three floats
		 *@param x, y, z : [in] The new position values
		 */
		void SetPosition	( float x, float y, float z);
		/**
		 * Sets the relative scale from a Vector3f
		 *@param p_scale : [in] The new scale
		 */
		void SetScale		( const Vector3f & p_scale);
		/**
		 * Sets the relative scale from a three floats
		 *@param x, y, z : [in] The new scale values
		 */
		void SetScale		( float x, float y, float z);

		Geometry * GetNearestGeometry( Ray * p_pRay, float & p_fDistance, Face ** p_ppFace, Submesh ** p_ppSubmesh);

	public:
		inline const Vector3f &			GetPosition				()const									{ return m_position; }
		inline Vector3f					GetDerivedPosition		()const									{ return m_derivedPosition; }
		inline const Quaternion &		GetOrientation			()const									{ return m_orientation; }
		inline Quaternion				GetDerivedOrientation	()const									{ return m_derivedOrientation; }
		inline const Vector3f &			GetScale				()const									{ return m_scale; }
		inline Vector3f 				GetDerivedScale			()const									{ return m_derivedScale; }
		inline void						GetAxisAngle			( Vector3f & p_axis, float & p_angle)	{ m_orientation.ToAxisAngle( p_axis, p_angle); }
		inline bool						IsVisible				()const									{ return m_visible; }
		inline bool						IsDisplayable			()const									{ return m_displayable; }
		inline SceneNode *				GetParent				()const									{ return m_parent; }
		inline String					GetName					()const									{ return m_name; }
		inline SceneNodeStrMap			GetChilds				()const									{ return m_childs; }
		inline MovableObjectStrMap		GetAttachedGeometries	()const									{ return m_attachedGeometries; }
		inline SceneNode *				GetChild				( const String & p_name)				{ return (m_childs.find( p_name) != m_childs.end() ? m_childs.find( p_name)->second : NULL); }

		inline void SetVisible		( bool p_visible)						{ m_visible = p_visible; }
		inline void SetName			( const String & p_name)				{ m_name = p_name; }
	};
}

#endif

