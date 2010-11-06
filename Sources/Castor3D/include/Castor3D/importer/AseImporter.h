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
#ifndef ___ASE___
#define ___ASE___

#include "ExternalImporter.h"
#include "../scene/Module_Scene.h"
#include "../material/Module_Material.h"

static const Char * SCENE				= CU_T( "*SCENE");
static const Char * SCENE_NAME			= CU_T( "*SCENE_FILENAME");

static const Char * MATERIAL_LIST		= CU_T( "*MATERIAL_LIST");
static const Char * MATERIAL			= CU_T( "*MATERIAL");
static const Char * SUBMATERIAL			= CU_T( "*SUBMATERIAL");
static const Char * MATERIAL_COUNT		= CU_T( "*MATERIAL_COUNT");
static const Char * MATERIAL_NAME		= CU_T( "*MATERIAL_NAME");
static const Char * MATERIAL_DIFFUSE	= CU_T( "*MATERIAL_DIFFUSE");
static const Char * MATERIAL_AMBIENT	= CU_T( "*MATERIAL_AMBIENT");
static const Char * MATERIAL_SPECULAR	= CU_T( "*MATERIAL_SPECULAR");
static const Char * MATERIAL_SHININESS	= CU_T( "*MATERIAL_SHINE");
static const Char * TEXTURE				= CU_T( "*BITMAP");
static const Char * UTILE				= CU_T( "*UVW_U_TILING");
static const Char * VTILE				= CU_T( "*UVW_V_TILING");
static const Char * UOFFSET				= CU_T( "*UVW_U_OFFSET");
static const Char * VOFFSET				= CU_T( "*UVW_V_OFFSET");

static const Char * CAMERA				= CU_T( "*CAMERAOBJECT");
static const Char * CAMERA_SETTINGS		= CU_T( "*CAMERA_SETTINGS");
static const Char * CAMERA_NEAR			= CU_T( "*CAMERA_NEAR");
static const Char * CAMERA_FAR			= CU_T( "*CAMERA_FAR");
static const Char * CAMERA_FOV			= CU_T( "*CAMERA_FOV");

static const Char * OBJECT				= CU_T( "*GEOMOBJECT");
static const Char * MESH				= CU_T( "*MESH");
static const Char * NUM_VERTEX			= CU_T( "*MESH_NUMVERTEX");
static const Char * NUM_TVERTEX			= CU_T( "*MESH_NUMTVERTEX");
static const Char * NUM_FACES			= CU_T( "*MESH_NUMFACES");
static const Char * MESH_VERTEX_LIST	= CU_T( "*MESH_VERTEX_LIST");
static const Char * MESH_FACE_LIST		= CU_T( "*MESH_FACE_LIST");
static const Char * MESH_TVERTLIST		= CU_T( "*MESH_TVERTLIST");
static const Char * MESH_TFACELIST		= CU_T( "*MESH_TFACELIST");
static const Char * VERTEX				= CU_T( "*MESH_VERTEX");
static const Char * FACE				= CU_T( "*MESH_FACE");
static const Char * TVERTEX				= CU_T( "*MESH_TVERT");
static const Char * TFACE				= CU_T( "*MESH_TFACE");
static const Char * NORMALS				= CU_T( "*MESH_NORMALS");
static const Char * FACE_NORMAL			= CU_T( "*MESH_FACENORMAL");
static const Char * VERTEX_NORMAL		= CU_T( "*MESH_VERTEXNORMAL");
static const Char * MESH_MAPPINGCHANNEL	= CU_T( "*MESH_MAPPINGCHANNEL");
static const Char * MATERIAL_ID			= CU_T( "*MATERIAL_REF");

static const Char * NODE				= CU_T( "*NODE_TM");
static const Char * NODE_NAME			= CU_T( "*NODE_NAME");
static const Char * NODE_POSITION		= CU_T( "*TM_POS");
static const Char * NODE_AXIS			= CU_T( "*TM_ROTAXIS");
static const Char * NODE_ANGLE			= CU_T( "*TM_ROTANGLE");
static const Char * NODE_SCALE			= CU_T( "*TM_SCALE");

static const Char * LIGHT				= CU_T( "*LIGHT_OBJECT");
static const Char * LIGHT_TYPE			= CU_T( "*LIGHT_TYPE");
static const Char * LIGHT_SETTINGS		= CU_T( "*LIGHT_SETTINGS");
static const Char * LIGHT_COLOR			= CU_T( "*LIGHT_COLOR");

static const Char * ENDER				= CU_T( "}");


namespace Castor3D
{
	//! ASE file importer
	/*!
	Imports data from ASE (ASCII Scene Export) files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class CS3D_API AseImporter : public ExternalImporter
	{
	private:
		friend class Scene;

		File * m_pFile;
		String m_currentWord;
		Point3rPtrArray m_texCoords;
		MaterialPtrUIntMap m_materials;
		SceneNodePtrStrMap m_nodes;

	private:
		virtual bool _import();

	private:
		void _readAseFile();
		void _readSceneInfos();
		MaterialPtr _readMaterialInfos();
		void _readObjectInfos();
		void _readLightInfos();
		void _readCameraInfos();
		void _readCameraSettings( Camera * p_camera);
		SceneNodePtr _readNodeInfos( const String & p_name);
		void _readMeshInfos( MeshPtr p_mesh);
		void _readVertexList( SubmeshPtr p_submesh);
		void _readFaceList( SubmeshPtr p_submesh);
		void _readTexCoordsList( SubmeshPtr p_submesh);
		void _readTexFaceList( SubmeshPtr p_submesh);
		void _readMeshNormals( SubmeshPtr p_submesh);

		void _readMeshMappingChannel( SubmeshPtr p_submesh);

		String _retrieveString();
		void _retrieveVertex( real * p_position);
		void _retrieveFace( int * p_indices);
		void _retrieveTextureVertex( real * p_position);
		void _retrieveTextureFace( int * p_indices);
	};
}

#endif