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
#ifndef ___C3D_Geometry___
#define ___C3D_Geometry___

#include "../../Prerequisites.h"
#include "../../main/MovableObject.h"

namespace Castor3D
{
	//! Geometry handler class
	/*!
	Geometry description, with normals mode, material and mesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API Geometry : public MovableObject, public MemoryTraced<MovableObject>
	{
	protected:
		friend class Scene;
		Point3r m_center;					//!< The center's position, all vertexes position are relative to the center
		Quaternion m_orientation;			//!< The orientation
		Matrix4x4r m_matrix;				//!< The rotation matrix
		eNORMALS_MODE m_normalsMode;		//!< The normals mode
		MeshPtr m_mesh;						//!< The mesh
		bool m_changed;						//!< Tells if the geometry has changed (mesh, material ...)
		bool m_listCreated;					//!< Tells if the Vertex Buffer are generated
		bool m_visible;
		bool m_dirty;

	public :
		/**
		 * Constructor, not to be used by the user, use Scene::CreatePrimitive function
		 *@param p_mesh : [in] The mesh, default is NULL
		 *@param p_sn : [in] The scene node to which the geometry is attached
		 *@param p_name : [in] The geometry name, default is void
		 */
		Geometry( MeshPtr p_mesh=MeshPtr(), SceneNodePtr p_sn=SceneNodePtr(), const String & p_name = C3DEmptyString);
		/**
		 * Destructor
		 */
		virtual ~Geometry();
		/**
		 * Cleans all the object owned and created by the geometry
		 */
		void Cleanup();
		/**
		 * Creates the vertex and normal buffers
		 *@param p_nm : [in] Normals mode in which to create the normals buffer
		 *@param p_nbFaces : [in/out] Used to retrieve the faces number
		 *@param p_nbVertex : [in/out] Used to retrieve the vertexes number
		 */
		void CreateBuffers( eNORMALS_MODE p_nm, size_t & p_nbFaces, size_t & p_nbVertex);
		/**
		 * Writes the geometry in a file
		 *@param p_pFile : [in] file to write in
		 *@return true if successful, false if not
		 */
		bool Write( Castor::Utils::File & p_pFile)const;
		/**
		 * Renders the geometry in a given display mode
		 *@param p_displayMode : [in] the display mode in which the render must be made
		 */
		virtual void Render( eDRAW_TYPE p_displayMode);
		virtual void EndRender(){}
		/**
		 * Subdivides a submesh of the geometry
		 *@param p_index : [in] The submesh index
		 *@param p_mode : [in] The subdivision mode
		 */
		void Subdivide( unsigned int p_index, SubdividerPtr p_pSubdivider, bool p_bThreaded = false);
		/**
		 * Computes then returns the rotation matrix
		 *@return The rotation matrix
		 */
		const Matrix4x4r & GetRotationMatrix();

	public:
		/**@name Accessors */
		//@{
		inline MeshPtr				GetMesh			()const { return m_mesh; }
		inline bool					HasListsCreated	()const { return m_listCreated; }
		inline bool					IsVisible		()const { return m_visible; }
		/**
		 * @return The object center position pointer
		 */
		inline const Point3r	*	GetCenter		()const { return & m_center; }
		/**
		 * @return The orientation
		 */
		inline const Quaternion	&	GetOrientation	()const	{ return m_orientation; }
		/**
		 * @return The object center position
		 */
		inline const Point3r	&	GetPosition		()const { return m_center; }

		inline void	SetVisible		( bool p_visible)	{ m_visible = p_visible; }
		inline void	SetMesh			( MeshPtr p_pMesh)	{ m_mesh = p_pMesh; }
		/**
		 * @param p_position : [in] The new position
		 */
		inline void SetPosition		( const Point3r & p_position)			{ m_center = p_position; }
		/**
		 * @param p_orientation : [in] The new orientation
		 */
		inline void SetOrientation	( const Quaternion & p_orientation)		{ m_orientation = p_orientation; }
		//@}
	};
}

#endif
