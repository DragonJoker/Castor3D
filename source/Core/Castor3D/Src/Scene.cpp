#include "SceneManager.hpp"

#include "AnimatedObject.hpp"
#include "AnimatedObjectGroupManager.hpp"
#include "Animation.hpp"
#include "BlendStateManager.hpp"
#include "Bone.hpp"
#include "Buffer.hpp"
#include "CameraManager.hpp"
#include "BillboardManager.hpp"
#include "BlendState.hpp"
#include "CleanupEvent.hpp"
#include "DepthStencilStateManager.hpp"
#include "DirectionalLight.hpp"
#include "DynamicTexture.hpp"
#include "Engine.hpp"
#include "Face.hpp"
#include "FrameVariable.hpp"
#include "FrameVariableBuffer.hpp"
#include "FunctorEvent.hpp"
#include "GeometryManager.hpp"
#include "Importer.hpp"
#include "InitialiseEvent.hpp"
#include "LightManager.hpp"
#include "Material.hpp"
#include "MatrixFrameVariable.hpp"
#include "Mesh.hpp"
#include "MeshManager.hpp"
#include "OneFrameVariable.hpp"
#include "Overlay.hpp"
#include "Pass.hpp"
#include "Pipeline.hpp"
#include "PointFrameVariable.hpp"
#include "PointLight.hpp"
#include "Ray.hpp"
#include "RenderSystem.hpp"
#include "SceneNodeManager.hpp"
#include "ShaderManager.hpp"
#include "ShaderProgram.hpp"
#include "Skeleton.hpp"
#include "SpotLight.hpp"
#include "StaticTexture.hpp"
#include "Submesh.hpp"
#include "TextureUnit.hpp"
#include "Vertex.hpp"

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
							CameraSPtr l_camera = p_obj.GetCameraManager().Create( l_name, nullptr, Viewport::Ortho( *p_obj.GetEngine(), 0.0_r, 10.0_r, 0.0_r, 10.0_r, 0.0_r, 1.0_r ) );
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
		: OwnedBy< Engine >( p_engine )
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
		m_renderNodes.clear();
		m_opaqueRenderNodes.clear();
		m_transparentRenderNodes.clear();
		m_distanceSortedTransparentRenderNodes.clear();
		auto l_lock = Castor::make_unique_lock( m_mutex );

		for ( auto && l_overlay : m_overlays )
		{
			GetEngine()->PostEvent( MakeCleanupEvent( *l_overlay ) );
		}

		m_overlays.clear();

		m_animatedObjectGroupManager->Cleanup();
		m_cameraManager->Cleanup();
		m_billboardManager->Cleanup();
		m_geometryManager->Cleanup();
		m_lightManager->Cleanup();
		m_sceneNodeManager->Cleanup();

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
				l_context->RenderTexture( p_size, m_backgroundImage );
			}
		}
	}

	void Scene::Update()
	{
		if ( m_changed )
		{
			DoSortRenderNodes();
			m_changed = false;
		}

		m_animatedObjectGroupManager->Update();
	}

	void Scene::Render( RenderTechniqueBase & p_technique, Camera const & p_camera )
	{
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();
		Pipeline & l_pipeline = l_renderSystem->GetPipeline();
		ContextRPtr l_context = l_renderSystem->GetCurrentContext();
		auto l_lock = Castor::make_unique_lock( m_mutex );

		if ( !m_opaqueRenderNodes.empty() )
		{
			l_context->CullFace( eFACE_BACK );
			RenderSubmeshes( p_technique, p_camera, l_pipeline, m_opaqueRenderNodes );
		}

		if ( !m_transparentRenderNodes.empty() )
		{
			if ( l_context->IsMultiSampling() )
			{
				l_context->GetNoDepthWriteState()->Apply();
				l_context->CullFace( eFACE_FRONT );
				RenderSubmeshes( p_technique, p_camera, l_pipeline, m_transparentRenderNodes );
				l_context->CullFace( eFACE_BACK );
				RenderSubmeshes( p_technique, p_camera, l_pipeline, m_transparentRenderNodes );
			}
			else
			{
				l_context->GetNoDepthWriteState()->Apply();
				DoResortAlpha( m_transparentRenderNodes, p_camera, 1, m_distanceSortedTransparentRenderNodes );
				l_context->CullFace( eFACE_FRONT );
				DoRenderSubmeshesNonInstanced( p_technique, p_camera, l_pipeline, m_distanceSortedTransparentRenderNodes );
				l_context->CullFace( eFACE_BACK );
				DoRenderSubmeshesNonInstanced( p_technique, p_camera, l_pipeline, m_distanceSortedTransparentRenderNodes );
			}
		}

		if ( !m_billboardManager->IsEmpty() )
		{
			auto l_lock = make_unique_lock( *m_billboardManager );
			l_context->CullFace( eFACE_FRONT );
			DoRenderBillboards( p_technique, l_pipeline, m_billboardManager->begin(), m_billboardManager->end() );
			l_context->CullFace( eFACE_BACK );
			DoRenderBillboards( p_technique, l_pipeline, m_billboardManager->begin(), m_billboardManager->end() );
		}
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
				m_backgroundImage->Initialise( 0 );
				m_backgroundImage->Bind();
				m_backgroundImage->GenerateMipmaps();
				m_backgroundImage->Unbind();
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

	void Scene::AddOverlay( OverlaySPtr p_overlay )
	{
		auto l_lock = Castor::make_unique_lock( m_mutex );
		m_overlays.push_back( p_overlay );
	}

	uint32_t Scene::GetVertexCount()const
	{
		uint32_t l_return = 0;

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

	void Scene::RenderSubmeshes( RenderTechniqueBase & p_technique, Camera const & p_camera, Pipeline & p_pipeline, SubmeshRenderNodesByMaterialMap const & p_nodes )
	{
		if ( GetEngine()->GetRenderSystem()->HasInstancing() )
		{
			DoRenderSubmeshesInstanced( p_technique, p_camera, p_pipeline, p_nodes );
		}
		else
		{
			DoRenderSubmeshesNonInstanced( p_technique, p_camera, p_pipeline, p_nodes );
		}
	}

	void Scene::DoSortRenderNodes()
	{
		m_renderNodes.clear();
		m_opaqueRenderNodes.clear();
		m_transparentRenderNodes.clear();

		auto l_lock = make_unique_lock( GetGeometryManager() );

		for ( auto && l_primitive : GetGeometryManager() )
		{
			MeshSPtr l_mesh = l_primitive.second->GetMesh();
			SceneNodeSPtr l_sceneNode = l_primitive.second->GetParent();

			if ( l_mesh )
			{
				for ( auto && l_submesh : *l_mesh )
				{
					MaterialSPtr l_material( l_primitive.second->GetMaterial( l_submesh ) );

					if ( l_material )
					{
						stRENDER_NODE l_renderNode = { l_sceneNode, l_primitive.second, l_submesh, l_material };

						if ( l_material->HasAlphaBlending() )
						{
							auto l_itMap = m_transparentRenderNodes.insert( std::make_pair( l_material, SubmeshRenderNodesMap() ) ).first;
							auto l_itSubmesh = l_itMap->second.insert( std::make_pair( l_submesh, RenderNodeArray() ) ).first;
							l_itSubmesh->second.push_back( l_renderNode );
						}
						else
						{
							auto l_itMap = m_opaqueRenderNodes.insert( std::make_pair( l_material, SubmeshRenderNodesMap() ) ).first;
							auto l_itSubmesh = l_itMap->second.insert( std::make_pair( l_submesh, RenderNodeArray() ) ).first;
							l_itSubmesh->second.push_back( l_renderNode );
						}

						auto l_itMap = m_renderNodes.insert( std::make_pair( l_material, SubmeshRenderNodesMap() ) ).first;
						auto l_itSubmesh = l_itMap->second.insert( std::make_pair( l_submesh, RenderNodeArray() ) ).first;
						l_itSubmesh->second.push_back( l_renderNode );
					}
				}
			}
		}
	}

	void Scene::DoBindPass( RenderTechniqueBase & p_technique, Pipeline & p_pipeline, Pass & p_pass, Geometry const & p_geometry, uint32_t p_programFlags, uint64_t p_excludedMtxFlags )
	{
		ShaderProgramSPtr l_program;

		if ( p_pass.HasAutomaticShader() )
		{
			if ( !p_geometry.GetAnimatedObject() || !p_geometry.GetAnimatedObject()->IsPlayingAnimation() )
			{
				p_programFlags &= ~ePROGRAM_FLAG_SKINNING;
			}

			l_program = GetEngine()->GetShaderManager().GetAutomaticProgram( p_technique, p_pass.GetTextureFlags(), p_programFlags );
			l_program->Initialise();
			p_pass.BindToAutomaticProgram( l_program );
		}
		else
		{
			l_program = p_pass.GetShader();
		}

		//p_pass.Bind();

		FrameVariableBufferSPtr l_sceneBuffer = p_pass.GetSceneBuffer();

		if ( l_sceneBuffer && GetEngine()->GetPerObjectLighting() )
		{
			m_lightManager->BindLights( *l_program, *l_sceneBuffer );
			m_cameraManager->BindCamera( *l_sceneBuffer );
		}

		auto l_matrixBuffer = p_pass.GetMatrixBuffer();

		if ( l_matrixBuffer )
		{
			p_pipeline.ApplyMatrices( *l_matrixBuffer, ( 0xFFFFFFFFFFFFFFFF & ~p_excludedMtxFlags ) );
			auto l_animated = p_geometry.GetAnimatedObject();

			if ( l_animated && l_animated->IsPlayingAnimation() )
			{
				Matrix4x4rFrameVariableSPtr l_variable;
				l_matrixBuffer->GetVariable( Pipeline::MtxBones, l_variable );

				if ( l_variable )
				{
					l_animated->FillShader( *l_variable );
				}
			}
		}

		p_pass.Render();
	}

	void Scene::DoUnbindPass( Pass & p_pass )
	{
		p_pass.EndRender();
		FrameVariableBufferSPtr l_sceneBuffer = p_pass.GetSceneBuffer();

		if ( l_sceneBuffer && GetEngine()->GetPerObjectLighting() )
		{
			m_lightManager->UnbindLights( *p_pass.GetShader(), *l_sceneBuffer );
		}
	}

	void Scene::DoRenderSubmeshesNonInstanced( RenderTechniqueBase & p_technique, Camera const & p_camera, Pipeline & p_pipeline, SubmeshRenderNodesByMaterialMap const & p_nodes )
	{
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();

		for ( auto l_itNodes : p_nodes )
		{
			MaterialSPtr l_material = l_itNodes.first;

			for ( auto l_itSubmeshes : l_itNodes.second )
			{
				SubmeshSPtr l_submesh = l_itSubmeshes.first;

				for ( auto l_pass : *l_material )
				{
					for ( auto l_renderNode : l_itSubmeshes.second )
					{
						p_pipeline.SetModelMatrix( l_renderNode.m_node->GetDerivedTransformationMatrix() );
						DoBindPass( p_technique, p_pipeline, *l_pass, *l_renderNode.m_geometry, l_renderNode.m_submesh->GetProgramFlags(), 0 );
						l_renderNode.m_submesh->Draw( *l_pass );
						DoUnbindPass( *l_pass );
					}
				}
			}
		}
	}

	void Scene::DoRenderSubmeshesInstanced( RenderTechniqueBase & p_technique, Camera const & p_camera, Pipeline & p_pipeline, SubmeshRenderNodesByMaterialMap const & p_nodes )
	{
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();

		for ( auto l_itNodes : p_nodes )
		{
			MaterialSPtr l_material = l_itNodes.first;

			for ( auto l_itSubmeshes : l_itNodes.second )
			{
				SubmeshSPtr l_submesh = l_itSubmeshes.first;

				for ( auto l_pass : *l_material )
				{
					if ( l_submesh->GetRefCount( l_material ) > 1 && l_submesh->HasMatrixBuffer()
							&& ( l_submesh->GetProgramFlags() & ePROGRAM_FLAG_SKINNING ) != ePROGRAM_FLAG_SKINNING )
					{
						real * l_buffer = l_submesh->GetMatrixBuffer().data();

						for ( auto && l_renderNode : l_itSubmeshes.second )
						{
							std::memcpy( l_buffer, ( l_renderNode.m_node->GetDerivedTransformationMatrix().get_inverse() ).const_ptr(), 16 * sizeof( real ) );
							l_buffer += 16;
						}

						DoBindPass( p_technique, p_pipeline, *l_pass, *l_itSubmeshes.second[0].m_geometry, l_itSubmeshes.second[0].m_submesh->GetProgramFlags() | ePROGRAM_FLAG_INSTANCIATION, MASK_MTXMODE_MODEL );
						l_itSubmeshes.second[0].m_submesh->Draw( *l_pass );
						DoUnbindPass( *l_pass );
					}
					else
					{
						for ( auto l_renderNode : l_itSubmeshes.second )
						{
							p_pipeline.SetModelMatrix( l_renderNode.m_node->GetDerivedTransformationMatrix() );
							DoBindPass( p_technique, p_pipeline, *l_pass, *l_renderNode.m_geometry, l_renderNode.m_submesh->GetProgramFlags(), 0 );
							l_renderNode.m_submesh->Draw( *l_pass );
							DoUnbindPass( *l_pass );
						}
					}
				}
			}
		}
	}

	void Scene::DoRenderSubmeshesNonInstanced( RenderTechniqueBase & p_technique, Camera const & p_camera, Pipeline & p_pipeline, RenderNodeByDistanceMMap const & p_nodes )
	{
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();

		for ( auto l_it : p_nodes )
		{
			stRENDER_NODE const & l_renderNode = l_it.second;
			auto l_material = l_renderNode.m_material;
			auto l_submesh = l_renderNode.m_submesh;

			for ( auto l_pass : *l_material )
			{
				p_pipeline.SetModelMatrix( l_renderNode.m_node->GetDerivedTransformationMatrix() );
				DoBindPass( p_technique, p_pipeline, *l_pass, *l_renderNode.m_geometry, l_renderNode.m_submesh->GetProgramFlags(), 0 );
				l_renderNode.m_submesh->Draw( *l_pass );
				DoUnbindPass( *l_pass );
			}
		}
	}

	void Scene::DoResortAlpha( SubmeshRenderNodesByMaterialMap p_input, Camera const & p_camera, int p_sign, RenderNodeByDistanceMMap & p_output )
	{
		p_output.clear();

		for ( auto l_itMaterial : p_input )
		{
			for ( auto l_itSubmesh : l_itMaterial.second )
			{
				for ( auto l_renderNode : l_itSubmesh.second )
				{
					if ( l_renderNode.m_node && l_renderNode.m_node->IsDisplayable() && l_renderNode.m_node->IsVisible() )
					{
						if ( p_camera.IsVisible( l_renderNode.m_submesh->GetParent()->GetCollisionBox(), l_renderNode.m_node->GetDerivedTransformationMatrix() ) )
						{
							Matrix4x4r l_mtxMeshGlobal = l_renderNode.m_node->GetDerivedTransformationMatrix().get_inverse().transpose();
							Point3r l_position = p_camera.GetParent()->GetDerivedPosition();
							Point3r l_ptCameraLocal = l_mtxMeshGlobal * l_position;
							l_renderNode.m_submesh->SortFaces( l_ptCameraLocal );
							l_ptCameraLocal -= l_renderNode.m_submesh->GetCubeBox().GetCenter();
							p_output.insert( std::make_pair( p_sign * point::distance_squared( l_ptCameraLocal ), l_renderNode ) );
						}
					}
				}
			}
		}
	}

	void Scene::DoRenderBillboards( RenderTechniqueBase & p_technique, Pipeline & p_pipeline, BillboardListStrMapIt p_itBegin, BillboardListStrMapIt p_itEnd )
	{
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();

		for ( auto l_it = p_itBegin; l_it != p_itEnd; ++l_it )
		{
			if ( l_it->second->InitialiseShader( p_technique ) )
			{
				p_pipeline.SetModelMatrix( l_it->second->GetParent()->GetDerivedTransformationMatrix() );
				l_it->second->Render();
			}
		}
	}
}
