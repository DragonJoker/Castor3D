#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/SceneFileParser_Parsers.hpp"
#include "Castor3D/SceneFileParser.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/Camera.hpp"
#include "Castor3D/Light.hpp"
#include "Castor3D/SpotLight.hpp"
#include "Castor3D/PointLight.hpp"
#include "Castor3D/MovableObject.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Geometry.hpp"
#include "Castor3D/SmoothingGroup.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Mesh.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Overlay.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/ShaderManager.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/ShaderObject.hpp"
#include "Castor3D/FrameVariable.hpp"
#include "Castor3D/CgFrameVariable.hpp"
#include "Castor3D/Root.hpp"

using namespace Castor3D;

void Castor3D :: SceneFileParser :: ParseError( String const & p_strError, SceneFileContextPtr p_pContext)
{
	StringStream l_streamError;
	l_streamError << "Error Line #" << p_pContext->ui64Line << " / " << p_strError;
	Logger::LogMessage( l_streamError.str());
}

void Castor3D :: SceneFileParser :: ParseWarning( String const & p_strWarning, SceneFileContextPtr p_pContext)
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
	p_pContext->eSection = eSECTION_CAMERA;

	if (p_pContext->pScene)
	{
		p_pContext->eViewportType = eVIEWPORT_TYPE( -1);
		p_pContext->pSceneNode.reset();
		p_pContext->strName = p_strParams;
	}
	else
	{
		PARSING_ERROR( "Directive <camera> : No scene initialised. Enter a scene name");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootLight)
{
	p_pContext->eSection = eSECTION_LIGHT;

	if (p_pContext->pScene)
	{
		p_pContext->strName = p_strParams;
	}
	else
	{
		PARSING_ERROR( "Directive <light> : No scene initialised. Enter a scene name");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootSceneNode)
{
	p_pContext->eSection = eSECTION_NODE;

	if (p_pContext->pScene)
	{
		p_pContext->strName = p_strParams;
		p_pContext->pSceneNode = SceneNodePtr( p_pContext->pScene->CreateSceneNode( p_strParams, p_pContext->m_pGeneralParentNode.get()));
	}
	else
	{
		PARSING_ERROR( "Directive <scene_node> : No scene initialised. Enter a scene name");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootMaterial)
{
	p_pContext->eSection = eSECTION_MATERIAL;

	if (p_pContext->pScene)
	{
		Collection<Material, String> l_mtlCollection;
		p_pContext->pMaterial = l_mtlCollection.GetElement( p_strParams);
		if ( ! p_pContext->pMaterial)
		{
			p_pContext->pMaterial.reset( new Material( p_strParams, 0));
			l_mtlCollection.AddElement( p_strParams, p_pContext->pMaterial);
		}
		p_pContext->strName = p_strParams;
	}
	else
	{
		PARSING_ERROR( "Directive <material> : No scene initialised. Enter a scene name");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootObject)
{
	p_pContext->eSection = eSECTION_OBJECT;

	if (p_pContext->pScene)
	{
		p_pContext->strName = p_strParams;
		p_pContext->pGeometry = GeometryPtr( new Geometry( p_pContext->pScene, MeshPtr(), p_pContext->m_pGeneralParentNode, p_pContext->strName));
	}
	else
	{
		PARSING_ERROR( "Directive <object> : No scene initialised. Enter a scene name");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootFont)
{
	p_pContext->eSection = eSECTION_FONT;

	if (p_pContext->pScene)
	{
		p_pContext->strName = p_strParams;
	}
	else
	{
		PARSING_ERROR( "Directive <font> : No scene initialised. Enter a scene name");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootTextOverlay)
{
	p_pContext->eSection = eSECTION_TEXT_OVERLAY;

	if (p_pContext->pScene)
	{
		p_pContext->ePrevSection = eSECTION_ROOT;
		Collection<Overlay, String> l_ovlCollection;
		OverlayPtr l_pOverlay = l_ovlCollection.GetElement( p_strParams);
		if ( ! l_pOverlay)
		{
			l_pOverlay = OverlayPtr( new Overlay( p_pContext->pScene, p_strParams, p_pContext->pOverlay, eOVERLAY_TYPE_TEXT));
			l_ovlCollection.AddElement( p_strParams, l_pOverlay);
		}
		p_pContext->pTextOverlay = static_pointer_cast<TextOverlay>( l_pOverlay->GetOverlayCategory());
		p_pContext->pOverlay = l_pOverlay.get();
		p_pContext->pOverlay->SetVisible( false);
	}
	else
	{
		PARSING_ERROR( "Directive <text_overlay> : No scene initialised. Enter a scene name");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootAmbientLight)
{
	if (p_pContext->pScene)
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

	if (l_pParent)
	{
		p_pContext->pSceneNode = l_pParent;
	}
	else
	{
		PARSING_ERROR( "Directive <light::parent> : Node " + p_strParams + " does not exist");
	}

	if (p_pContext->pLight)
	{
		p_pContext->pLight->Detach();
		p_pContext->pSceneNode->AttachObject( p_pContext->pLight.get());
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightType)
{
	if (p_pContext->pSceneNode)
	{
		if (p_strParams == "point_light")
		{
			p_pContext->eLightType = eLIGHT_TYPE_POINT;
			p_pContext->pLight = p_pContext->pScene->CreateLight( p_pContext->strName, p_pContext->pSceneNode, eLIGHT_TYPE_POINT);
		}
		else if (p_strParams == "spot_light")
		{
			p_pContext->eLightType = eLIGHT_TYPE_SPOT;
			p_pContext->pLight = p_pContext->pScene->CreateLight( p_pContext->strName, p_pContext->pSceneNode, eLIGHT_TYPE_SPOT);
		}
		else if (p_strParams == "directional")
		{
			p_pContext->eLightType = eLIGHT_TYPE_DIRECTIONAL;
			p_pContext->pLight = p_pContext->pScene->CreateLight( p_pContext->strName, p_pContext->pSceneNode, eLIGHT_TYPE_DIRECTIONAL);
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
	if (p_pContext->pLight)
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
	if (p_pContext->pLight)
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
	if (p_pContext->pLight)
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
	if (p_pContext->pLight)
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
	if (p_pContext->pLight)
	{
		Point3f l_vVector;

		if (PARSE_V3( float, l_vVector))
		{
			if (p_pContext->eLightType == eLIGHT_TYPE_POINT)
			{
				static_pointer_cast<PointLight>( p_pContext->pLight->GetLightCategory())->SetAttenuation( l_vVector);
			}
			else if (p_pContext->eLightType == eLIGHT_TYPE_SPOT)
			{
				static_pointer_cast<SpotLight>( p_pContext->pLight->GetLightCategory())->SetAttenuation( l_vVector);
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
	if (p_pContext->pLight)
	{
		float l_fFloat = p_strParams.to_float();

		if (p_pContext->eLightType == eLIGHT_TYPE_SPOT)
		{
			static_pointer_cast<SpotLight>( p_pContext->pLight->GetLightCategory())->SetCutOff( l_fFloat);
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
	if (p_pContext->pLight)
	{
		float l_fFloat = p_strParams.to_float();

		if (p_pContext->eLightType == eLIGHT_TYPE_SPOT)
		{
			static_pointer_cast<SpotLight>( p_pContext->pLight->GetLightCategory())->SetExponent( l_fFloat);
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
	if (p_pContext->pLight)
	{
		Quaternion l_qQuaternion;

		if (PARSE_V4( real, l_qQuaternion))
		{
			if (p_pContext->eLightType == eLIGHT_TYPE_SPOT)
			{
				p_pContext->pLight->GetParent()->Rotate( l_qQuaternion);
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
	p_pContext->eSection = eSECTION_ROOT;
	p_pContext->pLight.reset();
	p_pContext->strName.clear();
	p_pContext->pSceneNode.reset();
    return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeParent)
{
	if (p_pContext->pSceneNode)
	{
		SceneNodePtr l_pParent = p_pContext->pScene->GetNode( p_strParams);

		if (l_pParent)
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
	if (p_pContext->pSceneNode)
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
	if (p_pContext->pSceneNode)
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
	if (p_pContext->pSceneNode)
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
	p_pContext->eSection = eSECTION_ROOT;
	p_pContext->pSceneNode.reset();
	p_pContext->strName.clear();
    return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectParent)
{
	if (p_pContext->pGeometry)
	{
		SceneNodePtr l_pParent = p_pContext->pScene->GetNode( p_strParams);

		if (l_pParent)
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
	p_pContext->eSection = eSECTION_MESH;
	p_pContext->strName2 = p_strParams;
	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectEnd)
{
	p_pContext->eSection = eSECTION_ROOT;
	p_pContext->pScene->AddGeometry( p_pContext->pGeometry);
	p_pContext->pGeometry.reset();
	p_pContext->strName.clear();
    return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshType)
{
	String l_strType = p_strParams;
	eMESH_TYPE l_eType = eMESH_TYPE_CUSTOM;
	UIntArray l_arrayFaces;
	FloatArray l_arraySizes;

	if (l_strType != "custom")
	{
		if ( ! p_pContext->pMesh)
		{
			StringArray l_arrayMeshInfos = p_strParams.split( " ");
			l_strType = l_arrayMeshInfos[0];

			if (l_strType == "cube")
			{
				l_eType = eMESH_TYPE_CUBE;
				l_arraySizes.push_back( l_arrayMeshInfos[1].to_real());
				l_arraySizes.push_back( l_arrayMeshInfos[2].to_real());
				l_arraySizes.push_back( l_arrayMeshInfos[3].to_real());
			}
			else if (l_strType == "cone")
			{
				l_eType = eMESH_TYPE_CONE;
				l_arrayFaces.push_back( l_arrayMeshInfos[1].to_int());
				l_arraySizes.push_back( l_arrayMeshInfos[2].to_real());
				l_arraySizes.push_back( l_arrayMeshInfos[3].to_real());
			}
			else if (l_strType == "cylinder")
			{
				l_eType = eMESH_TYPE_CYLINDER;
				l_arrayFaces.push_back( l_arrayMeshInfos[1].to_int());
				l_arraySizes.push_back( l_arrayMeshInfos[2].to_real());
				l_arraySizes.push_back( l_arrayMeshInfos[3].to_real());
			}
			else if (l_strType == "sphere")
			{
				l_eType = eMESH_TYPE_SPHERE;
				l_arrayFaces.push_back( l_arrayMeshInfos[1].to_int());
				l_arraySizes.push_back( l_arrayMeshInfos[2].to_real());
			}
			else if (l_strType == "icosaedron")
			{
				l_eType = eMESH_TYPE_ICOSAEDRON;
				l_arrayFaces.push_back( l_arrayMeshInfos[1].to_int());
				l_arraySizes.push_back( l_arrayMeshInfos[2].to_real());
			}
			else if (l_strType == "plane")
			{
				l_eType = eMESH_TYPE_PLANE;
				l_arrayFaces.push_back( l_arrayMeshInfos[1].to_int());
				l_arrayFaces.push_back( l_arrayMeshInfos[2].to_int());
				l_arraySizes.push_back( l_arrayMeshInfos[3].to_real());
				l_arraySizes.push_back( l_arrayMeshInfos[4].to_real());
			}
			else if (l_strType == "torus")
			{
				l_eType = eMESH_TYPE_TORUS;
				l_arrayFaces.push_back( l_arrayMeshInfos[1].to_int());
				l_arrayFaces.push_back( l_arrayMeshInfos[2].to_int());
				l_arraySizes.push_back( l_arrayMeshInfos[3].to_real());
				l_arraySizes.push_back( l_arrayMeshInfos[4].to_real());
			}

			if (l_eType != eMESH_TYPE_CUSTOM)
			{
				Collection<Mesh, String> l_mshCollection;
				p_pContext->pMesh = l_mshCollection.GetElement( p_pContext->strName);
				if ( ! p_pContext->pMesh)
				{
					p_pContext->pMesh = MeshPtr( new Mesh( p_pContext->strName, l_eType));
					p_pContext->pMesh->Initialise( l_arrayFaces, l_arraySizes);
					l_mshCollection.AddElement( p_pContext->strName, p_pContext->pMesh);
				}
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
		Collection<Mesh, String> l_mshCollection;
		p_pContext->pMesh = l_mshCollection.GetElement( p_pContext->strName);
		if ( ! p_pContext->pMesh)
		{
			p_pContext->pMesh = MeshPtr( new Mesh( p_pContext->strName, l_eType));
			p_pContext->pMesh->Initialise( l_arrayFaces, l_arraySizes);
			l_mshCollection.AddElement( p_pContext->strName, p_pContext->pMesh);
		}
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshFile)
{
	Collection<Mesh, String> l_mshCollection;
	p_pContext->pMesh = l_mshCollection.GetElement( cuEmptyString);
	if ( ! p_pContext->pMesh)
	{
		p_pContext->pMesh = MeshPtr( new Mesh( cuEmptyString, eMESH_TYPE_CUSTOM));
		l_mshCollection.AddElement( cuEmptyString, p_pContext->pMesh);
	}

	if ( ! p_pContext->pMesh)
	{
		PARSING_ERROR( "Directive <mesh::file> : Can't load mesh file " + p_strParams);
	}

	File l_file( p_pContext->pFile->GetFilePath() / p_strParams, File::eOPEN_MODE_READ | File::eOPEN_MODE_BINARY);
	p_pContext->pMesh->Unserialise( l_file, p_pContext->pScene);

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshMaterial)
{
	if (p_pContext->pMesh)
	{
		Root::MaterialManager * l_pManager = Root::GetSingleton()->GetMaterialManager();
		if (l_pManager->HasElement( p_strParams))
		{
			for (size_t i = 0 ; i < p_pContext->pMesh->GetNbSubmeshes() ; i++)
			{
				p_pContext->pMesh->GetSubmesh( i)->SetMaterial( l_pManager->GetElement( p_strParams));
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
	p_pContext->eSection = eSECTION_SUBMESH;

	if (p_pContext->pMesh)
	{
		p_pContext->pSubmesh = p_pContext->pMesh->CreateSubmesh( 0);
	}
	else
	{
		PARSING_ERROR( "Directive <mesh::submesh> : Mesh not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshEnd)
{
	p_pContext->eSection = eSECTION_OBJECT;

	if (p_pContext->pMesh)
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
	if (p_pContext->pSubmesh)
	{
		Root::MaterialManager * l_pManager = Root::GetSingleton()->GetMaterialManager();

		if (l_pManager->HasElement( p_strParams))
		{
			p_pContext->pSubmesh->SetMaterial( l_pManager->GetElement( p_strParams));
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
	if (p_pContext->pSubmesh)
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
	p_pContext->eSection = eSECTION_SMOOTHING_GROUP;

	if (p_pContext->pSubmesh)
	{
		p_pContext->uiSmoothingGroup = p_pContext->pSubmesh->AddSmoothingGroup()->GetGroupID();
	}
	else
	{
		PARSING_ERROR( "Directive <submesh::smoothing_group> : Submesh not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshEnd)
{
	p_pContext->eSection = eSECTION_MESH;

	if (p_pContext->pSubmesh)
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
	p_pContext->eSection = eSECTION_SUBMESH;
	p_pContext->uiSmoothingGroup = 0;
	p_pContext->iFace1 = -1;
	p_pContext->iFace2 = -1;
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MaterialPass)
{
	p_pContext->eSection = eSECTION_PASS;

	if (p_pContext->pMaterial)
	{
		p_pContext->pMaterial->CreatePass();
		p_pContext->uiPass = p_pContext->pMaterial->GetNbPasses() - 1;
	}
	else
	{
		PARSING_ERROR( "Directive <material::pass> : Material not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_MaterialEnd)
{
	p_pContext->eSection = eSECTION_ROOT;
	Root::GetSingleton()->GetMaterialManager()->SetToInitialise( p_pContext->pMaterial);
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
			std::map<String, Pass::eSRC_BLEND>::iterator l_itSrc = Pass::MapSrcBlendFactors.find( l_arrayParams[0]);
			std::map<String, Pass::eDST_BLEND>::iterator l_itDst = Pass::MapDstBlendFactors.find( l_arrayParams[1]);

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
	p_pContext->eSection = eSECTION_TEXTURE_UNIT;

	if (p_pContext->uiPass >= 0)
	{
		p_pContext->pTextureUnit = p_pContext->pMaterial->GetPass( p_pContext->uiPass)->AddTextureUnit();
	}
	else
	{
		PARSING_ERROR( "Directive <pass::texture_unit> : Pass not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassGlShader)
{
	p_pContext->eSection = eSECTION_GLSL_SHADER;

	if (p_pContext->uiPass >= 0)
	{
		p_pContext->pShaderProgram = RenderSystem::CreateShaderProgram<GlslShaderProgram>();
	}
	else
	{
		PARSING_ERROR( "Directive <pass::gl_shader> : Pass not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassHlShader)
{
	p_pContext->eSection = eSECTION_HLSL_SHADER;

	if (p_pContext->uiPass >= 0)
	{
		p_pContext->pShaderProgram = RenderSystem::CreateShaderProgram<HlslShaderProgram>();
	}
	else
	{
		PARSING_ERROR( "Directive <pass::hl_shader> : Pass not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassCgShader)
{
	p_pContext->eSection = eSECTION_CG_SHADER;

	if (p_pContext->uiPass >= 0)
	{
		p_pContext->pShaderProgram = RenderSystem::CreateShaderProgram<CgShaderProgram>();
	}
	else
	{
		PARSING_ERROR( "Directive <pass::cg_shader> : Pass not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassEnd)
{
	p_pContext->eSection = eSECTION_MATERIAL;
	p_pContext->uiPass = -1;
	p_pContext->strName.clear();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitImage)
{
	if (p_pContext->pTextureUnit)
	{
		if (File::FileExists( p_pContext->pFile->GetFilePath() / p_strParams))
		{
			Collection<Image, String> l_imgCollection;
			ImagePtr l_pImage = l_imgCollection.GetElement(  p_pContext->pFile->GetFilePath() / p_strParams);
			if ( ! l_pImage)
			{
				l_pImage = ImagePtr( new Image( p_pContext->pFile->GetFilePath() / p_strParams));
				l_imgCollection.AddElement( p_pContext->pFile->GetFilePath() / p_strParams, l_pImage);
			}
			Loader<Image>::Load( * l_pImage, p_pContext->pFile->GetFilePath() / p_strParams);
			p_pContext->pTextureUnit->SetTexture( eTEXTURE_TYPE_2D, l_pImage);
		}
		else if (File::FileExists( p_strParams))
		{
			Collection<Image, String> l_imgCollection;
			ImagePtr l_pImage = l_imgCollection.GetElement(  p_strParams);
			if ( ! l_pImage)
			{
				l_pImage = ImagePtr( new Image(  p_strParams));
				l_imgCollection.AddElement( p_strParams, l_pImage);
			}
			Loader<Image>::Load( * l_pImage, p_strParams);
			p_pContext->pTextureUnit->SetTexture( eTEXTURE_TYPE_2D, l_pImage);
		}
		else
		{
			PARSING_ERROR( "Directive <texture_unit::image> : File " + (p_pContext->pFile->GetFilePath() / p_strParams) + " not found, check the relativeness of the path");
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
	if (p_pContext->pTextureUnit)
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
	if (p_pContext->pTextureUnit)
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
	if (p_pContext->pTextureUnit)
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

IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitRgbBlend)
{
	bool l_bContinue = p_pContext->pTextureUnit.use_count() > 0;
	StringArray l_arrayParams;
	std::map <String, TextureUnit::eBLEND_SOURCE>::const_iterator l_itArg1;
	std::map <String, TextureUnit::eBLEND_SOURCE>::const_iterator l_itArg2;
	std::map <String, TextureUnit::eRGB_BLEND_FUNC>::const_iterator l_itMode;

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
		l_itMode = TextureUnit::MapTextureRgbFunctions.find( l_arrayParams[0]);
		l_bContinue = l_itMode != TextureUnit::MapTextureRgbFunctions.end();
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
		p_pContext->pTextureUnit->SetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_0, l_itArg1->second);
		p_pContext->pTextureUnit->SetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_1, l_itArg2->second);
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
	bool l_bContinue = p_pContext->pTextureUnit.use_count() > 0;
	StringArray l_arrayParams;
	std::map <String, TextureUnit::eBLEND_SOURCE>::const_iterator l_itArg1;
	std::map <String, TextureUnit::eBLEND_SOURCE>::const_iterator l_itArg2;
	std::map <String, TextureUnit::eALPHA_BLEND_FUNC>::const_iterator l_itMode;

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
		l_itMode = TextureUnit::MapTextureAlphaFunctions.find( l_arrayParams[0]);
		l_bContinue = l_itMode != TextureUnit::MapTextureAlphaFunctions.end();
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
		p_pContext->pTextureUnit->SetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_0, l_itArg1->second);
		p_pContext->pTextureUnit->SetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_1, l_itArg2->second);
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
	p_pContext->eSection = eSECTION_PASS;
	p_pContext->pTextureUnit.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlVertexShader)
{
	p_pContext->eSection = eSECTION_GLSL_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_VERTEX);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_VERTEX);
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::vertex_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlPixelShader)
{
	p_pContext->eSection = eSECTION_GLSL_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_PIXEL);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_PIXEL);
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::pixel_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlGeometryShader)
{
	p_pContext->eSection = eSECTION_GLSL_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_GEOMETRY);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_GEOMETRY);
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::geometry_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlHullShader)
{
	p_pContext->eSection = eSECTION_GLSL_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_HULL);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_HULL);
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::hull_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlDomainShader)
{
	p_pContext->eSection = eSECTION_GLSL_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_DOMAIN);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_DOMAIN);
	}
	else
	{
		PARSING_ERROR( "Directive <gl_shader_program::domain_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderEnd)
{
	p_pContext->eSection = eSECTION_PASS;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetShader( p_pContext->pShaderProgram);
	}

	p_pContext->pShaderProgram.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramFile)
{
	if (p_pContext->pShaderObject)
	{
		p_pContext->pShaderObject->SetFile( p_pContext->pFile->GetFilePath() / p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <gl_program::file> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramText)
{
	if (p_pContext->pShaderObject)
	{
	}
	else
	{
		PARSING_ERROR( "Directive <gl_program::text> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramVariable)
{
	p_pContext->eSection = eSECTION_GLSL_SHADER_VARIABLE;

	if ( ! p_pContext->pShaderObject)
	{
		PARSING_ERROR( "Directive <gl_program::variable> : GPU Program not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderProgramEnd)
{
	p_pContext->eSection = eSECTION_GLSL_SHADER;
	p_pContext->pShaderObject.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableType)
{
	if (p_pContext->pShaderProgram && p_pContext->pShaderObject)
	{
		if ( ! p_pContext->pFrameVariable)
		{
			if (p_strParams == "int")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<OneIntFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "uint")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<OneUIntFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "float")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<OneFloatFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "vec2i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point2iFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "vec3i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point3iFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "vec4i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point4iFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "vec2ui")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point2uiFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "vec3ui")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point3uiFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "vec4ui")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point4uiFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "vec2f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point2fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "vec3f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point3fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "vec4f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point4fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "mat2x2f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x2fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "mat2x3f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x3fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "mat2x4f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x4fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "mat3x2f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x2fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "mat3x3f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x3fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "mat3x4f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x4fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "mat4x2f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x2fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "mat4x3f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x3fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else if (p_strParams == "mat4x4f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x4fFrameVariable>( 1, p_pContext->pShaderObject);
			}
			else
			{
				PARSING_ERROR( "Directive <gl_variable::type> : Unknown variable type : " + p_strParams);
			}
		}
		else
		{
			PARSING_ERROR( "Directive <gl_variable::type> : Variable type already set");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <gl_variable::type> : Shader program or object not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableName)
{
	if (p_pContext->pFrameVariable)
	{
		p_pContext->pFrameVariable->SetName( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <gl_variable::name> : Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableValue)
{
	if (p_pContext->pFrameVariable)
	{
		p_pContext->pFrameVariable->SetValue( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <gl_variable::value> : Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_GlShaderVariableEnd)
{
	p_pContext->eSection = eSECTION_GLSL_SHADER_PROGRAM;
	p_pContext->pFrameVariable.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlVertexShader)
{
	p_pContext->eSection = eSECTION_HLSL_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_VERTEX);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_VERTEX);
	}
	else
	{
		PARSING_ERROR( "Directive <hl_shader_program::vertex_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlPixelShader)
{
	p_pContext->eSection = eSECTION_HLSL_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_PIXEL);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_PIXEL);
	}
	else
	{
		PARSING_ERROR( "Directive <hl_shader_program::pixel_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlGeometryShader)
{
	p_pContext->eSection = eSECTION_HLSL_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_GEOMETRY);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_GEOMETRY);
	}
	else
	{
		PARSING_ERROR( "Directive <hl_shader_program::geometry_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlHullShader)
{
	p_pContext->eSection = eSECTION_HLSL_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_HULL);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_HULL);
	}
	else
	{
		PARSING_ERROR( "Directive <hl_shader_program::hull_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlDomainShader)
{
	p_pContext->eSection = eSECTION_HLSL_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_DOMAIN);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_DOMAIN);
	}
	else
	{
		PARSING_ERROR( "Directive <hl_shader_program::domain_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlFile)
{
	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->SetFile( p_pContext->pFile->GetFilePath() / p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <hl_shader_program::file> : Shader not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderEnd)
{
	p_pContext->eSection = eSECTION_PASS;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetShader( p_pContext->pShaderProgram);
	}

	p_pContext->pShaderProgram.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderProgramFile)
{
	if (p_pContext->pShaderObject)
	{
		p_pContext->pShaderObject->SetFile( p_pContext->pFile->GetFilePath() / p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <hl_program::file> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderProgramEntry)
{
	if (p_pContext->pShaderObject)
	{
		static_pointer_cast<HlslShaderObject>( p_pContext->pShaderObject)->SetEntryPoint( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <hl_program::entry> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderProgramText)
{
	if (p_pContext->pShaderObject)
	{
	}
	else
	{
		PARSING_ERROR( "Directive <hl_program::text> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderProgramVariable)
{
	p_pContext->eSection = eSECTION_HLSL_SHADER_VARIABLE;

	if ( ! p_pContext->pShaderObject)
	{
		PARSING_ERROR( "Directive <hl_program::variable> : GPU Program not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderProgramEnd)
{
	p_pContext->eSection = eSECTION_HLSL_SHADER;
	p_pContext->pShaderObject.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderVariableType)
{
	if (p_pContext->pShaderProgram && p_pContext->pShaderObject)
	{
		if ( ! p_pContext->pFrameVariable)
		{
			if (p_strParams == "int")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<OneIntFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "float")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<OneFloatFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec2i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point2iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec3i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point3iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec4i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point4iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec2f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point2fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec3f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point3fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec4f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point4fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat4x4f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x4fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else
			{
				PARSING_ERROR( "Directive <hl_variable::type> : Unknown variable type : " + p_strParams);
			}
		}
		else
		{
			PARSING_ERROR( "Directive <hl_variable::type> : Variable type already set");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <hl_variable::type> : Shader program or object not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderVariableName)
{
	if (p_pContext->pFrameVariable)
	{
		p_pContext->pFrameVariable->SetName( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <hl_variable::name> : Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderVariableValue)
{
	if (p_pContext->pFrameVariable)
	{
		p_pContext->pFrameVariable->SetValue( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <hl_variable::value> : Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_HlShaderVariableEnd)
{
	p_pContext->eSection = eSECTION_HLSL_SHADER_PROGRAM;
	p_pContext->pFrameVariable.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgVertexShader)
{
	p_pContext->eSection = eSECTION_CG_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_VERTEX);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_VERTEX);
	}
	else
	{
		PARSING_ERROR( "Directive <cg_shader_program::vertex_program> : Cg Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgPixelShader)
{
	p_pContext->eSection = eSECTION_CG_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_PIXEL);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_PIXEL);
	}
	else
	{
		PARSING_ERROR( "Directive <cg_shader_program::pixel_program> : Cg Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgGeometryShader)
{
	p_pContext->eSection = eSECTION_CG_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_GEOMETRY);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_GEOMETRY);
	}
	else
	{
		PARSING_ERROR( "Directive <cg_shader_program::geometry_program> : Cg Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgHullShader)
{
	p_pContext->eSection = eSECTION_CG_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_HULL);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_HULL);
	}
	else
	{
		PARSING_ERROR( "Directive <cg_shader_program::hull_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgDomainShader)
{
	p_pContext->eSection = eSECTION_CG_SHADER_PROGRAM;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_DOMAIN);
		p_pContext->pShaderObject = p_pContext->pShaderProgram->GetProgram( eSHADER_TYPE_DOMAIN);
	}
	else
	{
		PARSING_ERROR( "Directive <cg_shader_program::domain_program> : Shader not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgFile)
{
	if (p_pContext->pShaderProgram)
	{
		p_pContext->pShaderProgram->SetFile( p_pContext->pFile->GetFilePath() / p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <cg_shader_program::file> : Cg Shader not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderEnd)
{
	p_pContext->eSection = eSECTION_PASS;

	if (p_pContext->pShaderProgram)
	{
		p_pContext->pMaterial->GetPass( p_pContext->uiPass)->SetShader( p_pContext->pShaderProgram);
	}

	p_pContext->pShaderProgram.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramFile)
{
	if (p_pContext->pShaderObject)
	{
		p_pContext->pShaderObject->SetFile( p_pContext->pFile->GetFilePath() / p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <cg_program::file> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramText)
{
	if (p_pContext->pShaderObject)
	{
	}
	else
	{
		PARSING_ERROR( "Directive <cg_program::text> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramVariable)
{
	p_pContext->eSection = eSECTION_CG_SHADER_VARIABLE;

	if ( ! p_pContext->pShaderObject)
	{
		PARSING_ERROR( "Directive <cg_program::variable> : GPU Program not initialised");
	}

	return true;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramEntry)
{
	if (p_pContext->pShaderObject)
	{
		static_pointer_cast<CgShaderObject>( p_pContext->pShaderObject)->SetEntryPoint( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <cg_program::entry> : Shader Program not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderProgramEnd)
{
	p_pContext->eSection = eSECTION_CG_SHADER;
	p_pContext->pShaderObject.reset();
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderVariableType)
{
	if (p_pContext->pShaderProgram && p_pContext->pShaderObject)
	{
		if ( ! p_pContext->pFrameVariable)
		{
			if (p_strParams == "int")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<OneIntFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "float")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<OneFloatFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "double")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<OneDoubleFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec2i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point2iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec3i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point3iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec4i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point4iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec2f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point2fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec3f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point3fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec4f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point4fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec2d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point2dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec3d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point3dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "vec4d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Point4dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat2x2i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x2iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat2x3i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x3iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat2x4i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x4iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat3x2i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x2iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat3x3i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x3iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat3x4i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x4iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat4x2i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x2iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat4x3i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x3iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat4x4i")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x4iFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat2x2f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x2fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat2x3f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x3fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat2x4f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x4fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat3x2f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x2fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat3x3f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x3fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat3x4f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x4fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat4x2f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x2fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat4x3f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x3fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat4x4f")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x4fFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat2x2d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x2dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat2x3d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x3dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat2x4d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix2x4dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat3x2d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x2dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat3x3d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x3dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat3x4d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix3x4dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat4x2d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x2dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat4x3d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x3dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else if (p_strParams == "mat4x4d")
			{
				p_pContext->pFrameVariable = p_pContext->pShaderProgram->CreateFrameVariable<Matrix4x4dFrameVariable>( 1, p_pContext->pShaderObject->GetShaderType());
			}
			else
			{
				PARSING_ERROR( "Directive <cg_variable::type> : Unknown variable type : " + p_strParams);
			}
		}
		else
		{
			PARSING_ERROR( "Directive <cg_variable::type> : Cg Variable type already set");
		}
	}
	else
	{
		PARSING_ERROR( "Directive <cg_variable::type> : Shader program or object not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderVariableName)
{
	if (p_pContext->pFrameVariable)
	{
		p_pContext->pFrameVariable->SetName( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <cg_variable::name> : Cg Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderVariableValue)
{
	if (p_pContext->pFrameVariable)
	{
		p_pContext->pFrameVariable->SetValue( p_strParams);
	}
	else
	{
		PARSING_ERROR( "Directive <cg_variable::value> : Cg Variable not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_CgShaderVariableEnd)
{
	p_pContext->eSection = eSECTION_CG_SHADER_PROGRAM;
	p_pContext->pFrameVariable.reset();
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
		Collection<Font, String> l_fntCollection;
		FontPtr l_pFont = l_fntCollection.GetElement( p_pContext->strName);
		if ( ! l_pFont)
		{
			l_pFont = FontPtr( new Font( p_pContext->strName, p_pContext->uiUInt));
			l_fntCollection.AddElement( p_pContext->strName, l_pFont);
		}
		Loader<Font>::Load( * l_pFont, p_pContext->pFile->GetFilePath() / p_pContext->strName2, p_pContext->uiUInt);
	}

	p_pContext->eSection = eSECTION_ROOT;
	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayPosition)
{
	if (p_pContext->pOverlay)
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
	if (p_pContext->pOverlay)
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
	if (p_pContext->pOverlay)
	{
		Root::MaterialManager * l_pManager = Root::GetSingleton()->GetMaterialManager();
		p_pContext->pOverlay->SetMaterial( l_pManager->GetElement( p_strParams));
	}
	else
	{
		PARSING_ERROR( "Directive <overlay::material> : Overlay not initialised");
	}

	return false;
}

IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayFont)
{
	if (p_pContext->pTextOverlay)
	{
		Collection<Font, String> l_fontManager;
		if (l_fontManager.HasElement( p_strParams))
		{
			p_pContext->pTextOverlay->SetFont( p_strParams);
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
	if (p_pContext->pTextOverlay)
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

	if (l_pParent)
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
		if (p_strParams == cuT( "3d"))
		{
			p_pContext->eViewportType = eVIEWPORT_TYPE_3D;
		}
		else if (p_strParams == cuT( "2d"))
		{
			p_pContext->eViewportType = eVIEWPORT_TYPE_2D;
		}
		else
		{
			PARSING_ERROR( "Directive <camera::viewport> : Unknown viewport mode : " + p_strParams);
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
	if (p_pContext->pSceneNode && p_pContext->eViewportType != -1)
	{
		p_pContext->pScene->CreateCamera( p_pContext->strName, 100, 100, p_pContext->pSceneNode, p_pContext->eViewportType);
	}

	p_pContext->eViewportType = eVIEWPORT_TYPE( -1);
	p_pContext->pSceneNode.reset();
	p_pContext->eSection = eSECTION_ROOT;
	return false;
}
