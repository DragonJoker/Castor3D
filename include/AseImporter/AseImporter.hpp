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
#ifndef ___ASE___
#define ___ASE___

#include <Castor3D/Importer.hpp>

#ifndef _WIN32
#	define C3D_Ase_API
#else
#	ifdef AseImporter_EXPORTS
#		define C3D_Ase_API __declspec(dllexport)
#	else
#		define C3D_Ase_API __declspec(dllimport)
#	endif
#endif

namespace Castor3D
{
	static const String SCENE				= cuT( "*SCENE");
	static const String SCENE_NAME			= cuT( "*SCENE_FILENAME");

	static const String MATERIAL_LIST		= cuT( "*MATERIAL_LIST");
	static const String MATERIAL			= cuT( "*MATERIAL");
	static const String SUBMATERIAL			= cuT( "*SUBMATERIAL");
	static const String MATERIAL_COUNT		= cuT( "*MATERIAL_COUNT");
	static const String MATERIAL_NAME		= cuT( "*MATERIAL_NAME");
	static const String MATERIAL_DIFFUSE	= cuT( "*MATERIAL_DIFFUSE");
	static const String MATERIAL_AMBIENT	= cuT( "*MATERIAL_AMBIENT");
	static const String MATERIAL_SPECULAR	= cuT( "*MATERIAL_SPECULAR");
	static const String MATERIAL_SHININESS	= cuT( "*MATERIAL_SHINE");
	static const String TEXTURE				= cuT( "*BITMAP");
	static const String UTILE				= cuT( "*UVW_U_TILING");
	static const String VTILE				= cuT( "*UVW_V_TILING");
	static const String UOFFSET				= cuT( "*UVW_U_OFFSET");
	static const String VOFFSET				= cuT( "*UVW_V_OFFSET");

	static const String CAMERA				= cuT( "*CAMERAOBJECT");
	static const String CAMERA_SETTINGS		= cuT( "*CAMERA_SETTINGS");
	static const String CAMERA_NEAR			= cuT( "*CAMERA_NEAR");
	static const String CAMERA_FAR			= cuT( "*CAMERA_FAR");
	static const String CAMERA_FOV			= cuT( "*CAMERA_FOV");

	static const String OBJECT				= cuT( "*GEOMOBJECT");
	static const String MESH				= cuT( "*MESH");
	static const String NUM_VERTEX			= cuT( "*MESH_NUMVERTEX");
	static const String NUM_TVERTEX			= cuT( "*MESH_NUMTVERTEX");
	static const String NUM_FACES			= cuT( "*MESH_NUMFACES");
	static const String MESH_VERTEX_LIST	= cuT( "*MESH_VERTEX_LIST");
	static const String MESH_FACE_LIST		= cuT( "*MESH_FACE_LIST");
	static const String MESH_TVERTLIST		= cuT( "*MESH_TVERTLIST");
	static const String MESH_TFACELIST		= cuT( "*MESH_TFACELIST");
	static const String VERTEX				= cuT( "*MESH_VERTEX");
	static const String FACE				= cuT( "*MESH_FACE");
	static const String TVERTEX				= cuT( "*MESH_TVERT");
	static const String TFACE				= cuT( "*MESH_TFACE");
	static const String NORMALS				= cuT( "*MESH_NORMALS");
	static const String FACE_NORMAL			= cuT( "*MESH_FACENORMAL");
	static const String VERTEX_NORMAL		= cuT( "*MESH_VERTEXNORMAL");
	static const String MESH_MAPPINGCHANNEL	= cuT( "*MESH_MAPPINGCHANNEL");
	static const String MATERIAL_ID			= cuT( "*MATERIAL_REF");

	static const String NODE				= cuT( "*NODE_TM");
	static const String NODE_NAME			= cuT( "*NODE_NAME");
	static const String NODE_POSITION		= cuT( "*TM_POS");
	static const String NODE_AXIS			= cuT( "*TM_ROTAXIS");
	static const String NODE_ANGLE			= cuT( "*TM_ROTANGLE");
	static const String NODE_SCALE			= cuT( "*TM_SCALE");

	static const String LIGHT				= cuT( "*LIGHT_OBJECT");
	static const String LIGHT_TYPE			= cuT( "*LIGHT_TYPE");
	static const String LIGHT_SETTINGS		= cuT( "*LIGHT_SETTINGS");
	static const String LIGHT_COLOR			= cuT( "*LIGHT_COLOR");

	static const String ENDER				= cuT( "}");

	//! ASE file importer
	/*!
	Imports data from ASE (ASCII Scene Export) files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class AseImporter : public Importer, public MemoryTraced<AseImporter>
	{
	private:
		friend class Scene;

		File * m_pFile;
		String m_currentWord;
		Point3rPtrArray m_texCoords;
		MaterialPtrUIntMap m_materials;
		SceneNodePtrStrMap m_nodes;

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
		SceneNodePtr _readNodeInfos( String const & p_name);
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
