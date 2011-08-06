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
#ifndef ___C3D_Mesh___
#define ___C3D_Mesh___

#include "Prerequisites.hpp"

namespace Castor3D
{
	class MeshCategory : public Serialisable, public Clonable<MeshCategory>
	{
	private:
		eMESH_TYPE		m_eMeshType;
		Mesh		*	m_pMesh;

	public:
		MeshCategory( eMESH_TYPE p_eMeshType=eMESH_TYPE_CUSTOM);
		virtual ~MeshCategory();
		/**
		 * Generates the mesh points
		 */
		virtual void GeneratePoints();
		/**
		 * Clone function, used by Factory
		 */
		virtual MeshCategoryPtr Clone();
		/**
		* Initialises the mesh faces number and dimensions
		*@param p_arrayFaces : [in] The faces numbers
		*@param p_arrayDimensions : [in] The mesh dimensions
		 */
		virtual void Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions);
		/**
		 * Initialises all the normals
		 *@param p_bReverted : [in] Tells if the normals are reverted
		 */
		virtual void ComputeNormals( bool p_bReverted = false);

		inline eMESH_TYPE	GetMeshType	()const { return m_eMeshType; }
		inline Mesh *		GetMesh		()const { return m_pMesh; }

		inline void SetMesh	( Mesh * val)	{ m_pMesh = val; }

		DECLARE_SERIALISE_MAP()
	};
	//! The mesh representation
	/*!
	A mesh is a collection of submeshes. It has a defined mesh type (torus, custom...) and a name.
	If an attempt to create a mesh with the same name, you retrieve the existing one.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API Mesh : public Serialisable, public Resource<Mesh>, public MemoryTraced<Mesh>
	{
	protected:
		friend class MeshCategory;
		MeshCategoryPtr		m_pMeshCategory;		//!< The mesh category
		bool				m_modified;				//!< Tells whether or not the mesh is modified

		CubeBox				m_box;					//!< The combo box container
		SphereBox			m_sphere;				//!< The sphere container

		SubmeshPtrArray		m_submeshes;			//!< The submeshes array
		eNORMALS_MODE		m_normalsMode;
		bool				m_bOK;

	public:
		/**@name Construction */
		//@{
		/**
		 * Constructor
		 */
		Mesh( eMESH_TYPE p_eMeshType = eMESH_TYPE_CUSTOM);
		/**
		 * Constructor, only Factory<Mesh> can create a mesh
		 *@param p_name : [in] This mesh name
		 */
		Mesh( String const & p_name, eMESH_TYPE p_eType);
		//@}
		/**
		 * Destructor
		 */
		~Mesh();
		/**
		 * Registers all meshes types
		 */
		static void Register();
		/**
		 * Cleans up all submeshes
		 */
		void Cleanup();
		/**
		 * Initialises mesh's vertex and faces
		 */
		void Initialise( UIntArray p_arrayFaces, FloatArray p_arrayDimensions);
		/**
		 * Computes the combo box
		 */
		void ComputeContainers();
		/**
		 * Returns the total number of faces
		 *@return The faces number
		 */
		size_t GetNbFaces()const;
		/**
		 * Returns the total number of vertex
		 *@return The vertex number
		 */
		size_t GetNbVertex()const;
		/**
		* Tries to retrieve the submesh at the given index
		*@param p_index : [in] The wanted submesh index
		*@return The found submesh, nullptr if not found
		*/
		SubmeshPtr GetSubmesh( unsigned int p_index)const;
		/**
		* Creates a submesh
		*@param p_nbSmoothgroups : [in] The wanted number of smoothgroups
		*@return The created submesh, nullptr if not found
		*/
		SubmeshPtr CreateSubmesh( unsigned int p_nbSmoothgroups);
		/**
		 * Clones the mesh, with a new name
		 *@param p_name : [in] The name of the cloned mesh
		 *@return The cloned mesh
		 */
		MeshPtr Clone( String const & p_name);
		/**
		 * Initialises the face normals
		 */
		void ComputeFlatNormals();
		/**
		 * Initialises the vertex normals
		 */
		void ComputeSmoothNormals();
		/**
		 * Initialises all the normals
		 *@param p_bReverted : [in] Tells if the normals are reverted
		 */
		void ComputeNormals( bool p_bReverted = false);
		/**
		 * Initialises vertex and texture buffers
		 */
		void InitialiseBuffers();
		/**
		 * Fills the normals buffer of the given mode
		 *@param p_nm : [in] The wanted NormalsMode
		 */
		void SetNormals( eNORMALS_MODE p_nm);
		/**
		 * Subdivides a submesh, using the given method
		 *@param p_index : [in] The index of the submesh to subdivide
		 *@param p_mode : [in] The subdivision mode
		 */
		bool Subdivide( unsigned int p_index, SubdividerPtr p_pSubdivider, bool p_bThreaded = false);

		/**@name Accessors */
		//@{
		inline void SetModified		( bool p_modified)		{m_modified = p_modified;}

		inline bool					IsOk					()const { return m_bOK; }
		inline size_t				GetNbSubmeshes			()const { return m_submeshes.size(); }
		inline eMESH_TYPE			GetMeshType				()const	{ return m_pMeshCategory->GetMeshType();}
		inline bool					IsModified				()const	{ return m_modified;}
		inline const CubeBox	&	GetCubeBox				()const	{ return m_box; }
		inline const SphereBox	&	GetSphere				()const	{ return m_sphere; }
		inline CubeBox			&	GetCubeBox				()		{ return m_box; }
		inline SphereBox		&	GetSphere				()		{ return m_sphere; }
		//@}

	public:
		/**
		 * Generates the vertex list, used for defined primitives, for custom meshes, dummy.
		 */
		virtual void GeneratePoints(){}

	private:
		void _setVBsForFaceSmooth( FacePtr p_face, unsigned int & p_trianglesIndex);
		void _setVBsForFaceFlat( FacePtr p_face, unsigned int & p_trianglesIndex);
		DECLARE_SERIALISE_MAP()
		DECLARE_POST_UNSERIALISE()
	};
}

#endif

