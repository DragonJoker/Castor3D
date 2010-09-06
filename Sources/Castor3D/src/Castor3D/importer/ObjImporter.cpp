#include "PrecompiledHeader.h"

#include "importer/Module_Importer.h"

#include "importer/ObjImporter.h"
#include "material/MaterialManager.h"
#include "material/Material.h"
#include "material/Pass.h"
#include "material/TextureUnit.h"

#include "geometry/mesh/MeshManager.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/SmoothingGroup.h"
#include "render_system/MeshRenderer.h"
#include "render_system/Buffer.h"
#include "geometry/primitives/Geometry.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "Log.h"

using namespace Castor3D;

ObjImporter :: ObjImporter()
	:	m_pCurrentMaterial( NULL),
		m_pFile( NULL),
		m_objectHasUV( false),
		m_pCurrentSubmesh( NULL),
		m_pSmoothingGroup( NULL),
		m_bReadingFaces( false),
		m_bReadingVertex( true),
		m_iNbTexCoords( 0)
{
}

bool ObjImporter :: _import()
{
	m_pFile = new FileIO( m_fileName, FileIO::eRead);

	m_pScene = SceneManager::GetSingleton().GetElementByName( "MainScene");

	size_t l_uiSlashIndex = 0;
	size_t l_uiIndex = 0;

	if ((l_uiIndex = m_fileName.find_last_of( "\\")) != String::npos)
	{
		l_uiSlashIndex = l_uiIndex + 1;
	}

	if ((l_uiIndex = m_fileName.find_last_of( "/")) != String::npos)
	{
		l_uiSlashIndex = max( l_uiSlashIndex, l_uiIndex + 1);
	}

	size_t l_uiDotIndex = m_fileName.find_last_of( ".");

	UIntArray l_faces;
	FloatArray l_sizes;
	String l_name = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_meshName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_materialName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);

	if (MeshManager::GetSingletonPtr()->HasElement( l_meshName))
	{
		m_pMesh = MeshManager::GetSingletonPtr()->GetElementByName( l_meshName);
	}
	else
	{
		m_pMesh = MeshManager::GetSingletonPtr()->CreateMesh( l_meshName, l_faces, l_sizes, Mesh::eCustom);
		Log::LogMessage( C3D_T( "CreatePrimitive - Mesh %s created"), l_meshName.c_str());
	}

	_readObjFile();

	bool l_bHasMaterial = m_pCurrentMaterial != NULL;

	if ( ! l_bHasMaterial)
	{
		m_pCurrentMaterial = MaterialManager::GetSingleton().CreateMaterial( l_materialName);
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

	m_pMesh->SetNormals();

	SceneNode * l_pNode = m_pScene->CreateSceneNode( l_name);

	Geometry * l_pGeometry = new Geometry( m_pMesh, l_pNode, l_name);
	Log::LogMessage( C3D_T( "PlyImporter::_import - Geometry %s created"), l_name.c_str());

	m_geometries.insert( GeometryStrMap::value_type( l_name, l_pGeometry));

	delete m_pFile;

	return true;
}

void ObjImporter :: _createSubmesh()
{
	m_pCurrentSubmesh = m_pMesh->CreateSubmesh( 0);
	m_pCurrentSubmesh->GetRenderer()->GetTriangles()->Cleanup();
	m_pCurrentSubmesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
	m_pCurrentSubmesh->GetRenderer()->GetLines()->Cleanup();
	m_pCurrentSubmesh->GetRenderer()->GetLinesTexCoords()->Cleanup();
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

			if (l_strSection == C3D_T( "mtllib"))
			{
				_readMatFileIO( l_strValue);
			}
			else if (l_strSection == C3D_T( "usemtl"))
			{
				_applyMaterial( l_strValue);
			}
			else if (l_strSection == C3D_T( "group"))
			{
				_selectSubmesh( l_strValue);
			}
			else
			{
				l_char = l_strSection.at( 0);
				String l_strLine = l_strSection + C3D_T( " ") + l_strValue;

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
//					Log::LogMessage( l_strSection + " " + l_strValue);
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

			m_pFile = new FileIO( m_fileName, FileIO::eRead);
		}
	}

//	Log::LogMessage( "LastLine : " + l_strSection + " " + l_strValue);
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
		if (m_pCurrentSubmesh != NULL && MaterialManager::GetSingleton().HasElement( p_strMaterialName))
		{
			m_pCurrentSubmesh->SetMaterial( MaterialManager::GetSingleton().GetElementByName( p_strMaterialName));
		}
	}
}

void ObjImporter :: _readMatFileIO( const String & p_strFileName)
{
	if ( ! m_bReadingFaces)
	{
		m_pMatFile = new FileIO( m_filePath + p_strFileName, FileIO::eRead);

		String l_strLine;
		Char l_char = 0;

		while (m_pMatFile->IsOk())
		{
			m_pMatFile->ReadLine( l_strLine, 1000);

			if ( ! l_strLine.empty())
			{
				if (l_strLine.find( C3D_T( "newmtl")) != String::npos)
				{
					StringArray l_arraySplitted = l_strLine.Split( C3D_T( " "));

					if (l_arraySplitted.size() > 1)
					{
						if (MaterialManager::GetSingleton().HasElement( l_arraySplitted[1]))
						{
							m_pCurrentMaterial = MaterialManager::GetSingleton().GetElementByName( l_arraySplitted[1]);
						}
						else
						{
							m_pCurrentMaterial = MaterialManager::GetSingleton().CreateMaterial( l_arraySplitted[1]);
						}

						m_fAlpha = 1.0f;
					}
				}
				else if (l_strLine.find( C3D_T( "illum")) != String::npos)
				{
				}
				else
				{
					l_char = l_strLine.at( 0);

					if (l_char == C3D_T( 'K'))
					{
						_readMatLightComponent( l_strLine.substr( 1, String::npos));
					}
					else if (l_char == C3D_T( 'T') && l_strLine.at( 1) == C3D_T( 'r'))
					{
						_readMatTransparency( l_strLine.substr( 3, String::npos));
					}
					else if (l_char == C3D_T( 'd'))
					{
						_readMatTransparency( l_strLine.substr( 2, String::npos));
					}
					else if (l_char == C3D_T( 'N'))
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
		StringArray l_arraySplitted = l_line.Split( C3D_T( " "));

		if (l_arraySplitted.size() >= 3)
		{
			float l_fR, l_fG, l_fB;
			l_fR = (float)atof( l_arraySplitted[0].char_str());
			l_fG = (float)atof( l_arraySplitted[1].char_str());
			l_fB = (float)atof( l_arraySplitted[2].char_str());

			if (l_char == C3D_T( 'a'))
			{
				m_pCurrentMaterial->GetPass( 0)->SetAmbient( l_fR, l_fG, l_fB, m_fAlpha);
			}
			else if (l_char == C3D_T( 'd'))
			{
				m_pCurrentMaterial->GetPass( 0)->SetDiffuse( l_fR, l_fG, l_fB, m_fAlpha);
			}
			else if (l_char == C3D_T( 's'))
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
		m_fAlpha = (float)atof( p_strLine.char_str());
		const float * l_pfAmbient = m_pCurrentMaterial->GetPass( 0)->GetAmbient();
		const float * l_pfDiffuse = m_pCurrentMaterial->GetPass( 0)->GetDiffuse();
		const float * l_pfSpecular = m_pCurrentMaterial->GetPass( 0)->GetSpecular();
		const float * l_pfemissive = m_pCurrentMaterial->GetPass( 0)->GetEmissive();
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

		float l_fValue = (float)atof( l_line.char_str());

		if (l_char == C3D_T( 's'))
		{
			m_pCurrentMaterial->GetPass( 0)->SetShininess( l_fValue);
		}
	}
}

void ObjImporter :: _readSubmeshInfo( const String & p_strLine)
{
	if (m_bReadingFaces)
	{
		StringArray l_arraySplitted = p_strLine.Split( C3D_T( " "));

		if (l_arraySplitted.size() > 1)
		{
			if (m_mapSubmeshes.find( l_arraySplitted[1]) == m_mapSubmeshes.end())
			{
				_createSubmesh();
				m_mapSubmeshes.insert( std::map <String, Submesh *>::value_type( l_arraySplitted[1], m_pCurrentSubmesh));
			}
			else
			{
				m_pCurrentSubmesh = m_mapSubmeshes.find( l_arraySplitted[1])->second;
			}

			m_pSmoothingGroup = NULL;
		}
	}
}

void ObjImporter :: _readGroupInfo( const String & p_strLine)
{
	if (m_bReadingFaces)
	{
		StringArray l_arraySplitted = p_strLine.Split( C3D_T( " "));

		if (l_arraySplitted.size() > 1)
		{
			if (m_mapSmoothGroups.find( l_arraySplitted[1]) == m_mapSmoothGroups.end())
			{
				if (m_pCurrentSubmesh == NULL)
				{
					_createSubmesh();
				}

				m_pSmoothingGroup = m_pCurrentSubmesh->AddSmoothingGroup();
				m_mapSmoothGroups.insert( std::map <String, SmoothingGroup *>::value_type( l_arraySplitted[1], m_pSmoothingGroup));
				m_mapSmoothGroupSubmesh.insert( std::map <String, Submesh *>::value_type( l_arraySplitted[1], m_pCurrentSubmesh));
			}
			else
			{
				m_pSmoothingGroup = m_mapSmoothGroups.find( l_arraySplitted[1])->second;
				m_pCurrentSubmesh = m_mapSmoothGroupSubmesh.find( l_arraySplitted[1])->second;
			}
		}
	}
}

void ObjImporter :: _readVertexInfo( const String & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		Point3D<float> l_vertex;
		Point2D<float> l_coords;
		String l_line;
		Char l_char = 0;
		Char l_cDump = 0;

		l_char = p_strLine.at( 0);
		l_line = p_strLine.substr( 1, String::npos);

		if (l_char == C3D_T( ' '))
		{
			StringArray l_arraySplitted;
			sscanf_s( l_line.char_str(), "%f %f %f", & l_vertex.x, & l_vertex.y, & l_vertex.z);
			m_arrayVertex.push_back( new Vector3f( l_vertex.x, l_vertex.y, l_vertex.z));
		}
		else if (l_char == C3D_T( 't'))
		{
			m_iNbTexCoords++;
			sscanf_s( l_line.char_str(), "%f %f", & l_coords.x, & l_coords.y);
			m_textureCoords.push_back( l_coords);
			m_objectHasUV = true;
		}
		else if (l_char != C3D_T( 'n'))
		{
			Log::LogMessage( "Unknown vertex info : [%c]", l_char);
		}
	}
}


void ObjImporter :: _readFaceInfo( const String & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		return;
	}

	if (m_pSmoothingGroup == NULL)
	{
		m_pSmoothingGroup = m_pCurrentSubmesh->AddSmoothingGroup();
		m_mapSmoothGroups.insert( std::map <String, SmoothingGroup *>::value_type( "-1", m_pSmoothingGroup));
		if (m_mapSmoothGroupSubmesh.find( "-1") == m_mapSmoothGroupSubmesh.end())
		{
			m_mapSmoothGroupSubmesh.insert( std::map <String, Submesh *>::value_type( "-1", m_pCurrentSubmesh));
		}
	}

	String l_line;

//	Log::LogMessage( "Line : " + p_strLine);

	l_line = p_strLine.substr( 1, String::npos);

	StringArray l_arraySplitted = l_line.Split( C3D_T( " "));

	if (l_arraySplitted.size() >= 3)
	{
		int l_iNbAdditionalFaces = l_arraySplitted.size() - 3;

		std::vector <int> l_arrayVertex;
		std::vector <int> l_arrayCoords;

		for (size_t i = 0 ; i < l_arraySplitted.size() ; i++)
		{
			l_arrayVertex.push_back( 0);

			if (m_objectHasUV)
			{
				StringArray l_arraySlashSplitted = l_arraySplitted[i].Split( C3D_T( "/"));
				l_arrayCoords.push_back( 0);
				l_arrayVertex[i] = atoi( l_arraySlashSplitted[0].char_str());
				l_arrayCoords[i] = atoi( l_arraySlashSplitted[1].char_str());
			}
			else
			{
				if (l_arraySplitted[i].find( "/") != String::npos)
				{
					StringArray l_arraySlashSplitted = l_arraySplitted[i].Split( C3D_T( "/"));
					l_arrayVertex[i] = atoi( l_arraySlashSplitted[0].char_str());
				}
				else
				{
					sscanf_s( l_arraySplitted[i].char_str(), "%d", & l_arrayVertex[i]);
				}
			}
		}

		Face * l_pFace;
		int l_iIndex;
		Vector3f * l_pV1, * l_pV2, * l_pV3;

		if ((l_iIndex = m_pCurrentSubmesh->IsInMyVertex( *m_arrayVertex[l_arrayVertex[1] - 1])) == -1)
		{
			l_pV1 = m_pCurrentSubmesh->AddVertex( new Vector3f( *m_arrayVertex[l_arrayVertex[1] - 1]));
		}
		else
		{
			l_pV1 = m_pCurrentSubmesh->GetVertex( l_iIndex);
		}

		if ((l_iIndex = m_pCurrentSubmesh->IsInMyVertex( *m_arrayVertex[l_arrayVertex[0] - 1])) == -1)
		{
			l_pV2 = m_pCurrentSubmesh->AddVertex( new Vector3f( *m_arrayVertex[l_arrayVertex[0] - 1]));
		}
		else
		{
			l_pV2 = m_pCurrentSubmesh->GetVertex( l_iIndex);
		}

		if ((l_iIndex = m_pCurrentSubmesh->IsInMyVertex( *m_arrayVertex[l_arrayVertex[l_arrayVertex.size() - 1] - 1])) == -1)
		{
			l_pV3 = m_pCurrentSubmesh->AddVertex( new Vector3f( *m_arrayVertex[l_arrayVertex[l_arrayVertex.size() - 1] - 1]));
		}
		else
		{
			l_pV3 = m_pCurrentSubmesh->GetVertex( l_iIndex);
		}

		if ((l_pFace = m_pCurrentSubmesh->AddFace( l_pV1, l_pV2, l_pV3, m_pSmoothingGroup->m_idGroup - 1)) != NULL)
		{
			if (m_objectHasUV)
			{
				SetTexCoordV1( l_pFace, m_textureCoords[l_arrayCoords[1] - 1].x, m_textureCoords[l_arrayCoords[1] - 1].y);
				SetTexCoordV2( l_pFace, m_textureCoords[l_arrayCoords[0] - 1].x, m_textureCoords[l_arrayCoords[0] - 1].y);
				SetTexCoordV3( l_pFace, m_textureCoords[l_arrayCoords[l_arrayCoords.size() - 1] - 1].x, m_textureCoords[l_arrayCoords[l_arrayCoords.size() - 1] - 1].y);
			}
		}
		else
		{
			Log::LogMessage( "NULL Face !!!");
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

				if ((l_iIndex = m_pCurrentSubmesh->IsInMyVertex( *m_arrayVertex[l_iCurrentVertexIndex - 1])) == -1)
				{
					l_pV1 = m_pCurrentSubmesh->AddVertex( new Vector3f( *m_arrayVertex[l_iCurrentVertexIndex - 1]));
				}
				else
				{
					l_pV1 = m_pCurrentSubmesh->GetVertex( l_iIndex);
				}

				if ((l_iIndex = m_pCurrentSubmesh->IsInMyVertex( *m_arrayVertex[l_iPreviousVertexIndex - 1])) == -1)
				{
					l_pV2 = m_pCurrentSubmesh->AddVertex( new Vector3f( *m_arrayVertex[l_iPreviousVertexIndex - 1]));
				}
				else
				{
					l_pV2 = m_pCurrentSubmesh->GetVertex( l_iIndex);
				}

				if ((l_iIndex = m_pCurrentSubmesh->IsInMyVertex( *m_arrayVertex[l_iLastVertexIndex - 1])) == -1)
				{
					l_pV3 = m_pCurrentSubmesh->AddVertex( new Vector3f( *m_arrayVertex[l_iLastVertexIndex - 1]));
				}
				else
				{
					l_pV3 = m_pCurrentSubmesh->GetVertex( l_iIndex);
				}

				if ((l_pFace = m_pCurrentSubmesh->AddFace( l_pV1, l_pV2, l_pV3, m_pSmoothingGroup->m_idGroup - 1)) != NULL)
				{
					if (m_objectHasUV)
					{
						l_iCurrentCoordIndex = l_arrayCoords[i + 2];
						SetTexCoordV1( l_pFace, m_textureCoords[l_iCurrentCoordIndex - 1].x, m_textureCoords[l_iCurrentCoordIndex - 1].y);
						SetTexCoordV2( l_pFace, m_textureCoords[l_iPreviousCoordIndex - 1].x, m_textureCoords[l_iPreviousCoordIndex - 1].y);
						SetTexCoordV3( l_pFace, m_textureCoords[l_iLastCoordIndex - 1].x, m_textureCoords[l_iLastCoordIndex - 1].y);
						l_iPreviousCoordIndex = l_iCurrentCoordIndex;
					}
				}
				else
				{
					Log::LogMessage( "NULL Face !!!");
				}

				l_iPreviousVertexIndex = l_iCurrentVertexIndex;
			}
		}
	}
	else if (l_arraySplitted.size() > 0)
	{
		Log::LogMessage( "Too few vertices %d", l_arraySplitted.size());
	}

//	Log::LogMessage( "Line : " + p_strLine + " End");
}