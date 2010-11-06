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
#ifndef ___C3D_Scene___
#define ___C3D_Scene___

#include "Module_Scene.h"
#include "../geometry/Module_Geometry.h"
#include "../light/Light.h"
#include "../geometry/mesh/Mesh.h"
#include "../camera/Viewport.h"

namespace Castor3D
{
	//! Scene handler class
	/*!
	A scene is a collection of lights, scene nodes and geometries. It has at least one camera to render it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Scene
	{
	private:
		String m_name;									//!< The scene name
		SceneNodePtr m_rootNode;						//!< The root scene node, each node is attached at the root node if not attached to another one
		CameraPtr m_rootCamera;							//!< The root camera, necessary for any render

		LightPtrStrMap m_addedLights;					//!< The lights map
		SceneNodePtrStrMap m_addedNodes;				//!< The nodes map
		GeometryPtrStrMap m_addedPrimitives;			//!< The geometries map
		CameraPtrStrMap m_addedCameras;					//!< The other cameras

		LightPtrArray m_arrayLightsToDelete;			//!< The lights to delete array
		SceneNodePtrArray m_arrayNodesToDelete;			//!< The nodes to delete array
		GeometryPtrArray m_arrayPrimitivesToDelete;		//!< The geometries to delete array
		CameraPtrArray m_arrayCamerasToDelete;			//!< The cameras to delete array

		GeometryPtrStrMap m_newlyAddedPrimitives;		//!< The newly added geometries, it is used to make the vertex buffer of them, then they are removed from the map.

		size_t m_nbFaces;								//!< The number of faces in the scene
		size_t m_nbVertex;								//!< The number of vertexes in the scene

		bool m_changed;									//!< Tells if the scene has changed, id est if a geometry has been created or added to it => Vertex buffers need to be generated

		NormalsMode m_normalsMode;

		GeometryPtr m_selectedGeometry;

		Colour m_clAmbientLight;

		Castor::MultiThreading::RecursiveMutex m_mutex;			//!< The mutex, to make the Scene threadsafe

	public:
		/**
		 * Constructor
		 *@param p_name : [in] The scene name
		 */
		Scene( const String & p_name);
		/**
		 * Destructor
		 */
		~Scene();
		/**
		 * Clears the maps, leaves the root node and the root camera
		 */
		void ClearScene();
		/**
		 * Renders the scene in a given display mode
		 *@param p_displayMode : [in] The mode in which the display must be made
		 *@param p_tslf : [in] The time elapsed since the last frame was rendered
		 */
		void Render( eDRAW_TYPE p_displayMode, real p_tslf);
		/**
		 * Creates a scene node in the scene, attached to the root node if th given parent is NULL
		 *@param p_name : [in] The node name, default is empty
		 *@param p_parent : [in] The parent node, if NULL, the created node will be attached to root
		 */
		SceneNodePtr CreateSceneNode( const String & p_name, SceneNodePtr p_parent = NULL);
		/**
		 * Creates a primitive, given a MeshType and the primitive definitions
		 *@param p_name : [in] The primitive name
		 *@param p_type : [in] The primitive mesh type (plane, cube, torus...)
		 *@param p_meshName : [in] The mesh name, creates a new mesh if it doesn't exist
		 *@param p_faces : [in] The faces numbers
		 *@param p_size : [in] The geometry dimensions
		 */
		GeometryPtr CreatePrimitive( const String & p_name, Mesh::eTYPE p_type,
									const String & p_meshName, UIntArray p_faces,
									FloatArray p_size);
		/**
		 * Creates a camera
		 *@param p_name : [in] The camera name
		 *@param p_ww, p_wh : [in] The window size
		 *@param p_type : [in] The viewport projection type
		 */
		Camera * CreateCamera( const String & p_name, int p_ww, int p_wh,
							   Viewport::eTYPE p_type);
		/**
		* Creates a light
		*@param p_type : [in] The light type
		*@param p_name : [in] The light name
		 */
		LightPtr CreateLight( Light::eTYPE p_type, const String & p_name);
		/**
		 * Creates the vertex buffers in a given normals mode, and tells if the face's or vertex's normals are shown
		 *@param p_nm : [in] The normals mode (face or vertex)
		 *@param p_showNormals : [in] Whether or not to show the face or vertex normals
		 */
		void CreateList( NormalsMode p_nm, bool p_showNormals);
		/**
		 * Retrieves the node with the given name
		 *@param p_name : [in] The name of the node
		 *@return The named node, NULL if not found
		 */
		SceneNodePtr GetNode( const String & p_name)const;
		/**
		 * Adds a node to the scene
		 *@param p_node : [in] The node to add
		 */
		void AddNode( SceneNodePtr p_node);
		/**
		 * Adds a light to the scene
		 *@param p_light : [in] The light to add
		 */
		void AddLight( LightPtr p_light);
		/**
		 * Adds a geometry to the scene
		 *@param p_geometry : [in] The geometry to add
		 */
		void AddGeometry( GeometryPtr p_geometry);
		/**
		 * Retrieves the geometry with the given name
		 *@param p_name : [in] the name of the geometry
		 *@return The named geometry, NULL if not found
		 */
		GeometryPtr GetGeometry( const String & p_name);
		/**
		 * Removes the light given in argument from the scene and deletes it
		 *@param p_pLight : [in] The light to remove
		 */
		void RemoveLight( LightPtr p_pLight);
		/**
		 * Removes the node given in argument from the scene and deletes it
		 *@param p_pNode : [in] The node to remove
		 */
		void RemoveNode( SceneNodePtr p_pNode);
		/**
		 * Removes the geometry given in argument from the scene and deletes it
		 *@param p_pGeometry : [in] The geometry to remove
		 */
		void RemoveGeometry( GeometryPtr p_pGeometry);
		/**
		 * Removes the camera given in argument from the scene and deletes it
		 *@param p_pCamera : [in] The camera to remove
		 */
		void RemoveCamera( CameraPtr p_pCamera);
		/**
		 * Removes all the lights from the scene
		 */
		void RemoveAllLights();
		/**
		 * Removes all the nodes from the scene
		 */
		void RemoveAllNodes();
		/**
		 * Removes all the geometries from the scene
		 */
		void RemoveAllGeometries();
		/**
		 * Removes all the cameras from the scene
		 */
		void RemoveAllCameras();
		/**
		 * Retrieves a map of visibility of the geometries, sorted by geometry name
		 *@return The visibility map
		 */
		BoolStrMap GetGeometriesVisibility();
		/**
		 * Writes the scene in a file
		 *@param p_pFile : [in] file to write in
		 *@return true if successful, false if not
		 */
		bool Write( Castor::Utils::File & p_pFile)const;
		/**
		 * Reads the scene from a file
		 *@param p_file : [in] file to read from
		 *@return true if successful, false if not
		 */
		bool Read( Castor::Utils::File & p_file);
		/**
		* Reads the scene from a 3DS file
		*@param p_file : [in] file to read from
		*@return true if successful, false if not
		*/
		bool Import3DS( const String & p_file);
		/**
		* Reads the scene from a Obj file
		*@param p_file : [in] file to read from
		*@return true if successful, false if not
		*/
		bool ImportObj( const String & p_file);
		/**
		* Reads the scene from a MD2 (Quake 2 Model) file
		*@param p_file : [in] file to read from
		*@param p_texName : [in] The texture name
		*@return true if successful, false if not
		*/
		bool ImportMD2( const String & p_file, const String & p_texName=C3DEmptyString);
		/**
		* Reads the scene from a MD3 (Quake 3 Model) file
		*@param p_file : [in] file to read from
		*@return true if successful, false if not
		*/
		bool ImportMD3( const String & p_file);
		/**
		* Reads the scene from a ASE (ASCII Scene Export) file
		*@param p_file : [in] file to read from
		*@return true if successful, false if not
		*/
		bool ImportASE( const String & p_file);
		/**
		* Reads the scene from a PLY () file
		*@param p_file : [in] file to read from
		*@return true if successful, false if not
		*/
		bool ImportPLY( const String & p_file);
		/**
		* Reads the scene from a BSP (Quake 3 Map) file
		*@param p_file : [in] file to read from
		*@return true if successful, false if not
		*/
		bool ImportBSP( const String & p_file);
		/**
		 * Selects the nearest element in the ray's way
		 *@param p_ray : [in] The ray
		 *@param p_geo : [out] The nearest met geometry
		 *@param p_submesh : [out] The nearest met submesh
		 *@param p_face : [out] The nearest met Face
		 *@param p_vertex : [out] The nearest met Vertex
		 */
		void Select( Ray * p_ray, GeometryPtr * p_geo, SubmeshPtr * p_submesh, FacePtr * p_face, Point3rPtr * p_vertex);

		void Merge( ScenePtr p_pScene);

	private:
		bool _writeLights( Castor::Utils::File & p_pFile)const;
		bool _writeGeometries( Castor::Utils::File & p_pFile)const;
		bool _importExternal( const String & p_fileName, ExternalImporterPtr p_importer);
		void _deleteToDelete();

	public:
		/**
		 * @return The scene name
		 */
		inline String			GetName			()const { return m_name; }
		/**
		 * @return The root node
		 */
		inline SceneNodePtr		GetRootNode		()const { return m_rootNode; }
		/**
		 * @return The root camera
		 */
		inline CameraPtr		GetRootCamera	()const { return m_rootCamera; }
		/**
		 * @return The geometries number
		 */
		inline size_t			GetNbGeometries	()const { return m_addedPrimitives.size(); }
		/**
		 * @return Tells if the name has changed
		 */
		inline bool				HasChanged		()const { return m_changed; }
		/**
		 * @return The lights
		 */
		inline LightPtrStrMap						GetLights				()const { return m_addedLights; }
		inline Colour								GetAmbientLight			()const	{ return m_clAmbientLight; }
		inline SceneNodePtrStrMap::iterator			GetNodesIterator		()		{ return m_addedNodes.begin(); }
		inline SceneNodePtrStrMap::const_iterator	GetNodesEnd				()		{ return m_addedNodes.end(); }
		inline LightPtrStrMap::iterator				GetLightsIterator		()		{ return m_addedLights.begin(); }
		inline LightPtrStrMap::const_iterator		GetLightsEnd			()		{ return m_addedLights.end(); }
		inline GeometryPtrStrMap::iterator			GetGeometriesIterator	()		{ return m_addedPrimitives.begin(); }
		inline GeometryPtrStrMap::const_iterator	GetGeometriesEnd		()		{ return m_addedPrimitives.end(); }

		inline void SetAmbientLight( const Colour & val) { m_clAmbientLight = val; }
	};

}

#endif

