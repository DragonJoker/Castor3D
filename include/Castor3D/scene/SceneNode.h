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

#include "../Prerequisites.h"
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
	class C3D_API SceneNode : public Serialisable, public Textable
	{
	public:
		static unsigned long long Count;								//!< The total number of scene nodes

	protected:
		typedef KeyedContainer< String, SceneNode *>::Map		SceneNodePtrStrMap;
		typedef KeyedContainer< String, MovableObject *>::Map	MovableObjectPtrStrMap;

	protected:
		String m_strName;								//!< The node's name
		bool m_bDisplayable;							//!< Tells if it is displayable. A node is displayable if his parent is either displayable or the root node
		bool m_bVisible;								//!< The visible status. If a node is hidden, all objects attached to it are hidden
		Quaternion m_qOrientation;						//!< The relative orientation of the node
		Point3r m_ptPosition;							//!< The relative position of the node
		Point3r m_ptScale;								//!< The relative scale transform value of the node
		Quaternion m_qDerivedOrientation;				//!< The absolute orientation of the node
		Point3r m_ptDerivedPosition;					//!< The absolute position of the node
		Point3r m_ptDerivedScale;						//!< The absolute scale transform value of the node
		Matrix4x4r m_mtxMatrix;							//!< The rotation matrix

		SceneNode * m_pParent;							//!< This node's parent
		SceneNodePtrStrMap m_mapChilds;					//!< This node's childs		
		MovableObjectPtrStrMap m_mapAttachedObjects;	//!< This node's attached geometries
		Scene * m_pScene;

	public:
		/**
		* Constructor
		*@param p_name : [in] The node's name. If empty the name is "SceneNode<s_nbSceneNodes>"
		*/
		SceneNode( Scene * p_pScene, const String & p_name = C3DEmptyString);
		/**
		 * Destructor
		 */
		virtual ~SceneNode();
		/**
		 * Draws the geometries and the child's ones in the display mode given
		 *@param p_displayMode : [in] The mode in which the display must be done
		 */
		virtual void Render( ePRIMITIVE_TYPE p_displayMode);
		/**
		 * Draws the geometries and the child's ones in the display mode given
		 *@param p_displayMode : [in] The mode in which the display must be done
		 */
		virtual void EndRender();
		/**
		 * Attaches a MovableObject to the node
		 *@param p_pObject : [in] The geometry to attach
		 */
		void AttachObject( MovableObject * p_pObject);
		/**
		 * Detaches a MovableObject from the node
		 *@param p_pObject : [in] The geometry to detach
		 */
		void DetachObject( MovableObject * p_pObject);
		/**
		 * Sets the parent
		 *@param p_parent : [in] The new parent
		 */
		void AttachTo( SceneNode * p_parent);
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
		 *@param x, y, z, w : [in] The orientation to add to current one
		 */
		void Rotate( real x, real y, real z, real w);
		/**
		 * Rotate the node with the given orientation
		 *@param p_quat : [in] The orientation to add to current one
		 */
		void Rotate( const Quaternion & p_quat);
		/**
		 * Rotate the node with the given orientation
		 *@param p_axis : [in] The axis
		 *@param p_angle : [in] The angle
		 */
		void Rotate( const Point3r & p_axis, const Angle & p_angle);
		/**
		 * Rotate the node with the given orientation
		 *@param p_quat : [in] The orientation to add to current one
		 */
		void Rotate( real * p_quat);
		/**
		 * Translates the node
		 *@param x, y, z : [in] The translation value
		 */
		void Translate( real x, real y, real z);
		/**
		 * Translates the node
		 *@param p_t : [in] The translation value
		 */
		void Translate( const Point3r & p_t);
		/**
		 * Translates the node
		 *@param p_pCoords : [in] The translation value
		 */
		void Translate( real * p_pCoords);
		/**
		 * Scales the node
		 *@param x, y, z : [in] The scale value
		 */
		void Scale( real x, real y, real z);
		/**
		 * Scales the node
		 *@param p_s : [in] The scale value
		 */
		void Scale( const Point3r & p_s);
		/**
		 * Scales the node
		 *@param p_pCoords : [in] The scale value
		 */
		void Scale( real * p_pCoords);
		/**
		 * Sets the orientation
		 *@param p_orientation : [in] The new orientation
		 */
		void SetOrientation	( const Quaternion & p_orientation);
		/**
		 * Sets the orientation
		 *@param p_axis : [in] The axis
		 *@param p_angle : [in] The angle
		 */
		void SetOrientation	( const Point3r & p_axis, const Angle & p_angle);
		/**
		 * Sets the orientation
		 *@param x,y,z,w : [in] The new orientation
		 */
		void SetOrientation	( real x, real y, real z, real w);
		/**
		 * Sets the orientation
		 *@param p_quat : [in] The new orientation
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
		 *@param p_pCoords : [in] The new position values
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
		 *@param p_pCoords : [in] The new scale values
		 */
		void SetScale		( real * p_pCoords);
		/**
		 * Creates the vertex buffer of attached geometries
		 *@param p_nm : [in] The normals mode (face or vertex)
		 *@param p_showNormals : [in] Tells if we show normals (of faces or vertex)
		 *@param p_nbFaces : [in/out] The faces number
		 *@param p_nbVertex : [in/out] The vertex number
		 */
		void CreateList( eNORMALS_MODE p_nm, bool p_showNormals, size_t & p_nbFaces, size_t & p_nbVertex)const;
		/**
		 * Gets the nearest geometry held by this node or it's children nodes, which is hit by the ray
		 */
		Geometry * GetNearestGeometry( Ray * p_pRay, real & p_fDistance, FacePtr * p_ppFace, SubmeshPtr * p_ppSubmesh);

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

		/**@name Accessors */
		//@{
		inline const Point3r											&	GetPosition				()const										{ return m_ptPosition; }
		inline Point3r													&	GetPosition				()											{ return m_ptPosition; }
		inline Point3r														GetDerivedPosition		()const										{ return m_ptDerivedPosition; }
		inline const Quaternion											&	GetOrientation			()const										{ return m_qOrientation; }
		inline Quaternion													GetDerivedOrientation	()const										{ return m_qDerivedOrientation; }
		inline const Point3r											&	GetScale				()const										{ return m_ptScale; }
		inline Point3r														GetDerivedScale			()const										{ return m_ptDerivedScale; }
		inline void															GetAxisAngle			( Point3r & p_axis, Angle & p_angle)		{ m_qOrientation.ToAxisAngle( p_axis, p_angle); }
		inline bool															IsVisible				()const										{ return m_bVisible; }
		inline bool															IsDisplayable			()const										{ return m_bDisplayable; }
		inline SceneNode												*	GetParent				()const										{ return m_pParent; }
		inline String														GetName					()const										{ return m_strName; }
		inline const KeyedContainer< String, SceneNode *>::Map			&	GetChilds				()const										{ return m_mapChilds; }
		inline const KeyedContainer< String, MovableObject *>::Map		&	GetAttachedObjects		()const										{ return m_mapAttachedObjects; }
		inline KeyedContainer< String, SceneNode *>::Map				&	GetChilds				()											{ return m_mapChilds; }
		inline KeyedContainer< String, MovableObject *>::Map			&	GetAttachedObjects		()											{ return m_mapAttachedObjects; }
		inline SceneNode *													GetChild				( const String & p_name)					{ return (m_mapChilds.find( p_name) != m_mapChilds.end() ? m_mapChilds.find( p_name)->second : NULL); }
		inline const Matrix4x4r &											GetRotationMatrix		()const										{ return m_mtxMatrix; }
		inline void SetVisible		( bool p_visible)						{ m_bVisible = p_visible; }
		inline void SetName			( const String & p_name)				{ m_strName = p_name; }
		//@}

	private:
		bool _writeOne( Castor::Utils::File & p_file)const;
	};
}

#endif