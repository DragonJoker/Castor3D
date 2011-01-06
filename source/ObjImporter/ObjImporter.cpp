#include <Castor3D/Prerequisites.h>

using namespace Castor::Templates;

#include <Castor3D/material/MaterialManager.h>
#include <Castor3D/material/Material.h>
#include <Castor3D/material/Pass.h>
#include <Castor3D/material/TextureUnit.h>

#include <Castor3D/geometry/mesh/MeshManager.h>
#include <Castor3D/geometry/mesh/Mesh.h>
#include <Castor3D/geometry/mesh/Submesh.h>
#include <Castor3D/geometry/basic/Vertex.h>
#include <Castor3D/geometry/basic/SmoothingGroup.h>
#include <Castor3D/render_system/Buffer.h>
#include <Castor3D/geometry/primitives/Geometry.h>
#include <Castor3D/geometry/basic/Face.h>
#include <Castor3D/scene/SceneManager.h>
#include <Castor3D/scene/Scene.h>
#include <Castor3D/scene/SceneNode.h>
#include <Castor3D/main/Version.h>

#include "ObjImporter/ObjImporter.h"

using namespace Castor3D;

//*************************************************************************************************

extern "C" C3D_Obj_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version( 0, 6);
}

extern "C" C3D_Obj_API Importer * CreateImporter( SceneManager * p_pManager)
{
	Importer * l_pReturn( new ObjImporter( p_pManager));

	return l_pReturn;
}

//*************************************************************************************************

ObjImporter :: ObjImporter( SceneManager * p_pManager)
	:	Importer( p_pManager)
	,	m_pFile( NULL)
	,	m_objectHasUV( false)
	,	m_bReadingFaces( false)
	,	m_bReadingVertex( true)
	,	m_iNbTexCoords( 0)
	,	m_iGroup( -1)
{
}

bool ObjImporter :: _import()
{
	m_pFile = new File( m_fileName, File::eRead);

	m_pScene = m_pManager->GetElementByName( "MainScene");

	size_t l_uiSlashIndex = 0;
	size_t l_uiIndex = 0;

	if ((l_uiIndex = m_fileName.find_last_of( "\\")) != String::npos)
	{
		l_uiSlashIndex = l_uiIndex + 1;
	}

	if ((l_uiIndex = m_fileName.find_last_of( "/")) != String::npos)
	{
		l_uiSlashIndex = std::max( l_uiSlashIndex, l_uiIndex + 1);
	}

	size_t l_uiDotIndex = m_fileName.find_last_of( ".");

	UIntArray l_faces;
	FloatArray l_sizes;
	String l_name = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_meshName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_materialName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);

	if (m_pManager->GetMeshManager()->HasElement( l_meshName))
	{
		m_pMesh = m_pManager->GetMeshManager()->GetElementByName( l_meshName);
	}
	else
	{
		m_pMesh = m_pManager->GetMeshManager()->CreateMesh( l_meshName, l_faces, l_sizes, Mesh::eCustom);
		Logger::LogMessage( CU_T( "CreatePrimitive - Mesh ") + l_meshName + CU_T( " created"));
	}

	_readObjFile();

	bool l_bHasMaterial =  ! m_pCurrentMaterial == NULL;

	if ( ! l_bHasMaterial)
	{
		m_pCurrentMaterial = m_pManager->GetMaterialManager()->CreateMaterial( l_materialName);
	}

	for (size_t i = 0 ; i < m_pMesh->GetNbSubmeshes() ; i++)
	{
		m_pCurrentSubmesh = m_pMesh->GetSubmesh( i);

		m_pCurrentSubmesh->ComputeContainers();
		m_pCurrentSubmesh->GenerateBuffers();

		if ( ! l_bHasMaterial)
		{
			m_pCurrentSubmesh->SetMaterial( m_pCurrentMaterial);
		}
	}

	m_pMesh->ComputeNormals();

	SceneNodePtr l_pNode = m_pScene->CreateSceneNode( l_name);

	GeometryPtr l_pGeometry( new Geometry( m_pMesh, l_pNode, l_name));
	Logger::LogMessage( CU_T( "PlyImporter::_import - Geometry ") + l_name + CU_T( " created"));

	m_geometries.insert( GeometryPtrStrMap::value_type( l_name, l_pGeometry));

	delete m_pFile;

	return true;
}

void ObjImporter :: _createSubmesh()
{
	m_pCurrentSubmesh = m_pMesh->CreateSubmesh( 0);
}

void ObjImporter :: _readObjFile()
{
	String l_strSection;
	String l_strValue;
	String l_strLine;
	Char l_char = 0;

	while (m_pFile->IsOk())
	{
		m_pFile->ReadLine( l_strLine, 1000);

		if (l_strLine.size() >= 1)
		{
			l_strSection = l_strLine.substr( 0, l_strLine.find_first_of( ' '));
			l_strValue = l_strLine.substr( l_strLine.find_first_of( ' ') + 1, String::npos);

			if (l_strSection == CU_T( "mtllib"))
			{
				_readMatFile( l_strValue);
			}
			else if (l_strSection == CU_T( "usemtl"))
			{
				_applyMaterial( l_strValue);
			}
			else if (l_strSection == CU_T( "group"))
			{
				_selectSubmesh( l_strValue);
			}
			else
			{
				l_char = l_strSection.at( 0);
				String l_strLine = l_strSection + CU_T( " ") + l_strValue;

				switch( l_char)
				{
				case 'v':
					_readVertexInfo( l_strLine.substr( 1, String::npos));
				break;

				case 'g':
					_readSubmeshInfo( l_strLine);
				break;

				case 's':
					_readGroupInfo( l_strLine);
				break;

				case 'f':
					_readFaceInfo( l_strLine.substr( 1, String::npos));
				break;

				case '#':
//					Logger::LogMessage( l_strSection + " " + l_strValue);
				break;

				default:
					break;
				}
			}
		}

		if ( ! m_pFile->IsOk() && ! m_bReadingFaces)
		{
			delete m_pFile;
			m_bReadingVertex = false;
			m_bReadingFaces = true;

			m_pFile = new File( m_fileName, File::eRead);
		}
	}

//	Logger::LogMessage( CU_T( "LastLine : " + l_strSection + " " + l_strValue);
}

void ObjImporter :: _selectSubmesh( const String & p_strName)
{
	if (m_bReadingFaces)
	{
		if (m_mapSubmeshes.find( p_strName) != m_mapSubmeshes.end())
		{
			m_pCurrentSubmesh = m_mapSubmeshes.find( p_strName)->second;
		}
	}
}

void ObjImporter :: _applyMaterial( const String & p_strMaterialName)
{
	if (m_bReadingFaces)
	{
		if ( ! m_pCurrentSubmesh == NULL && m_pManager->GetMaterialManager()->HasElement( p_strMaterialName))
		{
			m_pCurrentSubmesh->SetMaterial( m_pManager->GetMaterialManager()->GetElementByName( p_strMaterialName));
		}
	}
}

void ObjImporter :: _readMatFile( const String & p_strFileName)
{
	if ( ! m_bReadingFaces)
	{
		m_pMatFile = new File( m_filePath + p_strFileName, File::eRead);

		String l_strLine;
		Char l_char = 0;

		while (m_pMatFile->IsOk())
		{
			m_pMatFile->ReadLine( l_strLine, 1000);

			if ( ! l_strLine.empty())
			{
				if (l_strLine.find( CU_T( "newmtl")) != String::npos)
				{
					StringArray l_arraySplitted = l_strLine.Split( CU_T( " "));

					if (l_arraySplitted.size() > 1)
					{
						if (m_pManager->GetMaterialManager()->HasElement( l_arraySplitted[1]))
						{
							m_pCurrentMaterial = m_pManager->GetMaterialManager()->GetElementByName( l_arraySplitted[1]);
						}
						else
						{
							m_pCurrentMaterial = m_pManager->GetMaterialManager()->CreateMaterial( l_arraySplitted[1]);
						}

						m_fAlpha = 1.0f;
					}
				}
				else if (l_strLine.find( CU_T( "illum")) != String::npos)
				{
				}
				else
				{
					l_char = l_strLine.at( 0);

					if (l_char == CU_T( 'K'))
					{
						_readMatLightComponent( l_strLine.substr( 1, String::npos));
					}
					else if (l_char == CU_T( 'T') && l_strLine.at( 1) == CU_T( 'r'))
					{
						_readMatTransparency( l_strLine.substr( 3, String::npos));
					}
					else if (l_char == CU_T( 'd'))
					{
						_readMatTransparency( l_strLine.substr( 2, String::npos));
					}
					else if (l_char == CU_T( 'N'))
					{
						_readMatLightRefDifExp( l_strLine.substr( 1, String::npos));
					}
				}
			}
		}

		delete m_pMatFile;
	}
}

void ObjImporter :: _readMatLightComponent( const String & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		String l_line;
		Char l_char = 0;
		Char l_cDump = 0;

		l_char = p_strLine.at( 0);
		l_line = p_strLine.substr( 2, String::npos);
		StringArray l_arraySplitted = l_line.Split( CU_T( " "));

		if (l_arraySplitted.size() >= 3)
		{
			float l_fR, l_fG, l_fB;
			l_fR = l_arraySplitted[0].ToFloat();
			l_fG = l_arraySplitted[1].ToFloat();
			l_fB = l_arraySplitted[2].ToFloat();

			if (l_char == CU_T( 'a'))
			{
				m_pCurrentMaterial->GetPass( 0)->SetAmbient( l_fR, l_fG, l_fB, m_fAlpha);
			}
			else if (l_char == CU_T( 'd'))
			{
				m_pCurrentMaterial->GetPass( 0)->SetDiffuse( l_fR, l_fG, l_fB, m_fAlpha);
			}
			else if (l_char == CU_T( 's'))
			{
				m_pCurrentMaterial->GetPass( 0)->SetSpecular( l_fR, l_fG, l_fB, m_fAlpha);
			}
		}
	}
}

void ObjImporter :: _readMatTransparency( const String & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		m_fAlpha = p_strLine.ToFloat();
		const float * l_pfAmbient = m_pCurrentMaterial->GetPass( 0)->GetAmbient().const_ptr();
		const float * l_pfDiffuse = m_pCurrentMaterial->GetPass( 0)->GetDiffuse().const_ptr();
		const float * l_pfSpecular = m_pCurrentMaterial->GetPass( 0)->GetSpecular().const_ptr();
		const float * l_pfemissive = m_pCurrentMaterial->GetPass( 0)->GetEmissive().const_ptr();
		m_pCurrentMaterial->GetPass( 0)->SetAmbient( l_pfAmbient[0], l_pfAmbient[1], l_pfAmbient[2], m_fAlpha);
		m_pCurrentMaterial->GetPass( 0)->SetDiffuse( l_pfDiffuse[0], l_pfDiffuse[1], l_pfDiffuse[2], m_fAlpha);
		m_pCurrentMaterial->GetPass( 0)->SetSpecular( l_pfSpecular[0], l_pfSpecular[1], l_pfSpecular[2], m_fAlpha);
		m_pCurrentMaterial->GetPass( 0)->SetEmissive( l_pfemissive[0], l_pfemissive[1], l_pfemissive[2], m_fAlpha);
	}
}

void ObjImporter :: _readMatLightRefDifExp( const String & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		String l_line;
		Char l_char = 0;

		l_char = p_strLine.at( 0);
		l_line = p_strLine.substr( 2, String::npos);

		float l_fValue = l_line.ToFloat();

		if (l_char == CU_T( 's'))
		{
			m_pCurrentMaterial->GetPass( 0)->SetShininess( l_fValue);
		}
	}
}

void ObjImporter :: _readSubmeshInfo( const String & p_strLine)
{
	if (m_bReadingFaces)
	{
		StringArray l_arraySplitted = p_strLine.Split( CU_T( " "));

		if (l_arraySplitted.size() > 1)
		{
			if (m_mapSubmeshes.find( l_arraySplitted[1]) == m_mapSubmeshes.end())
			{
				_createSubmesh();
				m_mapSubmeshes.insert( SubmeshPtrStrMap::value_type( l_arraySplitted[1], m_pCurrentSubmesh));
			}
			else
			{
				m_pCurrentSubmesh = m_mapSubmeshes.find( l_arraySplitted[1])->second;
			}

			m_iGroup = -1;
		}
	}
}

void ObjImporter :: _readGroupInfo( const String & p_strLine)
{
	if (m_bReadingFaces)
	{
		StringArray l_arraySplitted = p_strLine.Split( CU_T( " "));

		if (l_arraySplitted.size() > 1)
		{
			if (m_mapSmoothGroups.find( l_arraySplitted[1]) == m_mapSmoothGroups.end())
			{
				if (m_pCurrentSubmesh == NULL)
				{
					_createSubmesh();
				}

				SmoothingGroupPtr l_group = m_pCurrentSubmesh->AddSmoothingGroup();
				m_iGroup = l_group->GetGroupID() - 1;
				m_mapSmoothGroups.insert( IntStrMap::value_type( l_arraySplitted[1], m_iGroup));
				m_mapSmoothGroupSubmesh.insert( SubmeshPtrStrMap::value_type( l_arraySplitted[1], m_pCurrentSubmesh));
			}
			else
			{
				m_iGroup = m_mapSmoothGroups.find( l_arraySplitted[1])->second;
				m_pCurrentSubmesh = m_mapSmoothGroupSubmesh.find( l_arraySplitted[1])->second;
			}
		}
	}
}

void ObjImporter :: _readVertexInfo( const String & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		Point3r l_vertex;
		Point2r l_coords;
		String l_line;
		Char l_char = 0;
		Char l_cDump = 0;

		l_char = p_strLine.at( 0);
		l_line = p_strLine.substr( 1, String::npos);

		if (l_char == CU_T( ' '))
		{
			StringArray l_arraySplitted;
			sscanf_s( l_line.char_str(), "%f %f %f", & l_vertex[0], & l_vertex[1], & l_vertex[2]);
			m_arrayVertex.push_back( Point3r( l_vertex[0], l_vertex[1], l_vertex[2]));
		}
		else if (l_char == CU_T( 't'))
		{
			m_iNbTexCoords++;
			sscanf_s( l_line.char_str(), "%f %f", & l_coords[0], & l_coords[1]);
			m_textureCoords.push_back( l_coords);
			m_objectHasUV = true;
		}
		else if (l_char != CU_T( 'n'))
		{
			Logger::LogMessage( CU_T( "Unknown vertex info : [%c]"), l_char);
		}
	}
}


void ObjImporter :: _readFaceInfo( const String & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		return;
	}

	if (m_iGroup == -1)
	{
		SmoothingGroupPtr l_group = m_pCurrentSubmesh->AddSmoothingGroup();
		m_iGroup = l_group->GetGroupID() - 1;
		m_mapSmoothGroups.insert( IntStrMap::value_type( "-1", m_iGroup));

		if (m_mapSmoothGroupSubmesh.find( "-1") == m_mapSmoothGroupSubmesh.end())
		{
			m_mapSmoothGroupSubmesh.insert( SubmeshPtrStrMap::value_type( "-1", m_pCurrentSubmesh));
		}
	}

	String l_line;

//	Logger::LogMessage( CU_T( "Line : " + p_strLine);

	l_line = p_strLine.substr( 1, String::npos);

	StringArray l_arraySplitted = l_line.Split( CU_T( " "));

	if (l_arraySplitted.size() >= 3)
	{
		int l_iNbAdditionalFaces = l_arraySplitted.size() - 3;

		IntArray l_arrayVertex;
		IntArray l_arrayCoords;

		for (size_t i = 0 ; i < l_arraySplitted.size() ; i++)
		{
			l_arrayVertex.push_back( 0);

			if (m_objectHasUV)
			{
				StringArray l_arraySlashSplitted = l_arraySplitted[i].Split( CU_T( "/"));
				l_arrayCoords.push_back( 0);
				l_arrayVertex[i] = l_arraySlashSplitted[0].ToInt();
				l_arrayCoords[i] = l_arraySlashSplitted[1].ToInt();
			}
			else
			{
				if (l_arraySplitted[i].find( "/") != String::npos)
				{
					StringArray l_arraySlashSplitted = l_arraySplitted[i].Split( CU_T( "/"));
					l_arrayVertex[i] = l_arraySlashSplitted[0].ToInt();
				}
				else
				{
					sscanf_s( l_arraySplitted[i].char_str(), "%d", & l_arrayVertex[i]);
				}
			}
		}

		int l_iIndex;
		IdPoint3rPtr l_pV1, l_pV2, l_pV3;

		if ((l_iIndex = m_pCurrentSubmesh->IsInMyPoints( m_arrayVertex[l_arrayVertex[1] - 1])) == -1)
		{
			l_pV1 = m_pCurrentSubmesh->AddPoint( m_arrayVertex[l_arrayVertex[1] - 1]);
		}
		else
		{
			l_pV1 = m_pCurrentSubmesh->GetPoint( l_iIndex);
		}

		if ((l_iIndex = m_pCurrentSubmesh->IsInMyPoints( m_arrayVertex[l_arrayVertex[0] - 1])) == -1)
		{
			l_pV2 = m_pCurrentSubmesh->AddPoint( m_arrayVertex[l_arrayVertex[0] - 1]);
		}
		else
		{
			l_pV2 = m_pCurrentSubmesh->GetPoint( l_iIndex);
		}

		if ((l_iIndex = m_pCurrentSubmesh->IsInMyPoints( m_arrayVertex[l_arrayVertex[l_arrayVertex.size() - 1] - 1])) == -1)
		{
			l_pV3 = m_pCurrentSubmesh->AddPoint( m_arrayVertex[l_arrayVertex[l_arrayVertex.size() - 1] - 1]);
		}
		else
		{
			l_pV3 = m_pCurrentSubmesh->GetPoint( l_iIndex);
		}

		FacePtr l_face = m_pCurrentSubmesh->AddFace( l_pV1->GetIndex(), l_pV2->GetIndex(), l_pV3->GetIndex(), m_iGroup);

		if (m_objectHasUV)
		{
			l_face->SetVertexTexCoords( 0, m_textureCoords[l_arrayCoords[1] - 1][0], 						m_textureCoords[l_arrayCoords[1] - 1][1]);
			l_face->SetVertexTexCoords( 1, m_textureCoords[l_arrayCoords[0] - 1][0], 						m_textureCoords[l_arrayCoords[0] - 1][1]);
			l_face->SetVertexTexCoords( 2, m_textureCoords[l_arrayCoords[l_arrayCoords.size() - 1] - 1][0],	m_textureCoords[l_arrayCoords[l_arrayCoords.size() - 1] - 1][1]);
		}

		if (l_iNbAdditionalFaces > 0)
		{
			int l_iPreviousVertexIndex = l_arrayVertex[1];
			int l_iCurrentVertexIndex;
			int l_iLastVertexIndex = l_arrayVertex[l_arrayVertex.size() - 1];

			int l_iPreviousCoordIndex;
			int l_iCurrentCoordIndex;
			int l_iLastCoordIndex;

			if (m_objectHasUV)
			{
				l_iPreviousCoordIndex = l_arrayCoords[1];
				l_iLastCoordIndex = l_arrayCoords[l_arrayCoords.size() - 1];
			}

			for (int i = 0 ; i < l_iNbAdditionalFaces ; i++)
			{
				l_iCurrentVertexIndex = l_arrayVertex[i + 2];

				if ((l_iIndex = m_pCurrentSubmesh->IsInMyPoints( m_arrayVertex[l_iCurrentVertexIndex - 1])) == -1)
				{
					l_pV1 = m_pCurrentSubmesh->AddPoint( m_arrayVertex[l_iCurrentVertexIndex - 1]);
				}
				else
				{
					l_pV1 = m_pCurrentSubmesh->GetPoint( l_iIndex);
				}

				if ((l_iIndex = m_pCurrentSubmesh->IsInMyPoints( m_arrayVertex[l_iPreviousVertexIndex - 1])) == -1)
				{
					l_pV2 = m_pCurrentSubmesh->AddPoint( m_arrayVertex[l_iPreviousVertexIndex - 1]);
				}
				else
				{
					l_pV2 = m_pCurrentSubmesh->GetPoint( l_iIndex);
				}

				if ((l_iIndex = m_pCurrentSubmesh->IsInMyPoints( m_arrayVertex[l_iLastVertexIndex - 1])) == -1)
				{
					l_pV3 = m_pCurrentSubmesh->AddPoint( m_arrayVertex[l_iLastVertexIndex - 1]);
				}
				else
				{
					l_pV3 = m_pCurrentSubmesh->GetPoint( l_iIndex);
				}

				FacePtr l_faceB = m_pCurrentSubmesh->AddFace( l_pV1->GetIndex(), l_pV2->GetIndex(), l_pV3->GetIndex(), m_iGroup);

				if (m_objectHasUV)
				{
					l_iCurrentCoordIndex = l_arrayCoords[i + 2];
					l_faceB->SetVertexTexCoords( 0, m_textureCoords[l_iCurrentCoordIndex - 1][0],	m_textureCoords[l_iCurrentCoordIndex - 1][1]);
					l_faceB->SetVertexTexCoords( 1, m_textureCoords[l_iPreviousCoordIndex - 1][0],	m_textureCoords[l_iPreviousCoordIndex - 1][1]);
					l_faceB->SetVertexTexCoords( 2, m_textureCoords[l_iLastCoordIndex - 1][0],		m_textureCoords[l_iLastCoordIndex - 1][1]);
					l_iPreviousCoordIndex = l_iCurrentCoordIndex;
				}

				l_iPreviousVertexIndex = l_iCurrentVertexIndex;
			}
		}
	}
	else if (l_arraySplitted.size() > 0)
	{
		Logger::LogMessage( CU_T( "Too few vertices %d"), l_arraySplitted.size());
	}

//	Logger::LogMessage( CU_T( "Line : " + p_strLine + " End");
}

//*************************************************************************************************