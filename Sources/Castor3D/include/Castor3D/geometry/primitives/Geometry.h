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
#ifndef ___C3D_Geometry___
#define ___C3D_Geometry___

#include "MovableObject.h"
#include "../../material/Module_Material.h"

namespace Castor3D
{
	//! Geometry handler class
	/*!
	Geometry description, with normals mode, material and mesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Geometry : public MovableObject
	{
	protected:
		friend class Scene;
		NormalsMode m_normalsMode;		//!< The normals mode
		Mesh * m_mesh;					//!< The mesh
		bool m_changed;					//!< Tells if the geometry has changed (mesh, material ...)
		bool m_listCreated;				//!< Tells if the Vertex Buffer are generated
		bool m_visible;
		bool m_dirty;

	public :
		/**
		 * Constructor, not to be used by the user, use Scene::CreatePrimitive function
		 *@param p_mesh : [in] The mesh, default is NULL
		 *@param p_sn : [in] The scene node to which the geometry is attached
		 *@param p_name : [in] The geometry name, default is void
		 */
		Geometry( Mesh * p_mesh=NULL, SceneNode * p_sn=NULL, const String & p_name = C3DEmptyString);
		/**
		 * Destructor
		 */
		~Geometry();
		/**
		 * Cleans all the object owned and created by this geometry
		 */
		void Cleanup();
		/**
		 * Creates the vertex and normal buffers
		 *@param p_nm : [in] Normals mode in which to create the normals buffer
		 *@param p_nbFaces : [in/out] Used to retrieve the faces number
		 *@param p_nbVertex : [in/out] Used to retrieve the vertexes number
		 */
		void CreateBuffers( NormalsMode p_nm, size_t & p_nbFaces, size_t & p_nbVertex);
		/**
		 * Writes the geometry in a file
		 *@param p_file : [in] file to write in
		 *@return true if successful, false if not
		 */
		bool Write( General::Utils::File & p_file)const;
		/**
		 * Reads the geometry from a file
		 *@param p_file : [in] file to read from
		 *@param p_scene : [in] The scene where we can find the node to attach the geometry
		 *@return true if successful, false if not
		 */
		bool Read( General::Utils::File & p_file, Scene * p_scene);
		/**
		 * Renders this geometry in a given display mode
		 *@param p_displayMode : [in] the display mode in which the render must be made
		 */
		void Render( DrawType p_displayMode);
		/**
		 * Subdivides the submesh of this geometry
		 *@param
		 *@return
		 */
		void Subdivide( unsigned int p_index, SubdivisionMode p_mode);

	public:
		/**
		 * @return The mesh
		 */
		inline Mesh *	GetMesh			()const { return m_mesh; }
		inline bool		HasListsCreated	()const { return m_listCreated; }
		inline bool		IsVisible		()const { return m_visible; }

		inline void SetVisible( bool p_visible) { m_visible = p_visible; }
		inline void SetMesh( Mesh * p_pMesh)	{ m_mesh = p_pMesh; }
	};
}

#endif
