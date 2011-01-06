#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/scene/SceneFileParser_Parsers.h"
#include "Castor3D/scene/SceneFileParser.h"
#include "Castor3D/material/Material.h"
#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/material/Pass.h"
#include "Castor3D/material/TextureUnit.h"
#include "Castor3D/scene/Scene.h"
#include "Castor3D/scene/SceneManager.h"
#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/camera/Camera.h"
#include "Castor3D/light/Light.h"
#include "Castor3D/light/SpotLight.h"
#include "Castor3D/light/PointLight.h"
#include "Castor3D/main/MovableObject.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/primitives/Geometry.h"
#include "Castor3D/geometry/basic/SmoothingGroup.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/geometry/mesh/Mesh.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/geometry/mesh/MeshManager.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/shader/ShaderManager.h"
#include "Castor3D/shader/ShaderProgram.h"
#include "Castor3D/shader/ShaderObject.h"
#include "Castor3D/shader/FrameVariable.h"
#include "Castor3D/shader/Cg/CgShaderProgram.h"
#include "Castor3D/shader/Cg/CgShaderObject.h"
#include "Castor3D/shader/Cg/CgFrameVariable.h"



using namespace Castor3D;

void Castor3D :: SceneFileParser :: ParseError( const String & p_strError, SceneFileContextPtr p_pContext)
{
	StringStream l_streamError;
	l_streamError << "Error Line #" << p_pContext->ui64Line << " / " << p_strError;
	Logger::LogMessage( l_streamError.str());
}

void Castor3D :: SceneFileParser :: ParseWarning( const String & p_strWarning, SceneFileContextPtr p_pContext)
{
	StringStream l_streamWarning;
	l_streamWarning << "Warning Line #" << p_pContext->ui64Line << " / " << p_strWarning;
	Logger::LogMessage( l_streamWarning.str());
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootSceneName)
{
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootCamera)
{
	if ( ! p_pContext->pScene == NULL)
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
	if ( ! p_pContext->pScene == NULL)
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
	if ( ! p_pContext->pScene == NULL)
	{
		p_pContext->eSection = SceneFileContext::eNode;
		p_pContext->strName = p_strParams;
		p_pContext->pSceneNode = SceneNodePtr( p_pContext->pScene->CreateSceneNode( p_strParams));
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
	if ( ! p_pContext->pScene == NULL)
	{
		p_pContext->eSection = SceneFileContext::eMaterial;
		p_pContext->pMaterial = p_pContext->m_pMaterialManager->CreateMaterial( p_strParams, 0);
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
	if ( ! p_pContext->pScene == NULL)
	{
		p_pContext->eSection = SceneFileContext::eObject;
		p_pContext->strName = p_strParams;
		p_pContext->pGeometry = GeometryPtr( new Geometry( MeshPtr(), SceneNodePtr(), p_pContext->strName));
		return true;
	}
	else
	{
		PARSING_ERROR( "No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootAmbientLight)
{
	if ( ! p_pContext->pScene == NULL)
	{
		Colour l_clColour;

		if (PARSE_V4( float, l_clColour))
		{
			p_pContext->pScene->SetAmbientLight( l_clColour);
		}
	}
	else
	{
		PARSING_ERROR( "No scene initialised. Enter a scene name");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightParent)
{
	SceneNodePtr l_pParent = p_pContext->pScene->GetNode( p_strParams);

	if ( ! l_pParent == NULL)
	{
		p_pContext->pSceneNode = l_pParent;
	}
	else
	{
		PARSING_ERROR( "Node " + p_strParams + " does not exist");
	}

	if ( ! p_pContext->pLight == NULL)
	{
		p_pContext->pLight->Detach();
		p_pContext->pSceneNode->AttachObject( p_pContext->pLight.get());
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightType)
{
	if (p_pContext->pSceneNode != NULL)
	{
		if (p_strParams == "point_light")
		{
			p_pContext->eLightType = Light::ePoint;
			p_pContext->pLight = p_pContext->pScene->CreateLight( p_pContext->eLightType, p_pContext->strName, p_pContext->pSceneNode);
		}
		else if (p_strParams == "spot_light")
		{
			p_pContext->eLightType = Light::eSpot;
			p_pContext->pLight = p_pContext->pScene->CreateLight( p_pContext->eLightType, p_pContext->strName, p_pContext->pSceneNode);
		}
		else if (p_strParams == "directional")
		{
			p_pContext->eLightType = Light::eDirectional;
			p_pContext->pLight = p_pContext->pScene->CreateLight( p_pContext->eLightType, p_pContext->strName, p_pContext->pSceneNode);
		}
		else
		{
			PARSING_ERROR( "Unknown light type : " + p_strParams);
		}
	}
	else
	{
		PARSING_ERROR( "Light node undefined : " + p_strParams);
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightPosition)
{
	if ( ! p_pContext->pLight == NULL)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			p_pContext->pLight->GetParent()->SetPosition( l_vVector[0], l_vVector[1], l_vVector[2]);
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
	if ( ! p_pContext->pLight == NULL)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			p_pContext->pLight->SetAmbient( l_vVector.ptr());
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
	if ( ! p_pContext->pLight == NULL)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			p_pContext->pLight->SetDiffuse( l_vVector.ptr());
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
	if ( ! p_pContext->pLight == NULL)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			p_pContext->pLight->SetSpecular( l_vVector.ptr());
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
	if ( ! p_pContext->pLight == NULL)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			if (p_pContext->eLightType == Light::ePoint)
			{
				static_pointer_cast<PointLight>( p_pContext->pLight)->SetAttenuation( l_vVector);
			}
			else if (p_pContext->eLightType == Light::eSpot)
			{
				static_pointer_cast<SpotLight>( p_pContext->pLight)->SetAttenuation( l_vVector);
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
	if ( ! p_pContext->pLight == NULL)
	{
		float l_fFloat = float( atof( p_strParams.c_str()));

		if (p_pContext->eLightType == Light::eSpot)
		{
			static_pointer_cast<SpotLight>( p_pContext->pLight)->SetCutOff( l_fFloat);
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
	if ( ! p_pContext->pLight == NULL)
	{
		float l_fFloat = float( atof( p_strParams.c_str()));

		if (p_pContext->eLightType == Light::eSpot)
		{
			static_pointer_cast<SpotLight>( p_pContext->pLight)->SetExponent( l_fFloat);
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
	if ( ! p_pContext->pLight == NULL)
	{
		Quaternion l_qQuaternion;

		if (PARSE_V4( real, l_qQuaternion))
		{
			if (p_pContext->eLightType == Light::eSpot)
			{
				static_pointer_cast<SpotLight>( p_pContext->pLight)->GetParent()->Rotate( l_qQuaternion);
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
	p_pContext->pLight.reset();
	p_pContext->strName.clear();
	p_pContext->pSceneNode.reset();
    return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeParent)
{
	if ( ! p_pContext->pSceneNode == NULL)
	{
		SceneNodePtr l_pParent = p_pContext->pScene->GetNode( p_strParams);

		if ( ! l_pParent == NULL)
		{
			p_pContext->pSceneNode->AttachTo( l_pParent.get());
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
	if ( ! p_pContext->pSceneNode == NULL)
	{
		Point3r l_vVector;

		if (PARSE_V3( real, l_vVector))
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
	if ( ! p_pContext->pSceneNode == NULL)
	{
		Quaternion l_qQuaternion;

		if (PARSE_V4( real, l_qQuaternion))
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
	if ( ! p_pContext->pSceneNode == NULL)
	{
		Point3r l_vVector;

		if (PARSE_V3( real, l_vVector))
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
	p_pContext->pSceneNode.reset();
	p_pContext->strName.clear();
    return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectParent)
{
	if ( ! p_pContext->pGeometry == NULL)
	{
		SceneNodePtr l_pParent = p_pContext->pScene->GetNode( p_strParams);

		if ( ! l_pParent == NULL)
		{
			l_pParent->AttachObject( p_pContext->pGeometry.get());
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
	p_pContext->pGeometry.reset();
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
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[1].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[2].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[3].c_str())));
			}
			else if (l_strType == "cone")
			{
				l_eType = Mesh::eCone;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[2].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[3].c_str())));
			}
			else if (l_strType == "cylinder")
			{
				l_eType = Mesh::eCylinder;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[2].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[3].c_str())));
			}
			else if (l_strType == "sphere")
			{
				l_eType = Mesh::eSphere;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[2].c_str())));
			}
			else if (l_strType == "icosaedron")
			{
				l_eType = Mesh::eIcosaedron;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[2].c_str())));
			}
			else if (l_strType == "plane")
			{
				l_eType = Mesh::ePlane;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[2].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[3].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[4].c_str())));
			}
			else if (l_strType == "torus")
			{
				l_eType = Mesh::eTorus;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[2].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[3].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[4].c_str())));
			}

			if (l_eType != Mesh::eCustom)
			{
				p_pContext->pMesh = p_pContext->m_pMeshManager->CreateMesh( p_pContext->strName, l_arrayFaces, l_arraySizes, l_eType);
			}
			else
			{
				PARSING_ERROR( "Unknown mesh type : " + l_strType);
			}
		}
		else
		{
			PARSING_ERROR( "Mesh already initialised, ignoring that type directive");
		}
	}
	else
	{
		p_pContext->pMesh = p_pContext->m_pMeshManager->CreateMesh( p_pContext->strName, l_arrayFaces, l_arraySizes, l_eType);
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshFile)
{
	MeshLoader l_loader;
	p_pContext->pMesh = l_loader.LoadFromFile( p_pContext->m_pMeshManager, p_pContext->pFile->GetFilePath() + "/" + p_strParams);

	if (p_pContext->pMesh == NULL)
	{
		PARSING_ERROR( "Can't load mesh file " + p_strParams);
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshMaterial)
{
	if ( ! p_pContext->pMesh == NULL)
	{
		if (p_pContext->m_pMaterialManager->HasElement( p_strParams))
		{
			for (size_t i = 0 ; i < p_pContext->pMesh->GetNbSubmeshes() ; i++)
			{
				p_pContext->pMesh->GetSubmesh( i)->SetMaterial( p_pContext->m_pMaterialManager->GetElement( p_strParams));
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
	if ( ! p_pContext->pMesh == NULL)
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

	if ( ! p_pContext->pMesh == NULL)
	{
		p_pContext->pMesh->ComputeNormals();
		p_pContext->pGeometry->SetMesh( p_pContext->pMesh);
	}

	p_pContext->pMesh.reset();
	p_pContext->strName2.clear();
    return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshMaterial)
{
	if ( ! p_pContext->pSubmesh == NULL)
	{
		if (p_pContext->m_pMaterialManager->HasElement( p_strParams))
		{
			p_pContext->pSubmesh->SetMaterial( p_pContext->m_pMaterialManager->GetElement( p_strParams));
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
	if ( ! p_pContext->pSubmesh == NULL)
	{
		Point3r l_vVector;

		if (PARSE_V3( real, l_vVector))
		{
			p_pContext->pSubmesh->AddPoint( l_vVector);
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
	if ( ! p_pContext->pSubmesh == NULL)
	{
		p_pContext->eSection = SceneFileContext::eSmoothingGroup;
		p_pContext->uiSmoothingGroup = p_pContext->pSubmesh->AddSmoothingGroup()->GetGroupID();

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
	if ( ! p_pContext->pSubmesh == NULL)
	{
		p_pContext->pSubmesh->GenerateBuffers();
	}
	p_pContext->pSubmesh.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SmoothingGroupFace)
{
	if (p_pContext->uiSmoothingGroup != 0)
	{
		Point3i l_pt3Indices;
		StringArray l_arrayValues = p_strParams.Split( " ");
		p_pContext->iFace1 = -1;
		p_pContext->iFace2 = -1;

		if (l_arrayValues.size() >= 4)
		{
			Point4i l_pt4Indices;

			if (PARSE_V4( int, l_pt4Indices))
			{
				p_pContext->pSubmesh->AddQuadFace( l_pt4Indices[0], l_pt4Indices[1], l_pt4Indices[2], l_pt4Indices[3], p_pContext->uiSmoothingGroup - 1);
				p_pContext->iFace1 = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetNbFaces() - 2;
				p_pContext->iFace2 = p_pContext->iFace1 + 1;
			}
		}
		else if (PARSE_V3( int, l_pt3Indices))
		{
			p_pContext->pSubmesh->AddFace( l_pt3Indices[0], l_pt3Indices[1], l_pt3Indices[2], p_pContext->uiSmoothingGroup - 1);
			p_pContext->iFace1 = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetNbFaces() - 1;
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
	if (p_pContext->uiSmoothingGroup != 0)
	{
		Point3i l_pt3Indices;
		StringArray l_arrayValues = p_strParams.Split( " ", 20);

		if (l_arrayValues.size() >= 6)
		{
			if (p_pContext->iFace1 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace1);
				l_face->SetVertexTexCoords( 0, l_arrayValues[0].ToReal(), l_arrayValues[1].ToReal(), true);
				l_face->SetVertexTexCoords( 1, l_arrayValues[2].ToReal(), l_arrayValues[3].ToReal(), true);
				l_face->SetVertexTexCoords( 2, l_arrayValues[4].ToReal(), l_arrayValues[5].ToReal(), true);
			}
		}

		if (l_arrayValues.size() >= 8)
		{
			if (p_pContext->iFace2 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace2);
				l_face->SetVertexTexCoords( 0, l_arrayValues[0].ToReal(), l_arrayValues[1].ToReal(), true);
				l_face->SetVertexTexCoords( 1, l_arrayValues[4].ToReal(), l_arrayValues[5].ToReal(), true);
				l_face->SetVertexTexCoords( 2, l_arrayValues[6].ToReal(), l_arrayValues[7].ToReal(), true);
			}
		}
	}
	else
	{
		PARSING_ERROR( "Smoothing Group not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SmoothingGroupFaceNormals)
{
	if (p_pContext->uiSmoothingGroup != 0)
	{
		Point3i l_pt3Indices;
		StringArray l_arrayValues = p_strParams.Split( " ", 20);

		if (l_arrayValues.size() >= 9)
		{
			if (p_pContext->iFace1 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace1);
				l_face->SetVertexNormal( 0, l_arrayValues[0].ToReal(), l_arrayValues[1].ToReal(), l_arrayValues[2].ToReal(), true);
				l_face->SetVertexNormal( 1, l_arrayValues[3].ToReal(), l_arrayValues[4].ToReal(), l_arrayValues[5].ToReal(), true);
				l_face->SetVertexNormal( 2, l_arrayValues[6].ToReal(), l_arrayValues[7].ToReal(), l_arrayValues[8].ToReal(), true);
			}
		}

		if (l_arrayValues.size() >= 12)
		{
			if (p_pContext->iFace2 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace2);
				l_face->SetVertexNormal( 0, l_arrayValues[0].ToReal(), l_arrayValues[ 1].ToReal(), l_arrayValues[ 2].ToReal(), true);
				l_face->SetVertexNormal( 1, l_arrayValues[6].ToReal(), l_arrayValues[ 7].ToReal(), l_arrayValues[ 8].ToReal(), true);
				l_face->SetVertexNormal( 2, l_arrayValues[9].ToReal(), l_arrayValues[10].ToReal(), l_arrayValues[11].ToReal(), true);
			}
		}
	}
	else
	{
		PARSING_ERROR( "Smoothing Group not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SmoothingGroupFaceTangents)
{
	if (p_pContext->uiSmoothingGroup != 0)
	{
		Point3i l_pt3Indices;
		StringArray l_arrayValues = p_strParams.Split( " ", 20);

		if (l_arrayValues.size() >= 9)
		{
			if (p_pContext->iFace1 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace1);
				l_face->SetVertexTangent( 0, l_arrayValues[0].ToReal(), l_arrayValues[1].ToReal(), l_arrayValues[2].ToReal(), true);
				l_face->SetVertexTangent( 1, l_arrayValues[3].ToReal(), l_arrayValues[4].ToReal(), l_arrayValues[5].ToReal(), true);
				l_face->SetVertexTangent( 2, l_arrayValues[6].ToReal(), l_arrayValues[7].ToReal(), l_arrayValues[8].ToReal(), true);
			}
		}

		if (l_arrayValues.size() >= 12)
		{
			if (p_pContext->iFace2 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace2);
				l_face->SetVertexTangent( 0, l_arrayValues[0].ToReal(), l_arrayValues[ 1].ToReal(), l_arrayValues[ 2].ToReal(), true);
				l_face->SetVertexTangent( 1, l_arrayValues[6].ToReal(), l_arrayValues[ 7].ToReal(), l_arrayValues[ 8].ToReal(), true);
				l_face->SetVertexTangent( 2, l_arrayValues[9].ToReal(), l_arrayValues[10].ToReal(), l_arrayValues[11].ToReal(), true);
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
	p_pContext->uiSmoothingGroup = 0;
	p_pContext->iFace1 = -1;
	p_pContext->iFace2 = -1;
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MaterialPass)
{
	if ( ! p_pContext->pMaterial == NULL)
	{
		p_pContext->eSection = SceneFileContext::ePass;
		p_pContext->pMaterial->CreatePass();
		p_pContext->uiPass = p_pContext->pMaterial->GetNbPasses() - 1;

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
	p_pContext->m_pMaterialManager->SetToInitialise( p_pContext->pMaterial);
	p_pContext->pMaterial.reset();
	p_pContext->strName.clear();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAmbient)
{
	if (p_pContext->uiPass >= 0)
	{
		Colour l_crColour;

		if (PARSE_V4( float, l_crColour))
		{
			p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetAmbient( l_crColour);
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
	if (p_pContext->uiPass >= 0)
	{
		Colour l_crColour;

		if (PARSE_V4( float, l_crColour))
		{
			p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetDiffuse( l_crColour);
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
	if (p_pContext->uiPass >= 0)
	{
		Colour l_crColour;

		if (PARSE_V4( float, l_crColour))
		{
			p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetSpecular( l_crColour);
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
	if (p_pContext->uiPass >= 0)
	{
		Colour l_crColour;

		if (PARSE_V4( float, l_crColour))
		{
			p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetEmissive( l_crColour);
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
	if (p_pContext->uiPass >= 0)
	{
		float l_fFloat = float( atof( p_strParams.c_str()));
		p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetShininess( l_fFloat);
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlpha)
{
	if (p_pContext->uiPass >= 0)
	{
		float l_fFloat = float( atof( p_strParams.c_str()));
		p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetAlpha( l_fFloat);
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassBaseTexColour)
{
	if (p_pContext->uiPass >= 0)
	{
		Colour l_crColour;

		if (PARSE_V4( float, l_crColour))
		{
			p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetTexBaseColour( l_crColour);
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
	if (p_pContext->uiPass >= 0)
	{
		bool l_bDouble = p_strParams == "true";
		p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetDoubleFace( l_bDouble);
	}
	else
	{
		PARSING_ERROR( "Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassTextureUnit)
{
	if (p_pContext->uiPass >= 0)
	{
		p_pContext->eSection = SceneFileContext::eTextureUnit;
		p_pContext->pTextureUnit = p_pContext->pMaterial->GetPass( p_pContext->uiPass)->AddTextureUnit();
		return true;
	}
	else
	{
		PARSING_ERROR( "Pass not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassGlShader)
{
	if (p_pContext->uiPass >= 0)
	{
		p_pContext->eSection = SceneFileContext::eGlShader;
		p_pContext->pShaderProgram = p_pContext->m_pShaderManager->CreateShaderProgramFromFiles( C3DEmptyString, C3DEmptyString, C3DEmptyString);
		return true;
	}
	else
	{
		PARSING_ERROR( "Pass not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassCgShader)
{
	if (p_pContext->uiPass >= 0)
	{
		p_pContext->eSection = SceneFileContext::eCgShader;
		p_pContext->pCgShaderProgram = p_pContext->m_pShaderManager->CreateCgShaderProgramFromFiles( C3DEmptyString, C3DEmptyString, C3DEmptyString);
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
	p_pContext->uiPass = -1;
	p_pContext->strName.clear();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitImage)
{
	if ( ! p_pContext->pTextureUnit == NULL)
	{
		if (File::FileExists( p_pContext->pFile->GetFilePath() + "/" + p_strParams))
		{
			p_pContext->pTextureUnit->SetTexture2D( p_pContext->m_pImageManager->CreateImage( p_pContext->pFile->GetFilePath() + "/" + p_strParams, p_pContext->pFile->GetFilePath() + "/" + p_strParams));
		}
		else if (File::FileExists( p_strParams))
		{
			p_pContext->pTextureUnit->SetTexture2D( p_pContext->m_pImageManager->CreateImage( p_strParams, p_strParams));
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
	if ( ! p_pContext->pTextureUnit == NULL)
	{
		Colour l_crColour;

		if (PARSE_V4( float, l_crColour))
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
	if ( ! p_pContext->pTextureUnit == NULL)
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
	p_pContext->pTextureUnit.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlVertexShader)
{
	if ( ! p_pContext->pShaderProgram == NULL)
	{
		p_pContext->eSection = SceneFileContext::eGlShaderProgram;
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetVertexProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlPixelShader)
{
	if ( ! p_pContext->pShaderProgram == NULL)
	{
		p_pContext->eSection = SceneFileContext::eGlShaderProgram;
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetFragmentProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlGeometryShader)
{
	if ( ! p_pContext->pShaderProgram == NULL)
	{
		p_pContext->eSection = SceneFileContext::eGlShaderProgram;
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetGeometryProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderEnd)
{
	p_pContext->eSection = SceneFileContext::ePass;
	p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetShader( p_pContext->pShaderProgram);
	p_pContext->pShaderProgram.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramFile)
{
	if ( ! p_pContext->pShaderObject == NULL)
	{
		p_pContext->pShaderObject->SetFile( p_pContext->pFile->GetFilePath() + "/" + p_strParams);
	}
	else
	{
		PARSING_ERROR( "Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramText)
{
	if ( ! p_pContext->pShaderObject == NULL)
	{
	}
	else
	{
		PARSING_ERROR( "Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramVariable)
{
	if ( ! p_pContext->pShaderObject == NULL)
	{
		p_pContext->eSection = SceneFileContext::eGlShaderVariable;
		return true;
	}
	else
	{
		PARSING_ERROR( "GPU Program not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramEnd)
{
	p_pContext->eSection = SceneFileContext::eGlShader;
	p_pContext->pShaderObject.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableType)
{
	if (p_pContext->pFrameVariable == NULL)
	{
		if (p_strParams == "int")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new OneFrameVariable<int>());
		}
		else if (p_strParams == "uint")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new OneFrameVariable<unsigned int>());
		}
		else if (p_strParams == "float")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new OneFrameVariable<float>());
		}
		else if (p_strParams == "vec1i")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<int, 1>());
		}
		else if (p_strParams == "vec2i")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<int, 2>());
		}
		else if (p_strParams == "vec3i")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<int,3>());
		}
		else if (p_strParams == "vec4i")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<int, 4>());
		}
		else if (p_strParams == "vec1ui")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<unsigned int, 1>());
		}
		else if (p_strParams == "vec2ui")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<unsigned int, 2>());
		}
		else if (p_strParams == "vec3ui")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<unsigned int,3>());
		}
		else if (p_strParams == "vec4ui")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<unsigned int, 4>());
		}
		else if (p_strParams == "vec2f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<float, 2>());
		}
		else if (p_strParams == "vec3f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<float, 3>());
		}
		else if (p_strParams == "vec4f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new PointFrameVariable<float, 4>());
		}
		else if (p_strParams == "mat2x2f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 2, 2>());
		}
		else if (p_strParams == "mat2x3f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 2, 3>());
		}
		else if (p_strParams == "mat2x4f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 2, 4>());
		}
		else if (p_strParams == "mat3x2f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 3, 2>());
		}
		else if (p_strParams == "mat3x3f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 3, 3>());
		}
		else if (p_strParams == "mat3x4f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 3, 4>());
		}
		else if (p_strParams == "mat4x2f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 4, 2>());
		}
		else if (p_strParams == "mat4x3f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 4, 3>());
		}
		else if (p_strParams == "mat4x4f")
		{
			p_pContext->pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 4, 4>());
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

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableName)
{
	if ( ! p_pContext->pFrameVariable == NULL)
	{
		p_pContext->pFrameVariable->SetName( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableValue)
{
	if ( ! p_pContext->pFrameVariable == NULL)
	{
		p_pContext->pFrameVariable->SetValue( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableEnd)
{
	p_pContext->eSection = SceneFileContext::eGlShaderProgram;
	p_pContext->pShaderProgram->AddFrameVariable( p_pContext->pFrameVariable, p_pContext->pShaderObject);
	p_pContext->pFrameVariable.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgVertexShader)
{
	if ( ! p_pContext->pCgShaderProgram == NULL)
	{
		p_pContext->eSection = SceneFileContext::eCgShaderProgram;
		p_pContext->pCgShaderObject = p_pContext->pCgShaderProgram->GetVertexProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Cg Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgPixelShader)
{
	if ( ! p_pContext->pCgShaderProgram == NULL)
	{
		p_pContext->eSection = SceneFileContext::eCgShaderProgram;
		p_pContext->pCgShaderObject = p_pContext->pCgShaderProgram->GetFragmentProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Cg Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgGeometryShader)
{
	if ( ! p_pContext->pCgShaderProgram == NULL)
	{
		p_pContext->eSection = SceneFileContext::eCgShaderProgram;
		p_pContext->pCgShaderObject = p_pContext->pCgShaderProgram->GetGeometryProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Cg Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderEnd)
{
	p_pContext->eSection = SceneFileContext::ePass;
	p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetShader( p_pContext->pCgShaderProgram);
	p_pContext->pCgShaderProgram.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramFile)
{
	if ( ! p_pContext->pCgShaderObject == NULL)
	{
		p_pContext->pCgShaderObject->SetFile( p_pContext->pFile->GetFilePath() + "/" + p_strParams);
	}
	else
	{
		PARSING_ERROR( "Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramText)
{
	if ( ! p_pContext->pCgShaderObject == NULL)
	{
	}
	else
	{
		PARSING_ERROR( "Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramVariable)
{
	if ( ! p_pContext->pCgShaderObject == NULL)
	{
		p_pContext->eSection = SceneFileContext::eCgShaderVariable;
		return true;
	}
	else
	{
		PARSING_ERROR( "GPU Program not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramEnd)
{
	p_pContext->eSection = SceneFileContext::eCgShader;
	p_pContext->pCgShaderObject.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderVariableType)
{
	if (p_pContext->pCgFrameVariable == NULL)
	{
		if (p_strParams == "float")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgOneFrameVariable<float>());
		}
		else if (p_strParams == "double")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgOneFrameVariable<double>());
		}
		else if (p_strParams == "vec1f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgPointFrameVariable<float, 1>());
		}
		else if (p_strParams == "vec2f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgPointFrameVariable<float, 2>());
		}
		else if (p_strParams == "vec3f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgPointFrameVariable<float, 3>());
		}
		else if (p_strParams == "vec4f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgPointFrameVariable<float, 4>());
		}
		else if (p_strParams == "vec1d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgPointFrameVariable<double, 1>());
		}
		else if (p_strParams == "vec2d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgPointFrameVariable<double, 2>());
		}
		else if (p_strParams == "vec3d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgPointFrameVariable<double, 3>());
		}
		else if (p_strParams == "vec4d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgPointFrameVariable<double, 4>());
		}
		else if (p_strParams == "mat2x2f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<float, 2, 2>());
		}
		else if (p_strParams == "mat2x3f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<float, 2, 3>());
		}
		else if (p_strParams == "mat2x4f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<float, 2, 4>());
		}
		else if (p_strParams == "mat3x2f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<float, 3, 2>());
		}
		else if (p_strParams == "mat3x3f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<float, 3, 3>());
		}
		else if (p_strParams == "mat3x4f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<float, 3, 4>());
		}
		else if (p_strParams == "mat4x2f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<float, 4, 2>());
		}
		else if (p_strParams == "mat4x3f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<float, 4, 3>());
		}
		else if (p_strParams == "mat4x4f")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<float, 4, 4>());
		}
		else if (p_strParams == "mat2x2d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<double, 2, 2>());
		}
		else if (p_strParams == "mat2x3d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<double, 2, 3>());
		}
		else if (p_strParams == "mat2x4d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<double, 2, 4>());
		}
		else if (p_strParams == "mat3x2d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<double, 3, 2>());
		}
		else if (p_strParams == "mat3x3d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<double, 3, 3>());
		}
		else if (p_strParams == "mat3x4d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<double, 3, 4>());
		}
		else if (p_strParams == "mat4x2d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<double, 4, 2>());
		}
		else if (p_strParams == "mat4x3d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<double, 4, 3>());
		}
		else if (p_strParams == "mat4x4d")
		{
			p_pContext->pCgFrameVariable = CgFrameVariablePtr( new CgMatrixFrameVariable<double, 4, 4>());
		}
		else
		{
			PARSING_ERROR( "Unknown Cg variable type : " + p_strParams);
		}
	}
	else
	{
		PARSING_ERROR( "Cg Variable type already set");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderVariableName)
{
	if ( ! p_pContext->pCgFrameVariable == NULL)
	{
		p_pContext->pCgFrameVariable->SetName( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Cg Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderVariableValue)
{
	if ( ! p_pContext->pCgFrameVariable == NULL)
	{
		p_pContext->pCgFrameVariable->SetValue( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Cg Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderVariableEnd)
{
	p_pContext->eSection = SceneFileContext::eCgShaderProgram;
	p_pContext->pCgShaderProgram->AddFrameVariable( p_pContext->pCgFrameVariable, p_pContext->pCgShaderObject);
	p_pContext->pCgFrameVariable.reset();
	return false;
}