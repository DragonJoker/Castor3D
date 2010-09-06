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
#ifndef ___C3D_Mesh___
#define ___C3D_Mesh___

namespace Castor3D
{
	//! Mesh resource loader
	/*!
	Loads and saves meshes from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API MeshLoader : General::Resource::ResourceLoader <Mesh>
	{
	public:
		Mesh * LoadFromFileIO( const String & p_file);
		Mesh * LoadFromExtFileIO( const String & p_file);
		bool SaveToFileIO( const String & p_file, Mesh * p_mesh);

	protected:
		Mesh * _loadFrom3DS( const String & p_file);
		Mesh * _loadFromAse( const String & p_file);
		Mesh * _loadFromObj( const String & p_file);
		Mesh * _loadFromPly( const String & p_file);
		Mesh * _loadFromMd2( const String & p_file);
		Mesh * _loadFromMd3( const String & p_file);
	};
	//! The mesh representation
	/*!
	A mesh is a collection of submeshes. It has a defined mesh type (torus, custom...) and a name.
	If an attempt to create a mesh with the same name, you retrieve this one.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API Mesh : General::Resource::Resource
	{
	public:
		//! The mesh types enumerator
		/*!
		Actually, there are 8 mesh types defined : custom, cone, cylinder, sphere, cube, torus, plane and icosaedron
		*/
		typedef enum eTYPE
		{
			eCustom,		//!< Custom mesh type => User defined vertex...
			eCone,			//!< Cone mesh type
			eCylinder,		//!< Cylinder mesh type
			eSphere,		//!< Rectangular faces sphere mesh type
			eCube,			//!< Cube mesh type
			eTorus,			//!< Torus mesh type
			ePlane,			//!< Plane mesh type
			eIcosaedron,	//!< Triangular faces sphere mesh type
			eProjection,	//!< Projection mesh type
		} eTYPE;

	protected:
		friend class MeshManager;
		friend class MeshLoader;

		String m_name;							//!< The mesh name
		eTYPE m_meshType;						//!< The mesh type
		bool m_modified;						//!< Tells whether or not the mesh is modified

		ComboBox * m_box;						//!< The combo box container
		Sphere * m_sphere;						//!< The sphere container
		SubdivisionMode m_preferredSubdivMode;	//!< The mesh's preferred subdivision mode, default is smTriangle, children classes should set it

		SubmeshPtrArray m_submeshes;			//!< The submeshes array
		NormalsMode m_normalsMode;
		bool m_bOK;
		/**
		 * Constructor, only MeshManager can create a mesh
		 *@param p_name : [in] This mesh name
		 */
		Mesh( const String & p_name=C3DEmptyString);

	public :
		/**
		 * Destructor, only MeshManager can destroy a mesh
		 */
		~Mesh();
		/**
		 * Cleans up all this mesh's submeshes
		 */
		void Cleanup();
		/**
		 * Computes the combo box
		 */
		void ComputeContainers();
		/**
		 * Returns the total number of faces of this submesh
		 *@return The faces number
		 */
		size_t GetNbFaces()const;
		/**
		 * Returns the total number of vertex of this submesh
		 *@return The vertex number
		 */
		size_t GetNbVertex()const;
		/**
		* Tries to retrieve the submesh at the given index
		*@param p_index : [in] The wanted submesh index
		*@return The found submesh, NULL if not found
		*/
		Submesh * GetSubmesh( unsigned int p_index);
		/**
		* Creates a submesh
		*@param p_nbSmoothgroups : [in] The wanted number of smoothgroups
		*@return The created submesh, NULL if not found
		*/
		Submesh * CreateSubmesh( unsigned int p_nbSmoothgroups);
		/**
		 * Clones this mesh, with a new name
		 *@param p_name : [in] The name of the cloned mesh
		 *@return The cloned mesh
		 */
		Mesh * Clone( const String & p_name);
		/**
		 * Initialises the face normals
		 */
		void SetFlatNormals();
		/**
		 * Initialises the vertex normals
		 */
		void SetSmoothNormals();
		/**
		 * Initialises all the normals
		 *@param p_bReverted : [in] Tells if the normals are reverted
		 */
		void SetNormals( bool p_bReverted = false);
		/**
		 * Initialises vertex and texture buffers
		 */
		void CreateBuffers();
		/**
		 * Fills the normals buffer of the given mode
		 *@param p_nm : [in] The wanted NormalsMode
		 */
		void CreateNormalsBuffers( NormalsMode p_nm);
		/**
		 * Subdivides a submesh, using the given method
		 *@param p_index : [in] The index of the submesh to subdivide
		 *@param p_mode : [in] The subdivision mode
		 */
		virtual bool Subdivide( unsigned int p_index, SubdivisionMode p_mode);

	public:
		/**
		 * Generates the vertex list, used for defined primitives, for custom meshes, this is dummy
		 */
		virtual void GeneratePoints(){}

	private:
		void _setVBsForFaceSmooth( Face * p_face, unsigned int & p_trianglesIndex);
		void _setVBsForFaceFlat( Face * p_face, unsigned int & p_trianglesIndex);

	public:
		inline void SetModified		( bool p_modified)		{m_modified = p_modified;}
		inline void SetMeshType		( eTYPE p_type)			{m_meshType = p_type;}

		inline bool			IsOk					()const { return m_bOK; }
		inline size_t		GetNbSubmeshes			()const { return m_submeshes.size(); }
		inline eTYPE		GetMeshType				()const	{ return m_meshType;}
		inline bool			IsModified				()const	{ return m_modified;}
		inline String		GetName					()const	{ return m_name;}
		inline ComboBox *	GetComboBox				()const { return m_box; }
		inline Sphere *		GetSphere				()const { return m_sphere; }
	};
}

#endif

