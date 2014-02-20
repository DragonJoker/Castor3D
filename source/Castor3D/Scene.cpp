#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Scene.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/Animation.hpp"
#include "Castor3D/AnimatedObjectGroup.hpp"
#include "Castor3D/AnimatedObject.hpp"
#include "Castor3D/Light.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/Mesh.hpp"
#include "Castor3D/Geometry.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/Camera.hpp"
#include "Castor3D/Ray.hpp"
#include "Castor3D/Importer.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/PointLight.hpp"
#include "Castor3D/SpotLight.hpp"
#include "Castor3D/Skeleton.hpp"
#include "Castor3D/DirectionalLight.hpp"
#include "Castor3D/FrameVariable.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/BlendState.hpp"
#include "Castor3D/BillboardList.hpp"
#include "Castor3D/ShaderManager.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

struct AnmObjGrpUpdater
{
	void operator()( std::pair< String, AnimatedObjectGroupSPtr > p_pair ){ p_pair.second->Update(); }
};

class LghtRenderer
{
public:
	void operator()( LightSPtr p_pLight ) { p_pLight->Render(); }
	void operator()( std::pair< int, LightSPtr > p_pair ) { p_pair.second->Render(); }
};

class LghtUnrenderer
{
public:
	void operator()( LightSPtr p_pLight ) { p_pLight->EndRender(); }
	void operator()( std::pair< int, LightSPtr > p_pair ) { p_pair.second->EndRender(); }
};

class SubmeshAnimUpdater
{
private:
	real m_rAnimationTime;

public:
	SubmeshAnimUpdater( real p_rAnimationTime )
		:	m_rAnimationTime	( p_rAnimationTime	)
	{
	}
	void operator()( SubmeshSPtr p_pSubmesh/*, const aiNode* pNode, const Matrix4f& ParentTransform*/ )
	{/*
		String l_strNodeName( pNode->mName.data );

		const aiAnimation* pAnimation = m_pScene->mAnimations[0];

		Matrix4f NodeTransformation(pNode->mTransformation);

		const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

		if (pNodeAnim) {
			// Interpolate scaling and generate scaling transformation matrix
			aiVector3D Scaling;
			CalcInterpolatedScaling(Scaling, m_rAnimationTime, pNodeAnim);
			Matrix4f ScalingM;
			ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

			// Interpolate rotation and generate rotation transformation matrix
			aiQuaternion RotationQ;
			CalcInterpolatedRotation(RotationQ, m_rAnimationTime, pNodeAnim);
			Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

			// Interpolate translation and generate translation transformation matrix
			aiVector3D Translation;
			CalcInterpolatedPosition(Translation, m_rAnimationTime, pNodeAnim);
			Matrix4f TranslationM;
			TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

			// Combine the above transformations
			NodeTransformation = TranslationM * RotationM * ScalingM;
		}

		Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

		if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
		{
			uint BoneIndex = m_BoneMapping[NodeName];
			m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
		}

		for (uint i = 0 ; i < pNode->mNumChildren ; i++)
		{
			ReadNodeHeirarchy( m_rAnimationTime, pNode->mChildren[i], GlobalTransformation);
		}*/
	}
};

//*************************************************************************************************

Scene::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< Scene, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool Scene::TextLoader :: operator ()( Scene const & p_scene, TextFile & p_file)
{
	bool l_bReturn = true;

	Logger::LogMessage( cuT( "Scene :: Write - Scene Name"));

	p_file.WriteText( cuT( "scene " ) + p_scene.GetName() + cuT( "\n{\n" ) );

	if (l_bReturn)
	{
		Logger::LogMessage( cuT( "Scene :: Write - Camera Nodes" ) );
		l_bReturn = SceneNode::TextLoader()( * p_scene.GetCameraRootNode(), p_file);
	}

	if( l_bReturn )
	{
		Logger::LogMessage( cuT( "Scene :: Write - Object Nodes" ) );
		l_bReturn = SceneNode::TextLoader()( * p_scene.GetObjectRootNode(), p_file);
	}

	if (l_bReturn)
	{
		Logger::LogMessage( cuT( "Scene :: Write - Cameras"));
		CameraPtrStrMapConstIt l_it = p_scene.CamerasBegin();

		while (l_bReturn && l_it != p_scene.CamerasEnd())
		{
			l_bReturn = Camera::TextLoader()( * l_it->second, p_file);
			++l_it;
		}
	}

	if (l_bReturn)
	{
		Logger::LogMessage( cuT( "Scene :: Write - Lights"));

		LightPtrIntMapConstIt l_it = p_scene.LightsBegin();

		while (l_bReturn && l_it != p_scene.LightsEnd())
		{
			switch (l_it->second->GetLightType())
			{
			case eLIGHT_TYPE_DIRECTIONAL:
				l_bReturn = DirectionalLight::TextLoader()( * std::static_pointer_cast<DirectionalLight>( l_it->second->GetLightCategory()), p_file);
				break;

			case eLIGHT_TYPE_POINT:
				l_bReturn = PointLight::TextLoader()( * std::static_pointer_cast<PointLight>( l_it->second->GetLightCategory()), p_file);
				break;

			case eLIGHT_TYPE_SPOT:
				l_bReturn = SpotLight::TextLoader()( * std::static_pointer_cast<SpotLight>( l_it->second->GetLightCategory()), p_file);
				break;

			default:
				l_bReturn = false;
			}
			++l_it;
		}
	}

	if (l_bReturn)
	{
		Logger::LogMessage( cuT( "Scene :: Write - Geometries"));

		GeometryPtrStrMapConstIt l_it = p_scene.GeometriesBegin();

		while (l_bReturn && l_it != p_scene.GeometriesEnd())
		{
			l_bReturn = Geometry::TextLoader()( * l_it->second, p_file);
			++l_it;
		}
	}

	p_file.WriteText( cuT( "}" ) );

	return l_bReturn;
}

//*************************************************************************************************

Scene :: Scene( Engine * p_pEngine, LightFactory & p_lightFactory, String const & p_name )
	:	m_strName				( p_name											)
	,	m_rootCameraNode		(													)
	,	m_rootObjectNode		(													)
	,	m_nbFaces				( 0													)
	,	m_nbVertex				( 0													)
	,	m_changed				( false												)
	,	m_lightFactory			( p_lightFactory									)
	,	m_pEngine				( p_pEngine											)
{
	m_rootNode			= std::make_shared< SceneNode >( this, cuT( "RootNode"			) );
	m_rootCameraNode	= std::make_shared< SceneNode >( this, cuT( "CameraRootNode"	) );
	m_rootObjectNode	= std::make_shared< SceneNode >( this, cuT( "ObjectRootNode"	) );
	m_rootCameraNode->AttachTo( m_rootNode.get());
	m_rootObjectNode->AttachTo( m_rootNode.get());
}

Scene :: ~Scene()
{
	ClearScene();
	DoRemoveAll( m_addedCameras, m_arrayCamerasToDelete );
	DoRemoveAll( m_addedNodes, m_arrayNodesToDelete );
	m_arrayCamerasToDelete.clear();
	m_arrayNodesToDelete.clear();
	m_rootNode.reset();
	m_rootCameraNode.reset();
	m_rootObjectNode.reset();
}

void Scene :: ClearScene()
{
	m_mapSubmeshesAlpha.clear();
	m_mapSubmeshesNoAlpha.clear();
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	DoRemoveAll( m_addedLights, m_arrayLightsToDelete );
	DoRemoveAll( m_addedPrimitives, m_arrayPrimitivesToDelete );
	DoRemoveAll( m_mapBillboardsLists, m_arrayBillboardsToDelete );

	if( m_pBackgroundImage )
	{
		m_pEngine->PostEvent( std::make_shared< CleanupEvent< TextureBase > >( *m_pBackgroundImage ) );
	}

	m_arrayLightsToDelete.clear();
	m_arrayPrimitivesToDelete.clear();
}

void Scene :: Render( eTOPOLOGY p_eTopology, double CU_PARAM_UNUSED( p_dFrameTime ), Camera const & p_camera )
{
	RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();
	Pipeline * l_pPipeline = l_pRenderSystem->GetPipeline();
	ContextRPtr l_pContext = l_pRenderSystem->GetCurrentContext();
	PassSPtr l_pPass;
	MaterialSPtr l_pMaterial;
	SubmeshRendererSPtr l_pRenderer;
	DoDeleteToDelete();

	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	DoUpdateAnimations();

	if( m_pBackgroundImage && !m_pBackgroundImage->IsInitialised() )
	{
		m_pBackgroundImage->Initialise( 0 );
	}

	if( !m_arraySubmeshesNoAlpha.empty() || !m_mapSubmeshesAlpha.empty() || !m_arraySubmeshesAlpha.empty() || !m_mapBillboardsLists.empty() )
	{

		if( !l_pRenderSystem->ForceShaders() )
		{
			std::for_each( m_mapLights.begin(), m_mapLights.end(), LghtRenderer() );
		}

		l_pPipeline->MatrixMode( eMTXMODE_MODEL );
		l_pPipeline->LoadIdentity();

		if( m_arraySubmeshesNoAlpha.size() )
		{
			l_pContext->CullFace( eFACE_BACK );

			if( l_pRenderSystem->HasInstancing() )
			{
				DoRenderSubmeshes( p_camera, *l_pPipeline, p_eTopology, m_mapSubmeshesNoAlpha.begin(), m_mapSubmeshesNoAlpha.end() );
			}
			else
			{
				DoRenderSubmeshes( p_camera, *l_pPipeline, p_eTopology, m_arraySubmeshesNoAlpha.begin(), m_arraySubmeshesNoAlpha.end() );
			}
		}

		if( !m_mapSubmeshesAlpha.empty() || !m_arraySubmeshesAlpha.empty() )
		{
			if( l_pContext->IsMultiSampling() )
			{
				l_pContext->CullFace( eFACE_FRONT );
				DoRenderSubmeshes( p_camera, *l_pPipeline, p_eTopology, m_arraySubmeshesAlpha.begin(), m_arraySubmeshesAlpha.end() );
				l_pContext->CullFace( eFACE_BACK );
				DoRenderSubmeshes( p_camera, *l_pPipeline, p_eTopology, m_arraySubmeshesAlpha.begin(), m_arraySubmeshesAlpha.end() );
			}
			else
			{
				DoResortAlpha( p_camera );
				l_pContext->CullFace( eFACE_FRONT );
				DoRenderSubmeshes( *l_pPipeline, p_eTopology, m_mapSubmeshesAlpha.begin(), m_mapSubmeshesAlpha.end() );
				l_pContext->CullFace( eFACE_BACK );
				DoRenderSubmeshes( *l_pPipeline, p_eTopology, m_mapSubmeshesAlpha.begin(), m_mapSubmeshesAlpha.end() );
			}
		}

		if( !m_mapBillboardsLists.empty() )
		{
			l_pContext->CullFace( eFACE_BACK );
			DoRenderBillboards( *l_pPipeline, m_mapBillboardsLists.begin(), m_mapBillboardsLists.end() );
		}

		l_pPipeline->MatrixMode( eMTXMODE_VIEW );

		if( !l_pRenderSystem->ForceShaders() )
		{
			std::for_each( m_mapLights.begin(), m_mapLights.end(), LghtUnrenderer() );
		}
	}
}

bool Scene :: SetBackgroundImage( Path const & p_pathFile )
{
	bool		l_bReturn = false;
	ImageSPtr	l_pImage;

	if( !p_pathFile.empty() )
	{
		l_pImage = m_pEngine->GetImageManager().find( p_pathFile.GetFileName() );

		if( !l_pImage && File::FileExists( p_pathFile ) )
		{
			l_pImage = std::make_shared< Image >( p_pathFile.GetFileName(), p_pathFile );
			m_pEngine->GetImageManager().insert( p_pathFile.GetFileName(), l_pImage );
		}
	}

	if( l_pImage )
	{
		StaticTextureSPtr l_pStaTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
		l_pStaTexture->SetDimension( eTEXTURE_DIMENSION_2D );
		l_pStaTexture->SetImage( l_pImage->GetPixels() );
		m_pBackgroundImage = l_pStaTexture;
		l_bReturn = true;
	}

	return l_bReturn;
}

void Scene :: InitialiseGeometries()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	m_nbFaces = 0;
	m_nbVertex = 0;

	if (m_newlyAddedPrimitives.size() > 0)
	{
		GeometryPtrStrMapIt l_it = m_newlyAddedPrimitives.begin();

		for (; l_it != m_newlyAddedPrimitives.end(); ++l_it)
		{
			l_it->second->CreateBuffers( m_nbFaces, m_nbVertex );
		}

		m_newlyAddedPrimitives.clear();
	}
	else
	{
		m_rootNode->CreateBuffers( m_nbFaces, m_nbVertex );
	}

	String l_strToLog = cuT( "Scene :: CreateList - [" ) + m_strName + cuT( "] - NbVertex : %d - NbFaces : %d" );
	Logger::LogMessage( l_strToLog.c_str(), m_nbVertex, m_nbFaces );
	DoSortByAlpha();
	m_changed = false;
}

SceneNodeSPtr Scene :: CreateSceneNode( String const & p_name, SceneNode * p_parent )
{
	SceneNodeSPtr l_pReturn;

	if (p_name != cuT( "RootNode") )
	{
		if( DoCheckObject( p_name, m_addedNodes, cuT( "SceneNode" ) ) )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();

			l_pReturn = std::make_shared< SceneNode >( this, p_name );

			if (p_parent)
			{
				l_pReturn->AttachTo( p_parent);
			}
			else
			{
				l_pReturn->AttachTo( m_rootNode.get());
			}

			m_addedNodes[p_name] = l_pReturn;
		}
		else
		{
			l_pReturn = m_addedNodes.find( p_name )->second;
		}
	}
	else
	{
		Logger::LogWarning( cuT( "Scene :: CreateSceneNode - Can't create scene node [RootNode] - Another scene node with the same name already exists") );
		l_pReturn = m_addedNodes.find( p_name)->second;
	}

	return l_pReturn;
}

SceneNodeSPtr Scene :: CreateSceneNode( String const & p_name, SceneNodeSPtr p_parent)
{
	return CreateSceneNode( p_name, p_parent.get() );
}

GeometrySPtr Scene :: CreateGeometry( String const & p_name, eMESH_TYPE p_type, String const & p_meshName, UIntArray p_faces, RealArray p_size)
{
	GeometrySPtr l_pReturn;

	if( DoCheckObject( p_name, m_addedPrimitives, cuT( "Geometry" ) ) )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		MeshSPtr l_pMesh = m_pEngine->CreateMesh( p_type, p_meshName, p_faces, p_size );

		if( l_pMesh )
		{
 			l_pReturn = std::make_shared< Geometry >( this, l_pMesh, nullptr, p_name );
			Logger::LogMessage( cuT( "Scene :: CreatePrimitive - Geometry [" ) + p_name + cuT( "] - Created" ) );
			m_addedPrimitives[p_name] = l_pReturn;
			m_newlyAddedPrimitives[p_name] = l_pReturn;
			m_changed = true;
		}
		else
		{
			Logger::LogError( cuT( "Scene :: CreatePrimitive - Can't create primitive [") + p_name + cuT( "] - Mesh creation failed" ) );
		}
	}
	else
	{
		l_pReturn = m_addedPrimitives.find( p_name )->second;
	}

	return l_pReturn;
}

GeometrySPtr Scene :: CreateGeometry( String const & p_strName )
{
	GeometrySPtr l_pReturn;

	if( DoCheckObject( p_strName, m_addedPrimitives, cuT( "Geometry" ) ) )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
 		l_pReturn = std::make_shared< Geometry >( this, nullptr, m_rootObjectNode, p_strName );
	}
	else
	{
		l_pReturn = m_addedPrimitives.find( p_strName )->second;
	}

	return l_pReturn;
}

CameraSPtr Scene :: CreateCamera( String const & p_name, int p_ww, int p_wh, SceneNodeSPtr p_pNode, eVIEWPORT_TYPE p_type )
{
	CameraSPtr l_pReturn;

	if( DoCheckObject( p_name, m_addedCameras, cuT( "Camera" ) ) )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		l_pReturn = std::make_shared< Camera >( this, p_name, Size( p_ww, p_wh ), p_pNode, p_type );
		m_addedCameras[p_name] = l_pReturn;
		Logger::LogMessage( cuT( "Scene :: CreateCamera - Camera [") + p_name + cuT( "] created"));
	}

	return l_pReturn;
}

CameraSPtr Scene :: CreateCamera( String const & p_name, SceneNodeSPtr p_pNode, ViewportSPtr p_pViewport )
{
	CameraSPtr l_pReturn;

	if( DoCheckObject( p_name, m_addedCameras, cuT( "Camera" ) ) )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		l_pReturn = std::make_shared< Camera >( this, p_name, p_pNode, p_pViewport );
		m_addedCameras[p_name] = l_pReturn;
		Logger::LogMessage( cuT( "Scene :: CreateCamera - Camera [") + p_name + cuT( "] created"));
	}

	return l_pReturn;
}

LightSPtr Scene :: CreateLight( String const & p_name, SceneNodeSPtr p_pNode, eLIGHT_TYPE p_eLightType)
{
	LightSPtr l_pReturn;

	if( DoCheckObject( p_name, m_addedLights, cuT( "Light" ) ) )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		l_pReturn = std::make_shared< Light >( m_lightFactory, this, p_pNode, p_name, p_eLightType );
		AddLight( l_pReturn );
		Logger::LogMessage( cuT( "Scene :: CreateLight - Light [") + p_name + cuT( "] created" ) );
	}
	else
	{
		l_pReturn = m_addedLights.find( p_name )->second;
	}

	return l_pReturn;
}

AnimatedObjectGroupSPtr Scene :: CreateAnimatedObjectGroup( String const & p_name )
{
	AnimatedObjectGroupSPtr l_pReturn;

	if( DoCheckObject( p_name, m_addedGroups, cuT( "AnimatedObjectGroup" ) ) )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		l_pReturn = std::make_shared< AnimatedObjectGroup >( this, p_name );
		AddAnimatedObjectGroup( l_pReturn );
		Logger::LogMessage( cuT( "Scene :: CreateAnimatedObjectGroup - AnimatedObjectGroup [") + p_name + cuT( "] created" ) );
	}
	else
	{
		l_pReturn = m_addedGroups.find( p_name )->second;
	}

	return l_pReturn;
}

void Scene :: AddNode( SceneNodeSPtr p_node )
{
	DoAddObject( p_node, m_addedNodes, cuT( "SceneNode" ) );
}

void Scene :: AddLight( LightSPtr p_light )
{
	if( DoAddObject( p_light, m_addedLights, cuT( "Light" ) ) )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		int l_iIndex = 0;
		bool l_bFound = false;
		LightPtrIntMapIt l_itMap = m_mapLights.begin();

		while( l_itMap != m_mapLights.end() && !l_bFound )
		{
			if( l_itMap->first != l_iIndex )
			{
				l_bFound = true;
			}
			else
			{
				l_iIndex++;
				l_itMap++;
			}
		}

		p_light->SetIndex( l_iIndex );
		m_mapLights.insert( std::make_pair( l_iIndex, p_light ) );
	}
}

void Scene :: AddGeometry( GeometrySPtr p_geometry )
{
	if( DoAddObject( p_geometry, m_addedPrimitives, cuT( "Geometry" ) ) )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		m_newlyAddedPrimitives[p_geometry->GetName()] = p_geometry;
		m_changed = true;
	}
}

void Scene :: AddBillboards( BillboardListSPtr p_pList )
{
	if( DoAddObject( p_pList, m_mapBillboardsLists, cuT( "Billboard" ) ) )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		m_changed = true;
	}
}

void Scene :: AddAnimatedObjectGroup( AnimatedObjectGroupSPtr p_pGroup )
{
	DoAddObject( p_pGroup, m_addedGroups, cuT( "AnimatedObjectGroup" ) );
}

SceneNodeSPtr Scene :: GetNode( String const & p_name)const
{
	SceneNodeSPtr l_pReturn;

	if( p_name == cuT( "RootNode" ) )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		l_pReturn = m_rootNode;
	}
	else
	{
		l_pReturn = DoGetObject( m_addedNodes, p_name );
	}

	return l_pReturn;
}

GeometrySPtr Scene :: GetGeometry( String const & p_name )const
{
	return DoGetObject( m_addedPrimitives, p_name );
}

LightSPtr Scene :: GetLight( String const & p_name )const
{
	return DoGetObject( m_addedLights, p_name );
}

BillboardListSPtr Scene :: GetBillboards( String const & p_name )const
{
	return DoGetObject( m_mapBillboardsLists, p_name );
}

CameraSPtr Scene :: GetCamera( String const & p_name )const
{
	return DoGetObject( m_addedCameras, p_name );
}

AnimatedObjectGroupSPtr Scene :: GetAnimatedObjectGroup( String const & p_name )const
{
	return DoGetObject( m_addedGroups, p_name );
}

void Scene :: RemoveNode( SceneNodeSPtr p_pNode )
{
	if( p_pNode )
	{
		DoRemoveObject( p_pNode, m_addedNodes, m_arrayNodesToDelete );
	}
}

void Scene :: RemoveGeometry( GeometrySPtr p_geometry)
{
	if( p_geometry )
	{
		DoRemoveObject( p_geometry, m_addedPrimitives, m_arrayPrimitivesToDelete );
	}
}

void Scene :: RemoveBillboards( BillboardListSPtr p_pList )
{
	if( p_pList )
	{
		DoRemoveObject( p_pList, m_mapBillboardsLists, m_arrayBillboardsToDelete );
	}
}

void Scene :: RemoveLight( LightSPtr p_pLight)
{
	if( p_pLight )
	{
		DoRemoveObject( p_pLight, m_addedLights, m_arrayLightsToDelete );

		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		LightPtrIntMapIt l_itMap = m_mapLights.find( p_pLight->GetIndex() );

		if( l_itMap != m_mapLights.end() )
		{
			m_mapLights.erase( l_itMap );
		}
	}
}

void Scene :: RemoveCamera( CameraSPtr p_pCamera )
{
	if( p_pCamera )
	{
		DoRemoveObject( p_pCamera, m_addedCameras, m_arrayCamerasToDelete );
	}
}

void Scene :: RemoveAnimatedObjectGroup( AnimatedObjectGroupSPtr p_pGroup )
{
	if( p_pGroup )
	{
		std::vector< AnimatedObjectGroupSPtr > l_arrayDelete;
		DoRemoveObject( p_pGroup, m_addedGroups, l_arrayDelete );
	}
}

void Scene :: RemoveAllNodes()
{
	DoRemoveAll( m_addedNodes, m_arrayNodesToDelete );
}

void Scene :: RemoveAllLights()
{
	m_mapLights.clear();
	DoRemoveAll( m_addedLights, m_arrayLightsToDelete );
}

void Scene :: RemoveAllGeometries()
{
	DoRemoveAll( m_addedPrimitives, m_arrayPrimitivesToDelete );
}

void Scene :: RemoveAllBillboards()
{
	while( m_mapBillboardsLists.size() )
	{
		m_arrayBillboardsToDelete.push_back( m_mapBillboardsLists.begin()->second );
		m_mapBillboardsLists.erase( m_mapBillboardsLists.begin() );
	}
}

void Scene :: RemoveAllCameras()
{
	DoRemoveAll( m_addedCameras, m_arrayCamerasToDelete );
}

void Scene :: RemoveAllAnimatedObjectGroups()
{
	m_addedGroups.clear();
}

void Scene :: Merge( SceneSPtr p_pScene )
{
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_pScene->m_mutex );
		DoMerge( p_pScene, p_pScene->m_addedNodes,				m_addedNodes			);
		DoMerge( p_pScene, p_pScene->m_addedLights,				m_addedLights			);
		DoMerge( p_pScene, p_pScene->m_addedCameras,			m_addedCameras			);
		DoMerge( p_pScene, p_pScene->m_addedPrimitives,			m_addedPrimitives		);
		DoMerge( p_pScene, p_pScene->m_newlyAddedPrimitives,	m_newlyAddedPrimitives	);
		DoMerge( p_pScene, p_pScene->m_mapBillboardsLists,		m_mapBillboardsLists	);
//		DoMerge( p_pScene, p_pScene->m_addedGroups,				m_addedGroups			);
/*
		Castor::String l_strName;

		for( BillboardListStrMapIt l_it = p_pScene->m_mapBillboardsLists.begin(); l_it != p_pScene->m_mapBillboardsLists.end(); ++l_it )
		{
			l_strName = l_it->first;

			while( m_mapBillboardsLists.find( l_strName ) != m_mapBillboardsLists.end() )
			{
				l_strName = p_pScene->GetName() + cuT( "_" ) + l_strName;
			}

//			l_it->second->SetName( l_strName );
			m_mapBillboardsLists.insert( std::make_pair( l_strName, l_it->second ) );
		}

		p_pScene->m_mapBillboardsLists.clear();
*/
		m_changed = true;
		m_clAmbientLight = p_pScene->GetAmbientLight();
	}

	p_pScene->ClearScene();
}

bool Scene :: ImportExternal( String const & p_fileName, Importer & p_importer )
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = true;
	SceneSPtr l_pScene = p_importer.ImportScene( p_fileName, Parameters() );

	if( l_pScene )
	{
		Merge( l_pScene );
		m_changed = true;
		l_bReturn = true;
	}

	return l_bReturn;
}

void Scene :: Select( Ray * p_ray, GeometrySPtr & p_geo, SubmeshSPtr & p_submesh, FaceSPtr * p_face, Vertex * p_vertex)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	Point3r l_min;
	Point3r l_max;
	GeometrySPtr l_geo, l_selectedGeo;
	MeshSPtr l_mesh;
	SubmeshSPtr l_submesh, l_selectedSubmesh;
	FaceSPtr l_face, l_selectedFace;
	Vertex * l_pSelectedVertex = NULL;
	real l_geoDist = 10e6, l_faceDist = 10e6, l_vertexDist = 10e6;
	real l_curgeoDist, l_curfaceDist, l_curvertexDist;
	CubeBox l_box;
	SphereBox l_sphere;
	Point3r l_ptCoords;

	for (GeometryPtrStrMap::iterator l_it = m_addedPrimitives.begin(); l_it != m_addedPrimitives.end(); ++l_it)
	{
		l_geo = l_it->second;

		if (l_geo->IsVisible())
		{
			l_mesh = l_geo->GetMesh();
			l_sphere = l_mesh->GetCollisionSphere();

			if ((l_curgeoDist = p_ray->Intersects( l_sphere)) > 0.0f)
			{
				l_box = l_mesh->GetCollisionBox();

				if (p_ray->Intersects( l_box) > 0.0f)
				{
					l_geoDist = l_curgeoDist;

					std::for_each( l_mesh->Begin(), l_mesh->End(), [&]( SubmeshSPtr p_pSubmesh )
					{
						l_sphere = p_pSubmesh->GetSphere();

						if (p_ray->Intersects( l_sphere) >= 0.0f)
						{
							l_box = p_pSubmesh->GetCubeBox();

							if (p_ray->Intersects( l_box) >= 0.0f)
							{
								for (uint32_t k = 0; k < p_pSubmesh->GetFacesCount(); k++)
								{
									l_face = p_pSubmesh->GetFace( k );

									if( (l_curfaceDist = p_ray->Intersects( *l_face, *l_submesh )) < l_faceDist )
									{
										if( (l_curvertexDist = p_ray->Intersects( Vertex::GetPosition( p_pSubmesh->GetPoint( l_face->GetVertexIndex( 0 ) ), l_ptCoords ) )) < l_vertexDist )
										{
											l_vertexDist = l_curvertexDist;
											l_geoDist = l_curgeoDist;
											l_faceDist = l_curfaceDist;
											l_selectedGeo = l_geo;
											l_selectedSubmesh = l_submesh;
											l_selectedFace = l_face;
//											l_pSelectedVertex = p_pSubmesh->GetPoint( l_face->GetVertexIndex( 0 ) );
										}

										if( (l_curvertexDist = p_ray->Intersects( Vertex::GetPosition( p_pSubmesh->GetPoint( l_face->GetVertexIndex( 1 ) ), l_ptCoords ) )) < l_vertexDist )
										{
											l_vertexDist = l_curvertexDist;
											l_geoDist = l_curgeoDist;
											l_faceDist = l_curfaceDist;
											l_selectedGeo = l_geo;
											l_selectedSubmesh = l_submesh;
											l_selectedFace = l_face;
//											l_pSelectedVertex = p_pSubmesh->GetPoint( l_face->GetVertexIndex( 1 ) );
										}

										if( (l_curvertexDist = p_ray->Intersects( Vertex::GetPosition( p_pSubmesh->GetPoint( l_face->GetVertexIndex( 2 ) ), l_ptCoords ) )) < l_vertexDist )
										{
											l_vertexDist = l_curvertexDist;
											l_geoDist = l_curgeoDist;
											l_faceDist = l_curfaceDist;
											l_selectedGeo = l_geo;
											l_selectedSubmesh = l_submesh;
											l_selectedFace = l_face;
//											l_pSelectedVertex = p_pSubmesh->GetPoint( l_face->GetVertexIndex( 2 ) );
										}
									}
								}
							}
						}
					} );
				}
			}
		}
	}

	if (p_geo)
	{
		p_geo = l_selectedGeo;
	}

	if (p_submesh)
	{
		p_submesh = l_selectedSubmesh;
	}

	if (p_face)
	{
		p_face = & l_selectedFace;
	}

	if (p_vertex)
	{
		p_vertex = l_pSelectedVertex;
	}
}

void Scene :: AddOverlay( OverlaySPtr p_pOverlay)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	m_arrayOverlays.push_back( p_pOverlay );
}

void Scene :: DoDeleteToDelete()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();

	std::for_each(m_arrayPrimitivesToDelete.begin(), m_arrayPrimitivesToDelete.end(), [&]( GeometrySPtr p_pGeometry )
	{
		p_pGeometry->Detach();
	} );

	m_arrayBillboardsToDelete.clear();
	m_arrayLightsToDelete.clear();
	m_arrayPrimitivesToDelete.clear();
	m_arrayNodesToDelete.clear();
	m_arrayCamerasToDelete.clear();
}

void Scene :: DoUpdateAnimations()
{
	std::for_each( m_addedGroups.begin(), m_addedGroups.end(), AnmObjGrpUpdater() );
}

void Scene :: DoSortByAlpha()
{
	m_mapSubmeshesNoAlpha.clear();
	m_arraySubmeshesAlpha.clear();

	std::for_each( m_addedPrimitives.begin(), m_addedPrimitives.end(), [&]( std::pair< String, GeometrySPtr > p_pair )
	{
		MeshSPtr l_pMesh = p_pair.second->GetMesh();
		uint32_t l_uiId = p_pair.second->GetMeshID();
		SceneNode *	l_pNode = p_pair.second->GetParent();

		if( l_pMesh )
		{
			std::for_each( l_pMesh->Begin(), l_pMesh->End(), [&]( SubmeshSPtr p_pSubmesh )
			{
				MaterialSPtr l_pMaterial( p_pSubmesh->GetMaterial() );
				stRENDER_NODE l_renderNode = { l_pNode, p_pair.second, p_pSubmesh, l_uiId, l_pMaterial };

				if( l_pMaterial->HasAlphaBlending() )
				{
					m_arraySubmeshesAlpha.push_back( l_renderNode );
				}
				else
				{
					m_arraySubmeshesNoAlpha.push_back( l_renderNode );

					SubmeshNodesByMaterialMapIt l_itMap = m_mapSubmeshesNoAlpha.find( l_pMaterial );

					if( l_itMap == m_mapSubmeshesNoAlpha.end() )
					{
						m_mapSubmeshesNoAlpha.insert( std::make_pair( l_pMaterial, SubmeshNodesMap() ) );
						l_itMap = m_mapSubmeshesNoAlpha.find( l_pMaterial );
					}

					SubmeshNodesMapIt l_itSubmesh = l_itMap->second.find( p_pSubmesh );

					if( l_itSubmesh == l_itMap->second.end() )
					{
						l_itMap->second.insert( std::make_pair( p_pSubmesh, RenderNodeArray() ) );
						l_itSubmesh = l_itMap->second.find( p_pSubmesh );
					}

					l_itSubmesh->second.push_back( l_renderNode );
				}
			} );
		}
	} );
}

void Scene :: DoRenderSubmeshes( Camera const & p_camera, Pipeline & p_pipeline, eTOPOLOGY p_eTopology, RenderNodeArrayConstIt p_begin, RenderNodeArrayConstIt p_end )
{
	RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();

	std::for_each( p_begin, p_end, [&]( stRENDER_NODE const & p_renderNode )
	{
		if( p_renderNode.m_pNode && p_renderNode.m_pNode->IsDisplayable() && p_renderNode.m_pNode->IsVisible() )
		{
			if( p_camera.IsVisible( p_renderNode.m_pSubmesh->GetParent()->GetCollisionBox(), p_renderNode.m_pNode->GetDerivedTransformationMatrix() ) )
			{

				if( l_pRenderSystem->HasInstancing() && p_renderNode.m_pSubmesh->GetRefCount() > 1 )
				{
					SubmeshRendererSPtr l_pRenderer = p_renderNode.m_pSubmesh->GetRenderer();
					MatrixBufferSPtr l_pMtxBuffer = l_pRenderer->GetMatrices();

					if( l_pMtxBuffer )
					{
						uint32_t l_uiSize = l_pMtxBuffer->GetSize();
						real * l_pBuffer = l_pMtxBuffer->data();

						if( (p_renderNode.m_pSubmesh->GetProgramFlags() & ePROGRAM_FLAG_SKINNING) == ePROGRAM_FLAG_SKINNING )
						{
							std::memcpy( l_pBuffer, p_renderNode.m_pNode->GetDerivedTransformationMatrix().get_inverse().const_ptr(), 16 * sizeof( real ) );
						}
						else
						{
							std::memcpy( l_pBuffer, p_renderNode.m_pNode->GetDerivedTransformationMatrix().const_ptr(), 16 * sizeof( real ) );
						}
					}

					DoRenderSubmesh( p_pipeline, p_renderNode, p_eTopology );
				}
				else
				{
					p_pipeline.PushMatrix();

					if( (p_renderNode.m_pSubmesh->GetProgramFlags() & ePROGRAM_FLAG_SKINNING) == ePROGRAM_FLAG_SKINNING )
					{
						p_pipeline.MultMatrix( p_renderNode.m_pNode->GetDerivedTransformationMatrix().get_inverse() );
					}
					else
					{
						p_pipeline.MultMatrix( p_renderNode.m_pNode->GetDerivedTransformationMatrix() );
					}

					DoRenderSubmesh( p_pipeline, p_renderNode, p_eTopology );
					p_pipeline.PopMatrix();
				}
			}
		}
	} );
}

void Scene :: DoRenderSubmeshes( Camera const & p_camera, Pipeline & p_pipeline, eTOPOLOGY p_eTopology, SubmeshNodesByMaterialMapConstIt p_begin, SubmeshNodesByMaterialMapConstIt p_end )
{
	std::for_each( p_begin, p_end, [&]( SubmeshNodesByMaterialMap::value_type p_pair )
	{
		MaterialSPtr l_pMaterial = p_pair.first;

		std::for_each( p_pair.second.begin(), p_pair.second.end(), [&]( SubmeshNodesMap::value_type p_arrayPair )
		{
			SubmeshSPtr l_pSubmesh = p_arrayPair.first;

			if( l_pSubmesh->GetRefCount() > 1 )
			{
				MatrixBufferSPtr l_pMtxBuffer = l_pSubmesh->GetRenderer()->GetMatrices();

				if( l_pMtxBuffer )
				{
					uint32_t l_uiSize = l_pMtxBuffer->GetSize();
					real * l_pBuffer = l_pMtxBuffer->data();

					for( uint32_t i = 0; i < l_pSubmesh->GetRefCount(); ++i )
					{
						if( (l_pSubmesh->GetProgramFlags() & ePROGRAM_FLAG_SKINNING) == ePROGRAM_FLAG_SKINNING )
						{
							std::memcpy( l_pBuffer, p_arrayPair.second[i].m_pNode->GetDerivedTransformationMatrix().get_inverse().const_ptr(), 16 * sizeof( real ) );
						}
						else
						{
							std::memcpy( l_pBuffer, p_arrayPair.second[i].m_pNode->GetDerivedTransformationMatrix().const_ptr(), 16 * sizeof( real ) );
						}

						l_pBuffer += 16;
					}

					DoRenderSubmesh( p_pipeline, p_arrayPair.second[0], p_eTopology );
				}
			}
			else
			{
				p_pipeline.PushMatrix();

				if( (l_pSubmesh->GetProgramFlags() & ePROGRAM_FLAG_SKINNING) == ePROGRAM_FLAG_SKINNING )
				{
					p_pipeline.MultMatrix( p_arrayPair.second[0].m_pNode->GetDerivedTransformationMatrix().get_inverse() );
				}
				else
				{
					p_pipeline.MultMatrix( p_arrayPair.second[0].m_pNode->GetDerivedTransformationMatrix() );
				}

				DoRenderSubmesh( p_pipeline, p_arrayPair.second[0], p_eTopology );
				p_pipeline.PopMatrix();
			}
		} );

	} );
}

void Scene :: DoRenderSubmeshes( Pipeline & p_pipeline, eTOPOLOGY p_eTopology, RenderNodeByDistanceMMapConstIt p_begin, RenderNodeByDistanceMMapConstIt p_end )
{
	RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();

	std::for_each( p_begin, p_end, [&]( RenderNodeByDistanceMMap::value_type const & p_pair )
	{
		stRENDER_NODE const & l_renderNode = p_pair.second;

		if( l_pRenderSystem->HasInstancing() && l_renderNode.m_pSubmesh->GetRefCount() > 1 )
		{
			MatrixBufferSPtr l_pMtxBuffer = l_renderNode.m_pSubmesh->GetRenderer()->GetMatrices();

			if( l_pMtxBuffer )
			{
				uint32_t l_uiSize = l_pMtxBuffer->GetSize();
				real * l_pBuffer = l_pMtxBuffer->data();

				if( (l_renderNode.m_pSubmesh->GetProgramFlags() & ePROGRAM_FLAG_SKINNING) == ePROGRAM_FLAG_SKINNING )
				{
					std::memcpy( l_pBuffer, l_renderNode.m_pNode->GetDerivedTransformationMatrix().get_inverse().const_ptr(), 16 * sizeof( real ) );
				}
				else
				{
					std::memcpy( l_pBuffer, l_renderNode.m_pNode->GetDerivedTransformationMatrix().const_ptr(), 16 * sizeof( real ) );
				}

				DoRenderSubmesh( p_pipeline, l_renderNode, p_eTopology );
			}
		}
		else
		{
			p_pipeline.PushMatrix();

			if( (l_renderNode.m_pSubmesh->GetProgramFlags() & ePROGRAM_FLAG_SKINNING) == ePROGRAM_FLAG_SKINNING )
			{
				p_pipeline.MultMatrix( l_renderNode.m_pNode->GetDerivedTransformationMatrix().get_inverse() );
			}
			else
			{
				p_pipeline.MultMatrix( l_renderNode.m_pNode->GetDerivedTransformationMatrix() );
			}

			DoRenderSubmesh( p_pipeline, l_renderNode, p_eTopology );
			p_pipeline.PopMatrix();
		}

	} );
}

void Scene :: DoResortAlpha( Camera const & p_camera )
{
	m_mapSubmeshesAlpha.clear();

	std::for_each( m_arraySubmeshesAlpha.begin(), m_arraySubmeshesAlpha.end(), [&]( stRENDER_NODE const & p_renderNode )
	{
		if( p_renderNode.m_pNode && p_renderNode.m_pNode->IsDisplayable() && p_renderNode.m_pNode->IsVisible() )
		{
			if( p_camera.IsVisible( p_renderNode.m_pSubmesh->GetParent()->GetCollisionBox(), p_renderNode.m_pNode->GetDerivedTransformationMatrix() ) )
			{
				Matrix4x4r l_mtxMeshGlobal = p_renderNode.m_pNode->GetDerivedTransformationMatrix().get_inverse().transpose();
				Point3r l_position = p_renderNode.m_pNode->GetDerivedPosition();
				Point3r l_ptCameraLocal = l_mtxMeshGlobal * l_position;
				p_renderNode.m_pSubmesh->SortFaces( l_ptCameraLocal );
				l_ptCameraLocal -= p_renderNode.m_pSubmesh->GetCubeBox().GetCenter();
				m_mapSubmeshesAlpha.insert( std::make_pair( point::distance_squared( l_ptCameraLocal ), p_renderNode ) );
			}
		}
	} );
}

void Scene :: DoRenderSubmesh( Pipeline & p_pipeline, stRENDER_NODE p_node, eTOPOLOGY p_eTopology )
{
	ShaderProgramBaseSPtr l_pProgram;
	uint32_t l_uiCount = 0;
	uint32_t l_uiSize = p_node.m_pMaterial->GetPassCount();
	PassPtrArrayConstIt l_itEnd = p_node.m_pMaterial->End();

	for( PassPtrArrayIt l_it = p_node.m_pMaterial->Begin(); l_it != l_itEnd; ++l_it )
	{
		PassSPtr l_pPass = *l_it;

		if( l_pPass->HasAutomaticShader() )
		{
			if( m_pEngine->GetRenderSystem()->GetCurrentContext()->IsDeferredShadingSet() )
			{
				l_pProgram = m_pEngine->GetShaderManager().GetAutomaticProgram( l_pPass->GetTextureFlags(), p_node.m_pSubmesh->GetProgramFlags() |  ePROGRAM_FLAG_DEFERRED );
			}
			else
			{
				l_pProgram = m_pEngine->GetShaderManager().GetAutomaticProgram( l_pPass->GetTextureFlags(), p_node.m_pSubmesh->GetProgramFlags() );
			}

			l_pPass->BindToProgram( l_pProgram );
		}
		else
		{
			l_pProgram = l_pPass->GetShader< ShaderProgramBase >();
		}

		p_pipeline.ApplyMatrices( *l_pProgram );
		AnimatedObjectSPtr l_pAnimObject = p_node.m_pGeometry->GetAnimatedObject();

		if( l_pAnimObject )
		{
			SkeletonSPtr l_pSkeleton = l_pAnimObject->GetSkeleton();

			if( l_pSkeleton )
			{
				int i = 0;
				Matrix4x4rFrameVariableSPtr l_pVariable;
				l_pProgram->GetMatrixBuffer()->GetVariable( Pipeline::MtxBones, l_pVariable );

				if( l_pVariable )
				{
					Matrix4x4r l_mtxFinal;

					std::for_each( l_pAnimObject->AnimationsBegin(), l_pAnimObject->AnimationsEnd(), [&]( std::pair< String, AnimationSPtr > p_pair )
					{
						l_mtxFinal.set_identity();

						std::for_each( l_pSkeleton->Begin(), l_pSkeleton->End(), [&]( BoneSPtr p_pBone )
						{
							MovingObjectBaseSPtr l_pMoving = p_pair.second->GetMovingObject( p_pBone );

							if( l_pMoving )
							{
								l_mtxFinal *= l_pMoving->GetFinalTransformation();
							}
						} );

						l_pVariable->SetValue( l_mtxFinal.const_ptr(), i++ );
					} );
				}
			}
		}

		l_pPass->Render( l_uiCount++, l_uiSize );
		p_node.m_pSubmesh->GetRenderer()->Draw( p_eTopology, *l_pPass );
		l_pPass->EndRender();
	}
}

void Scene :: DoRenderBillboards( Pipeline & p_pipeline, BillboardListStrMapIt p_itBegin, BillboardListStrMapIt p_itEnd )
{
	RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();

	std::for_each( p_itBegin, p_itEnd, [&]( BillboardListStrMap::value_type const & p_pair )
	{
		p_pipeline.PushMatrix();
		p_pipeline.MultMatrix( p_pair.second->GetParent()->GetDerivedTransformationMatrix() );
		p_pair.second->Render();
		p_pipeline.PopMatrix();

	} );
}

//*************************************************************************************************
