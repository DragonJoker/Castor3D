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

		FileIO * m_pFile;
		String m_currentWord;
		Vector3fPtrArray m_texCoords;
		std::map <size_t, Material *> m_materials;
		std::map <String, SceneNode *> m_nodes;

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
	};
}

#endif