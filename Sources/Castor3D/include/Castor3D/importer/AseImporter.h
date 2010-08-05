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
#ifndef ___ASE___
#define ___ASE___

#include "ExternalImporter.h"
/*
static const String SCENE				= "*SCENE";
static const String SCENE_NAME			= "*SCENE_FILENAME";

static const String MATERIAL_LIST		= "*MATERIAL_LIST";
static const String MATERIAL			= "*MATERIAL";
static const String SUBMATERIAL			= "*SUBMATERIAL";
static const String MATERIAL_COUNT      = "*MATERIAL_COUNT";
static const String MATERIAL_NAME		= "*MATERIAL_NAME";
static const String MATERIAL_DIFFUSE	= "*MATERIAL_DIFFUSE";
static const String MATERIAL_AMBIENT	= "*MATERIAL_AMBIENT";
static const String MATERIAL_SPECULAR	= "*MATERIAL_SPECULAR";
static const String MATERIAL_SHININESS	= "*MATERIAL_SHINE";
static const String TEXTURE				= "*BITMAP";
static const String UTILE				= "*UVW_U_TILING";
static const String VTILE				= "*UVW_V_TILING";
static const String UOFFSET				= "*UVW_U_OFFSET";
static const String VOFFSET				= "*UVW_V_OFFSET";

static const String CAMERA				= "*CAMERAOBJECT";
static const String CAMERA_SETTINGS		= "*CAMERA_SETTINGS";
static const String CAMERA_NEAR			= "*CAMERA_NEAR";
static const String CAMERA_FAR			= "*CAMERA_FAR";
static const String CAMERA_FOV			= "*CAMERA_FOV";

static const String OBJECT				= "*GEOMOBJECT";
static const String MESH				= "*MESH";
static const String MESH_VERTEX_LIST	= "*MESH_VERTEX_LIST";
static const String MESH_FACE_LIST		= "*MESH_FACE_LIST";
static const String MESH_TVERTLIST		= "*MESH_TVERTLIST";
static const String MESH_TFACELIST		= "*MESH_TFACELIST";
static const String VERTEX				= "*MESH_VERTEX";
static const String FACE				= "*MESH_FACE";
static const String TVERTEX				= "*MESH_TVERT";
static const String TFACE				= "*MESH_TFACE";
static const String NORMALS				= "*MESH_NORMALS";
static const String FACE_NORMAL			= "*MESH_FACENORMAL";
static const String VERTEX_NORMAL		= "*MESH_VERTEXNORMAL";
static const String MESH_MAPPINGCHANNEL	= "*MESH_MAPPINGCHANNEL";
static const String MATERIAL_ID			= "*MATERIAL_REF";

static const String NODE				= "*NODE_TM";
static const String NODE_NAME			= "*NODE_NAME";
static const String NODE_POSITION		= "*TM_POS";
static const String NODE_AXIS			= "*TM_ROTAXIS";
static const String NODE_ANGLE			= "*TM_ROTANGLE";
static const String NODE_SCALE			= "*TM_SCALE";

static const String LIGHT				= "*LIGHT_OBJECT";
static const String LIGHT_TYPE			= "*LIGHT_TYPE";
static const String LIGHT_SETTINGS		= "*LIGHT_SETTINGS";
static const String LIGHT_COLOR			= "*LIGHT_COLOR";
*/

static const Char * SCENE				= C3D_T( "*SCENE");
static const Char * SCENE_NAME			= C3D_T( "*SCENE_FILENAME");

static const Char * MATERIAL_LIST		= C3D_T( "*MATERIAL_LIST");
static const Char * MATERIAL			= C3D_T( "*MATERIAL");
static const Char * SUBMATERIAL			= C3D_T( "*SUBMATERIAL");
static const Char * MATERIAL_COUNT		= C3D_T( "*MATERIAL_COUNT");
static const Char * MATERIAL_NAME		= C3D_T( "*MATERIAL_NAME");
static const Char * MATERIAL_DIFFUSE	= C3D_T( "*MATERIAL_DIFFUSE");
static const Char * MATERIAL_AMBIENT	= C3D_T( "*MATERIAL_AMBIENT");
static const Char * MATERIAL_SPECULAR	= C3D_T( "*MATERIAL_SPECULAR");
static const Char * MATERIAL_SHININESS	= C3D_T( "*MATERIAL_SHINE");
static const Char * TEXTURE				= C3D_T( "*BITMAP");
static const Char * UTILE				= C3D_T( "*UVW_U_TILING");
static const Char * VTILE				= C3D_T( "*UVW_V_TILING");
static const Char * UOFFSET				= C3D_T( "*UVW_U_OFFSET");
static const Char * VOFFSET				= C3D_T( "*UVW_V_OFFSET");

static const Char * CAMERA				= C3D_T( "*CAMERAOBJECT");
static const Char * CAMERA_SETTINGS		= C3D_T( "*CAMERA_SETTINGS");
static const Char * CAMERA_NEAR			= C3D_T( "*CAMERA_NEAR");
static const Char * CAMERA_FAR			= C3D_T( "*CAMERA_FAR");
static const Char * CAMERA_FOV			= C3D_T( "*CAMERA_FOV");

static const Char * OBJECT				= C3D_T( "*GEOMOBJECT");
static const Char * MESH				= C3D_T( "*MESH");
static const Char * NUM_VERTEX			= C3D_T( "*MESH_NUMVERTEX");
static const Char * NUM_TVERTEX			= C3D_T( "*MESH_NUMTVERTEX");
static const Char * NUM_FACES			= C3D_T( "*MESH_NUMFACES");
static const Char * MESH_VERTEX_LIST	= C3D_T( "*MESH_VERTEX_LIST");
static const Char * MESH_FACE_LIST		= C3D_T( "*MESH_FACE_LIST");
static const Char * MESH_TVERTLIST		= C3D_T( "*MESH_TVERTLIST");
static const Char * MESH_TFACELIST		= C3D_T( "*MESH_TFACELIST");
static const Char * VERTEX				= C3D_T( "*MESH_VERTEX");
static const Char * FACE				= C3D_T( "*MESH_FACE");
static const Char * TVERTEX				= C3D_T( "*MESH_TVERT");
static const Char * TFACE				= C3D_T( "*MESH_TFACE");
static const Char * NORMALS				= C3D_T( "*MESH_NORMALS");
static const Char * FACE_NORMAL			= C3D_T( "*MESH_FACENORMAL");
static const Char * VERTEX_NORMAL		= C3D_T( "*MESH_VERTEXNORMAL");
static const Char * MESH_MAPPINGCHANNEL	= C3D_T( "*MESH_MAPPINGCHANNEL");
static const Char * MATERIAL_ID			= C3D_T( "*MATERIAL_REF");

static const Char * NODE				= C3D_T( "*NODE_TM");
static const Char * NODE_NAME			= C3D_T( "*NODE_NAME");
static const Char * NODE_POSITION		= C3D_T( "*TM_POS");
static const Char * NODE_AXIS			= C3D_T( "*TM_ROTAXIS");
static const Char * NODE_ANGLE			= C3D_T( "*TM_ROTANGLE");
static const Char * NODE_SCALE			= C3D_T( "*TM_SCALE");

static const Char * LIGHT				= C3D_T( "*LIGHT_OBJECT");
static const Char * LIGHT_TYPE			= C3D_T( "*LIGHT_TYPE");
static const Char * LIGHT_SETTINGS		= C3D_T( "*LIGHT_SETTINGS");
static const Char * LIGHT_COLOR			= C3D_T( "*LIGHT_COLOR");

static const Char * ENDER				= C3D_T( "}");


namespace Castor3D
{
	class CS3D_API AseImporter : public ExternalImporter
	{
	private:
		friend class Scene;

		File * m_pFile;
		String m_currentWord;
		Vector3fPtrArray m_texCoords;
		std::map <size_t, Material *> m_materials;
		std::map <String, SceneNode *> m_nodes;
/*
		std::ifstream * m_fileStream;
		SceneNodePtrArray m_nodes;
*/

	private:
		virtual bool _import();

	private:
		void _readAseFile();
		void _readSceneInfos();
		Material * _readMaterialInfos();
		void _readObjectInfos();
		void _readLightInfos();
		void _readCameraInfos();
		void _readCameraSettings( Camera * p_camera);
		SceneNode * _readNodeInfos( const String & p_name);
		void _readMeshInfos( Mesh * p_mesh);
		void _readVertexList( Submesh * p_submesh);
		void _readFaceList( Submesh * p_submesh);
		void _readTexCoordsList( Submesh * p_submesh);
		void _readTexFaceList( Submesh * p_submesh);
		void _readMeshNormals( Submesh * p_submesh);

		void _readMeshMappingChannel( Submesh * p_submesh);

		String _retrieveString();
		void _retrieveVertex( float * p_position);
		void _retrieveFace( int * p_indices);
		void _retrieveTextureVertex( float * p_position);
		void _retrieveTextureFace( int * p_indices);

/*
	private:
		void _readAseFile( Imported3DModel * p_model);
		int _getObjectCount();
		int _getNodeCount();
		int _getMaterialCount();
		void _getTextureInfo( ImportedMaterialInfo * p_texture, int p_desiredMaterial);
		void _moveToObject( int p_desiredObject);
		float _readFloat();
		void _readObjectInfo( Imported3DObject * p_object, int p_desiredObject);
		void _getTextureName( ImportedMaterialInfo * p_texture);
		void _getMaterialName( ImportedMaterialInfo * p_texture);
		void _readObjectData( Imported3DModel * p_model, Imported3DObject * p_object, int p_desiredObject);
		void _getNodeInfo( SceneNode * p_node, int p_desiredNode);
		void _getData( Imported3DModel * p_model, Imported3DObject * p_object, char * p_strDesiredData, int p_desiredObject);
		void _readVertex( Imported3DObject * p_object);
		void _readTextureVertex( Imported3DObject * p_object, ImportedMaterialInfo p_texture);
		void _readFace( Imported3DObject * p_object);
		void _readTextureFace( Imported3DObject * p_object);
*/
	};
}

#endif