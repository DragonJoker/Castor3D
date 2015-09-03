#include "Scene.hpp"

#include "Engine.hpp"
#include "CleanupEvent.hpp"
#include "DepthStencilState.hpp"
#include "FrameVariableBuffer.hpp"
#include "FunctorEvent.hpp"
#include "MatrixFrameVariable.hpp"
#include "Buffer.hpp"
#include "Animation.hpp"
#include "AnimatedObjectGroup.hpp"
#include "AnimatedObject.hpp"
#include "Light.hpp"
#include "SceneNode.hpp"
#include "Mesh.hpp"
#include "Geometry.hpp"
#include "Engine.hpp"
#include "Material.hpp"
#include "Pass.hpp"
#include "Submesh.hpp"
#include "Pipeline.hpp"
#include "Camera.hpp"
#include "Ray.hpp"
#include "Importer.hpp"
#include "InitialiseEvent.hpp"
#include "Face.hpp"
#include "Vertex.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"
#include "Skeleton.hpp"
#include "DirectionalLight.hpp"
#include "FrameVariable.hpp"
#include "ShaderProgram.hpp"
#include "BlendState.hpp"
#include "BillboardList.hpp"
#include "ShaderManager.hpp"
#include "StaticTexture.hpp"
#include "DynamicTexture.hpp"
#include "TextureUnit.hpp"
#include "OneFrameVariable.hpp"
#include "PointFrameVariable.hpp"
#include "RenderSystem.hpp"

#include <Image.hpp>
#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		void ApplyLightComponent( float p_exp, float p_cut, int p_index, int & p_offset, PxBufferBase & p_data )
		{
			float * l_pDst = reinterpret_cast< float * >( p_data.get_at( p_index * 10 + p_offset++, 0 ) );
			*l_pDst++ = p_exp;
			*l_pDst++ = p_cut;
		}

		void ApplyLightComponent( Point3f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )
		{
			uint8_t * l_pDst = p_data.get_at( p_index * 10 + p_offset++, 0 );
			memcpy( l_pDst, p_component.const_ptr(), 3 * sizeof( float ) );
		}

		void ApplyLightComponent( Point4f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )
		{
			uint8_t * l_pDst = p_data.get_at( p_index * 10 + p_offset++, 0 );
			memcpy( l_pDst, p_component.const_ptr(), 4 * sizeof( float ) );
		}

		void ApplyLightMtxComponent( float const * p_component, int p_index, int & p_offset, PxBufferBase & p_data )
		{
			uint8_t * l_pDst = p_data.get_at( p_index * 10 + p_offset++, 0 );
			memcpy( l_pDst, p_component, 4 * sizeof( float ) );
		}

		void ApplyLightComponent( Matrix4x4f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )
		{
			ApplyLightMtxComponent( p_component[0], p_index, p_offset, p_data );
			ApplyLightMtxComponent( p_component[1], p_index, p_offset, p_data );
			ApplyLightMtxComponent( p_component[2], p_index, p_offset, p_data );
			ApplyLightMtxComponent( p_component[3], p_index, p_offset, p_data );
		}

		void ApplyLightComponent( Matrix4x4d const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )
		{
			ApplyLightComponent( Matrix4x4f( p_component.const_ptr() ), p_index, p_offset, p_data );
		}

		void ApplyLightColourIntensity( Point4f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )
		{
			float * l_pDst = reinterpret_cast< float * >( p_data.get_at( p_index * 10 + p_offset++, 0 ) );
			*l_pDst++ = p_component[2];// R
			*l_pDst++ = p_component[1];// G
			*l_pDst++ = p_component[0];// B
			*l_pDst++ = p_component[3];// I
		}
	}

	//*************************************************************************************************

	Scene::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Scene, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool Scene::TextLoader::operator()( Scene const & p_scene, TextFile & p_file )
	{
		Logger::LogInfo( cuT( "Scene::Write - Scene Name" ) );
		bool l_bReturn = p_file.WriteText( cuT( "scene \"" ) + p_scene.GetName() + cuT( "\"\n{\n" ) ) > 0;

		if ( l_bReturn )
		{
			l_bReturn = p_file.Print( 256, cuT( "\tbackground_colour " ) ) > 0 && Colour::TextLoader()( p_scene.GetBackgroundColour(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.Print( 256, cuT( "\tambient_light " ) ) > 0 && Colour::TextLoader()( p_scene.GetAmbientLight(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			Logger::LogInfo( cuT( "Scene::Write - Camera Nodes" ) );
			l_bReturn = SceneNode::TextLoader()( *p_scene.GetCameraRootNode(), p_file );
		}

		if ( l_bReturn )
		{
			Logger::LogInfo( cuT( "Scene::Write - Object Nodes" ) );
			l_bReturn = SceneNode::TextLoader()( *p_scene.GetObjectRootNode(), p_file );
		}

		if ( l_bReturn )
		{
			Logger::LogInfo( cuT( "Scene::Write - Cameras" ) );
			CameraPtrStrMapConstIt l_it = p_scene.CamerasBegin();

			while ( l_bReturn && l_it != p_scene.CamerasEnd() )
			{
				l_bReturn = Camera::TextLoader()( *l_it->second, p_file );
				++l_it;
			}
		}

		if ( l_bReturn )
		{
			Logger::LogInfo( cuT( "Scene::Write - Lights" ) );
			LightPtrIntMapConstIt l_it = p_scene.LightsBegin();

			while ( l_bReturn && l_it != p_scene.LightsEnd() )
			{
				switch ( l_it->second->GetLightType() )
				{
				case eLIGHT_TYPE_DIRECTIONAL:
					l_bReturn = DirectionalLight::TextLoader()( *l_it->second->GetDirectionalLight(), p_file );
					break;

				case eLIGHT_TYPE_POINT:
					l_bReturn = PointLight::TextLoader()( *l_it->second->GetPointLight(), p_file );
					break;

				case eLIGHT_TYPE_SPOT:
					l_bReturn = SpotLight::TextLoader()( *l_it->second->GetSpotLight(), p_file );
					break;

				default:
					l_bReturn = false;
				}

				++l_it;
			}
		}

		if ( l_bReturn )
		{
			Logger::LogInfo( cuT( "Scene::Write - Geometries" ) );
			GeometryPtrStrMapConstIt l_it = p_scene.GeometriesBegin();

			while ( l_bReturn && l_it != p_scene.GeometriesEnd() )
			{
				l_bReturn = Geometry::TextLoader()( *l_it->second, p_file );
				++l_it;
			}
		}

		p_file.WriteText( cuT( "}\n\n" ) );
		return l_bReturn;
	}

	//*************************************************************************************************

	Scene::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< Scene >( p_path )
	{
	}

	bool Scene::BinaryParser::Fill( Scene const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SCENE );
		l_bReturn = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetBackgroundColour(), eCHUNK_TYPE_SCENE_BACKGROUND, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetAmbientLight(), eCHUNK_TYPE_SCENE_AMBIENT, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = SceneNode::BinaryParser( m_path ).Fill( *p_obj.GetCameraRootNode(), l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = SceneNode::BinaryParser( m_path ).Fill( *p_obj.GetObjectRootNode(), l_chunk );
		}

		if ( l_bReturn )
		{
			CameraPtrStrMapConstIt l_it = p_obj.CamerasBegin();

			while ( l_bReturn && l_it != p_obj.CamerasEnd() )
			{
				l_bReturn = Camera::BinaryParser( m_path ).Fill( *l_it->second, l_chunk );
				++l_it;
			}
		}

		if ( l_bReturn )
		{
			LightPtrIntMapConstIt l_it = p_obj.LightsBegin();

			while ( l_bReturn && l_it != p_obj.LightsEnd() )
			{
				switch ( l_it->second->GetLightType() )
				{
				case eLIGHT_TYPE_DIRECTIONAL:
					l_bReturn = DirectionalLight::BinaryParser( m_path ).Fill( *l_it->second->GetDirectionalLight(), l_chunk );
					break;

				case eLIGHT_TYPE_POINT:
					l_bReturn = PointLight::BinaryParser( m_path ).Fill( *l_it->second->GetPointLight(), l_chunk );
					break;

				case eLIGHT_TYPE_SPOT:
					l_bReturn = SpotLight::BinaryParser( m_path ).Fill( *l_it->second->GetSpotLight(), l_chunk );
					break;

				default:
					l_bReturn = false;
				}

				++l_it;
			}
		}

		if ( l_bReturn )
		{
			GeometryPtrStrMapConstIt l_it = p_obj.GeometriesBegin();

			while ( l_bReturn && l_it != p_obj.GeometriesEnd() )
			{
				l_bReturn = Geometry::BinaryParser( m_path ).Fill( *l_it->second, l_chunk );
				++l_it;
			}
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool Scene::BinaryParser::Parse( Scene & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		Colour l_colour;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_NAME:
					l_bReturn = DoParseChunk( l_name, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetName( l_name );
					}

					break;

				case eCHUNK_TYPE_SCENE_BACKGROUND:
					l_bReturn = DoParseChunk( l_colour, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetBackgroundColour( l_colour );
					}

					break;

				case eCHUNK_TYPE_SCENE_AMBIENT:
					l_bReturn = DoParseChunk( l_colour, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetAmbientLight( l_colour );
					}

					break;

				case eCHUNK_TYPE_SCENE_NODE:
				{
					BinaryChunk l_chunkNode;
					l_bReturn = l_chunk.GetSubChunk( l_chunkNode );

					if ( l_bReturn )
					{
						switch ( l_chunkNode.GetChunkType() )
						{
						case eCHUNK_TYPE_NAME:
							l_bReturn = DoParseChunk( l_name, l_chunkNode );
							break;

						default:
							l_bReturn = false;
							break;
						}

						if ( l_bReturn )
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
								l_node = p_obj.GetNode( l_name );
							}

							if ( !l_node )
							{
								l_node = p_obj.CreateSceneNode( l_name );
							}

							l_bReturn = SceneNode::BinaryParser( m_path ).Parse( *l_node, l_chunk );
						}
					}
				}
				break;

				case eCHUNK_TYPE_CAMERA:
				{
					BinaryChunk l_chunkCamera;
					l_bReturn = l_chunk.GetSubChunk( l_chunkCamera );

					if ( l_bReturn )
					{
						switch ( l_chunkCamera.GetChunkType() )
						{
						case eCHUNK_TYPE_NAME:
							l_bReturn = DoParseChunk( l_name, l_chunkCamera );
							break;

						default:
							l_bReturn = false;
							break;
						}

						if ( l_bReturn )
						{
							CameraSPtr l_camera = p_obj.CreateCamera( l_name, 100, 100, nullptr, eVIEWPORT_TYPE_3D );
							l_bReturn = Camera::BinaryParser( m_path ).Parse( *l_camera, l_chunk );
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

					while ( l_bReturn && ( l_name.empty() || l_type == eLIGHT_TYPE_COUNT ) )
					{
						BinaryChunk l_chunkLight;
						l_bReturn = l_chunk.GetSubChunk( l_chunkLight );

						if ( l_bReturn )
						{
							switch ( l_chunkLight.GetChunkType() )
							{
							case eCHUNK_TYPE_NAME:
								l_bReturn = DoParseChunk( l_name, l_chunkLight );
								break;

							case eCHUNK_TYPE_MOVABLE_NODE:
								l_bReturn = DoParseChunk( l_nodeName, l_chunkLight );

								if ( l_bReturn )
								{
									l_node = p_obj.GetNode( l_nodeName );
								}

								break;

							case eCHUNK_TYPE_LIGHT_TYPE:
								l_bReturn = DoParseChunk( l_type, l_chunkLight );
								break;

							default:
								l_bReturn = false;
								break;
							}
						}
					}

					if ( l_bReturn && !l_name.empty() && l_type != eLIGHT_TYPE_COUNT )
					{
						LightSPtr l_light = p_obj.CreateLight( l_name, l_node, l_type );
						l_bReturn = Light::BinaryParser( m_path ).Parse( *l_light, l_chunk );
					}
				}
				break;

				case eCHUNK_TYPE_GEOMETRY:
				{
					BinaryChunk l_chunkGeometry;
					l_bReturn = l_chunk.GetSubChunk( l_chunkGeometry );

					if ( l_bReturn )
					{
						switch ( l_chunkGeometry.GetChunkType() )
						{
						case eCHUNK_TYPE_NAME:
							l_bReturn = DoParseChunk( l_name, l_chunkGeometry );
							break;

						default:
							l_bReturn = false;
							break;
						}

						if ( l_bReturn )
						{
							GeometrySPtr l_geometry = p_obj.CreateGeometry( l_name );
							l_bReturn = Geometry::BinaryParser( m_path ).Parse( *l_geometry, l_chunk );
						}
					}
				}
				break;

				default:
					l_bReturn = false;
					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	Scene::Scene( Engine * p_pEngine, LightFactory & p_lightFactory, String const & p_name )
		: m_strName( p_name )
		, m_rootCameraNode()
		, m_rootObjectNode()
		, m_nbFaces( 0 )
		, m_nbVertex( 0 )
		, m_changed( false )
		, m_lightFactory( p_lightFactory )
		, m_pEngine( p_pEngine )
		, m_pLightsTexture( std::make_shared< TextureUnit >( p_pEngine ) )
		, m_bLightsChanged( true )
	{
		m_pLightsData = PxBufferBase::create( Size( 1000, 1 ), ePIXEL_FORMAT_ARGB32F );
		DynamicTextureSPtr l_pTexture = GetEngine()->GetRenderSystem()->CreateDynamicTexture();
		l_pTexture->SetDimension( eTEXTURE_DIMENSION_1D );
		l_pTexture->SetImage( m_pLightsData );
		SamplerSPtr l_pSampler = GetEngine()->GetLightsSampler();
		m_pLightsTexture->SetAutoMipmaps( true );
		m_pLightsTexture->SetSampler( l_pSampler );
		m_pLightsTexture->SetTexture( l_pTexture );

		for ( int i = 0; i < 100; i++ )
		{
			m_setFreeLights.insert( i );
		}

		m_pEngine->PostEvent( MakeInitialiseEvent( *m_pLightsTexture ) );
	}

	Scene::~Scene()
	{
		ClearScene();
		DoRemoveAll( m_addedCameras, m_arrayCamerasToDelete );
		DoRemoveAll( m_addedNodes, m_arrayNodesToDelete );
		m_alphaDepthState.reset();
		m_arrayCamerasToDelete.clear();
		m_arrayNodesToDelete.clear();
		m_rootNode.reset();
		m_rootCameraNode.reset();
		m_rootObjectNode.reset();
	}

	void Scene::Initialise()
	{
		m_rootNode = std::make_shared< SceneNode >( shared_from_this(), cuT( "RootNode" ) );
		m_rootCameraNode = std::make_shared< SceneNode >( shared_from_this(), cuT( "CameraRootNode" ) );
		m_rootObjectNode = std::make_shared< SceneNode >( shared_from_this(), cuT( "ObjectRootNode" ) );
		m_alphaDepthState = m_pEngine->CreateDepthStencilState( m_strName + cuT( "_AlphaDepthState" ) );
		m_rootCameraNode->AttachTo( m_rootNode );
		m_rootObjectNode->AttachTo( m_rootNode );
		m_addedNodes.insert( std::make_pair( cuT( "ObjectRootNode" ), m_rootObjectNode ) );

		m_alphaDepthState.lock()->SetDepthMask( eWRITING_MASK_ZERO );
	}

	void Scene::ClearScene()
	{
		m_mapSubmeshesAlpha.clear();
		m_mapSubmeshesAlphaSorted.clear();
		m_mapSubmeshesNoAlpha.clear();
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		m_arrayOverlays.clear();
		DoRemoveAll( m_addedLights, m_arrayLightsToDelete );
		DoRemoveAll( m_addedPrimitives, m_arrayPrimitivesToDelete );
		DoRemoveAll( m_mapBillboardsLists, m_arrayBillboardsToDelete );

		DepthStencilStateSPtr state = m_alphaDepthState.lock();

		if ( state )
		{
			state->Cleanup();
		}

		if ( m_pBackgroundImage )
		{
			m_pEngine->PostEvent( MakeCleanupEvent( *m_pBackgroundImage ) );
		}

		m_arrayLightsToDelete.clear();
		m_arrayPrimitivesToDelete.clear();

		m_pEngine->PostEvent( MakeCleanupEvent( *m_pLightsTexture ) );
	}

	void Scene::RenderBackground( Camera const & p_camera )
	{
		if ( m_pBackgroundImage )
		{
			if ( m_pBackgroundImage->IsInitialised() )
			{
				RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();
				ContextRPtr l_pContext = l_pRenderSystem->GetCurrentContext();
				l_pContext->GetBackgroundDSState()->Apply();
#if !defined( NDEBUG )
				Colour l_save = GetEngine()->GetRenderSystem()->GetCurrentContext()->GetClearColour();
				l_pContext->SetClearColour( Colour::from_predef( Colour::ePREDEFINED_FULLALPHA_DARKBLUE ) );
#endif
				l_pContext->BToBRender( Size( p_camera.GetWidth(), p_camera.GetHeight() ), m_pBackgroundImage, eBUFFER_COMPONENT_COLOUR );
#if !defined( NDEBUG )
				l_pContext->SetClearColour( l_save );
#endif
			}
		}
	}

	void Scene::Render( eTOPOLOGY p_eTopology, double CU_PARAM_UNUSED( p_dFrameTime ), Camera const & p_camera )
	{
		RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();
		Pipeline * l_pPipeline = l_pRenderSystem->GetPipeline();
		ContextRPtr l_pContext = l_pRenderSystem->GetCurrentContext();
		PassSPtr l_pPass;
		MaterialSPtr l_pMaterial;
		DoDeleteToDelete();
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		DoUpdateAnimations();

		if ( !m_arraySubmeshesNoAlpha.empty() || !m_arraySubmeshesAlpha.empty() || !m_mapBillboardsLists.empty() )
		{
			l_pPipeline->MatrixMode( eMTXMODE_MODEL );
			l_pPipeline->LoadIdentity();

			if ( !m_arraySubmeshesNoAlpha.empty() )
			{
				l_pContext->CullFace( eFACE_BACK );

				if ( l_pRenderSystem->HasInstancing() )
				{
					DoRenderSubmeshesInstanced( p_camera, *l_pPipeline, p_eTopology, m_mapSubmeshesNoAlpha.begin(), m_mapSubmeshesNoAlpha.end() );
				}
				else
				{
					DoRenderSubmeshesNonInstanced( p_camera, *l_pPipeline, p_eTopology, m_arraySubmeshesNoAlpha.begin(), m_arraySubmeshesNoAlpha.end() );
				}
			}

			if ( !m_mapSubmeshesAlpha.empty() || !m_arraySubmeshesAlpha.empty() )
			{
				if ( l_pContext->IsMultiSampling() )
				{
					if ( l_pRenderSystem->HasInstancing() )
					{
						l_pContext->CullFace( eFACE_FRONT );
						DoRenderSubmeshesInstanced( p_camera, *l_pPipeline, p_eTopology, m_mapSubmeshesAlpha.begin(), m_mapSubmeshesAlpha.end() );
						l_pContext->CullFace( eFACE_BACK );
						DoRenderSubmeshesInstanced( p_camera, *l_pPipeline, p_eTopology, m_mapSubmeshesAlpha.begin(), m_mapSubmeshesAlpha.end() );
					}
					else
					{
						l_pContext->CullFace( eFACE_FRONT );
						DoRenderSubmeshesNonInstanced( p_camera, *l_pPipeline, p_eTopology, m_arraySubmeshesAlpha.begin(), m_arraySubmeshesAlpha.end() );
						l_pContext->CullFace( eFACE_BACK );
						DoRenderSubmeshesNonInstanced( p_camera, *l_pPipeline, p_eTopology, m_arraySubmeshesAlpha.begin(), m_arraySubmeshesAlpha.end() );
					}
				}
				else
				{
					DepthStencilStateSPtr state = m_alphaDepthState.lock();

					if ( state )
					{
						state->Apply();
					}

					DoResortAlpha( p_camera, m_arraySubmeshesAlpha.begin(), m_arraySubmeshesAlpha.end(), m_mapSubmeshesAlphaSorted, 1 );
					l_pContext->CullFace( eFACE_FRONT );
					DoRenderAlphaSortedSubmeshes( *l_pPipeline, p_eTopology, m_mapSubmeshesAlphaSorted.begin(), m_mapSubmeshesAlphaSorted.end() );
					l_pContext->CullFace( eFACE_BACK );
					DoRenderAlphaSortedSubmeshes( *l_pPipeline, p_eTopology, m_mapSubmeshesAlphaSorted.begin(), m_mapSubmeshesAlphaSorted.end() );
				}
			}

			if ( !m_mapBillboardsLists.empty() )
			{
				l_pContext->CullFace( eFACE_FRONT );
				DoRenderBillboards( *l_pPipeline, m_mapBillboardsLists.begin(), m_mapBillboardsLists.end() );
				l_pContext->CullFace( eFACE_BACK );
				DoRenderBillboards( *l_pPipeline, m_mapBillboardsLists.begin(), m_mapBillboardsLists.end() );
			}
		}
	}

	bool Scene::SetBackgroundImage( Path const & p_pathFile )
	{
		bool l_bReturn = false;
		ImageSPtr l_pImage;

		if ( !p_pathFile.empty() )
		{
			l_pImage = m_pEngine->GetImageManager().find( p_pathFile.GetFileName() );

			if ( !l_pImage && File::FileExists( p_pathFile ) )
			{
				l_pImage = std::make_shared< Image >( p_pathFile.GetFileName(), p_pathFile );
				m_pEngine->GetImageManager().insert( p_pathFile.GetFileName(), l_pImage );
			}
		}

		if ( l_pImage )
		{
			StaticTextureSPtr l_pStaTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
			l_pStaTexture->SetDimension( eTEXTURE_DIMENSION_2D );
			l_pStaTexture->SetImage( l_pImage->GetPixels() );
			m_pBackgroundImage = l_pStaTexture;
			m_pEngine->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [this]()
			{
				m_pBackgroundImage->Create();
				m_pBackgroundImage->Initialise( 0 );
				m_pBackgroundImage->Bind();
				m_pBackgroundImage->GenerateMipmaps();
				m_pBackgroundImage->Unbind();
			} ) );
			l_bReturn = true;
		}

		return l_bReturn;
	}

	void Scene::InitialiseGeometries()
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		m_nbFaces = 0;
		m_nbVertex = 0;

		if ( m_newlyAddedPrimitives.size() > 0 )
		{
			for ( auto && l_pair: m_newlyAddedPrimitives )
			{
				l_pair.second->CreateBuffers( m_nbFaces, m_nbVertex );
			}

			m_newlyAddedPrimitives.clear();
		}
		else
		{
			m_rootNode->CreateBuffers( m_nbFaces, m_nbVertex );
		}

		Logger::LogInfo( StringStream() << cuT( "Scene::CreateList - [" ) << m_strName << cuT( "] - NbVertex : " ) << m_nbVertex << cuT( " - NbFaces : " ) << m_nbFaces );
		DoSortByAlpha();
		DepthStencilStateSPtr state = m_alphaDepthState.lock();

		if ( state )
		{
			state->Initialise();
		}

		m_changed = false;
	}

	SceneNodeSPtr Scene::CreateSceneNode( String const & p_name )
	{
		SceneNodeSPtr l_pReturn;

		if ( p_name != cuT( "RootNode" ) )
		{
			if ( DoCheckObject( p_name, m_addedNodes, cuT( "SceneNode" ) ) )
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				l_pReturn = std::make_shared< SceneNode >( shared_from_this(), p_name );
				Logger::LogInfo( cuT( "Scene::CreateSceneNode - SceneNode [" ) + p_name + cuT( "] - Created" ) );
				l_pReturn->AttachTo( m_rootNode );
				m_addedNodes[p_name] = l_pReturn;
			}
			else
			{
				l_pReturn = m_addedNodes.find( p_name )->second;
			}
		}
		else
		{
			Logger::LogWarning( cuT( "Scene::CreateSceneNode - Can't create scene node [RootNode] - Another scene node with the same name already exists" ) );
			l_pReturn = m_addedNodes.find( p_name )->second;
		}

		return l_pReturn;
	}

	SceneNodeSPtr Scene::CreateSceneNode( String const & p_name, SceneNodeSPtr p_parent )
	{
		SceneNodeSPtr l_pReturn;

		if ( p_name != cuT( "RootNode" ) )
		{
			if ( DoCheckObject( p_name, m_addedNodes, cuT( "SceneNode" ) ) )
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				l_pReturn = std::make_shared< SceneNode >( shared_from_this(), p_name );
				Logger::LogInfo( cuT( "Scene::CreateSceneNode - SceneNode [" ) + p_name + cuT( "] - Created" ) );

				if ( p_parent )
				{
					l_pReturn->AttachTo( p_parent );
				}
				else
				{
					l_pReturn->AttachTo( m_rootNode );
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
			Logger::LogWarning( cuT( "Scene::CreateSceneNode - Can't create scene node [RootNode] - Another scene node with the same name already exists" ) );
			l_pReturn = m_addedNodes.find( p_name )->second;
		}

		return l_pReturn;
	}

	GeometrySPtr Scene::CreateGeometry( String const & p_name, eMESH_TYPE p_type, String const & p_meshName, UIntArray p_faces, RealArray p_size )
	{
		GeometrySPtr l_pReturn;

		if ( DoCheckObject( p_name, m_addedPrimitives, cuT( "Geometry" ) ) )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			MeshSPtr l_pMesh = m_pEngine->CreateMesh( p_type, p_meshName, p_faces, p_size );

			if ( l_pMesh )
			{
				l_pReturn = std::make_shared< Geometry >( shared_from_this(), l_pMesh, nullptr, p_name );
				Logger::LogInfo( cuT( "Scene::CreatePrimitive - Geometry [" ) + p_name + cuT( "] - Created" ) );
				m_addedPrimitives[p_name] = l_pReturn;
				m_newlyAddedPrimitives[p_name] = l_pReturn;
				m_changed = true;
			}
			else
			{
				Logger::LogError( cuT( "Scene::CreatePrimitive - Can't create primitive [" ) + p_name + cuT( "] - Mesh creation failed" ) );
			}
		}
		else
		{
			l_pReturn = m_addedPrimitives.find( p_name )->second;
		}

		return l_pReturn;
	}

	GeometrySPtr Scene::CreateGeometry( String const & p_name )
	{
		GeometrySPtr l_pReturn;

		if ( DoCheckObject( p_name, m_addedPrimitives, cuT( "Geometry" ) ) )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			l_pReturn = std::make_shared< Geometry >( shared_from_this(), nullptr, m_rootObjectNode, p_name );
			m_rootObjectNode->AttachObject( l_pReturn );
			Logger::LogInfo( cuT( "Scene::CreatePrimitive - Geometry [" ) + p_name + cuT( "] - Created" ) );
		}
		else
		{
			l_pReturn = m_addedPrimitives.find( p_name )->second;
		}

		return l_pReturn;
	}

	CameraSPtr Scene::CreateCamera( String const & p_name, int p_ww, int p_wh, SceneNodeSPtr p_pNode, eVIEWPORT_TYPE p_type )
	{
		CameraSPtr l_pReturn;

		if ( DoCheckObject( p_name, m_addedCameras, cuT( "Camera" ) ) )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			l_pReturn = std::make_shared< Camera >( shared_from_this(), p_name, p_pNode, Size( p_ww, p_wh ), p_type );

			if ( p_pNode )
			{
				p_pNode->AttachObject( l_pReturn );
			}

			m_addedCameras[p_name] = l_pReturn;
			Logger::LogInfo( cuT( "Scene::CreateCamera - Camera [" ) + p_name + cuT( "] created" ) );
		}

		return l_pReturn;
	}

	CameraSPtr Scene::CreateCamera( String const & p_name, SceneNodeSPtr p_pNode, ViewportSPtr p_pViewport )
	{
		CameraSPtr l_pReturn;

		if ( DoCheckObject( p_name, m_addedCameras, cuT( "Camera" ) ) )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			l_pReturn = std::make_shared< Camera >( shared_from_this(), p_name, p_pNode, p_pViewport );

			if ( p_pNode )
			{
				p_pNode->AttachObject( l_pReturn );
			}

			m_addedCameras[p_name] = l_pReturn;
			Logger::LogInfo( cuT( "Scene::CreateCamera - Camera [" ) + p_name + cuT( "] created" ) );
		}

		return l_pReturn;
	}

	LightSPtr Scene::CreateLight( String const & p_name, SceneNodeSPtr p_pNode, eLIGHT_TYPE p_eLightType )
	{
		LightSPtr l_pReturn;

		if ( DoCheckObject( p_name, m_addedLights, cuT( "Light" ) ) )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			l_pReturn = std::make_shared< Light >( m_lightFactory, shared_from_this(), p_eLightType, p_pNode, p_name );

			if ( p_pNode )
			{
				p_pNode->AttachObject( l_pReturn );
			}

			AddLight( l_pReturn );
			Logger::LogInfo( cuT( "Scene::CreateLight - Light [" ) + p_name + cuT( "] created" ) );
		}
		else
		{
			l_pReturn = m_addedLights.find( p_name )->second;
		}

		return l_pReturn;
	}

	AnimatedObjectGroupSPtr Scene::CreateAnimatedObjectGroup( String const & p_name )
	{
		AnimatedObjectGroupSPtr l_pReturn;

		if ( DoCheckObject( p_name, m_addedGroups, cuT( "AnimatedObjectGroup" ) ) )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			l_pReturn = std::make_shared< AnimatedObjectGroup >( shared_from_this(), p_name );
			AddAnimatedObjectGroup( l_pReturn );
			Logger::LogInfo( cuT( "Scene::CreateAnimatedObjectGroup - AnimatedObjectGroup [" ) + p_name + cuT( "] created" ) );
		}
		else
		{
			l_pReturn = m_addedGroups.find( p_name )->second;
		}

		return l_pReturn;
	}

	void Scene::AddNode( SceneNodeSPtr p_node )
	{
		DoAddObject( p_node, m_addedNodes, cuT( "SceneNode" ) );
	}

	void Scene::AddLight( LightSPtr p_light )
	{
		if ( DoAddObject( p_light, m_addedLights, cuT( "Light" ) ) )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			int l_iIndex = 0;
			bool l_bFound = false;
			LightPtrIntMapIt l_itMap = m_mapLights.begin();

			while ( l_itMap != m_mapLights.end() && !l_bFound )
			{
				if ( l_itMap->first != l_iIndex )
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

	void Scene::AddGeometry( GeometrySPtr p_geometry )
	{
		if ( DoAddObject( p_geometry, m_addedPrimitives, cuT( "Geometry" ) ) )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			m_newlyAddedPrimitives[p_geometry->GetName()] = p_geometry;
			m_changed = true;
		}
	}

	void Scene::AddBillboards( BillboardListSPtr p_pList )
	{
		if ( DoAddObject( p_pList, m_mapBillboardsLists, cuT( "Billboard" ) ) )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			m_changed = true;
		}
	}

	void Scene::AddAnimatedObjectGroup( AnimatedObjectGroupSPtr p_pGroup )
	{
		DoAddObject( p_pGroup, m_addedGroups, cuT( "AnimatedObjectGroup" ) );
	}

	SceneNodeSPtr Scene::GetNode( String const & p_name )const
	{
		SceneNodeSPtr l_pReturn;

		if ( p_name == cuT( "RootNode" ) )
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

	GeometrySPtr Scene::GetGeometry( String const & p_name )const
	{
		return DoGetObject( m_addedPrimitives, p_name );
	}

	LightSPtr Scene::GetLight( String const & p_name )const
	{
		return DoGetObject( m_addedLights, p_name );
	}

	BillboardListSPtr Scene::GetBillboards( String const & p_name )const
	{
		return DoGetObject( m_mapBillboardsLists, p_name );
	}

	CameraSPtr Scene::GetCamera( String const & p_name )const
	{
		return DoGetObject( m_addedCameras, p_name );
	}

	AnimatedObjectGroupSPtr Scene::GetAnimatedObjectGroup( String const & p_name )const
	{
		return DoGetObject( m_addedGroups, p_name );
	}

	void Scene::RemoveNode( SceneNodeSPtr p_pNode )
	{
		if ( p_pNode )
		{
			DoRemoveObject( p_pNode, m_addedNodes, m_arrayNodesToDelete );
		}
	}

	void Scene::RemoveGeometry( GeometrySPtr p_geometry )
	{
		if ( p_geometry )
		{
			DoRemoveObject( p_geometry, m_addedPrimitives, m_arrayPrimitivesToDelete );
		}
	}

	void Scene::RemoveBillboards( BillboardListSPtr p_pList )
	{
		if ( p_pList )
		{
			DoRemoveObject( p_pList, m_mapBillboardsLists, m_arrayBillboardsToDelete );
		}
	}

	void Scene::RemoveLight( LightSPtr p_pLight )
	{
		if ( p_pLight )
		{
			DoRemoveObject( p_pLight, m_addedLights, m_arrayLightsToDelete );
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			LightPtrIntMapIt l_itMap = m_mapLights.find( p_pLight->GetIndex() );

			if ( l_itMap != m_mapLights.end() )
			{
				m_mapLights.erase( l_itMap );
			}
		}
	}

	void Scene::RemoveCamera( CameraSPtr p_pCamera )
	{
		if ( p_pCamera )
		{
			DoRemoveObject( p_pCamera, m_addedCameras, m_arrayCamerasToDelete );
		}
	}

	void Scene::RemoveAnimatedObjectGroup( AnimatedObjectGroupSPtr p_pGroup )
	{
		if ( p_pGroup )
		{
			std::vector< AnimatedObjectGroupSPtr > l_arrayDelete;
			DoRemoveObject( p_pGroup, m_addedGroups, l_arrayDelete );
		}
	}

	void Scene::RemoveAllNodes()
	{
		DoRemoveAll( m_addedNodes, m_arrayNodesToDelete );
	}

	void Scene::RemoveAllLights()
	{
		m_mapLights.clear();
		DoRemoveAll( m_addedLights, m_arrayLightsToDelete );
	}

	void Scene::RemoveAllGeometries()
	{
		DoRemoveAll( m_addedPrimitives, m_arrayPrimitivesToDelete );
	}

	void Scene::RemoveAllBillboards()
	{
		while ( m_mapBillboardsLists.size() )
		{
			m_arrayBillboardsToDelete.push_back( m_mapBillboardsLists.begin()->second );
			m_mapBillboardsLists.erase( m_mapBillboardsLists.begin() );
		}
	}

	void Scene::RemoveAllCameras()
	{
		DoRemoveAll( m_addedCameras, m_arrayCamerasToDelete );
	}

	void Scene::RemoveAllAnimatedObjectGroups()
	{
		m_addedGroups.clear();
	}

	void Scene::Merge( SceneSPtr p_pScene )
	{
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_pScene->m_mutex );
			DoMerge( p_pScene, p_pScene->m_addedNodes, m_addedNodes );
			DoMerge( p_pScene, p_pScene->m_addedLights, m_addedLights );
			DoMerge( p_pScene, p_pScene->m_addedCameras, m_addedCameras );
			DoMerge( p_pScene, p_pScene->m_addedPrimitives, m_addedPrimitives );
			DoMerge( p_pScene, p_pScene->m_newlyAddedPrimitives, m_newlyAddedPrimitives );
			DoMerge( p_pScene, p_pScene->m_mapBillboardsLists, m_mapBillboardsLists );
			//DoMerge( p_pScene, p_pScene->m_addedGroups, m_addedGroups );


			//Castor::String l_strName;

			//for( auto && l_pair: p_pScene->m_mapBillboardsLists )
			//{
			//	l_strName = l_pair.first;

			//	while( m_mapBillboardsLists.find( l_strName ) != m_mapBillboardsLists.end() )
			//	{
			//		l_strName = p_pScene->GetName() + cuT( "_" ) + l_strName;
			//	}

			//	//l_pair.second->SetName( l_strName );
			//	m_mapBillboardsLists.insert( std::make_pair( l_strName, l_pair.second ) );
			//}

			//p_pScene->m_mapBillboardsLists.clear();
			m_changed = true;
			m_clAmbientLight = p_pScene->GetAmbientLight();
		}
		p_pScene->ClearScene();
	}

	bool Scene::ImportExternal( String const & p_fileName, Importer & p_importer )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		bool l_bReturn = true;
		SceneSPtr l_pScene = p_importer.ImportScene( p_fileName, Parameters() );

		if ( l_pScene )
		{
			Merge( l_pScene );
			m_changed = true;
			l_bReturn = true;
		}

		return l_bReturn;
	}

	void Scene::Select( Ray * p_ray, GeometrySPtr & p_geo, SubmeshSPtr & p_submesh, FaceSPtr * p_face, Vertex * p_vertex )
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

		for ( auto && l_pair: m_addedPrimitives )
		{
			l_geo = l_pair.second;

			if ( l_geo->IsVisible() )
			{
				l_mesh = l_geo->GetMesh();
				l_sphere = l_mesh->GetCollisionSphere();

				if ( ( l_curgeoDist = p_ray->Intersects( l_sphere ) ) > 0.0f )
				{
					l_box = l_mesh->GetCollisionBox();

					if ( p_ray->Intersects( l_box ) > 0.0f )
					{
						l_geoDist = l_curgeoDist;

						for ( auto && l_it: *l_mesh )
						{
							l_sphere = l_it->GetSphere();

							if ( p_ray->Intersects( l_sphere ) >= 0.0f )
							{
								l_box = l_it->GetCubeBox();

								if ( p_ray->Intersects( l_box ) >= 0.0f )
								{
									for ( uint32_t k = 0; k < l_it->GetFaceCount(); k++ )
									{
										l_face = l_it->GetFace( k );

										if ( ( l_curfaceDist = p_ray->Intersects( *l_face, *l_submesh ) ) < l_faceDist )
										{
											if ( ( l_curvertexDist = p_ray->Intersects( Vertex::GetPosition( l_it->GetPoint( l_face->GetVertexIndex( 0 ) ), l_ptCoords ) ) ) < l_vertexDist )
											{
												l_vertexDist = l_curvertexDist;
												l_geoDist = l_curgeoDist;
												l_faceDist = l_curfaceDist;
												l_selectedGeo = l_geo;
												l_selectedSubmesh = l_submesh;
												l_selectedFace = l_face;
												//l_pSelectedVertex = p_pSubmesh->GetPoint( l_face->GetVertexIndex( 0 ) );
											}

											if ( ( l_curvertexDist = p_ray->Intersects( Vertex::GetPosition( l_it->GetPoint( l_face->GetVertexIndex( 1 ) ), l_ptCoords ) ) ) < l_vertexDist )
											{
												l_vertexDist = l_curvertexDist;
												l_geoDist = l_curgeoDist;
												l_faceDist = l_curfaceDist;
												l_selectedGeo = l_geo;
												l_selectedSubmesh = l_submesh;
												l_selectedFace = l_face;
												//l_pSelectedVertex = p_pSubmesh->GetPoint( l_face->GetVertexIndex( 1 ) );
											}

											if ( ( l_curvertexDist = p_ray->Intersects( Vertex::GetPosition( l_it->GetPoint( l_face->GetVertexIndex( 2 ) ), l_ptCoords ) ) ) < l_vertexDist )
											{
												l_vertexDist = l_curvertexDist;
												l_geoDist = l_curgeoDist;
												l_faceDist = l_curfaceDist;
												l_selectedGeo = l_geo;
												l_selectedSubmesh = l_submesh;
												l_selectedFace = l_face;
												//l_pSelectedVertex = p_pSubmesh->GetPoint( l_face->GetVertexIndex( 2 ) );
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		if ( p_geo )
		{
			p_geo = l_selectedGeo;
		}

		if ( p_submesh )
		{
			p_submesh = l_selectedSubmesh;
		}

		if ( p_face )
		{
			p_face = & l_selectedFace;
		}

		if ( p_vertex )
		{
			p_vertex = l_pSelectedVertex;
		}
	}

	void Scene::AddOverlay( OverlaySPtr p_pOverlay )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		m_arrayOverlays.push_back( p_pOverlay );
	}

	uint32_t Scene::GetVertexCount()const
	{
		uint32_t l_return = 0;

		for ( auto && l_pair: m_addedPrimitives )
		{
			l_return += l_pair.second->GetMesh()->GetVertexCount();
		}

		return l_return;
	}

	uint32_t Scene::GetFaceCount()const
	{
		uint32_t l_return = 0;

		for ( auto && l_pair: m_addedPrimitives )
		{
			l_return += l_pair.second->GetMesh()->GetFaceCount();
		}

		return l_return;
	}

	void Scene::DoDeleteToDelete()
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();

		for ( auto l_geometry: m_arrayPrimitivesToDelete )
		{
			l_geometry->Detach();
		}

		m_arrayBillboardsToDelete.clear();
		m_arrayLightsToDelete.clear();
		m_arrayPrimitivesToDelete.clear();
		m_arrayNodesToDelete.clear();
		m_arrayCamerasToDelete.clear();
	}

	void Scene::DoUpdateAnimations()
	{
		for ( auto && l_pair: m_addedGroups )
		{
			l_pair.second->Update();
		}
	}

	void Scene::DoSortByAlpha()
	{
		m_mapSubmeshesNoAlpha.clear();
		m_arraySubmeshesNoAlpha.clear();
		m_mapSubmeshesAlpha.clear();
		m_arraySubmeshesAlpha.clear();

		for ( auto && l_primitive: m_addedPrimitives )
		{
			MeshSPtr l_pMesh = l_primitive.second->GetMesh();
			SceneNodeSPtr l_pNode = l_primitive.second->GetParent();

			if ( l_pMesh )
			{
				for ( auto && l_submesh: *l_pMesh )
				{
					MaterialSPtr l_pMaterial( l_primitive.second->GetMaterial( l_submesh ) );
					stRENDER_NODE l_renderNode = { l_pNode, l_primitive.second, l_submesh, l_pMaterial };

					if ( l_pMaterial->HasAlphaBlending() )
					{
						m_arraySubmeshesAlpha.push_back( l_renderNode );
						SubmeshNodesByMaterialMapIt l_itMap = m_mapSubmeshesAlpha.find( l_pMaterial );

						if ( l_itMap == m_mapSubmeshesAlpha.end() )
						{
							m_mapSubmeshesAlpha.insert( std::make_pair( l_pMaterial, SubmeshNodesMap() ) );
							l_itMap = m_mapSubmeshesAlpha.find( l_pMaterial );
						}

						SubmeshNodesMapIt l_itSubmesh = l_itMap->second.find( l_submesh );

						if ( l_itSubmesh == l_itMap->second.end() )
						{
							l_itMap->second.insert( std::make_pair( l_submesh, RenderNodeArray() ) );
							l_itSubmesh = l_itMap->second.find( l_submesh );
						}

						l_itSubmesh->second.push_back( l_renderNode );
					}
					else
					{
						m_arraySubmeshesNoAlpha.push_back( l_renderNode );
						SubmeshNodesByMaterialMapIt l_itMap = m_mapSubmeshesNoAlpha.find( l_pMaterial );

						if ( l_itMap == m_mapSubmeshesNoAlpha.end() )
						{
							m_mapSubmeshesNoAlpha.insert( std::make_pair( l_pMaterial, SubmeshNodesMap() ) );
							l_itMap = m_mapSubmeshesNoAlpha.find( l_pMaterial );
						}

						SubmeshNodesMapIt l_itSubmesh = l_itMap->second.find( l_submesh );

						if ( l_itSubmesh == l_itMap->second.end() )
						{
							l_itMap->second.insert( std::make_pair( l_submesh, RenderNodeArray() ) );
							l_itSubmesh = l_itMap->second.find( l_submesh );
						}

						l_itSubmesh->second.push_back( l_renderNode );
					}
				}
			}
		}
	}

	void Scene::DoRenderSubmeshesNonInstanced( Camera const & p_camera, Pipeline & p_pipeline, eTOPOLOGY p_eTopology, RenderNodeArrayConstIt p_begin, RenderNodeArrayConstIt p_end )
	{
		RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();

		for ( RenderNodeArrayConstIt l_itNodes = p_begin; l_itNodes != p_end; ++l_itNodes )
		{
			if ( l_itNodes->m_pNode && l_itNodes->m_pNode->IsDisplayable() && l_itNodes->m_pNode->IsVisible() )
			{
				if ( p_camera.IsVisible( l_itNodes->m_pSubmesh->GetParent()->GetCollisionBox(), l_itNodes->m_pNode->GetDerivedTransformationMatrix() ) )
				{
					if ( l_pRenderSystem->HasInstancing() && l_itNodes->m_pSubmesh->GetRefCount( l_itNodes->m_pMaterial ) > 1 )
					{
						DoRenderSubmeshInstancedSingle( p_pipeline, *l_itNodes, p_eTopology );
					}
					else
					{
						DoRenderSubmeshNonInstanced( p_pipeline, *l_itNodes, p_eTopology );
					}
				}
			}
		}
	}

	void Scene::DoRenderSubmeshesInstanced( Camera const & p_camera, Pipeline & p_pipeline, eTOPOLOGY p_eTopology, SubmeshNodesByMaterialMapConstIt p_begin, SubmeshNodesByMaterialMapConstIt p_end )
	{
		RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();

		for ( SubmeshNodesByMaterialMapConstIt l_itNodes = p_begin; l_itNodes != p_end; ++l_itNodes )
		{
			MaterialSPtr l_pMaterial = l_itNodes->first;

			for ( SubmeshNodesMapConstIt l_itSubmeshes = l_itNodes->second.begin(); l_itSubmeshes != l_itNodes->second.end(); ++l_itSubmeshes )
			{
				SubmeshSPtr l_pSubmesh = l_itSubmeshes->first;

				if ( l_pRenderSystem->HasInstancing() && l_pSubmesh->GetRefCount( l_pMaterial ) > 1 )
				{
					DoRenderSubmeshInstancedMultiple( p_pipeline, l_itSubmeshes->second, p_eTopology );
				}
				else
				{
					DoRenderSubmeshNonInstanced( p_pipeline, l_itSubmeshes->second[0], p_eTopology );
				}
			}
		}
	}

	void Scene::DoRenderAlphaSortedSubmeshes( Pipeline & p_pipeline, eTOPOLOGY p_eTopology, RenderNodeByDistanceMMapConstIt p_begin, RenderNodeByDistanceMMapConstIt p_end )
	{
		RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();

		for ( RenderNodeByDistanceMMapConstIt l_it = p_begin; l_it != p_end; ++l_it )
		{
			stRENDER_NODE const & l_renderNode = l_it->second;
			SubmeshSPtr l_pSubmesh = l_renderNode.m_pSubmesh;
			SceneNodeSPtr l_pNode = l_renderNode.m_pNode;

			if ( l_pRenderSystem->HasInstancing() && l_pSubmesh->GetRefCount( l_renderNode.m_pMaterial ) > 1 )
			{
				DoRenderSubmeshInstancedSingle( p_pipeline, l_renderNode, p_eTopology );
			}
			else
			{
				DoRenderSubmeshNonInstanced( p_pipeline, l_renderNode, p_eTopology );
			}
		}
	}

	void Scene::DoResortAlpha( Camera const & p_camera, RenderNodeArrayIt p_begin, RenderNodeArrayIt p_end, RenderNodeByDistanceMMap & p_map, int p_sign )
	{
		p_map.clear();

		for ( RenderNodeArrayIt l_it = p_begin; l_it != p_end; ++l_it )
		{
			if ( l_it->m_pNode && l_it->m_pNode->IsDisplayable() && l_it->m_pNode->IsVisible() )
			{
				if ( p_camera.IsVisible( l_it->m_pSubmesh->GetParent()->GetCollisionBox(), l_it->m_pNode->GetDerivedTransformationMatrix() ) )
				{
					Matrix4x4r l_mtxMeshGlobal = l_it->m_pNode->GetDerivedTransformationMatrix().get_inverse().transpose();
					Point3r l_position = l_it->m_pNode->GetDerivedPosition();
					Point3r l_ptCameraLocal = l_mtxMeshGlobal * l_position;
					l_it->m_pSubmesh->SortFaces( l_ptCameraLocal );
					l_ptCameraLocal -= l_it->m_pSubmesh->GetCubeBox().GetCenter();
					p_map.insert( std::make_pair( p_sign * point::distance_squared( l_ptCameraLocal ), *l_it ) );
				}
			}
		}
	}

	void Scene::DoRenderSubmeshInstancedMultiple( Pipeline & p_pipeline, RenderNodeArray const & p_nodes, eTOPOLOGY p_eTopology )
	{
		SubmeshSPtr l_pSubmesh = p_nodes[0].m_pSubmesh;
		SceneNodeSPtr l_pNode = p_nodes[0].m_pNode;
		MaterialSPtr l_pMaterial = p_nodes[0].m_pMaterial;

		if ( l_pSubmesh->GetGeometryBuffers()->HasMatrixBuffer() )
		{
			MatrixBuffer & l_mtxBuffer = l_pSubmesh->GetGeometryBuffers()->GetMatrixBuffer();
			uint32_t l_uiSize = l_mtxBuffer.GetSize();
			real * l_pBuffer = l_mtxBuffer.data();
			uint32_t l_count = l_pSubmesh->GetRefCount( l_pMaterial );

			for ( auto && l_it: p_nodes )
			{
				if ( ( l_pSubmesh->GetProgramFlags() & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
				{
					std::memcpy( l_pBuffer, l_it.m_pNode->GetDerivedTransformationMatrix().get_inverse().const_ptr(), 16 * sizeof( real ) );
				}
				else
				{
					std::memcpy( l_pBuffer, l_it.m_pNode->GetDerivedTransformationMatrix().const_ptr(), 16 * sizeof( real ) );
				}

				l_pBuffer += 16;
			}

			DoRenderSubmesh( p_pipeline, p_nodes[0], p_eTopology );
		}
	}

	void Scene::DoRenderSubmeshInstancedSingle( Pipeline & p_pipeline, stRENDER_NODE const & p_node, eTOPOLOGY p_eTopology )
	{
		SubmeshSPtr l_pSubmesh = p_node.m_pSubmesh;
		SceneNodeSPtr l_pNode = p_node.m_pNode;

		if ( l_pSubmesh->GetGeometryBuffers()->HasMatrixBuffer() )
		{
			MatrixBuffer & l_mtxBuffer = l_pSubmesh->GetGeometryBuffers()->GetMatrixBuffer();
			uint32_t l_uiSize = l_mtxBuffer.GetSize();
			real * l_pBuffer = l_mtxBuffer.data();

			if ( ( l_pSubmesh->GetProgramFlags() & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
			{
				std::memcpy( l_pBuffer, l_pNode->GetDerivedTransformationMatrix().get_inverse().const_ptr(), 16 * sizeof( real ) );
			}
			else
			{
				std::memcpy( l_pBuffer, l_pNode->GetDerivedTransformationMatrix().const_ptr(), 16 * sizeof( real ) );
			}
		}

		DoRenderSubmesh( p_pipeline, p_node, p_eTopology );
	}

	void Scene::DoRenderSubmeshNonInstanced( Pipeline & p_pipeline, stRENDER_NODE const & p_node, eTOPOLOGY p_eTopology )
	{
		SubmeshSPtr l_pSubmesh = p_node.m_pSubmesh;
		SceneNodeSPtr l_pNode = p_node.m_pNode;
		p_pipeline.PushMatrix();

		if ( ( l_pSubmesh->GetProgramFlags() & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
		{
			p_pipeline.MultMatrix( l_pNode->GetDerivedTransformationMatrix().get_inverse() );
		}
		else
		{
			p_pipeline.MultMatrix( l_pNode->GetDerivedTransformationMatrix() );
		}

		DoRenderSubmesh( p_pipeline, p_node, p_eTopology );
		p_pipeline.PopMatrix();
	}

	void Scene::DoRenderSubmesh( Pipeline & p_pipeline, stRENDER_NODE const & p_node, eTOPOLOGY p_eTopology )
	{
		ShaderProgramBaseSPtr l_pProgram;
		uint32_t l_uiCount = 0;
		uint32_t l_uiSize = p_node.m_pMaterial->GetPassCount();

		for ( auto l_pass: *p_node.m_pMaterial )
		{
			if ( l_pass->HasAutomaticShader() )
			{
				uint32_t l_uiProgramFlags = p_node.m_pSubmesh->GetProgramFlags();

				if ( p_node.m_pSubmesh->GetRefCount( p_node.m_pMaterial ) > 1 )
				{
					l_uiProgramFlags |= ePROGRAM_FLAG_INSTANCIATION;
				}

				if ( m_pEngine->GetRenderSystem()->GetCurrentContext()->IsDeferredShadingSet() )
				{
					l_uiProgramFlags |= ePROGRAM_FLAG_DEFERRED;
				}

				l_pProgram = m_pEngine->GetShaderManager().GetAutomaticProgram( l_pass->GetTextureFlags(), l_uiProgramFlags );
				l_pass->BindToAutomaticProgram( l_pProgram );
			}
			else
			{
				l_pProgram = l_pass->GetShader< ShaderProgramBase >();
			}

			FrameVariableBufferSPtr l_frameBuffer = l_pass->GetMatrixBuffer();

			if ( l_frameBuffer )
			{
				p_pipeline.ApplyMatrices( *l_frameBuffer, 0xFFFFFFFFFFFFFFFF );
				DoApplySkeleton( *l_frameBuffer, p_node.m_pGeometry->GetAnimatedObject() );
			}

			FrameVariableBufferSPtr l_sceneBuffer = l_pass->GetSceneBuffer();
			DoBindLights( *l_pProgram, *l_sceneBuffer );
			DoBindCamera( *l_sceneBuffer );
			l_pass->Render( l_uiCount++, l_uiSize );
			p_node.m_pSubmesh->Draw( p_eTopology, *l_pass );
			l_pass->EndRender();
			DoUnbindLights( *l_pProgram, *l_sceneBuffer );
		}
	}

	void Scene::DoApplySkeleton( FrameVariableBuffer const & p_matrixBuffer, AnimatedObjectSPtr p_object )
	{
		if ( p_object )
		{
			SkeletonSPtr l_pSkeleton = p_object->GetSkeleton();

			if ( l_pSkeleton )
			{
				int i = 0;
				Matrix4x4rFrameVariableSPtr l_pVariable;
				p_matrixBuffer.GetVariable( Pipeline::MtxBones, l_pVariable );

				if ( l_pVariable )
				{
					Matrix4x4r l_mtxFinal;

					for ( auto && l_it = p_object->AnimationsBegin(); l_it != p_object->AnimationsEnd(); ++l_it )
					{
						l_mtxFinal.set_identity();

						for ( BonePtrArrayIt l_itBones = l_pSkeleton->Begin(); l_itBones != l_pSkeleton->End(); ++l_itBones )
						{
							MovingObjectBaseSPtr l_pMoving = l_it->second->GetMovingObject( *l_itBones );

							if ( l_pMoving )
							{
								l_mtxFinal *= l_pMoving->GetFinalTransformation();
							}
						}

						l_pVariable->SetValue( l_mtxFinal.const_ptr(), i++ );
					}
				}
			}
		}
	}

	void Scene::DoRenderBillboards( Pipeline & p_pipeline, BillboardListStrMapIt p_itBegin, BillboardListStrMapIt p_itEnd )
	{
		RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();

		for ( BillboardListStrMapIt l_it = p_itBegin; l_it != p_itEnd; ++l_it )
		{
			p_pipeline.PushMatrix();
			p_pipeline.MultMatrix( l_it->second->GetParent()->GetDerivedTransformationMatrix() );
			l_it->second->Render();
			p_pipeline.PopMatrix();
		}
	}

	void Scene::DoBindLight( LightSPtr p_light, int p_index, ShaderProgramBase & p_program )
	{
		int l_offset = 0;
		ApplyLightColourIntensity( p_light->GetAmbient(), p_index, l_offset, *m_pLightsData );
		ApplyLightColourIntensity( p_light->GetDiffuse(), p_index, l_offset, *m_pLightsData );
		ApplyLightColourIntensity( p_light->GetSpecular(), p_index, l_offset, *m_pLightsData );

		if ( p_light->GetLightType() == eLIGHT_TYPE_DIRECTIONAL )
		{
			ApplyLightComponent( p_light->GetDirectionalLight()->GetPositionType(), p_index, l_offset, *m_pLightsData );
			l_offset += 4;
		}
		else if ( p_light->GetLightType() == eLIGHT_TYPE_POINT )
		{
			PointLightSPtr l_light = p_light->GetPointLight();
			ApplyLightComponent( l_light->GetPositionType(), p_index, l_offset, *m_pLightsData );
			l_offset += 4; // To match the matrix for spot lights
			ApplyLightComponent( l_light->GetAttenuation(), p_index, l_offset, *m_pLightsData );
		}
		else
		{
			SpotLightSPtr l_light = p_light->GetSpotLight();
			ApplyLightComponent( l_light->GetPositionType(), p_index, l_offset, *m_pLightsData );
			Matrix4x4r l_orientation;
			p_light->GetParent()->GetOrientation().ToRotationMatrix( l_orientation );
			ApplyLightComponent( l_orientation, p_index, l_offset, *m_pLightsData );
			ApplyLightComponent( l_light->GetAttenuation(), p_index, l_offset, *m_pLightsData );
			ApplyLightComponent( l_light->GetExponent(), l_light->GetCutOff(), p_index, l_offset, *m_pLightsData );
		}
	}

	void Scene::DoUnbindLight( LightSPtr p_light, int p_index, ShaderProgramBase & p_program )
	{
	}

	void Scene::DoBindLights( ShaderProgramBase & p_program, FrameVariableBuffer & p_sceneBuffer )
	{
		RenderSystem * l_renderSystem = m_pEngine->GetRenderSystem();
		l_renderSystem->RenderAmbientLight( GetAmbientLight(), p_sceneBuffer );

		OneTextureFrameVariableSPtr l_lights = p_program.FindFrameVariable( ShaderProgramBase::Lights, eSHADER_TYPE_PIXEL );

		if ( l_lights )
		{
			l_lights->SetValue( m_pLightsTexture->GetTexture().get() );
		}

		OneIntFrameVariableSPtr l_lightsCount;
		p_sceneBuffer.GetVariable< int >( ShaderProgramBase::LightsCount, l_lightsCount );

		if ( l_lightsCount )
		{
			int l_index = 0;

			for ( auto && l_it: m_addedLights )
			{
				DoBindLight( l_it.second, l_index, p_program );
				l_lightsCount->GetValue( 0 )++;
			}
		}
		else
		{
			for ( auto && l_it: m_addedLights )
			{
				l_it.second->Render();
			}
		}

		m_pLightsTexture->Bind();

		if ( m_bLightsChanged )
		{
			m_pLightsTexture->UploadImage( false );
			m_bLightsChanged = false;
		}
	}

	void Scene::DoUnbindLights( ShaderProgramBase & p_program, FrameVariableBuffer & p_sceneBuffer )
	{
		m_pLightsTexture->Unbind();

		OneIntFrameVariableSPtr l_lightsCount;
		p_sceneBuffer.GetVariable< int >( ShaderProgramBase::LightsCount, l_lightsCount );

		if ( l_lightsCount )
		{
			int l_index = 0;

			for ( auto && l_it: m_addedLights )
			{
				DoUnbindLight( l_it.second, l_index, p_program );
				l_lightsCount->GetValue( 0 )--;
			}
		}
		else
		{
			for ( auto && l_it: m_addedLights )
			{
				l_it.second->EndRender();
			}
		}
	}

	void Scene::DoBindCamera( FrameVariableBuffer & p_sceneBuffer )
	{
		RenderSystem * l_renderSystem = m_pEngine->GetRenderSystem();
		Camera * l_pCamera = l_renderSystem->GetCurrentCamera();

		if ( l_pCamera )
		{
			Point3r l_position = l_pCamera->GetParent()->GetDerivedPosition();
			Point3rFrameVariableSPtr l_cameraPos;
			p_sceneBuffer.GetVariable( ShaderProgramBase::CameraPos, l_cameraPos );

			if ( l_cameraPos )
			{
				if ( l_renderSystem->GetMainContext()->IsDeferredShadingSet() )
				{
					//m_pCameraPos->SetValue( Castor::MtxUtils::mult( m_pRenderSystem->GetPipeline()->GetMatrix( eMTXMODE_VIEW ), l_position ) );
					l_cameraPos->SetValue( l_position );
				}
				else
				{
					l_cameraPos->SetValue( l_position );
				}
			}
		}
	}
}
