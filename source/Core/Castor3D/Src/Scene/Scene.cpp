#include "SceneManager.hpp"

#include "AnimatedObjectGroupManager.hpp"
#include "BlendStateManager.hpp"
#include "CameraManager.hpp"
#include "BillboardManager.hpp"
#include "DepthStencilStateManager.hpp"
#include "Engine.hpp"
#include "GeometryManager.hpp"
#include "LightManager.hpp"
#include "MaterialManager.hpp"
#include "MeshManager.hpp"
#include "OverlayManager.hpp"
#include "SamplerManager.hpp"
#include "SceneNodeManager.hpp"
#include "ShaderManager.hpp"
#include "WindowManager.hpp"

#include "Animation/AnimatedObject.hpp"
#include "Animation/Animation.hpp"
#include "Animation/Bone.hpp"
#include "Animation/Skeleton.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Face.hpp"
#include "Mesh/Importer.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Miscellaneous/Ray.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Shader/FrameVariable.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/MatrixFrameVariable.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Texture/DynamicTexture.hpp"
#include "Texture/StaticTexture.hpp"
#include "Texture/TextureUnit.hpp"
#include "Manager/ManagerView.hpp"

#include <Image.hpp>
#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	Scene::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		: Loader< Scene, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool Scene::TextLoader::operator()( Scene const & p_scene, TextFile & p_file )
	{
		Logger::LogInfo( cuT( "Scene::Write - Scene Name" ) );
		bool l_return = p_file.WriteText( cuT( "scene \"" ) + p_scene.GetName() + cuT( "\"\n{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "\tbackground_colour " ) ) > 0 && Colour::TextLoader()( p_scene.GetBackgroundColour(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "\tambient_light " ) ) > 0 && Colour::TextLoader()( p_scene.GetAmbientLight(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Camera Nodes" ) );
			l_return = SceneNode::TextLoader()( *p_scene.GetCameraRootNode(), p_file );
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Object Nodes" ) );
			l_return = SceneNode::TextLoader()( *p_scene.GetObjectRootNode(), p_file );
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Cameras" ) );
			auto l_lock = make_unique_lock( p_scene.GetCameraManager() );

			for ( auto const & l_it : p_scene.GetCameraManager() )
			{
				l_return &= Camera::TextLoader()( *l_it.second, p_file );
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Lights" ) );
			auto l_lock = make_unique_lock( p_scene.GetLightManager() );

			for ( auto const & l_it : p_scene.GetLightManager() )
			{
				auto l_light = l_it.second;

				switch ( l_light->GetType() )
				{
				case eLIGHT_TYPE_DIRECTIONAL:
					l_return &= DirectionalLight::TextLoader()( *l_light->GetDirectionalLight(), p_file );
					break;

				case eLIGHT_TYPE_POINT:
					l_return &= PointLight::TextLoader()( *l_light->GetPointLight(), p_file );
					break;

				case eLIGHT_TYPE_SPOT:
					l_return &= SpotLight::TextLoader()( *l_light->GetSpotLight(), p_file );
					break;

				default:
					l_return = false;
					break;
				}
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Scene::Write - Geometries" ) );
			auto l_lock = make_unique_lock( p_scene.GetGeometryManager() );

			for ( auto const & l_it : p_scene.GetGeometryManager() )
			{
				l_return &= Geometry::TextLoader()( *l_it.second, p_file );
			}
		}

		p_file.WriteText( cuT( "}\n\n" ) );
		return l_return;
	}

	//*************************************************************************************************

	Scene::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< Scene >( p_path )
	{
	}

	bool Scene::BinaryParser::Fill( Scene const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SCENE );
		l_return = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetBackgroundColour(), eCHUNK_TYPE_SCENE_BACKGROUND, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetAmbientLight(), eCHUNK_TYPE_SCENE_AMBIENT, l_chunk );
		}

		if ( l_return )
		{
			l_return = SceneNode::BinaryParser( m_path ).Fill( *p_obj.GetCameraRootNode(), l_chunk );
		}

		if ( l_return )
		{
			l_return = SceneNode::BinaryParser( m_path ).Fill( *p_obj.GetObjectRootNode(), l_chunk );
		}

		if ( l_return )
		{
			auto l_lock = make_unique_lock( p_obj.GetCameraManager() );

			for ( auto const & l_it : p_obj.GetCameraManager() )
			{
				l_return &= Camera::BinaryParser( m_path ).Fill( *l_it.second, l_chunk );
			}
		}

		if ( l_return )
		{
			auto l_lock = make_unique_lock( p_obj.GetLightManager() );

			for ( auto const & l_it : p_obj.GetLightManager() )
			{
				auto l_light = l_it.second;

				switch ( l_light->GetType() )
				{
				case eLIGHT_TYPE_DIRECTIONAL:
					l_return &= DirectionalLight::BinaryParser( m_path ).Fill( *l_light->GetDirectionalLight(), l_chunk );
					break;

				case eLIGHT_TYPE_POINT:
					l_return &= PointLight::BinaryParser( m_path ).Fill( *l_light->GetPointLight(), l_chunk );
					break;

				case eLIGHT_TYPE_SPOT:
					l_return &= SpotLight::BinaryParser( m_path ).Fill( *l_light->GetSpotLight(), l_chunk );
					break;

				default:
					l_return = false;
					break;
				}
			}
		}

		if ( l_return )
		{
			auto l_lock = make_unique_lock( p_obj.GetGeometryManager() );

			for ( auto const & l_it : p_obj.GetGeometryManager() )
			{
				l_return &= Geometry::BinaryParser( m_path ).Fill( *l_it.second, l_chunk );
			}
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool Scene::BinaryParser::Parse( Scene & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		Colour l_colour;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_NAME:
					l_return = DoParseChunk( l_name, l_chunk );

					if ( l_return )
					{
						p_obj.SetName( l_name );
					}

					break;

				case eCHUNK_TYPE_SCENE_BACKGROUND:
					l_return = DoParseChunk( l_colour, l_chunk );

					if ( l_return )
					{
						p_obj.SetBackgroundColour( l_colour );
					}

					break;

				case eCHUNK_TYPE_SCENE_AMBIENT:
					l_return = DoParseChunk( l_colour, l_chunk );

					if ( l_return )
					{
						p_obj.SetAmbientLight( l_colour );
					}

					break;

				case eCHUNK_TYPE_SCENE_NODE:
				{
					BinaryChunk l_chunkNode;
					l_return = l_chunk.GetSubChunk( l_chunkNode );

					if ( l_return )
					{
						switch ( l_chunkNode.GetChunkType() )
						{
						case eCHUNK_TYPE_NAME:
							l_return = DoParseChunk( l_name, l_chunkNode );
							break;

						default:
							l_return = false;
							break;
						}

						if ( l_return )
						{
							SceneNodeSPtr l_node;

							if ( l_name == cuT( "CameraRootNode" ) )
							{
								l_node = p_obj.GetCameraRootNode();
							}
							else if ( l_name == cuT( "ObjectRootNode" ) )
							{
								l_node = p_obj.GetObjectRootNode();
							}

							if ( !l_node )
							{
								l_node = p_obj.GetSceneNodeManager().Find( l_name );
							}

							if ( !l_node )
							{
								l_node = p_obj.GetSceneNodeManager().Create( l_name );
							}

							l_return = SceneNode::BinaryParser( m_path ).Parse( *l_node, l_chunk );
						}
					}
				}
				break;

				case eCHUNK_TYPE_CAMERA:
				{
					BinaryChunk l_chunkCamera;
					l_return = l_chunk.GetSubChunk( l_chunkCamera );

					if ( l_return )
					{
						switch ( l_chunkCamera.GetChunkType() )
						{
						case eCHUNK_TYPE_NAME:
							l_return = DoParseChunk( l_name, l_chunkCamera );
							break;

						default:
							l_return = false;
							break;
						}

						if ( l_return )
						{
							CameraSPtr l_camera = p_obj.GetCameraManager().Create( l_name, nullptr );
							l_return = Camera::BinaryParser( m_path ).Parse( *l_camera, l_chunk );
						}
					}
				}
				break;

				case eCHUNK_TYPE_LIGHT:
				{
					SceneNodeSPtr l_node;
					l_name.clear();
					String l_nodeName;
					eLIGHT_TYPE l_type = eLIGHT_TYPE_COUNT;

					while ( l_return && ( l_name.empty() || l_type == eLIGHT_TYPE_COUNT ) )
					{
						BinaryChunk l_chunkLight;
						l_return = l_chunk.GetSubChunk( l_chunkLight );

						if ( l_return )
						{
							switch ( l_chunkLight.GetChunkType() )
							{
							case eCHUNK_TYPE_NAME:
								l_return = DoParseChunk( l_name, l_chunkLight );
								break;

							case eCHUNK_TYPE_MOVABLE_NODE:
								l_return = DoParseChunk( l_nodeName, l_chunkLight );

								if ( l_return )
								{
									l_node = p_obj.GetSceneNodeManager().Find( l_nodeName );
								}

								break;

							case eCHUNK_TYPE_LIGHT_TYPE:
								l_return = DoParseChunk( l_type, l_chunkLight );
								break;

							default:
								l_return = false;
								break;
							}
						}
					}

					if ( l_return && !l_name.empty() && l_type != eLIGHT_TYPE_COUNT )
					{
						LightSPtr l_light = p_obj.GetLightManager().Create( l_name, l_node, l_type );
						l_return = Light::BinaryParser( m_path ).Parse( *l_light, l_chunk );
					}
				}
				break;

				case eCHUNK_TYPE_GEOMETRY:
				{
					BinaryChunk l_chunkGeometry;
					l_return = l_chunk.GetSubChunk( l_chunkGeometry );

					if ( l_return )
					{
						switch ( l_chunkGeometry.GetChunkType() )
						{
						case eCHUNK_TYPE_NAME:
							l_return = DoParseChunk( l_name, l_chunkGeometry );
							break;

						default:
							l_return = false;
							break;
						}

						if ( l_return )
						{
							GeometrySPtr l_geometry = p_obj.GetGeometryManager().Create( l_name, nullptr );
							l_return = Geometry::BinaryParser( m_path ).Parse( *l_geometry, l_chunk );
						}
					}
				}
				break;

				default:
					l_return = false;
					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	Scene::Scene( String const & p_name, Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, Named( p_name )
		, m_rootCameraNode()
		, m_rootObjectNode()
		, m_changed( false )
	{
		m_rootNode = std::make_shared< SceneNode >( cuT( "RootNode" ), *this );
		m_rootCameraNode = std::make_shared< SceneNode >( cuT( "CameraRootNode" ), *this );
		m_rootObjectNode = std::make_shared< SceneNode >( cuT( "ObjectRootNode" ), *this );
		m_rootCameraNode->AttachTo( m_rootNode );
		m_rootObjectNode->AttachTo( m_rootNode );

		m_animatedObjectGroupManager = std::make_unique< AnimatedObjectGroupManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_billboardManager = std::make_unique< BillboardManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_cameraManager = std::make_unique< CameraManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_geometryManager = std::make_unique< GeometryManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_lightManager = std::make_unique< LightManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );
		m_sceneNodeManager = std::make_unique< SceneNodeManager >( *this, m_rootNode, m_rootCameraNode, m_rootObjectNode );

		m_meshManagerView = std::make_unique< ManagerView< Mesh, MeshManager, eEVENT_TYPE_PRE_RENDER > >( GetName(), GetEngine()->GetMeshManager() );
		m_materialManagerView = std::make_unique< ManagerView< Material, MaterialManager, eEVENT_TYPE_PRE_RENDER > >( GetName(), GetEngine()->GetMaterialManager() );
		m_samplerManagerView = std::make_unique< ManagerView< Sampler, SamplerManager, eEVENT_TYPE_PRE_RENDER > >( GetName(), GetEngine()->GetSamplerManager() );
		m_windowManagerView = std::make_unique< ManagerView< RenderWindow, WindowManager, eEVENT_TYPE_POST_RENDER > >( GetName(), GetEngine()->GetWindowManager() );

		auto l_notify = [this]()
		{
			m_changed = true;
		};

		m_sceneNodeManager->Insert( cuT( "ObjectRootNode" ), m_rootObjectNode );
	}

	Scene::~Scene()
	{
		m_animatedObjectGroupManager.reset();
		m_billboardManager.reset();
		m_cameraManager.reset();
		m_geometryManager.reset();
		m_lightManager.reset();
		m_sceneNodeManager.reset();

		m_meshManagerView.reset();
		m_materialManagerView.reset();
		m_samplerManagerView.reset();
		m_windowManagerView.reset();

		if ( m_rootCameraNode )
		{
			m_rootCameraNode->Detach();
			m_rootCameraNode.reset();
		}

		if ( m_rootObjectNode )
		{
			m_rootObjectNode->Detach();
			m_rootObjectNode.reset();
		}

		if ( m_rootNode )
		{
			m_rootNode->Detach();
		}

		m_rootNode.reset();
	}

	void Scene::Initialise()
	{
	}

	void Scene::Cleanup()
	{
		auto l_lock = Castor::make_unique_lock( m_mutex );
		m_overlays.clear();
		m_animatedObjectGroupManager->Cleanup();
		m_cameraManager->Cleanup();
		m_billboardManager->Cleanup();
		m_geometryManager->Cleanup();
		m_lightManager->Cleanup();
		m_sceneNodeManager->Cleanup();
		m_meshManagerView->Clear();
		m_materialManagerView->Clear();
		m_samplerManagerView->Clear();
		m_windowManagerView->Clear();

		if ( m_backgroundImage )
		{
			GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [this]()
			{
				m_backgroundImage->Cleanup();
				m_backgroundImage->Destroy();
			} ) );
		}
	}

	void Scene::RenderBackground( Size const & p_size )
	{
		if ( m_backgroundImage )
		{
			if ( m_backgroundImage->IsInitialised() )
			{
				ContextRPtr l_context = GetEngine()->GetRenderSystem()->GetCurrentContext();
				l_context->GetNoDepthState()->Apply();
				l_context->RenderTexture( p_size, *m_backgroundImage );
			}
		}
	}

	void Scene::Update()
	{
		m_animatedObjectGroupManager->Update();
		m_changed = false;
	}

	bool Scene::SetBackgroundImage( Path const & p_pathFile )
	{
		bool l_return = false;
		ImageSPtr l_pImage;

		if ( !p_pathFile.empty() )
		{
			l_pImage = GetEngine()->GetImageManager().create( p_pathFile.GetFileName(), p_pathFile );
		}

		if ( l_pImage )
		{
			StaticTextureSPtr l_pStaTexture = GetEngine()->GetRenderSystem()->CreateStaticTexture();
			l_pStaTexture->SetType( eTEXTURE_TYPE_2D );
			l_pStaTexture->SetImage( l_pImage->GetPixels() );
			m_backgroundImage = l_pStaTexture;
			GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [this]()
			{
				m_backgroundImage->Create();
				m_backgroundImage->Initialise();
				m_backgroundImage->Bind( 0 );
				m_backgroundImage->GenerateMipmaps();
				m_backgroundImage->Unbind( 0 );
			} ) );
			l_return = true;
		}

		return l_return;
	}

	void Scene::Merge( SceneSPtr p_scene )
	{
		{
			auto l_lock = Castor::make_unique_lock( m_mutex );
			auto l_lockOther = Castor::make_unique_lock( p_scene->m_mutex );
			p_scene->GetAnimatedObjectGroupManager().MergeInto( *m_animatedObjectGroupManager );
			p_scene->GetCameraManager().MergeInto( *m_cameraManager );
			p_scene->GetBillboardManager().MergeInto( *m_billboardManager );
			p_scene->GetGeometryManager().MergeInto( *m_geometryManager );
			p_scene->GetLightManager().MergeInto( *m_lightManager );
			p_scene->GetSceneNodeManager().MergeInto( *m_sceneNodeManager );
			m_ambientLight = p_scene->GetAmbientLight();
			m_changed = true;
		}

		p_scene->Cleanup();
	}

	bool Scene::ImportExternal( String const & p_fileName, Importer & p_importer )
	{
		auto l_lock = Castor::make_unique_lock( m_mutex );
		bool l_return = true;
		SceneSPtr l_scene = p_importer.ImportScene( p_fileName, Parameters() );

		if ( l_scene )
		{
			Merge( l_scene );
			m_changed = true;
			l_return = true;
		}

		return l_return;
	}

	MeshSPtr Scene::ImportMesh( Castor::Path const & p_fileName, Importer & p_importer, Parameters const & p_parameters )
	{
		auto l_return = p_importer.ImportMesh( *this, p_fileName, p_parameters );

		if ( l_return )
		{
			m_meshManagerView->Insert( l_return->GetName(), l_return );
		}

		return l_return;
	}

	void Scene::AddOverlay( OverlaySPtr p_overlay )
	{
		auto l_lock = Castor::make_unique_lock( m_mutex );
		m_overlays.push_back( p_overlay );
	}

	uint32_t Scene::GetVertexCount()const
	{
		uint32_t l_return = 0;
		auto l_lock = make_unique_lock( *m_geometryManager );

		for ( auto && l_pair : *m_geometryManager )
		{
			auto l_mesh = l_pair.second->GetMesh();

			if ( l_mesh )
			{
				l_return += l_pair.second->GetMesh()->GetVertexCount();
			}
		}

		return l_return;
	}

	uint32_t Scene::GetFaceCount()const
	{
		uint32_t l_return = 0;
		auto l_lock = make_unique_lock( *m_geometryManager );

		for ( auto && l_pair : *m_geometryManager )
		{
			auto l_mesh = l_pair.second->GetMesh();

			if ( l_mesh )
			{
				l_return += l_mesh->GetFaceCount();
			}
		}

		return l_return;
	}
}
