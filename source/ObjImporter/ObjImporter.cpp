#include <Castor3D/Prerequisites.hpp>

using namespace Castor::Templates;

#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/TextureUnit.hpp>
#include <Castor3D/Mesh.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/Vertex.hpp>
#include <Castor3D/SmoothingGroup.hpp>
#include <Castor3D/Buffer.hpp>
#include <Castor3D/Geometry.hpp>
#include <Castor3D/Face.hpp>
#include <Castor3D/Scene.hpp>
#include <Castor3D/SceneNode.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Plugin.hpp>
#include <Castor3D/Root.hpp>

#include "ObjImporter/ObjImporter.hpp"

using namespace Castor3D;

//*************************************************************************************************

C3D_Obj_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_Obj_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_Obj_API String GetName()
{
	return cuT( "OBJ Importer Plugin");
}

C3D_Obj_API String GetExtension()
{
	return cuT( "OBJ");
}

C3D_Obj_API Importer * CreateImporter()
{
	Importer * l_pReturn( new ObjImporter());

	return l_pReturn;
}

//*************************************************************************************************

ObjImporter :: ObjImporter()
	:	Importer()
	,	m_pFile( nullptr)
	,	m_objectHasUV( false)
	,	m_bReadingFaces( false)
	,	m_bReadingVertex( true)
	,	m_iNbTexCoords( 0)
	,	m_iGroup( -1)
	,	m_pMatFile( nullptr)
{
}

bool ObjImporter :: _import()
{
	m_pFile = new File( m_fileName, File::eOPEN_MODE_READ);
	Collection<Scene, String> l_scnCollection;
	m_pScene = l_scnCollection.GetElement( "MainScene");

	UIntArray l_faces;
	FloatArray l_sizes;
	String l_name = m_fileName.GetFileName();
	String l_meshName = l_name.substr( 0, l_name.find_last_of( '.'));
	String l_materialName = l_meshName;

	Collection<Mesh, String> l_mshCollection;
	m_pMesh = l_mshCollection.GetElement( l_meshName);
	if ( ! m_pMesh)
	{
		m_pMesh = MeshPtr( new Mesh( l_meshName, eMESH_TYPE_CUSTOM));
		m_pMesh->Initialise( l_faces, l_sizes);
		l_mshCollection.AddElement( l_meshName, m_pMesh);
		Logger::LogMessage( cuT( "CreatePrimitive - Mesh ") + l_meshName + cuT( " created"));
	}

	_readObjFile();

	bool l_bHasMaterial = m_pCurrentMaterial.use_count() > 0;

	if ( ! l_bHasMaterial)
	{
		Collection<Material, String> l_mtlCollection;
		m_pCurrentMaterial = l_mtlCollection.GetElement( l_materialName);
		if ( ! m_pCurrentMaterial)
		{
			m_pCurrentMaterial = MaterialPtr( new Material( l_materialName, 1));
			l_mtlCollection.AddElement( l_materialName, m_pCurrentMaterial);
		}
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

	GeometryPtr l_pGeometry( new Geometry( m_pScene.get(), m_pMesh, l_pNode, l_name));
	Logger::LogMessage( cuT( "PlyImporter::_import - Geometry ") + l_name + cuT( " created"));

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
	xchar l_char = 0;

	while (m_pFile->IsOk())
	{
		m_pFile->ReadLine( l_strLine, 1000);

		if (l_strLine.size() >= 1)
		{
			l_strSection = l_strLine.substr( 0, l_strLine.find_first_of( ' '));
			l_strValue = l_strLine.substr( l_strLine.find_first_of( ' ') + 1, String::npos);

			if (l_strSection == cuT( "mtllib"))
			{
				_readMatFile( l_strValue);
			}
			else if (l_strSection == cuT( "usemtl"))
			{
				_applyMaterial( l_strValue);
			}
			else if (l_strSection == cuT( "group"))
			{
				_selectSubmesh( l_strValue);
			}
			else
			{
				l_char = l_strSection.at( 0);
				String l_strLine = l_strSection + cuT( " ") + l_strValue;

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

			m_pFile = new File( m_fileName, File::eOPEN_MODE_READ);
		}
	}

//	Logger::LogMessage( cuT( "LastLine : " + l_strSection + " " + l_strValue);
}

void ObjImporter :: _selectSubmesh( String const & p_strName)
{
	if (m_bReadingFaces)
	{
		if (m_mapSubmeshes.find( p_strName) != m_mapSubmeshes.end())
		{
			m_pCurrentSubmesh = m_mapSubmeshes.find( p_strName)->second;
		}
	}
}

void ObjImporter :: _applyMaterial( String const & p_strMaterialName)
{
	if (m_bReadingFaces)
	{
		Root::MaterialManager * l_pMtlManager = Root::GetSingleton()->GetMaterialManager();

		if (m_pCurrentSubmesh && l_pMtlManager->HasElement( p_strMaterialName))
		{
			m_pCurrentSubmesh->SetMaterial( l_pMtlManager->GetElement( p_strMaterialName));
		}
	}
}

void ObjImporter :: _readMatFile( String const & p_strFileName)
{
	if ( ! m_bReadingFaces)
	{
		try
		{
			m_pMatFile = new File( m_filePath + p_strFileName, File::eOPEN_MODE_READ);

			String l_strLine;
			xchar l_char = 0;
			Root::MaterialManager * l_pMtlManager = Root::GetSingleton()->GetMaterialManager();

			while (m_pMatFile->IsOk())
			{
				m_pMatFile->ReadLine( l_strLine, 1000);

				if ( ! l_strLine.empty())
				{
					if (l_strLine.find( cuT( "newmtl")) != String::npos)
					{
						StringArray l_arraySplitted = l_strLine.split( cuT( " "));

						if (l_arraySplitted.size() > 1)
						{
							if (l_pMtlManager->HasElement( l_arraySplitted[1]))
							{
								m_pCurrentMaterial = l_pMtlManager->GetElement( l_arraySplitted[1]);
							}
							else
							{
								Collection<Material, String> l_mtlCollection;
								m_pCurrentMaterial = l_mtlCollection.GetElement( l_arraySplitted[1]);
								if ( ! m_pCurrentMaterial)
								{
									m_pCurrentMaterial = MaterialPtr( new Material( l_arraySplitted[1], 1));
									l_mtlCollection.AddElement( l_arraySplitted[1], m_pCurrentMaterial);
								}
							}

							m_fAlpha = 1.0f;
						}
					}
					else if (l_strLine.find( cuT( "illum")) != String::npos)
					{
					}
					else
					{
						l_char = l_strLine.at( 0);

						if (l_char == cuT( 'K'))
						{
							_readMatLightComponent( l_strLine.substr( 1, String::npos));
						}
						else if (l_char == cuT( 'T') && l_strLine.at( 1) == cuT( 'r'))
						{
							_readMatTransparency( l_strLine.substr( 3, String::npos));
						}
						else if (l_char == cuT( 'd'))
						{
							_readMatTransparency( l_strLine.substr( 2, String::npos));
						}
						else if (l_char == cuT( 'N'))
						{
							_readMatLightRefDifExp( l_strLine.substr( 1, String::npos));
						}
					}
				}
			}

			delete m_pMatFile;
			m_pMatFile = nullptr;
		}
		catch (...)
		{
			delete m_pMatFile;
			m_pMatFile = nullptr;
		}
	}
}

void ObjImporter :: _readMatLightComponent( String const & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		String l_line;
		xchar l_char = 0;
		xchar l_cDump = 0;

		l_char = p_strLine.at( 0);
		l_line = p_strLine.substr( 2, String::npos);
		StringArray l_arraySplitted = l_line.split( cuT( " "));

		if (l_arraySplitted.size() >= 3)
		{
			float l_fR, l_fG, l_fB;
			l_fR = l_arraySplitted[0].to_float();
			l_fG = l_arraySplitted[1].to_float();
			l_fB = l_arraySplitted[2].to_float();

			if (l_char == cuT( 'a'))
			{
				m_pCurrentMaterial->GetPass( 0)->SetAmbient( l_fR, l_fG, l_fB, m_fAlpha);
			}
			else if (l_char == cuT( 'd'))
			{
				m_pCurrentMaterial->GetPass( 0)->SetDiffuse( l_fR, l_fG, l_fB, m_fAlpha);
			}
			else if (l_char == cuT( 's'))
			{
				m_pCurrentMaterial->GetPass( 0)->SetSpecular( l_fR, l_fG, l_fB, m_fAlpha);
			}
		}
	}
}

void ObjImporter :: _readMatTransparency( String const & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		m_fAlpha = p_strLine.to_float();
		float const * l_pfAmbient = m_pCurrentMaterial->GetPass( 0)->GetAmbient().const_ptr();
		float const * l_pfDiffuse = m_pCurrentMaterial->GetPass( 0)->GetDiffuse().const_ptr();
		float const * l_pfSpecular = m_pCurrentMaterial->GetPass( 0)->GetSpecular().const_ptr();
		float const * l_pfemissive = m_pCurrentMaterial->GetPass( 0)->GetEmissive().const_ptr();
		m_pCurrentMaterial->GetPass( 0)->SetAmbient( l_pfAmbient[0], l_pfAmbient[1], l_pfAmbient[2], m_fAlpha);
		m_pCurrentMaterial->GetPass( 0)->SetDiffuse( l_pfDiffuse[0], l_pfDiffuse[1], l_pfDiffuse[2], m_fAlpha);
		m_pCurrentMaterial->GetPass( 0)->SetSpecular( l_pfSpecular[0], l_pfSpecular[1], l_pfSpecular[2], m_fAlpha);
		m_pCurrentMaterial->GetPass( 0)->SetEmissive( l_pfemissive[0], l_pfemissive[1], l_pfemissive[2], m_fAlpha);
	}
}

void ObjImporter :: _readMatLightRefDifExp( String const & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		String l_line;
		xchar l_char = 0;

		l_char = p_strLine.at( 0);
		l_line = p_strLine.substr( 2, String::npos);

		float l_fValue = l_line.to_float();

		if (l_char == cuT( 's'))
		{
			m_pCurrentMaterial->GetPass( 0)->SetShininess( l_fValue);
		}
	}
}

void ObjImporter :: _readSubmeshInfo( String const & p_strLine)
{
	if (m_bReadingFaces)
	{
		StringArray l_arraySplitted = p_strLine.split( cuT( " "));

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

void ObjImporter :: _readGroupInfo( String const & p_strLine)
{
	if (m_bReadingFaces)
	{
		StringArray l_arraySplitted = p_strLine.split( cuT( " "));

		if (l_arraySplitted.size() > 1)
		{
			if (m_mapSmoothGroups.find( l_arraySplitted[1]) == m_mapSmoothGroups.end())
			{
				if ( ! m_pCurrentSubmesh)
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

void ObjImporter :: _readVertexInfo( String const & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		Point3r l_vertex;
		Point2r l_coords;
		String l_line;
		xchar l_char = 0;
		xchar l_cDump = 0;

		l_char = p_strLine.at( 0);
		l_line = p_strLine.substr( 1, String::npos);

		if (l_char == cuT( ' '))
		{
			StringArray l_arraySplitted;
			Sscanf( l_line.c_str(), cuT( "%f %f %f"), & l_vertex[0], & l_vertex[1], & l_vertex[2]);
			m_arrayVertex.push_back( Point3r( l_vertex[0], l_vertex[1], l_vertex[2]));
		}
		else if (l_char == cuT( 't'))
		{
			m_iNbTexCoords++;
			Sscanf( l_line.c_str(), cuT( "%f %f"), & l_coords[0], & l_coords[1]);
			m_textureCoords.push_back( l_coords);
			m_objectHasUV = true;
		}
		else if (l_char != cuT( 'n'))
		{
			Logger::LogMessage( cuT( "Unknown vertex info : [%c]"), l_char);
		}
	}
}


void ObjImporter :: _readFaceInfo( String const & p_strLine)
{
	if ( ! m_bReadingFaces)
	{
		return;
	}

	if (m_iGroup == -1)
	{
		if ( ! m_pCurrentSubmesh)
		{
			_createSubmesh();
			m_mapSubmeshes.insert( SubmeshPtrStrMap::value_type( "", m_pCurrentSubmesh));
		}

		SmoothingGroupPtr l_group = m_pCurrentSubmesh->AddSmoothingGroup();
		m_iGroup = l_group->GetGroupID() - 1;
		m_mapSmoothGroups.insert( IntStrMap::value_type( "-1", m_iGroup));

		if (m_mapSmoothGroupSubmesh.find( "-1") == m_mapSmoothGroupSubmesh.end())
		{
			m_mapSmoothGroupSubmesh.insert( SubmeshPtrStrMap::value_type( "-1", m_pCurrentSubmesh));
		}
	}

	String l_line;

//	Logger::LogMessage( cuT( "Line : " + p_strLine);

	l_line = p_strLine.substr( 1, String::npos);

	StringArray l_arraySplitted = l_line.split( cuT( " \n\r\t"));

	if (l_arraySplitted.size() >= 3)
	{
		int l_iNbAdditionalFaces = l_arraySplitted.size() - 3;

		IntArray l_arrayVertex;
		IntArray l_arrayCoords;

		for (size_t i = 0 ; i < l_arraySplitted.size() ; i++)
		{
			if ( ! l_arraySplitted[i].empty())
			{
				l_arrayVertex.push_back( 0);

				if (m_objectHasUV)
				{
					StringArray l_arraySlashSplitted = l_arraySplitted[i].split( cuT( "/"));
					l_arrayCoords.push_back( 0);
					l_arrayVertex[i] = l_arraySlashSplitted[0].to_int();
					l_arrayCoords[i] = l_arraySlashSplitted[1].to_int();
				}
				else
				{
					if (l_arraySplitted[i].find( "/") != String::npos)
					{
						StringArray l_arraySlashSplitted = l_arraySplitted[i].split( cuT( "/"));
						l_arrayVertex[i] = l_arraySlashSplitted[0].to_int();
					}
					else
					{
						Sscanf( l_arraySplitted[i].c_str(), cuT( "%d"), & l_arrayVertex[i]);
					}
				}
			}
		}

		int l_iIndex;
		IdPoint3rPtr l_pV1, l_pV2, l_pV3;
		int l_iVertexIndex = abs( l_arrayVertex[1]) - 1;

		if ((l_iIndex = m_pCurrentSubmesh->IsInMyPoints( m_arrayVertex[l_iVertexIndex])) == -1)
		{
			l_pV1 = m_pCurrentSubmesh->AddPoint( m_arrayVertex[l_iVertexIndex]);
		}
		else
		{
			l_pV1 = m_pCurrentSubmesh->GetPoint( l_iIndex);
		}

		l_iVertexIndex = abs( l_arrayVertex[0]) - 1;

		if ((l_iIndex = m_pCurrentSubmesh->IsInMyPoints( m_arrayVertex[l_iVertexIndex])) == -1)
		{
			l_pV2 = m_pCurrentSubmesh->AddPoint( m_arrayVertex[l_iVertexIndex]);
		}
		else
		{
			l_pV2 = m_pCurrentSubmesh->GetPoint( l_iIndex);
		}

		l_iVertexIndex = abs( l_arrayVertex[l_arrayVertex.size() - 1]) - 1;

		if ((l_iIndex = m_pCurrentSubmesh->IsInMyPoints( m_arrayVertex[l_iVertexIndex])) == -1)
		{
			l_pV3 = m_pCurrentSubmesh->AddPoint( m_arrayVertex[l_iVertexIndex]);
		}
		else
		{
			l_pV3 = m_pCurrentSubmesh->GetPoint( l_iIndex);
		}

		FacePtr l_face = m_pCurrentSubmesh->AddFace( l_pV1->GetIndex(), l_pV2->GetIndex(), l_pV3->GetIndex(), m_iGroup);

		if (m_objectHasUV)
		{
			l_face->SetVertexTexCoords( 0, m_textureCoords[abs( l_arrayCoords[1]) - 1][0], 							m_textureCoords[abs( l_arrayCoords[1]) - 1][1]);
			l_face->SetVertexTexCoords( 1, m_textureCoords[abs( l_arrayCoords[0]) - 1][0], 							m_textureCoords[abs( l_arrayCoords[0]) - 1][1]);
			l_face->SetVertexTexCoords( 2, m_textureCoords[abs( l_arrayCoords[l_arrayCoords.size() - 1]) - 1][0],	m_textureCoords[abs( l_arrayCoords[l_arrayCoords.size() - 1]) - 1][1]);
		}

		if (l_iNbAdditionalFaces > 0)
		{
			int l_iPreviousVertexIndex = abs( l_arrayVertex[1]);
			int l_iCurrentVertexIndex;
			int l_iLastVertexIndex = abs( l_arrayVertex[l_arrayVertex.size() - 1]);

			int l_iPreviousCoordIndex;
			int l_iCurrentCoordIndex;
			int l_iLastCoordIndex;

			if (m_objectHasUV)
			{
				l_iPreviousCoordIndex = abs( l_arrayCoords[1]);
				l_iLastCoordIndex = abs( l_arrayCoords[l_arrayCoords.size() - 1]);
			}

			for (int i = 0 ; i < l_iNbAdditionalFaces ; i++)
			{
				l_iCurrentVertexIndex = abs( l_arrayVertex[i + 2]);

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
					l_iCurrentCoordIndex = abs( l_arrayCoords[i + 2]);
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
		Logger::LogMessage( cuT( "Too few vertices %d"), l_arraySplitted.size());
	}

//	Logger::LogMessage( cuT( "Line : " + p_strLine + " End");
}

//*************************************************************************************************
