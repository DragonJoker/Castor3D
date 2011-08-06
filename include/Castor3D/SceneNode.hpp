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
#ifndef ___C3D_SceneNode___
#define ___C3D_SceneNode___

#include "Prerequisites.hpp"
#include "Renderable.hpp"

namespace Castor
{	namespace Resources
{
	//! SceneNode loader
	/*!
	Loads and saves scene nodes from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::SceneNode>
	{
	public:
		/**
		 * Writes a scene node into a text file
		 *@param p_file : [in] the file to write the scene node in
		 *@param p_node : [in] the scene node to write
		 */
		static bool Write( const Castor3D::SceneNode & p_node, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	//! The scene node handler class C3D_API
	/*!
	A scene node is a parent for nearly every object in a scene : geometry, camera...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API SceneNode : public Serialisable
	{
	public:
		static unsigned long long Count;								//!< The total number of scene nodes

	protected:
		typedef KeyedContainer< String, SceneNode *>::Map		SceneNodePtrStrMap;
		typedef KeyedContainer< String, MovableObject *>::Map	MovableObjectPtrStrMap;
		
		static Matrix4x4r MtxTranslate;
		static Matrix4x4r MtxRotate;
		static Matrix4x4r MtxScale;

	public:
		typedef SceneNodePtrStrMap::iterator					node_iterator;
		typedef SceneNodePtrStrMap::const_iterator				node_const_iterator;
		typedef MovableObjectPtrStrMap::iterator				object_iterator;
		typedef MovableObjectPtrStrMap::const_iterator			object_const_iterator;

	protected:
		String						m_strName;				//!< The node's name
		bool						m_bDisplayable;			//!< Tells if it is displayable. A node is displayable if his parent is either displayable or the root node
		bool						m_bVisible;				//!< The visible status. If a node is hidden, all objects attached to it are hidden
		Quaternion					m_qOrientation;			//!< The relative orientation of the node
		Point3r						m_ptPosition;			//!< The relative position of the node
		Point3r						m_ptScale;				//!< The relative scale transform value of the node
		Quaternion					m_qDerivedOrientation;	//!< The absolute orientation of the node
		Point3r						m_ptDerivedPosition;	//!< The absolute position of the node
		Point3r						m_ptDerivedScale;		//!< The absolute scale transform value of the node
		Matrix4x4r					m_mtxMatrix;			//!< The rotation matrix
		bool						m_bMtxChanged;

		SceneNode				*	m_pParent;				//!< This node's parent
		SceneNodePtrStrMap			m_mapChilds;			//!< This node's childs		
		MovableObjectPtrStrMap		m_mapAttachedObjects;	//!< This node's attached geometries
		Scene					*	m_pScene;

	public:
		/**
		* Constructor
		*/
		SceneNode();
		/**
		* Constructor
		*@param p_name : [in] The node's name. If empty the name is "SceneNode<s_nbSceneNodes>"
		*/
		SceneNode( Scene * p_pScene, String const & p_name = cuEmptyString);
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
		bool HasChild( String const & p_name);
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
		void DetachChild( String const & p_childName);
		/**
		 * Detaches all my childs
		 */
		void DetachAllChilds();
		/**
		 * Rotates around Y axis
		 *@param p_angle : [in] the rotation angle
		 */
		void Yaw( Angle const & p_angle);
		/**
		 * Rotates around Z axis
		 *@param p_angle : [in] the rotation angle
		 */
		void Pitch( Angle const & p_angle);
		/**
		 * Rotates around X axis
		 *@param p_angle : [in] the rotation angle
		 */
		void Roll( Angle const & p_angle);
		/**
		 * Rotate the node with the given orientation
		 *@param x, y, z, w : [in] The orientation to add to current one
		 */
		void Rotate( real x, real y, real z, real w);
		/**
		 * Rotate the node with the given orientation
		 *@param p_quat : [in] The orientation to add to current one
		 */
		void Rotate( Quaternion const & p_quat);
		/**
		 * Rotate the node with the given orientation
		 *@param p_axis : [in] The axis
		 *@param p_angle : [in] The angle
		 */
		void Rotate( Point3r const & p_axis, Angle const & p_angle);
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
		void Translate( Point3r const & p_t);
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
		void Scale( Point3r const & p_s);
		/**
		 * Scales the node
		 *@param p_pCoords : [in] The scale value
		 */
		void Scale( real * p_pCoords);
		/**
		 * Sets the orientation
		 *@param p_orientation : [in] The new orientation
		 */
		void SetOrientation	( Quaternion const & p_orientation);
		/**
		 * Sets the orientation
		 *@param p_axis : [in] The axis
		 *@param p_angle : [in] The angle
		 */
		void SetOrientation	( Point3r const & p_axis, Angle const & p_angle);
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
		void SetPosition	( Point3r const & p_position);
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
		void SetScale		( Point3r const & p_scale);
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

		/**@name Accessors */
		//@{
		inline const Point3r			&	GetPosition				()const									{ return m_ptPosition; }
		inline Point3r					&	GetPosition				()										{ return m_ptPosition; }
		inline const Point3r			&	GetDerivedPosition		()const									{ return m_ptDerivedPosition; }
		inline const Quaternion			&	GetOrientation			()const									{ return m_qOrientation; }
		inline const Quaternion			&	GetDerivedOrientation	()const									{ return m_qDerivedOrientation; }
		inline const Point3r			&	GetScale				()const									{ return m_ptScale; }
		inline const Point3r			&	GetDerivedScale			()const									{ return m_ptDerivedScale; }
		inline void							GetAxisAngle			( Point3r & p_axis, Angle & p_angle)	{ m_qOrientation.ToAxisAngle( p_axis, p_angle); }
		inline bool							IsVisible				()const									{ return m_bVisible; }
		inline bool							IsDisplayable			()const									{ return m_bDisplayable; }
		inline SceneNode				*	GetParent				()const									{ return m_pParent; }
		inline String						GetName					()const									{ return m_strName; }
		inline node_iterator				ChildsBegin				()										{ return m_mapChilds.begin(); }
		inline node_const_iterator			ChildsBegin				()const									{ return m_mapChilds.begin(); }
		inline node_const_iterator			ChildsEnd				()const									{ return m_mapChilds.end(); }
		inline object_iterator				ObjectsBegin			()										{ return m_mapAttachedObjects.begin(); }
		inline object_const_iterator		ObjectsBegin			()const									{ return m_mapAttachedObjects.begin(); }
		inline object_const_iterator		ObjectsEnd				()const									{ return m_mapAttachedObjects.end(); }
		inline SceneNode *					GetChild				( String const & p_name)				{ return (m_mapChilds.find( p_name) != m_mapChilds.end() ? m_mapChilds.find( p_name)->second : nullptr); }
		inline Matrix4x4r const &			GetRotationMatrix		()										{ _computeMatrix();return m_mtxMatrix; }

		inline void SetVisible		( bool p_visible)						{ m_bVisible = p_visible; }
		inline void SetName			( String const & p_name)				{ m_strName = p_name; }
		//@}

	private:
		void _computeMatrix();
		DECLARE_SERIALISE_MAP()
	};
}

#endif