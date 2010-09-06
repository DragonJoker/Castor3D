#include "PrecompiledHeader.h"

#include "scene/SceneFileParser_Parsers.h"
#include "scene/SceneFileParser.h"

#include "Log.h"

#include "material/Material.h"
#include "material/MaterialManager.h"
#include "material/Pass.h"
#include "material/TextureUnit.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"
#include "scene/SceneNode.h"
#include "camera/Camera.h"
#include "light/Light.h"
#include "light/SpotLight.h"
#include "light/PointLight.h"
#include "geometry/primitives/MovableObject.h"
#include "geometry/primitives/Geometry.h"
#include "geometry/basic/SmoothingGroup.h"
#include "geometry/basic/Face.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/mesh/MeshManager.h"
#include "render_system/RenderSystem.h"
#include "shader/ShaderProgram.h"
#include "shader/UniformVariable.h"

using namespace Castor3D;

void Castor3D :: SceneFileParser :: ParseError( const String & p_strError, SceneFileContext * p_pContext)
{
	StringStream l_streamError;
	l_streamError << "Error Line #" << p_pContext->ui64Line << " / " << p_strError;
	Log::LogMessage( l_streamError.str());
}

void Castor3D :: SceneFileParser :: ParseWarning( const String & p_strWarning, SceneFileContext * p_pContext)
{
	StringStream l_streamWarning;
	l_streamWarning << "Warning Line #" << p_pContext->ui64Line << " / " << p_strWarning;
	Log::LogMessage( l_streamWarning.str());
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootSceneName)
{
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootCamera)
{
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eCamera;
		p_pContext->strName = p_strParams;
		return true;
	}
	else
	{
		PARSING_ERROR( "No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootLight)
{
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eLight;
		p_pContext->strName = p_strParams;
		return true;
	}
	else
	{
		PARSING_ERROR( "No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootSceneNode)
{
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eNode;
		p_pContext->strName = p_strParams;
		p_pContext->pSceneNode = p_pContext->pScene->CreateSceneNode( p_strParams);
		return true;
	}
	else
	{
		PARSING_ERROR( "No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootMaterial)
{
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eMaterial;
		p_pContext->pMaterial = MaterialManager::GetSingleton().CreateMaterial( p_strParams, 0);
		p_pContext->strName = p_strParams;
		return true;
	}
	else
	{
		PARSING_ERROR( "No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootObject)
{
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eObject;
		p_pContext->strName = p_strParams;
		p_pContext->pGeometry = new Geometry( NULL, NULL, p_pContext->strName);
		return true;
	}
	else
	{
		PARSING_ERROR( "No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightType)
{
	if (p_strParams == "point_light")
	{
		p_pContext->eLightType = Light::ePoint;
		p_pContext->pLight = p_pContext->pScene->CreateLight( p_pContext->eLightType, p_pContext->strName);
	}
	else if (p_strParams == "spot_light")
	{
		p_pContext->eLightType = Light::eSpot;
		p_pContext->pLight = p_pContext->pScene->CreateLight( p_pContext->eLightType, p_pContext->strName);
	}
	else if (p_strParams == "directional")
	{
		p_pContext->eLightType = Light::eDirectional;
		p_pContext->pLight = p_pContext->pScene->CreateLight( p_pContext->eLightType, p_pContext->strName);
	}
	else
	{
		PARSING_ERROR( "Unknown light type : " + p_strParams);
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightPosition)
{
	if (p_pContext->pLight != NULL)
	{
		Vector3f l_vVector;

		if (PARSE_V3( l_vVector))
		{
			p_pContext->pLight->SetPosition( l_vVector.ptr());
		}
	}
	else
	{
		PARSING_ERROR( "Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightAmbient)
{
	if (p_pContext->pLight != NULL)
	{
		Vector3f l_vVector;

		if (PARSE_V3( l_vVector))
		{
			p_pContext->pLight->SetAmbient( l_vVector);
		}
	}
	else
	{
		PARSING_ERROR( "Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightDiffuse)
{
	if (p_pContext->pLight != NULL)
	{
		Vector3f l_vVector;

		if (PARSE_V3( l_vVector))
		{
			p_pContext->pLight->SetDiffuse( l_vVector);
		}
	}
	else
	{
		PARSING_ERROR( "Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightSpecular)
{
	if (p_pContext->pLight != NULL)
	{
		Vector3f l_vVector;

		if (PARSE_V3( l_vVector))
		{
			p_pContext->pLight->SetSpecular( l_vVector);
		}
	}
	else
	{
		PARSING_ERROR( "Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightAttenuation)
{
	if (p_pContext->pLight != NULL)
	{
		Vector3f l_vVector;

		if (PARSE_V3( l_vVector))
		{
			if (p_pContext->eLightType == Light::ePoint)
			{
				((PointLight *)p_pContext->pLight)->SetAttenuation( l_vVector);
			}
			else if (p_pContext->eLightType == Light::eSpot)
			{
				((SpotLight *)p_pContext->pLight)->SetAttenuation( l_vVector);
			}
			else
			{
				PARSING_ERROR( "Wrong type of light to apply attenuation components, needs spotlight or pointlight");
			}
		}
	}
	else
	{
		PARSING_ERROR( "Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightCutOff)
{
	if (p_pContext->pLight != NULL)
	{
		float l_fFloat = float( atof( p_strParams.c_str()));

		if (p_pContext->eLightType == Light::eSpot)
		{
			((SpotLight *)p_pContext->pLight)->SetCutOff( l_fFloat);
		}
		else
		{
			PARSING_ERROR( "Wrong type of light to apply a cut off, needs spotlight");
		}
	}
	else
	{
		PARSING_ERROR( "Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightExponent)
{
	if (p_pContext->pLight != NULL)
	{
		float l_fFloat = float( atof( p_strParams.c_str()));

		if (p_pContext->eLightType == Light::eSpot)
		{
			((SpotLight *)p_pContext->pLight)->SetExponent( l_fFloat);
		}
		else
		{
			PARSING_ERROR( "Wrong type of light to apply an exponent, needs spotlight");
		}
	}
	else
	{
		PARSING_ERROR( "Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightOrientation)
{
	if (p_pContext->pLight != NULL)
	{
		Quaternion l_qQuaternion;

		if (PARSE_V4( l_qQuaternion))
		{
			if (p_pContext->eLightType == Light::eSpot)
			{
				((SpotLight *)p_pContext->pLight)->Rotate( l_qQuaternion);
			}
			else
			{
				PARSING_ERROR( "Wrong type of light to apply an exponent, needs spotlight");
			}
		}
	}
	else
	{
		PARSING_ERROR( "Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightEnd)
{
	p_pContext->eSection = SceneFileContext::eNone;
	p_pContext->pLight = NULL;
	p_pContext->strName.clear();
    return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeParent)
{
	if (p_pContext->pSceneNode != NULL)
	{
		SceneNode * l_pParent = p_pContext->pScene->GetNode( p_strParams);

		if (l_pParent != NULL)
		{
			p_pContext->pSceneNode->AttachTo( l_pParent);
		}
		else
		{
			PARSING_ERROR( "Node " + p_strParams + " does not exist");
		}
	}
	else
	{
		PARSING_ERROR( "Scene node not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodePosition)
{
	if (p_pContext->pSceneNode != NULL)
	{
		Vector3f l_vVector;

		if (PARSE_V3( l_vVector))
		{
			p_pContext->pSceneNode->SetPosition( l_vVector);
		}
	}
	else
	{
		PARSING_ERROR( "Scene node not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeOrientation)
{
	if (p_pContext->pSceneNode != NULL)
	{
		Quaternion l_qQuaternion;

		if (PARSE_V4( l_qQuaternion))
		{
			p_pContext->pSceneNode->SetOrientation( l_qQuaternion);
		}
	}
	else
	{
		PARSING_ERROR( "Scene node not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeScale)
{
	if (p_pContext->pSceneNode != NULL)
	{
		Vector3f l_vVector;

		if (PARSE_V3( l_vVector))
		{
			p_pContext->pSceneNode->SetScale( l_vVector);
		}
	}
	else
	{
		PARSING_ERROR( "Scene node not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeEnd)
{
	p_pContext->eSection = SceneFileContext::eNone;
	p_pContext->pSceneNode = NULL;
	p_pContext->strName.clear();
    return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectParent)
{
	if (p_pContext->pGeometry != NULL)
	{
		SceneNode * l_pParent = p_pContext->pScene->GetNode( p_strParams);

		if (l_pParent != NULL)
		{
			p_pContext->pGeometry->SetParent( l_pParent);
			l_pParent->AttachGeometry( p_pContext->pGeometry);
		}
		else
		{
			PARSING_ERROR( "Node " + p_strParams + " does not exist");
		}
	}
	else
	{
		PARSING_ERROR( "Geometry not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectMesh)
{
	p_pContext->eSection = SceneFileContext::eMesh;
	p_pContext->strName2 = p_strParams;
	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectEnd)
{
	p_pContext->eSection = SceneFileContext::eNone;
	p_pContext->pScene->AddGeometry( p_pContext->pGeometry);
	p_pContext->pGeometry = NULL;
	p_pContext->strName.clear();
    return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshType)
{
	String l_strType = p_strParams;
	Mesh::eTYPE l_eType = Mesh::eCustom;
	UIntArray l_arrayFaces;
	FloatArray l_arraySizes;

	if (l_strType != "custom")
	{
		if (p_pContext->pMesh == NULL)
		{
			StringArray l_arrayMeshInfos = p_strParams.Split( " ");
			l_strType = l_arrayMeshInfos[0];

			if (l_strType == "cube")
			{
				l_eType = Mesh::eCube;
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[1].c_str())));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[2].c_str())));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[3].c_str())));
			}
			else if (l_strType == "cone")
			{
				l_eType = Mesh::eCone;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[2].c_str())));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[3].c_str())));
			}
			else if (l_strType == "cylinder")
			{
				l_eType = Mesh::eCylinder;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[2].c_str())));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[3].c_str())));
			}
			else if (l_strType == "sphere")
			{
				l_eType = Mesh::eSphere;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[2].c_str())));
			}
			else if (l_strType == "icosaedron")
			{
				l_eType = Mesh::eIcosaedron;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[2].c_str())));
			}
			else if (l_strType == "plane")
			{
				l_eType = Mesh::ePlane;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[2].c_str()));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[3].c_str())));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[4].c_str())));
			}
			else if (l_strType == "torus")
			{
				l_eType = Mesh::eTorus;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[2].c_str()));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[3].c_str())));
				l_arraySizes.push_back( float( atof( l_arrayMeshInfos[4].c_str())));
			}

			if (l_eType != Mesh::eCustom)
			{
				p_pContext->pMesh = MeshManager::GetSingleton().CreateMesh( p_pContext->strName, l_arrayFaces, l_arraySizes, l_eType);
			}
			else
			{
				PARSING_ERROR( "Unknown mesh type : " + l_strType);
			}
		}
		else
		{
			PARSING_ERROR( "Mesh already initialised, ignoring this type directive");
		}
	}
	else
	{
		p_pContext->pMesh = MeshManager::GetSingleton().CreateMesh( p_pContext->strName, l_arrayFaces, l_arraySizes, l_eType);
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshFile)
{
	MeshLoader l_loader;
	p_pContext->pMesh = l_loader.LoadFromExtFileIO( p_pContext->pFile->GetFilePath() + "/" + p_strParams);

	if (p_pContext->pMesh == NULL)
	{
		PARSING_ERROR( "Can't load mesh file " + p_strParams);
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshMaterial)
{
	if (p_pContext->pMesh != NULL)
	{
		if (MaterialManager::GetSingleton().HasElement( p_strParams))
		{
			for (size_t i = 0 ; i < p_pContext->pMesh->GetNbSubmeshes() ; i++)
			{
				p_pContext->pMesh->GetSubmesh( i)->SetMaterial( p_strParams);
			}
		}
		else
		{
			PARSING_ERROR( "Material " + p_strParams + " does not exist");
		}
	}
	else
	{
		PARSING_ERROR( "Mesh not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshSubmesh)
{
	if (p_pContext->pMesh != NULL)
	{
		p_pContext->eSection = SceneFileContext::eSubmesh;
		p_pContext->pSubmesh = p_pContext->pMesh->CreateSubmesh( 0);

		return true;
	}
	else
	{
		PARSING_ERROR( "Mesh not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshEnd)
{
	p_pContext->eSection = SceneFileContext::eObject;

	if (p_pContext->pMesh != NULL)
	{
		p_pContext->pMesh->SetNormals();
		p_pContext->pGeometry->SetMesh( p_pContext->pMesh);
	}

	p_pContext->pMesh = NULL;
	p_pContext->strName2.clear();
    return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshMaterial)
{
	if (p_pContext->pSubmesh != NULL)
	{
		if (MaterialManager::GetSingleton().HasElement( p_strParams))
		{
			p_pContext->pSubmesh->SetMaterial( p_strParams);
		}
		else
		{
			PARSING_ERROR( "Material " + p_strParams + " does not exist");
		}
	}
	else
	{
		PARSING_ERROR( "Submesh not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshVertex)
{
	if (p_pContext->pSubmesh != NULL)
	{
		Vector3f l_vVector;

		if (PARSE_V3( l_vVector))
		{
			p_pContext->pSubmesh->AddVertex( l_vVector.x, l_vVector.y, l_vVector.z);
		}
	}
	else
	{
		PARSING_ERROR( "Submesh not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshSmoothingGroup)
{
	if (p_pContext->pSubmesh != NULL)
	{
		p_pContext->eSection = SceneFileContext::eSmoothingGroup;
		p_pContext->pSmoothingGroup = p_pContext->pSubmesh->AddSmoothingGroup();

		return true;
	}
	else
	{
		PARSING_ERROR( "Submesh not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshEnd)
{
	p_pContext->eSection = SceneFileContext::eMesh;
	if (p_pContext->pSubmesh != NULL)
	{
		p_pContext->pSubmesh->GenerateBuffers();
	}
	p_pContext->pSubmesh = NULL;
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SmoothingGroupFace)
{
	if (p_pContext->pSmoothingGroup != NULL)
	{
		Point3D<int> l_pt3Indices;
		StringArray l_arrayValues = p_strParams.Split( " ");
		p_pContext->pFace1 = NULL;
		p_pContext->pFace2 = NULL;

		if (l_arrayValues.size() >= 4)
		{
			Point4D<int> l_pt4Indices;

			if (PARSE_V4( l_pt4Indices))
			{
				p_pContext->pFace1 = p_pContext->pSubmesh->AddFace( l_pt4Indices.x, l_pt4Indices.y, l_pt4Indices.z, p_pContext->pSmoothingGroup->m_idGroup - 1);
				p_pContext->pFace2 = p_pContext->pSubmesh->AddFace( l_pt4Indices.y, l_pt4Indices.w, l_pt4Indices.z, p_pContext->pSmoothingGroup->m_idGroup - 1);
			}
		}
		else if (PARSE_V3( l_pt3Indices))
		{
			p_pContext->pFace1 = p_pContext->pSubmesh->AddFace( l_pt3Indices.x, l_pt3Indices.y, l_pt3Indices.z, p_pContext->pSmoothingGroup->m_idGroup - 1);
		}
	}
	else
	{
		PARSING_ERROR( "Smoothing Group not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SmoothingGroupFaceUV)
{
	if (p_pContext->pSmoothingGroup != NULL)
	{
		Point3D<int> l_pt3Indices;
		StringArray l_arrayValues = p_strParams.Split( " ");

		if (l_arrayValues.size() >= 8)
		{
			if (p_pContext->pFace1 != NULL)
			{
				p_pContext->pFace1->m_vertex1TexCoord.x = (float)atof( l_arrayValues[0].c_str());
				p_pContext->pFace1->m_vertex1TexCoord.y = (float)atof( l_arrayValues[1].c_str());
				p_pContext->pFace1->m_vertex2TexCoord.x = (float)atof( l_arrayValues[2].c_str());
				p_pContext->pFace1->m_vertex2TexCoord.y = (float)atof( l_arrayValues[3].c_str());
				p_pContext->pFace1->m_vertex3TexCoord.x = (float)atof( l_arrayValues[4].c_str());
				p_pContext->pFace1->m_vertex3TexCoord.y = (float)atof( l_arrayValues[5].c_str());
			}

			if (p_pContext->pFace2 != NULL)
			{
				p_pContext->pFace2->m_vertex1TexCoord.x = (float)atof( l_arrayValues[2].c_str());
				p_pContext->pFace2->m_vertex1TexCoord.y = (float)atof( l_arrayValues[3].c_str());
				p_pContext->pFace2->m_vertex2TexCoord.x = (float)atof( l_arrayValues[6].c_str());
				p_pContext->pFace2->m_vertex2TexCoord.y = (float)atof( l_arrayValues[7].c_str());
				p_pContext->pFace2->m_vertex3TexCoord.x = (float)atof( l_arrayValues[4].c_str());
				p_pContext->pFace2->m_vertex3TexCoord.y = (float)atof( l_arrayValues[5].c_str());
			}
		}
		else if (l_arrayValues.size() >= 6)
		{
			if (p_pContext->pFace1 != NULL)
			{
				p_pContext->pFace1->m_vertex1TexCoord.x = (float)atof( l_arrayValues[0].c_str());
				p_pContext->pFace1->m_vertex1TexCoord.y = (float)atof( l_arrayValues[1].c_str());
				p_pContext->pFace1->m_vertex2TexCoord.x = (float)atof( l_arrayValues[2].c_str());
				p_pContext->pFace1->m_vertex2TexCoord.y = (float)atof( l_arrayValues[3].c_str());
				p_pContext->pFace1->m_vertex3TexCoord.x = (float)atof( l_arrayValues[4].c_str());
				p_pContext->pFace1->m_vertex3TexCoord.y = (float)atof( l_arrayValues[5].c_str());
			}
		}
	}
	else
	{
		PARSING_ERROR( "Smoothing Group not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SmoothingGroupEnd)
{
	p_pContext->eSection = SceneFileContext::eSubmesh;
	p_pContext->pSmoothingGroup = NULL;
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MaterialPass)
{
	if (p_pContext->pMaterial != NULL)
	{
		p_pContext->eSection = SceneFileContext::ePass;
		p_pContext->pPass = p_pContext->pMaterial->CreatePass();

		return true;
	}
	else
	{
		PARSING_ERROR( "Material not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MaterialEnd)
{
	p_pContext->eSection = SceneFileContext::eNone;
	MaterialManager::GetSingleton().SetToInitialise( p_pContext->pMaterial);
	p_pContext->pMaterial = NULL;
	p_pContext->strName.clear();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAmbient)
{
	if (p_pContext->pPass != NULL)
	{
		Colour l_crColour;

		if (PARSE_V4( l_crColour))
		{
			p_pContext->pPass->SetAmbient( l_crColour);
		}
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassDiffuse)
{
	if (p_pContext->pPass != NULL)
	{
		Colour l_crColour;

		if (PARSE_V4( l_crColour))
		{
			p_pContext->pPass->SetDiffuse( l_crColour);
		}
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassSpecular)
{
	if (p_pContext->pPass != NULL)
	{
		Colour l_crColour;

		if (PARSE_V4( l_crColour))
		{
			p_pContext->pPass->SetSpecular( l_crColour);
		}
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassEmissive)
{
	if (p_pContext->pPass != NULL)
	{
		Colour l_crColour;

		if (PARSE_V4( l_crColour))
		{
			p_pContext->pPass->SetEmissive( l_crColour);
		}
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassShininess)
{
	if (p_pContext->pPass != NULL)
	{
		float l_fFloat = float( atof( p_strParams.c_str()));
		p_pContext->pPass->SetShininess( l_fFloat);
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlpha)
{
	if (p_pContext->pPass != NULL)
	{
		float l_fFloat = float( atof( p_strParams.c_str()));
		p_pContext->pPass->SetAlpha( l_fFloat);
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassBaseTexColour)
{
	if (p_pContext->pPass != NULL)
	{
		Colour l_crColour;

		if (PARSE_V4( l_crColour))
		{
			p_pContext->pPass->SetTexBaseColour( l_crColour.r, l_crColour.g, l_crColour.b, l_crColour.a);
		}
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassDoubleFace)
{
	if (p_pContext->pPass != NULL)
	{
		bool l_bDouble = p_strParams == "true";
		p_pContext->pPass->SetDoubleFace( l_bDouble);
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassTextureUnit)
{
	if (p_pContext->pPass != NULL)
	{
		p_pContext->eSection = SceneFileContext::eTextureUnit;
		p_pContext->pTextureUnit = new TextureUnit( RenderSystem::GetSingletonPtr()->CreateTextureRenderer());
		p_pContext->pPass->AddTextureUnit( p_pContext->pTextureUnit);
		return true;
	}
	else
	{
		PARSING_ERROR( "Pass not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassShader)
{
	if (p_pContext->pPass != NULL)
	{
		p_pContext->eSection = SceneFileContext::eShader;
		p_pContext->pShaderProgram = RenderSystem::GetSingletonPtr()->CreateShaderProgram( "", "", "");
		return true;
	}
	else
	{
		PARSING_ERROR( "Pass not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassEnd)
{
	p_pContext->eSection = SceneFileContext::eMaterial;
	p_pContext->pPass = NULL;
	p_pContext->strName.clear();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitImage)
{
	if (p_pContext->pTextureUnit != NULL)
	{
		if (FileBase::FileExists( p_pContext->pFile->GetFilePath() + "/" + p_strParams))
		{
			p_pContext->pTextureUnit->SetTexture2D( p_pContext->pFile->GetFilePath() + "/" + p_strParams);
		}
		else if (FileBase::FileExists( p_strParams))
		{
			p_pContext->pTextureUnit->SetTexture2D( p_strParams);
		}
		else
		{
			PARSING_ERROR( "File " + p_pContext->pFile->GetFilePath() + "/" + p_strParams + " not found, check the relativeness of the path");
		}
	}
	else
	{
		PARSING_ERROR( "Texture Unit not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitColour)
{
	if (p_pContext->pTextureUnit != NULL)
	{
		Colour l_crColour;

		if (PARSE_V4( l_crColour))
		{
			p_pContext->pTextureUnit->SetPrimaryColour( l_crColour.ptr());
		}
	}
	else
	{
		PARSING_ERROR( "Texture Unit not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitMapType)
{
	if (p_pContext->pTextureUnit != NULL)
	{
		TextureUnit::eMAP_MODE eMapMode = TextureUnit::eMAP_MODE( atoi( p_strParams.c_str()));

		p_pContext->pTextureUnit->SetTextureMapMode( eMapMode);
	}
	else
	{
		PARSING_ERROR( "Texture Unit not initialised");
	}
	
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitEnd)
{
	p_pContext->eSection = SceneFileContext::ePass;
	p_pContext->pTextureUnit = NULL;
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVertexProgram)
{
	if (p_pContext->pShaderProgram != NULL)
	{
	}
	else
	{
		PARSING_ERROR( "GPU Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderPixelProgram)
{
	if (p_pContext->pShaderProgram != NULL)
	{
	}
	else
	{
		PARSING_ERROR( "GPU Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderGeometryProgram)
{
	if (p_pContext->pShaderProgram != NULL)
	{
	}
	else
	{
		PARSING_ERROR( "GPU Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVertexFile)
{
	if (p_pContext->pShaderProgram != NULL)
	{
		p_pContext->pShaderProgram->SetVertexFileIO( p_pContext->pFile->GetFilePath() + "/" + p_strParams);
	}
	else
	{
		PARSING_ERROR( "GPU Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderPixelFile)
{
	if (p_pContext->pShaderProgram != NULL)
	{
		p_pContext->pShaderProgram->SetFragmentFileIO( p_pContext->pFile->GetFilePath() + "/" + p_strParams);
	}
	else
	{
		PARSING_ERROR( "GPU Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderGeometryFile)
{
	if (p_pContext->pShaderProgram != NULL)
	{
		p_pContext->pShaderProgram->SetGeometryFileIO( p_pContext->pFile->GetFilePath() + "/" + p_strParams);
	}
	else
	{
		PARSING_ERROR( "GPU Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVariable)
{
	if (p_pContext->pShaderProgram != NULL)
	{
		p_pContext->eSection = SceneFileContext::eShaderVariable;
		return true;
	}
	else
	{
		PARSING_ERROR( "GPU Program not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderEnd)
{
	p_pContext->eSection = SceneFileContext::ePass;
	p_pContext->pPass->SetShader( p_pContext->pShaderProgram);
	p_pContext->pShaderProgram = NULL;
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVariableType)
{
	if (p_pContext->pUniformVariable == NULL)
	{
		if (p_strParams == "int")
		{
			p_pContext->pUniformVariable = new OneUniformVariable<int>();
		}
		else if (p_strParams == "float")
		{
			p_pContext->pUniformVariable = new OneUniformVariable<float>();
		}
		else if (p_strParams == "vec2i")
		{
			p_pContext->pUniformVariable = new Point2DUniformVariable<int>();
		}
		else if (p_strParams == "vec3i")
		{
			p_pContext->pUniformVariable = new Point3DUniformVariable<int>();
		}
		else if (p_strParams == "vec4i")
		{
			p_pContext->pUniformVariable = new Point4DUniformVariable<int>();
		}
		else if (p_strParams == "vec2f")
		{
			p_pContext->pUniformVariable = new Point2DUniformVariable<float>();
		}
		else if (p_strParams == "vec3f")
		{
			p_pContext->pUniformVariable = new Point3DUniformVariable<float>();
		}
		else if (p_strParams == "vec4f")
		{
			p_pContext->pUniformVariable = new Point4DUniformVariable<float>();
		}
		else if (p_strParams == "mat2f")
		{
			p_pContext->pUniformVariable = new Matrix2UniformVariable<float>();
		}
		else if (p_strParams == "mat3f")
		{
			p_pContext->pUniformVariable = new Matrix3UniformVariable<float>();
		}
		else if (p_strParams == "mat4f")
		{
			p_pContext->pUniformVariable = new Matrix4UniformVariable<float>();
		}
		else
		{
			PARSING_ERROR( "Unknown variable type : " + p_strParams);
		}
	}
	else
	{
		PARSING_ERROR( "Variable type already set");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVariableName)
{
	if (p_pContext->pUniformVariable != NULL)
	{
		p_pContext->pUniformVariable->SetName( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVariableValue)
{
	if (p_pContext->pUniformVariable != NULL)
	{
		p_pContext->pUniformVariable->SetValue( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVariableEnd)
{
	p_pContext->eSection = SceneFileContext::eShader;
	p_pContext->pShaderProgram->AddUniformVariable( p_pContext->pUniformVariable);
	p_pContext->pUniformVariable = NULL;
	return false;
}