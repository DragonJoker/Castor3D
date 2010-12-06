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

namespace Castor3D
{
	static const String SCENE				= "*SCENE";
	static const String SCENE_NAME			= "*SCENE_FILENAME";

	static const String MATERIAL_LIST		= "*MATERIAL_LIST";
	static const String MATERIAL			= "*MATERIAL";
	static const String SUBMATERIAL			= "*SUBMATERIAL";
	static const String MATERIAL_COUNT		= "*MATERIAL_COUNT";
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
	static const String NUM_VERTEX			= "*MESH_NUMVERTEX";
	static const String NUM_TVERTEX			= "*MESH_NUMTVERTEX";
	static const String NUM_FACES			= "*MESH_NUMFACES";
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

	static const String ENDER				= "}";

	//! ASE file importer
	/*!
	Imports data from ASE (ASCII Scene Export) files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API AseImporter : public ExternalImporter, public MemoryTraced<AseImporter>
	{
	private:
		friend class Scene;

		File * m_pFile;
		String m_currentWord;
		Point3rPtrArray m_texCoords;
		MaterialPtrUIntMap m_materials;
		NodePtrStrMap m_nodes;

	public:
		AseImporter();
		~AseImporter();

	private:
		virtual bool _import();

	private:
		void _readAseFile();
		void _readSceneInfos();
		MaterialPtr _readMaterialInfos();
		void _readObjectInfos();
		void _readLightInfos();
		void _readCameraInfos();
		void _readCameraSettings( CameraPtr p_camera);
		GeometryNodePtr _readNodeInfos( const String & p_name);
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