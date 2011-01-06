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

#include "../Prerequisites.h"
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
	class C3D_API Scene : public Managed<String, Scene, SceneManager>, public MemoryTraced<Scene>
	{
	private:
		String m_name;									//!< The scene name
		CameraPtr m_rootCamera;							//!< The root camera, necessary for any render
		SceneNodePtr m_rootNode;						//!< The root node

		SceneNodePtrStrMap m_addedNodes;				//!< The nodes map
		CameraPtrStrMap m_addedCameras;					//!< The other cameras
		LightPtrStrMap m_addedLights;					//!< Added lights
		GeometryPtrStrMap m_addedPrimitives;			//!< Added primitives

		LightPtrArray m_arrayLightsToDelete;			//!< The lights to delete array
		SceneNodePtrArray m_arrayNodesToDelete;				//!< The nodes to delete array
		GeometryPtrArray m_arrayPrimitivesToDelete;		//!< The geometries to delete array
		CameraPtrArray m_arrayCamerasToDelete;			//!< The cameras to delete array

		GeometryPtrStrMap m_newlyAddedPrimitives;		//!< The newly added geometries, it is used to make the vertex buffer of them, then they are removed from the map.

		size_t m_nbFaces;								//!< The number of faces in the scene
		size_t m_nbVertex;								//!< The number of vertexes in the scene

		bool m_changed;									//!< Tells if the scene has changed, id est if a geometry has been created or added to it => Vertex buffers need to be generated

		eNORMALS_MODE m_normalsMode;

		GeometryPtr m_selectedGeometry;

		Colour m_clAmbientLight;

		Castor::MultiThreading::RecursiveMutex m_mutex;			//!< The mutex, to make the Scene threadsafe
		AnimationManager * m_pAnimationManager;

	public:
		/**
		 * Constructor
		 *@param p_name : [in] The scene name
		 */
		Scene( SceneManager * p_pManager, const String & p_name);
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
		SceneNodePtr CreateSceneNode( const String & p_name, SceneNodePtr p_parent=SceneNodePtr());
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
		 *@param p_pNode : [in] The camera's parent node
		 *@param p_type : [in] The viewport projection type
		 */
		CameraPtr CreateCamera( const String & p_name, int p_ww, int p_wh, SceneNodePtr p_pNode,
							    Viewport::eTYPE p_type);
		/**
		* Creates a light
		*@param p_type : [in] The light type
		*@param p_name : [in] The light name
		*@param p_pNode : [in] The light's parent node
		 */
		LightPtr CreateLight( Light::eTYPE p_type, const String & p_name, SceneNodePtr p_pNode);
		/**
		 * Creates the vertex buffers in a given normals mode, and tells if the face's or vertex's normals are shown
		 *@param p_nm : [in] The normals mode (face or vertex)
		 *@param p_showNormals : [in] Whether or not to show the face or vertex normals
		 */
		void CreateList( eNORMALS_MODE p_nm, bool p_showNormals);
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
		 *@param p_file : [in] file to write in
		 *@return true if successful, false if not
		 */
		bool Write( Castor::Utils::File & p_file)const;
		/**
		 * Reads the scene from a file
		 *@param p_file : [in] file to read from
		 *@return true if successful, false if not
		 */
		bool Read( Castor::Utils::File & p_file);
		/**
		* Imports a scene from an foreign file
		*@param p_fileName : [in] file to read from
		*@param p_importer : [in] The importer, which is in charge of loading the scene
		*@return true if successful, false if not
		*/
		bool ImportExternal( const String & p_fileName, Importer * p_importer);
		/**
		 * Selects the nearest element in the ray's way
		 *@param p_ray : [in] The ray
		 *@param p_geo : [out] The nearest met geometry
		 *@param p_submesh : [out] The nearest met submesh
		 *@param p_face : [out] The nearest met Face
		 *@param p_vertex : [out] The nearest met Vertex
		 */
		void Select( Ray * p_ray, GeometryPtr & p_geo, SubmeshPtr & p_submesh, FacePtr * p_face, Vertex * p_vertex);
		/**
		 * Merges the content of the given scene to this scene
		 *@param p_pScene : [in] The scene to merge into this one
		 */
		void Merge( ScenePtr p_pScene);

	private:
		bool _writeLights( Castor::Utils::File & p_pFile)const;
		bool _writeGeometries( Castor::Utils::File & p_pFile)const;
		void _deleteToDelete();

		template <typename MapType>
		void _merge( ScenePtr p_pScene, MapType & p_map, MapType & p_myMap)
		{
			String l_strName;

			for (MapType::iterator l_it = p_map.begin() ; l_it != p_map.end() ; ++l_it)
			{
				if (l_it->second->GetParent()->GetName() == CU_T( "RootNode"))
				{
					l_it->second->Detach();
					l_it->second->AttachTo( m_rootNode.get());
				}

				l_strName = l_it->first;

				while (m_addedNodes.find( l_strName) != m_addedNodes.end())
				{
					l_strName = p_pScene->GetName() + CU_T( "_") + l_strName;
				}

				l_it->second->SetName( l_strName);
				p_myMap.insert( MapType::value_type( l_strName, l_it->second));
			}

			p_map.clear();
		}

		template <typename MapType>
		void _removeNodesAndUnattached( MapType & p_map)
		{
			Container<String>::Set l_setNodes;

			for (MapType::iterator l_it = p_map.begin() ; l_it != p_map.end() ; ++l_it)
			{
				if (l_it->second->GetParent() != NULL && l_setNodes.find( l_it->second->GetParent()->GetName()) == l_setNodes.end())
				{
					l_setNodes.insert( Container<String>::Set::value_type( l_it->second->GetParent()->GetName()));
				}
			}

			for (SceneNodePtrStrMap::iterator l_it = m_addedNodes.begin() ; l_it != m_addedNodes.end() ; ++l_it)
			{
				if (l_setNodes.find( l_it->first) == l_setNodes.end())
				{
					RemoveNode( l_it->second);
					l_it = m_addedNodes.begin();
				}
			}

			for (MapType::iterator l_it = p_map.begin() ; l_it != p_map.end() ; ++l_it)
			{
				if (l_it->second->GetParent() == NULL)
				{
					p_map.erase( l_it);
					l_it = p_map.begin();
				}
			}
		}

	public:
		/**@name Accessors */
		//@{
		inline String									GetName						()const { return m_name; }
		inline SceneNodePtr								GetRootNode					()const { return m_rootNode; }
		inline CameraPtr								GetRootCamera				()const { return m_rootCamera; }
		inline size_t									GetNbGeometries				()const { return m_addedPrimitives.size(); }
		inline bool										HasChanged					()const { return m_changed; }
		inline LightPtrStrMap							GetLights					()const { return m_addedLights; }
		inline Colour									GetAmbientLight				()const	{ return m_clAmbientLight; }
		inline SceneNodePtrStrMap::iterator				GetNodesIterator			()		{ return m_addedNodes.begin(); }
		inline SceneNodePtrStrMap::const_iterator		GetNodesEnd					()		{ return m_addedNodes.end(); }
		inline LightPtrStrMap::iterator					GetLightsIterator			()		{ return m_addedLights.begin(); }
		inline LightPtrStrMap::const_iterator			GetLightsEnd				()		{ return m_addedLights.end(); }
		inline GeometryPtrStrMap::iterator				GetGeometriesIterator		()		{ return m_addedPrimitives.begin(); }
		inline GeometryPtrStrMap::const_iterator		GetGeometriesEnd			()		{ return m_addedPrimitives.end(); }

		inline void SetAmbientLight( const Colour & val) { m_clAmbientLight[0] = val[0];m_clAmbientLight[1] = val[1];m_clAmbientLight[2] = val[2];m_clAmbientLight[3] = val[3]; }
		//@}
	};

}

#endif

