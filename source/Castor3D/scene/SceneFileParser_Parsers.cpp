#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/scene/SceneFileParser_Parsers.h"
#include "Castor3D/scene/SceneFileParser.h"
#include "Castor3D/material/Material.h"
#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/material/Pass.h"
#include "Castor3D/material/TextureUnit.h"
#include "Castor3D/material/TextureEnvironment.h"
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
#include "Castor3D/overlay/Overlay.h"
#include "Castor3D/overlay/OverlayManager.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/shader/ShaderManager.h"
#include "Castor3D/shader/ShaderProgram.h"
#include "Castor3D/shader/ShaderObject.h"
#include "Castor3D/shader/FrameVariable.h"
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
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eCamera;
		p_pContext->eViewportType = Viewport::eTYPE( -1);
		p_pContext->pSceneNode.reset();
		p_pContext->strName = p_strParams;
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <camera> : No scene initialised. Enter a scene name");
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
		PARSING_ERROR( "Directive <light> : No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootSceneNode)
{
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eNode;
		p_pContext->strName = p_strParams;
		p_pContext->pSceneNode = SceneNodePtr( p_pContext->pScene->CreateSceneNode( p_strParams, p_pContext->m_pGeneralParentNode.get()));
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <scene_node> : No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootMaterial)
{
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eMaterial;
		p_pContext->pMaterial = p_pContext->m_pMaterialManager->CreateMaterial( p_strParams, 0);
		p_pContext->strName = p_strParams;
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <material> : No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootObject)
{
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eObject;
		p_pContext->strName = p_strParams;
		p_pContext->pGeometry = GeometryPtr( new Geometry( p_pContext->pScene.get(), MeshPtr(), p_pContext->m_pGeneralParentNode, p_pContext->strName));
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <object> : No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootFont)
{
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eFont;
		p_pContext->strName = p_strParams;
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <font> : No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootTextOverlay)
{
	if (p_pContext->pScene != NULL)
	{
		p_pContext->eSection = SceneFileContext::eTextOverlay;
		p_pContext->ePrevSection = SceneFileContext::eNone;
		p_pContext->pTextOverlay = OverlayManager::GetSingleton().CreateOverlay<TextOverlay>( p_strParams, p_pContext->pOverlay);
		p_pContext->pOverlay = p_pContext->pTextOverlay.get();
		p_pContext->pOverlay->SetVisible( false);
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <text_overlay> : No scene initialised. Enter a scene name");
		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootAmbientLight)
{
	if (p_pContext->pScene != NULL)
	{
		Colour l_clColour;

		if (PARSE_V4( float, l_clColour))
		{
			p_pContext->pScene->SetAmbientLight( l_clColour);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <ambient_light> : No scene initialised. Enter a scene name");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightParent)
{
	SceneNodePtr l_pParent = p_pContext->pScene->GetNode( p_strParams);

	if (l_pParent != NULL)
	{
		p_pContext->pSceneNode = l_pParent;
	}
	else
	{
		PARSING_ERROR( "Directive <light::parent> : Node " + p_strParams + " does not exist");
	}

	if (p_pContext->pLight != NULL)
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
			p_pContext->pLight = p_pContext->pScene->CreateLight<PointLight>( p_pContext->strName, p_pContext->pSceneNode);
		}
		else if (p_strParams == "spot_light")
		{
			p_pContext->eLightType = Light::eSpot;
			p_pContext->pLight = p_pContext->pScene->CreateLight<SpotLight>( p_pContext->strName, p_pContext->pSceneNode);
		}
		else if (p_strParams == "directional")
		{
			p_pContext->eLightType = Light::eDirectional;
			p_pContext->pLight = p_pContext->pScene->CreateLight<DirectionalLight>( p_pContext->strName, p_pContext->pSceneNode);
		}
		else
		{
			PARSING_ERROR( "Directive <light::type> : Unknown light type : " + p_strParams);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <light::type> : Light node undefined : " + p_strParams);
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightPosition)
{
	if (p_pContext->pLight != NULL)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			p_pContext->pLight->GetParent()->SetPosition( l_vVector[0], l_vVector[1], l_vVector[2]);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <light::position> : Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightAmbient)
{
	if (p_pContext->pLight != NULL)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			p_pContext->pLight->SetAmbient( l_vVector.ptr());
		}
	}
	else
	{
		PARSING_ERROR( "Directive <light::ambient> : Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightDiffuse)
{
	if (p_pContext->pLight != NULL)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			p_pContext->pLight->SetDiffuse( l_vVector.ptr());
		}
	}
	else
	{
		PARSING_ERROR( "Directive <light::diffuse> : Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightSpecular)
{
	if (p_pContext->pLight != NULL)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			p_pContext->pLight->SetSpecular( l_vVector.ptr());
		}
	}
	else
	{
		PARSING_ERROR( "Directive <light::specular> : Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightAttenuation)
{
	if (p_pContext->pLight != NULL)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			if (p_pContext->eLightType == Light::ePoint)
			{
				reinterpret_cast<PointLight *>( p_pContext->pLight.get())->SetAttenuation( l_vVector);
			}
			else if (p_pContext->eLightType == Light::eSpot)
			{
				reinterpret_cast<SpotLight *>( p_pContext->pLight.get())->SetAttenuation( l_vVector);
			}
			else
			{
				PARSING_ERROR( "Directive <light::attenuation> : Wrong type of light to apply attenuation components, needs spotlight or pointlight");
			}
		}
	}
	else
	{
		PARSING_ERROR( "Directive <light::attenuation> : Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightCutOff)
{
	if (p_pContext->pLight != NULL)
	{
		float l_fFloat = p_strParams.to_float();

		if (p_pContext->eLightType == Light::eSpot)
		{
			reinterpret_cast<SpotLight *>( p_pContext->pLight.get())->SetCutOff( l_fFloat);
		}
		else
		{
			PARSING_ERROR( "Directive <light::cut_off> : Wrong type of light to apply a cut off, needs spotlight");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <light::cut_off> : Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightExponent)
{
	if (p_pContext->pLight != NULL)
	{
		float l_fFloat = p_strParams.to_float();

		if (p_pContext->eLightType == Light::eSpot)
		{
			reinterpret_cast<SpotLight *>( p_pContext->pLight.get())->SetExponent( l_fFloat);
		}
		else
		{
			PARSING_ERROR( "Directive <light::exponent> : Wrong type of light to apply an exponent, needs spotlight");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <light::exponent> : Light not initialised. Have you set it's type ?");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightOrientation)
{
	if (p_pContext->pLight != NULL)
	{
		Quaternion l_qQuaternion;

		if (PARSE_V4( real, l_qQuaternion))
		{
			if (p_pContext->eLightType == Light::eSpot)
			{
				reinterpret_cast<SpotLight *>( p_pContext->pLight.get())->GetParent()->Rotate( l_qQuaternion);
			}
			else
			{
				PARSING_ERROR( "Directive <light::orientation> : Wrong type of light to apply an exponent, needs spotlight");
			}
		}
	}
	else
	{
		PARSING_ERROR( "Directive <light::orientation> : Light not initialised. Have you set it's type ?");
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
	if (p_pContext->pSceneNode != NULL)
	{
		SceneNodePtr l_pParent = p_pContext->pScene->GetNode( p_strParams);

		if (l_pParent != NULL)
		{
			p_pContext->pSceneNode->AttachTo( l_pParent.get());
		}
		else
		{
			PARSING_ERROR( "Directive <scene_node::parent> : Node " + p_strParams + " does not exist");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <scene_node::parent> : Scene node not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodePosition)
{
	if (p_pContext->pSceneNode != NULL)
	{
		Point3r l_vVector;

		if (PARSE_V3( real, l_vVector))
		{
			p_pContext->pSceneNode->SetPosition( l_vVector);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <scene_node::position> : Scene node not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeOrientation)
{
	if (p_pContext->pSceneNode != NULL)
	{
		Quaternion l_qQuaternion;

		if (PARSE_V4( real, l_qQuaternion))
		{
			p_pContext->pSceneNode->SetOrientation( l_qQuaternion);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <scene_node::orientation> : Scene node not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeScale)
{
	if (p_pContext->pSceneNode != NULL)
	{
		Point3r l_vVector;

		if (PARSE_V3( real, l_vVector))
		{
			p_pContext->pSceneNode->SetScale( l_vVector);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <scene_node::scale> : Scene node not initialised.");
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
	if (p_pContext->pGeometry != NULL)
	{
		SceneNodePtr l_pParent = p_pContext->pScene->GetNode( p_strParams);

		if (l_pParent != NULL)
		{
			l_pParent->AttachObject( p_pContext->pGeometry.get());
		}
		else
		{
			PARSING_ERROR( "Directive <object::parent> : Node " + p_strParams + " does not exist");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <object::parent> : Geometry not initialised.");
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
	eMESH_TYPE l_eType = eCustom;
	UIntArray l_arrayFaces;
	FloatArray l_arraySizes;

	if (l_strType != "custom")
	{
		if (p_pContext->pMesh == NULL)
		{
			StringArray l_arrayMeshInfos = p_strParams.split( " ");
			l_strType = l_arrayMeshInfos[0];

			if (l_strType == "cube")
			{
				l_eType = eCube;
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[1].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[2].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[3].c_str())));
			}
			else if (l_strType == "cone")
			{
				l_eType = eCone;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[2].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[3].c_str())));
			}
			else if (l_strType == "cylinder")
			{
				l_eType = eCylinder;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[2].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[3].c_str())));
			}
			else if (l_strType == "sphere")
			{
				l_eType = eSphere;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[2].c_str())));
			}
			else if (l_strType == "icosaedron")
			{
				l_eType = eIcosaedron;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[2].c_str())));
			}
			else if (l_strType == "plane")
			{
				l_eType = ePlane;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[2].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[3].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[4].c_str())));
			}
			else if (l_strType == "torus")
			{
				l_eType = eTorus;
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[1].c_str()));
				l_arrayFaces.push_back( atoi( l_arrayMeshInfos[2].c_str()));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[3].c_str())));
				l_arraySizes.push_back( real( atof( l_arrayMeshInfos[4].c_str())));
			}

			if (l_eType != eCustom)
			{
				p_pContext->pMesh = p_pContext->m_pMeshManager->CreateMesh( p_pContext->strName, l_arrayFaces, l_arraySizes, l_eType);
			}
			else
			{
				PARSING_ERROR( "Directive <mesh::type> : Unknown mesh type : " + l_strType);
			}
		}
		else
		{
			PARSING_WARNING( "Directive <mesh::type> : Mesh already initialised => ignored");
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
		PARSING_ERROR( "Directive <mesh::file> : Can't load mesh file " + p_strParams);
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshMaterial)
{
	if (p_pContext->pMesh != NULL)
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
			PARSING_ERROR( "Directive <mesh::material> : Material " + p_strParams + " does not exist");
		}
	}
	else
	{
		PARSING_ERROR( "Directive mesh::<material> : Mesh not initialised");
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
		PARSING_ERROR( "Directive <mesh::submesh> : Mesh not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshEnd)
{
	p_pContext->eSection = SceneFileContext::eObject;

	if (p_pContext->pMesh != NULL)
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
	if (p_pContext->pSubmesh != NULL)
	{
		if (p_pContext->m_pMaterialManager->HasElement( p_strParams))
		{
			p_pContext->pSubmesh->SetMaterial( p_pContext->m_pMaterialManager->GetElement( p_strParams));
		}
		else
		{
			PARSING_ERROR( "Directive <submesh::material> : Material " + p_strParams + " does not exist");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <submesh::material> : Submesh not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshVertex)
{
	if (p_pContext->pSubmesh != NULL)
	{
		Point3r l_vVector;

		if (PARSE_V3( real, l_vVector))
		{
			p_pContext->pSubmesh->AddPoint( l_vVector);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <submesh::vertex> : Submesh not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshSmoothingGroup)
{
	if (p_pContext->pSubmesh != NULL)
	{
		p_pContext->eSection = SceneFileContext::eSmoothingGroup;
		p_pContext->uiSmoothingGroup = p_pContext->pSubmesh->AddSmoothingGroup()->GetGroupID();

		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <submesh::smoothing_group> : Submesh not initialised");

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

	p_pContext->pSubmesh.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SmoothingGroupFace)
{
	if (p_pContext->uiSmoothingGroup != 0)
	{
		Point3i l_pt3Indices;
		StringArray l_arrayValues = p_strParams.split( " ");
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
		PARSING_ERROR( "Directive <smoothing_group::face> : Smoothing Group not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SmoothingGroupFaceUV)
{
	if (p_pContext->uiSmoothingGroup != 0)
	{
		Point3i l_pt3Indices;
		StringArray l_arrayValues = p_strParams.split( " ", 20);

		if (l_arrayValues.size() >= 6)
		{
			if (p_pContext->iFace1 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace1);
				l_face->SetVertexTexCoords( 0, l_arrayValues[0].to_real(), l_arrayValues[1].to_real(), true);
				l_face->SetVertexTexCoords( 1, l_arrayValues[2].to_real(), l_arrayValues[3].to_real(), true);
				l_face->SetVertexTexCoords( 2, l_arrayValues[4].to_real(), l_arrayValues[5].to_real(), true);
			}
		}

		if (l_arrayValues.size() >= 8)
		{
			if (p_pContext->iFace2 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace2);
				l_face->SetVertexTexCoords( 0, l_arrayValues[0].to_real(), l_arrayValues[1].to_real(), true);
				l_face->SetVertexTexCoords( 1, l_arrayValues[4].to_real(), l_arrayValues[5].to_real(), true);
				l_face->SetVertexTexCoords( 2, l_arrayValues[6].to_real(), l_arrayValues[7].to_real(), true);
			}
		}
	}
	else
	{
		PARSING_ERROR( "Directive <smoothing_group::uv> : Smoothing Group not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SmoothingGroupFaceNormals)
{
	if (p_pContext->uiSmoothingGroup != 0)
	{
		Point3i l_pt3Indices;
		StringArray l_arrayValues = p_strParams.split( " ", 20);

		if (l_arrayValues.size() >= 9)
		{
			if (p_pContext->iFace1 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace1);
				l_face->SetVertexNormal( 0, l_arrayValues[0].to_real(), l_arrayValues[1].to_real(), l_arrayValues[2].to_real(), true);
				l_face->SetVertexNormal( 1, l_arrayValues[3].to_real(), l_arrayValues[4].to_real(), l_arrayValues[5].to_real(), true);
				l_face->SetVertexNormal( 2, l_arrayValues[6].to_real(), l_arrayValues[7].to_real(), l_arrayValues[8].to_real(), true);
			}
		}

		if (l_arrayValues.size() >= 12)
		{
			if (p_pContext->iFace2 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace2);
				l_face->SetVertexNormal( 0, l_arrayValues[0].to_real(), l_arrayValues[ 1].to_real(), l_arrayValues[ 2].to_real(), true);
				l_face->SetVertexNormal( 1, l_arrayValues[6].to_real(), l_arrayValues[ 7].to_real(), l_arrayValues[ 8].to_real(), true);
				l_face->SetVertexNormal( 2, l_arrayValues[9].to_real(), l_arrayValues[10].to_real(), l_arrayValues[11].to_real(), true);
			}
		}
	}
	else
	{
		PARSING_ERROR( "Directive <smoothing_group::normals> : Smoothing Group not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SmoothingGroupFaceTangents)
{
	if (p_pContext->uiSmoothingGroup != 0)
	{
		Point3i l_pt3Indices;
		StringArray l_arrayValues = p_strParams.split( " ", 20);

		if (l_arrayValues.size() >= 9)
		{
			if (p_pContext->iFace1 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace1);
				l_face->SetVertexTangent( 0, l_arrayValues[0].to_real(), l_arrayValues[1].to_real(), l_arrayValues[2].to_real(), true);
				l_face->SetVertexTangent( 1, l_arrayValues[3].to_real(), l_arrayValues[4].to_real(), l_arrayValues[5].to_real(), true);
				l_face->SetVertexTangent( 2, l_arrayValues[6].to_real(), l_arrayValues[7].to_real(), l_arrayValues[8].to_real(), true);
			}
		}

		if (l_arrayValues.size() >= 12)
		{
			if (p_pContext->iFace2 != -1)
			{
				FacePtr l_face = p_pContext->pSubmesh->GetSmoothGroup( p_pContext->uiSmoothingGroup - 1)->GetFace( p_pContext->iFace2);
				l_face->SetVertexTangent( 0, l_arrayValues[0].to_real(), l_arrayValues[ 1].to_real(), l_arrayValues[ 2].to_real(), true);
				l_face->SetVertexTangent( 1, l_arrayValues[6].to_real(), l_arrayValues[ 7].to_real(), l_arrayValues[ 8].to_real(), true);
				l_face->SetVertexTangent( 2, l_arrayValues[9].to_real(), l_arrayValues[10].to_real(), l_arrayValues[11].to_real(), true);
			}
		}
	}
	else
	{
		PARSING_ERROR( "Directive <smoothing_group::tangents> : Smoothing Group not initialised");
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
	if (p_pContext->pMaterial != NULL)
	{
		p_pContext->eSection = SceneFileContext::ePass;
		p_pContext->pMaterial->CreatePass();
		p_pContext->uiPass = p_pContext->pMaterial->GetNbPasses() - 1;

		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <material::pass> : Material not initialised");

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
		PARSING_ERROR( "Directive <pass::ambient> : Pass not initialised.");
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
		PARSING_ERROR( "Directive <pass::diffuse> : Pass not initialised.");
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
		PARSING_ERROR( "Directive <pass::specular> : Pass not initialised.");
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
		PARSING_ERROR( "Directive <pass::emissive> : Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassShininess)
{
	if (p_pContext->uiPass >= 0)
	{
		float l_fFloat = p_strParams.to_float();
		p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetShininess( l_fFloat);
	}
	else
	{
		PARSING_ERROR( "Directive <pass::shininess> : Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlpha)
{
	if (p_pContext->uiPass >= 0)
	{
		float l_fFloat = p_strParams.to_float();
		p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetAlpha( l_fFloat);
	}
	else
	{
		PARSING_ERROR( "Directive <pass::alpha> : Pass not initialised.");
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
		PARSING_ERROR( "Directive <pass::tex_base> : Pass not initialised.");
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
		PARSING_ERROR( "Directive <pass::double_face> : Pass not initialised.");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassBlendFunc)
{
	if (p_pContext->uiPass >= 0)
	{
		StringArray l_arrayParams = p_strParams.split( " \t,;");

		if (l_arrayParams.size() >= 2)
		{
			PassPtr l_pPass = p_pContext->pMaterial->GetPass( p_pContext->uiPass);
			std::map<String, Pass::eSRC_BLEND_FACTOR>::iterator l_itSrc = Pass::MapSrcBlendFactors.find( l_arrayParams[0]);
			std::map<String, Pass::eDST_BLEND_FACTOR>::iterator l_itDst = Pass::MapDstBlendFactors.find( l_arrayParams[1]);

			if (l_itSrc != Pass::MapSrcBlendFactors.end())
			{
				if (l_itDst != Pass::MapDstBlendFactors.end())
				{
					l_pPass->SetSrcBlendFactor( l_itSrc->second);
					l_pPass->SetDstBlendFactor( l_itDst->second);
				}
				else
				{
					PARSING_ERROR( "Directive <pass::blend_func> : Unknown Destination Blend Factor : " + l_arrayParams[1]);
				}
			}
			else
			{
				PARSING_ERROR( "Directive <pass::blend_func> : Unknown Source Blend Factor : " + l_arrayParams[0]);
			}
		}
		else
		{
			PARSING_ERROR( "Directive <pass::blend_func> : Wrong number of params.");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <pass::blend_func> : Pass not initialised.");
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
		PARSING_ERROR( "Directive <pass::texture_unit> : Pass not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassGlShader)
{
	if (p_pContext->uiPass >= 0)
	{
		p_pContext->eSection = SceneFileContext::eGlslShader;
		p_pContext->pGlslShaderProgram = RenderSystem::CreateShaderProgram<GlslShaderProgram>( C3DEmptyString, C3DEmptyString, C3DEmptyString);
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <pass::gl_shader> : Pass not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassHlShader)
{
	if (p_pContext->uiPass >= 0)
	{
		p_pContext->eSection = SceneFileContext::eHlslShader;
		p_pContext->pHlslShaderProgram = RenderSystem::CreateShaderProgram<HlslShaderProgram>( C3DEmptyString, C3DEmptyString, C3DEmptyString);
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <pass::hl_shader> : Pass not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassCgShader)
{
	if (p_pContext->uiPass >= 0)
	{
		p_pContext->eSection = SceneFileContext::eCgShader;
		p_pContext->pCgShaderProgram = RenderSystem::CreateShaderProgram<CgShaderProgram>( C3DEmptyString, C3DEmptyString, C3DEmptyString);
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <pass::cg_shader> : Pass not initialised");

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
	if (p_pContext->pTextureUnit != NULL)
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
			PARSING_ERROR( "Directive <texture_unit::image> : File " + p_pContext->pFile->GetFilePath() + "/" + p_strParams + " not found, check the relativeness of the path");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::image> : Texture Unit not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitColour)
{
	if (p_pContext->pTextureUnit != NULL)
	{
		Colour l_crColour;

		if (PARSE_V4( float, l_crColour))
		{
			p_pContext->pTextureUnit->SetPrimaryColour( l_crColour.ptr());
		}
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::colour> : Texture Unit not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitMapType)
{
	if (p_pContext->pTextureUnit != NULL)
	{
		TextureUnit::eMAP_MODE eMapMode = TextureUnit::eMAP_MODE( p_strParams.to_int());

		p_pContext->pTextureUnit->SetTextureMapMode( eMapMode);
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::map_type> : Texture Unit not initialised");
	}
	
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitAlphaFunc)
{
	if (p_pContext->pTextureUnit != NULL)
	{
		StringArray l_arrayParams = p_strParams.split( "\t ,");

		if (l_arrayParams.size() >= 2)
		{
			std::map <String, TextureUnit::eALPHA_FUNC>::const_iterator l_it = TextureUnit::MapAlphaFuncs.find( l_arrayParams[0]);

			if (l_it != TextureUnit::MapAlphaFuncs.end())
			{
				p_pContext->pTextureUnit->SetAlphaFunc( l_it->second);
				p_pContext->pTextureUnit->SetAlphaValue( l_arrayParams[1].to_float());
			}
			else
			{
				PARSING_ERROR( "Directive <texture_unit::alpha_func> : Unknown alpha function : " + l_arrayParams[0]);
			}
		}
		else
		{
			PARSING_ERROR( "Directive <texture_unit::alpha_func> : Wrong number of params for function alpha_func");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_func> : Texture Unit not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitBlendMode)
{
	if (p_pContext->pTextureUnit != NULL)
	{
		if ( ! p_strParams.empty())
		{
			std::map <String, TextureEnvironment::eMODE>::const_iterator l_itMode = TextureEnvironment::MapModes.find( p_strParams);

			if (l_itMode != TextureEnvironment::MapModes.end())
			{
				p_pContext->pTextureUnit->GetEnvironment()->SetMode( l_itMode->second);
			}
			else
			{
				PARSING_ERROR( "Directive <texture_unit::blend_mode> : Unknown blending mode : " + p_strParams);
			}
		}
		else
		{
			PARSING_ERROR( "Directive <texture_unit::blend_mode> : Wrong number of params");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::blend_mode> : Texture Unit not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitRGBCombination)
{
	bool l_bContinue = p_pContext->pTextureUnit != NULL;
	StringArray l_arrayParams;
	std::map <String, TextureEnvironment::eRGB_COMBINATION>::const_iterator l_itCombi;
	std::map <String, TextureEnvironment::eCOMBINATION_SOURCE>::const_iterator l_itSrc;
	std::map <String, TextureEnvironment::eRGB_OPERAND>::const_iterator l_itOpe;

	if (l_bContinue)
	{
		l_bContinue = p_pContext->pTextureUnit->GetEnvironment()->GetMode() == TextureEnvironment::eModeCombine;
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Texture Unit not initialised");
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams = p_strParams.split( "\t ,");
		l_bContinue = l_arrayParams.size() > 0;
	}
	else
	{
		PARSING_WARNING( "Directive <texture_unit::rgb_combine> : Blend mode isn't 'combine' => ignored");
		return false;
	}

	if (l_bContinue)
	{
		l_itCombi = TextureEnvironment::MapRgbCombinations.find( l_arrayParams[0]);
		l_bContinue = l_itCombi != TextureEnvironment::MapRgbCombinations.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Wrong number of params");
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());

		if (l_itCombi->second != TextureEnvironment::eRgbCombiNone)
		{
			l_bContinue = l_arrayParams.size() >= 2;
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetRgbCombination( l_itCombi->second);
			return false;
		}
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Unknown RGB combination mode : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_itOpe = TextureEnvironment::MapRgbOperands.find( l_arrayParams[0]);
		l_bContinue = l_itOpe != TextureEnvironment::MapRgbOperands.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Wrong number of params with a combination mode other than <none>");
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());
		p_pContext->pTextureUnit->GetEnvironment()->SetRgbOperand( 0, l_itOpe->second);
		l_itSrc = TextureEnvironment::MapCombinationSources.find( l_arrayParams[0]);
		l_bContinue = l_itSrc != TextureEnvironment::MapCombinationSources.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Unknown RGB operand : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());

		if (l_itSrc->second == TextureEnvironment::eCombiSrcTexture)
		{
			if (l_arrayParams.size() > 0 && l_arrayParams[0].is_integer())
			{
				p_pContext->pTextureUnit->GetEnvironment()->SetRgbSource( 0, l_itSrc->second, l_arrayParams[0].to_int());
				l_arrayParams.erase( l_arrayParams.begin());
			}
			else
			{
				PARSING_WARNING( "Directive <texture_unit::rgb_combine> : No texture specified for RGB source 'texture' defaulting to 0");
				p_pContext->pTextureUnit->GetEnvironment()->SetRgbSource( 0, l_itSrc->second);
			}
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetRgbSource( 0, l_itSrc->second);
		}

		if (l_itCombi->second != TextureEnvironment::eRgbCombiReplace)
		{
			l_bContinue = l_arrayParams.size() > 0;
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetRgbCombination( l_itCombi->second);
			return false;
		}
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Unknown Combination Source : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_itOpe = TextureEnvironment::MapRgbOperands.find( l_arrayParams[0]);
		l_bContinue = l_itOpe != TextureEnvironment::MapRgbOperands.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Wrong number of args for a combination mode other than <replace>");
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());
		p_pContext->pTextureUnit->GetEnvironment()->SetRgbOperand( 1, l_itOpe->second);
		l_itSrc = TextureEnvironment::MapCombinationSources.find( l_arrayParams[0]);
		l_bContinue = l_itSrc != TextureEnvironment::MapCombinationSources.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Unknown RGB operand : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());

		if (l_itSrc->second == TextureEnvironment::eCombiSrcTexture)
		{
			if (l_arrayParams.size() > 0 && l_arrayParams[0].is_integer())
			{
				p_pContext->pTextureUnit->GetEnvironment()->SetRgbSource( 1, l_itSrc->second, l_arrayParams[0].to_int());
				l_arrayParams.erase( l_arrayParams.begin());
			}
			else
			{
				PARSING_WARNING( "Directive <texture_unit::rgb_combine> : No texture specified for RGB source 'texture' defaulting to 0");
				p_pContext->pTextureUnit->GetEnvironment()->SetRgbSource( 1, l_itSrc->second);
			}
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetRgbSource( 1, l_itSrc->second);
		}

		if (l_itCombi->second == TextureEnvironment::eRgbCombiInterpolate)
		{
			l_bContinue = l_arrayParams.size() > 0;
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetRgbCombination( l_itCombi->second);
			return false;
		}
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Unknown Combination Source : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_itOpe = TextureEnvironment::MapRgbOperands.find( l_arrayParams[0]);
		l_bContinue = l_itOpe != TextureEnvironment::MapRgbOperands.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Wrong number of args for a combination mode <interpolate>");
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());
		p_pContext->pTextureUnit->GetEnvironment()->SetRgbOperand( 1, l_itOpe->second);
		l_itSrc = TextureEnvironment::MapCombinationSources.find( l_arrayParams[0]);
		l_bContinue = l_itSrc != TextureEnvironment::MapCombinationSources.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Unknown RGB operand : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());

		if (l_itSrc->second == TextureEnvironment::eCombiSrcTexture)
		{
			if (l_arrayParams.size() > 0 && l_arrayParams[0].is_integer())
			{
				p_pContext->pTextureUnit->GetEnvironment()->SetRgbSource( 1, l_itSrc->second, l_arrayParams[0].to_int());
				l_arrayParams.erase( l_arrayParams.begin());
			}
			else
			{
				PARSING_WARNING( "Directive <texture_unit::rgb_combine> : No texture specified for RGB source 'texture' defaulting to 0");
				p_pContext->pTextureUnit->GetEnvironment()->SetRgbSource( 1, l_itSrc->second);
			}
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetRgbSource( 1, l_itSrc->second);
		}

		p_pContext->pTextureUnit->GetEnvironment()->SetRgbCombination( l_itCombi->second);
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_combine> : Unknown Combination Source : " + l_arrayParams[0]);
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitAlphaCombination)
{
	bool l_bContinue = p_pContext->pTextureUnit != NULL;
	StringArray l_arrayParams;
	std::map <String, TextureEnvironment::eALPHA_COMBINATION>::const_iterator l_itCombi;
	std::map <String, TextureEnvironment::eCOMBINATION_SOURCE>::const_iterator l_itSrc;
	std::map <String, TextureEnvironment::eALPHA_OPERAND>::const_iterator l_itOpe;

	if (l_bContinue)
	{
		l_bContinue = p_pContext->pTextureUnit->GetEnvironment()->GetMode() == TextureEnvironment::eModeCombine;
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Texture Unit not initialised");
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams = p_strParams.split( "\t ,");
		l_bContinue = l_arrayParams.size() > 0;
	}
	else
	{
		PARSING_WARNING( "Directive <texture_unit::alpha_combine> : Blend mode isn't 'combine' => ignored");
		return false;
	}

	if (l_bContinue)
	{
		l_itCombi = TextureEnvironment::MapAlphaCombinations.find( l_arrayParams[0]);
		l_bContinue = l_itCombi != TextureEnvironment::MapAlphaCombinations.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Wrong number of params");
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());

		if (l_itCombi->second != TextureEnvironment::eAlphaCombiNone)
		{
			l_bContinue = l_arrayParams.size() >= 2;
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetAlphaCombination( l_itCombi->second);
			return false;
		}
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Unknown Alpha Combination Mode : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_itOpe = TextureEnvironment::MapAlphaOperands.find( l_arrayParams[0]);
		l_bContinue = l_itOpe != TextureEnvironment::MapAlphaOperands.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Wrong number of params with a combination mode other than <none>");
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());
		p_pContext->pTextureUnit->GetEnvironment()->SetAlphaOperand( 0, l_itOpe->second);
		l_itSrc = TextureEnvironment::MapCombinationSources.find( l_arrayParams[0]);
		l_bContinue = l_itSrc != TextureEnvironment::MapCombinationSources.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Unknown Alpha Operand : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());

		if (l_itSrc->second == TextureEnvironment::eCombiSrcTexture)
		{
			if (l_arrayParams.size() > 0 && l_arrayParams[0].is_integer())
			{
				p_pContext->pTextureUnit->GetEnvironment()->SetAlphaSource( 0, l_itSrc->second, l_arrayParams[0].to_int());
				l_arrayParams.erase( l_arrayParams.begin());
			}
			else
			{
				PARSING_WARNING( "Directive <texture_unit::alpha_combine> : No texture specified for Alpha Source 'texture' defaulting to 0");
				p_pContext->pTextureUnit->GetEnvironment()->SetAlphaSource( 0, l_itSrc->second);
			}
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetAlphaSource( 0, l_itSrc->second);
		}

		if (l_itCombi->second != TextureEnvironment::eAlphaCombiReplace)
		{
			l_bContinue = l_arrayParams.size() > 0;
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetAlphaCombination( l_itCombi->second);
			return false;
		}
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Unknown Combination Source : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_itOpe = TextureEnvironment::MapAlphaOperands.find( l_arrayParams[0]);
		l_bContinue = l_itOpe != TextureEnvironment::MapAlphaOperands.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Wrong number of args for a combination mode other than <replace>");
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());
		p_pContext->pTextureUnit->GetEnvironment()->SetAlphaOperand( 1, l_itOpe->second);
		l_itSrc = TextureEnvironment::MapCombinationSources.find( l_arrayParams[0]);
		l_bContinue = l_itSrc != TextureEnvironment::MapCombinationSources.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Unknown Alpha Operand : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());

		if (l_itSrc->second == TextureEnvironment::eCombiSrcTexture)
		{
			if (l_arrayParams.size() > 0 && l_arrayParams[0].is_integer())
			{
				p_pContext->pTextureUnit->GetEnvironment()->SetAlphaSource( 1, l_itSrc->second, l_arrayParams[0].to_int());
				l_arrayParams.erase( l_arrayParams.begin());
			}
			else
			{
				PARSING_WARNING( "Directive <texture_unit::alpha_combine> : No texture specified for Alpha Source 'texture' defaulting to 0");
				p_pContext->pTextureUnit->GetEnvironment()->SetAlphaSource( 1, l_itSrc->second);
			}
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetAlphaSource( 1, l_itSrc->second);
		}

		if (l_itCombi->second == TextureEnvironment::eAlphaCombiInterpolate)
		{
			l_bContinue = l_arrayParams.size() > 0;
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetAlphaCombination( l_itCombi->second);
			return false;
		}
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Unknown Combination Source : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_itOpe = TextureEnvironment::MapAlphaOperands.find( l_arrayParams[0]);
		l_bContinue = l_itOpe != TextureEnvironment::MapAlphaOperands.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Wrong number of args for a combination mode <interpolate>");
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());
		p_pContext->pTextureUnit->GetEnvironment()->SetAlphaOperand( 1, l_itOpe->second);
		l_itSrc = TextureEnvironment::MapCombinationSources.find( l_arrayParams[0]);
		l_bContinue = l_itSrc != TextureEnvironment::MapCombinationSources.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Unknown Alpha Operand : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_arrayParams.erase( l_arrayParams.begin());

		if (l_itSrc->second == TextureEnvironment::eCombiSrcTexture)
		{
			if (l_arrayParams.size() > 0 && l_arrayParams[0].is_integer())
			{
				p_pContext->pTextureUnit->GetEnvironment()->SetAlphaSource( 1, l_itSrc->second, l_arrayParams[0].to_int());
				l_arrayParams.erase( l_arrayParams.begin());
			}
			else
			{
				PARSING_WARNING( "Directive <texture_unit::alpha_combine> : No texture specified for Alpha Source 'texture' defaulting to 0");
				p_pContext->pTextureUnit->GetEnvironment()->SetAlphaSource( 1, l_itSrc->second);
			}
		}
		else
		{
			p_pContext->pTextureUnit->GetEnvironment()->SetAlphaSource( 1, l_itSrc->second);
		}

		p_pContext->pTextureUnit->GetEnvironment()->SetAlphaCombination( l_itCombi->second);
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_combine> : Unknown Combination Source : " + l_arrayParams[0]);
		return false;
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitRgbBlend)
{
	bool l_bContinue = p_pContext->pTextureUnit != NULL;
	StringArray l_arrayParams;
	std::map <String, TextureUnit::eTEXTURE_ARGUMENT>::const_iterator l_itArg1;
	std::map <String, TextureUnit::eTEXTURE_ARGUMENT>::const_iterator l_itArg2;
	std::map <String, TextureUnit::eTEXTURE_RGB_MODE>::const_iterator l_itMode;

	if (l_bContinue)
	{
		l_arrayParams = p_strParams.split( ",;\t ");
		l_bContinue = l_arrayParams.size() > 2;
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_blend> : Texture Unit not initialised");
		return false;
	}

	if (l_bContinue)
	{
		l_itMode = TextureUnit::MapTextureRgbModes.find( l_arrayParams[0]);
		l_bContinue = l_itMode != TextureUnit::MapTextureRgbModes.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_blend> : Not enough params, 3 required : mode arg1 arg2");
		return false;
	}

	if (l_bContinue)
	{
		l_itArg1 = TextureUnit::MapTextureArguments.find( l_arrayParams[1]);
		l_bContinue = l_itArg1 != TextureUnit::MapTextureArguments.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_blend> : Unknown mode : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_itArg2 = TextureUnit::MapTextureArguments.find( l_arrayParams[2]);
		l_bContinue = l_itArg2 != TextureUnit::MapTextureArguments.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_blend> : Unknown argument : " + l_arrayParams[1]);
		return false;
	}

	if (l_bContinue)
	{
		p_pContext->pTextureUnit->SetRgbMode( l_itMode->second);
		p_pContext->pTextureUnit->SetRgbArgument( l_itArg1->second, 0);
		p_pContext->pTextureUnit->SetRgbArgument( l_itArg2->second, 1);
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_blend> : Unknown argument : " + l_arrayParams[2]);
		return false;
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitAlphaBlend)
{
	bool l_bContinue = p_pContext->pTextureUnit != NULL;
	StringArray l_arrayParams;
	std::map <String, TextureUnit::eTEXTURE_ARGUMENT>::const_iterator l_itArg1;
	std::map <String, TextureUnit::eTEXTURE_ARGUMENT>::const_iterator l_itArg2;
	std::map <String, TextureUnit::eTEXTURE_ALPHA_MODE>::const_iterator l_itMode;

	if (l_bContinue)
	{
		l_arrayParams = p_strParams.split( ",;\t ");
		l_bContinue = l_arrayParams.size() > 2;
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_blend> : Texture Unit not initialised");
		return false;
	}

	if (l_bContinue)
	{
		l_itMode = TextureUnit::MapTextureAlphaModes.find( l_arrayParams[0]);
		l_bContinue = l_itMode != TextureUnit::MapTextureAlphaModes.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_blend> : Not enough params, 3 required : mode arg1 arg2");
		return false;
	}

	if (l_bContinue)
	{
		l_itArg1 = TextureUnit::MapTextureArguments.find( l_arrayParams[1]);
		l_bContinue = l_itArg1 != TextureUnit::MapTextureArguments.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_blend> : Unknown mode : " + l_arrayParams[0]);
		return false;
	}

	if (l_bContinue)
	{
		l_itArg2 = TextureUnit::MapTextureArguments.find( l_arrayParams[2]);
		l_bContinue = l_itArg2 != TextureUnit::MapTextureArguments.end();
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::alpha_blend> : Unknown argument : " + l_arrayParams[1]);
		return false;
	}

	if (l_bContinue)
	{
		p_pContext->pTextureUnit->SetAlpMode( l_itMode->second);
		p_pContext->pTextureUnit->SetAlpArgument( l_itArg1->second, 0);
		p_pContext->pTextureUnit->SetAlpArgument( l_itArg2->second, 1);
	}
	else
	{
		PARSING_ERROR( "Directive <texture_unit::rgb_blend> : Unknown argument : " + l_arrayParams[2]);
		return false;
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
	if (p_pContext->pGlslShaderProgram != NULL)
	{
		p_pContext->eSection = SceneFileContext::eGlslShaderProgram;
		p_pContext->pGlslShaderObject = p_pContext->pGlslShaderProgram->GetVertexProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::vertex_program> : Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlPixelShader)
{
	if (p_pContext->pGlslShaderProgram != NULL)
	{
		p_pContext->eSection = SceneFileContext::eGlslShaderProgram;
		p_pContext->pGlslShaderObject = p_pContext->pGlslShaderProgram->GetFragmentProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::pixel_program> : Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlGeometryShader)
{
	if (p_pContext->pGlslShaderProgram != NULL)
	{
		p_pContext->eSection = SceneFileContext::eGlslShaderProgram;
		p_pContext->pGlslShaderObject = p_pContext->pGlslShaderProgram->GetGeometryProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::geometry_program> : Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderEnd)
{
	p_pContext->eSection = SceneFileContext::ePass;
	p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetShader( p_pContext->pGlslShaderProgram);
	p_pContext->pGlslShaderProgram.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramFile)
{
	if (p_pContext->pGlslShaderObject != NULL)
	{
		p_pContext->pGlslShaderObject->SetFile( p_pContext->pFile->GetFilePath() / p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <program::file> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramText)
{
	if (p_pContext->pGlslShaderObject != NULL)
	{
	}
	else
	{
		PARSING_ERROR( "Directive <program::text> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramVariable)
{
	if (p_pContext->pGlslShaderObject != NULL)
	{
		p_pContext->eSection = SceneFileContext::eGlslShaderVariable;
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <program::variable> : GPU Program not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramEnd)
{
	p_pContext->eSection = SceneFileContext::eGlslShader;
	p_pContext->pGlslShaderObject.reset();
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
			PARSING_ERROR( "Directive <variable::type> : Unknown variable type : " + p_strParams);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <variable::type> : Variable type already set");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableName)
{
	if (p_pContext->pFrameVariable != NULL)
	{
		p_pContext->pFrameVariable->SetName( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <variable::name> : Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableValue)
{
	if (p_pContext->pFrameVariable != NULL)
	{
		p_pContext->pFrameVariable->SetValue( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <variable::value> : Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableEnd)
{
	p_pContext->eSection = SceneFileContext::eGlslShaderProgram;
	p_pContext->pGlslShaderProgram->AddFrameVariable( p_pContext->pFrameVariable, p_pContext->pGlslShaderObject);
	p_pContext->pFrameVariable.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlVertexShader)
{
	if (p_pContext->pHlslShaderProgram != NULL)
	{
		p_pContext->eSection = SceneFileContext::eHlslShaderProgram;
		p_pContext->pHlslShaderObject = p_pContext->pHlslShaderProgram->GetVertexProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::vertex_program> : Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlPixelShader)
{
	if (p_pContext->pHlslShaderProgram != NULL)
	{
		p_pContext->eSection = SceneFileContext::eHlslShaderProgram;
		p_pContext->pHlslShaderObject = p_pContext->pHlslShaderProgram->GetFragmentProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::pixel_program> : Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlGeometryShader)
{
	if (p_pContext->pHlslShaderProgram != NULL)
	{
		p_pContext->eSection = SceneFileContext::eHlslShaderProgram;
		p_pContext->pHlslShaderObject = p_pContext->pHlslShaderProgram->GetGeometryProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::geometry_program> : Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderEnd)
{
	p_pContext->eSection = SceneFileContext::ePass;
	p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetShader( p_pContext->pHlslShaderProgram);
	p_pContext->pHlslShaderProgram.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderProgramFile)
{
	if (p_pContext->pHlslShaderObject != NULL)
	{
		p_pContext->pHlslShaderObject->SetFile( p_pContext->pFile->GetFilePath() / p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <program::file> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderProgramText)
{
	if (p_pContext->pHlslShaderObject != NULL)
	{
	}
	else
	{
		PARSING_ERROR( "Directive <program::text> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderProgramVariable)
{
	if (p_pContext->pHlslShaderObject != NULL)
	{
		p_pContext->eSection = SceneFileContext::eHlslShaderVariable;
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <program::variable> : GPU Program not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderProgramEnd)
{
	p_pContext->eSection = SceneFileContext::eHlslShader;
	p_pContext->pHlslShaderObject.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderVariableType)
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
			PARSING_ERROR( "Directive <variable::type> : Unknown variable type : " + p_strParams);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <variable::type> : Variable type already set");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderVariableName)
{
	if (p_pContext->pFrameVariable != NULL)
	{
		p_pContext->pFrameVariable->SetName( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <variable::name> : Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderVariableValue)
{
	if (p_pContext->pFrameVariable != NULL)
	{
		p_pContext->pFrameVariable->SetValue( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <variable::value> : Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderVariableEnd)
{
	p_pContext->eSection = SceneFileContext::eHlslShaderProgram;
	p_pContext->pHlslShaderProgram->AddFrameVariable( p_pContext->pFrameVariable, p_pContext->pHlslShaderObject);
	p_pContext->pFrameVariable.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgVertexShader)
{
	if (p_pContext->pCgShaderProgram != NULL)
	{
		p_pContext->eSection = SceneFileContext::eCgShaderProgram;
		p_pContext->pCgShaderObject = p_pContext->pCgShaderProgram->GetVertexProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <cg_shader_program::vertex_program> : Cg Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgPixelShader)
{
	if (p_pContext->pCgShaderProgram != NULL)
	{
		p_pContext->eSection = SceneFileContext::eCgShaderProgram;
		p_pContext->pCgShaderObject = p_pContext->pCgShaderProgram->GetFragmentProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <cg_shader_program::pixel_program> : Cg Shader not initialised");

		return false;
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgGeometryShader)
{
	if (p_pContext->pCgShaderProgram != NULL)
	{
		p_pContext->eSection = SceneFileContext::eCgShaderProgram;
		p_pContext->pCgShaderObject = p_pContext->pCgShaderProgram->GetGeometryProgram();
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <cg_shader_program::geometry_program> : Cg Shader not initialised");

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
	if (p_pContext->pCgShaderObject != NULL)
	{
		p_pContext->pCgShaderObject->SetFile( p_pContext->pFile->GetFilePath() / p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <program::file> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramText)
{
	if (p_pContext->pCgShaderObject != NULL)
	{
	}
	else
	{
		PARSING_ERROR( "Directive <program::text> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramVariable)
{
	if (p_pContext->pCgShaderObject != NULL)
	{
		p_pContext->eSection = SceneFileContext::eCgShaderVariable;
		return true;
	}
	else
	{
		PARSING_ERROR( "Directive <program::variable> : GPU Program not initialised");

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
			PARSING_ERROR( "Directive <variable::type> : Unknown Cg variable type : " + p_strParams);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <variable::type> : Cg Variable type already set");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderVariableName)
{
	if (p_pContext->pCgFrameVariable != NULL)
	{
		p_pContext->pCgFrameVariable->SetName( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <variable::name> : Cg Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderVariableValue)
{
	if (p_pContext->pCgFrameVariable != NULL)
	{
		p_pContext->pCgFrameVariable->SetValue( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <variable::value> : Cg Variable not initialised");
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

IMPLEMENT_ATTRIBUTE_PARSER( Parser_FontFile)
{
	p_pContext->strName2 = p_strParams;
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_FontHeight)
{
	p_pContext->uiUInt = size_t( p_strParams.to_int());
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_FontEnd)
{
	if ( ! p_pContext->strName.empty() && ! p_pContext->strName2.empty())
	{
		p_pContext->m_pSceneManager->GetFontManager()->LoadFont( p_pContext->strName, p_pContext->pFile->GetFilePath() / p_pContext->strName2, p_pContext->uiUInt, p_pContext->m_pImageManager);
	}

	p_pContext->eSection = SceneFileContext::eNone;
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayPosition)
{
	if (p_pContext->pOverlay != NULL)
	{
		Point2r l_ptPosition;
		PARSE_V2( real, l_ptPosition);
		p_pContext->pOverlay->SetPosition( l_ptPosition);
	}
	else
	{
		PARSING_ERROR( "Directive <overlay::position> : Overlay not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlaySize)
{
	if (p_pContext->pOverlay != NULL)
	{
		Point2r l_ptSize;
		PARSE_V2( real, l_ptSize);
		p_pContext->pOverlay->SetSize( l_ptSize);
	}
	else
	{
		PARSING_ERROR( "Directive <overlay::size> : Overlay not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayMaterial)
{
	if (p_pContext->pOverlay != NULL)
	{
		p_pContext->pOverlay->SetMaterial( p_pContext->m_pMaterialManager->GetElement( p_strParams));
	}
	else
	{
		PARSING_ERROR( "Directive <overlay::material> : Overlay not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayFont)
{
	if (p_pContext->pTextOverlay != NULL)
	{
		if (p_pContext->m_pSceneManager->GetFontManager()->HasElement( p_strParams))
		{
			p_pContext->pTextOverlay->SetFont( p_strParams, p_pContext->m_pSceneManager->GetFontManager());
		}
		else
		{
			PARSING_ERROR( "Directive <text_overlay::font> : Unknown font");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <text_overlay::font> : TextOverlay not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayText)
{
	if (p_pContext->pTextOverlay != NULL)
	{
		p_pContext->pTextOverlay->SetCaption( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <text_overlay::text> : TextOverlay not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayEnd)
{
	p_pContext->pOverlay->SetVisible( true);
	p_pContext->pOverlay = p_pContext->pOverlay->GetParent();
	p_pContext->eSection = p_pContext->ePrevSection;
	p_pContext->pTextOverlay.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraParent)
{
	SceneNodePtr l_pParent = p_pContext->pScene->GetNode( p_strParams);

	if (l_pParent != NULL)
	{
		p_pContext->pSceneNode = l_pParent;
	}
	else
	{
		PARSING_ERROR( "Directive <camera::parent> : Node " + p_strParams + " does not exist");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraViewport)
{
	if ( ! p_strParams.empty())
	{
		if (p_strParams == CU_T( "3d"))
		{
			p_pContext->eViewportType = Viewport::e3DView;
		}
		else if (p_strParams == CU_T( "2d"))
		{
			p_pContext->eViewportType = Viewport::e2DView;
		}
		else
		{
			PARSING_ERROR( "Directive <camera::viewport> : Unknown viemport mode : " + p_strParams);
		}
	}
	else
	{
		PARSING_ERROR( "Directive <camera::viewport> : Wrong number of arguments");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraEnd)
{
	if (p_pContext->pSceneNode != NULL && p_pContext->eViewportType != -1)
	{
		p_pContext->pScene->CreateCamera( p_pContext->strName, 100, 100, p_pContext->pSceneNode, p_pContext->eViewportType);
	}

	p_pContext->eViewportType = Viewport::eTYPE( -1);
	p_pContext->pSceneNode.reset();
	p_pContext->eSection = SceneFileContext::eNone;
	return false;
}