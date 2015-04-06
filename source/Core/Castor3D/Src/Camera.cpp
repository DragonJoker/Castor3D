#include "Camera.hpp"
#include "CameraRenderer.hpp"
#include "Scene.hpp"
#include "SceneNode.hpp"
#include "Viewport.hpp"
#include "Pipeline.hpp"

#include <TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	bool Camera::TextLoader::operator()( Camera const & p_camera, TextFile & p_file )
	{
		bool l_bReturn = p_file.WriteText( cuT( "\tcamera \"" ) + p_camera.GetName() + cuT( "\"\n\t{\n" ) ) > 0;

		if ( l_bReturn )
		{
			l_bReturn = MovableObject::TextLoader()( p_camera, p_file );
		}

		if ( l_bReturn )
		{
			l_bReturn = Viewport::TextLoader()( * p_camera.GetViewport(), p_file );
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	Camera::BinaryParser::BinaryParser( Path const & p_path )
		:	MovableObject::BinaryParser( p_path )
	{
	}

	bool Camera::BinaryParser::Fill( Camera const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_CAMERA );

		if ( l_bReturn )
		{
			l_bReturn = MovableObject::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( uint8_t( p_obj.GetPrimitiveType() ), eCHUNK_TYPE_CAMERA_PRIMITIVES, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = Viewport::BinaryParser( m_path ).Fill( *p_obj.GetViewport(), l_chunk );
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool Camera::BinaryParser::Parse( Camera & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		uint8_t l_type;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_CAMERA_PRIMITIVES:
					l_bReturn = DoParseChunk( l_type, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetPrimitiveType( eTOPOLOGY( l_type ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT:
					l_bReturn = Viewport::BinaryParser( m_path ).Parse( *p_obj.GetViewport(), l_chunk );
					break;

				default:
					l_bReturn = MovableObject::BinaryParser( m_path ).Parse( p_obj, l_chunk );
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

	namespace
	{
		Point3r GetVertexP( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
		{
			Point3r l_ptReturn( p_min );

			if ( p_normal[0] >= 0 )
			{
				l_ptReturn[0] = p_max[0];
			}

			if ( p_normal[1] >= 0 )
			{
				l_ptReturn[1] = p_max[1];
			}

			if ( p_normal[2] >= 0 )
			{
				l_ptReturn[2] = p_max[2];
			}

			return l_ptReturn;
		}

		Point3r GetVertexN( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
		{
			Point3r l_ptReturn( p_max );

			if ( p_normal[0] >= 0 )
			{
				l_ptReturn[0] = p_min[0];
			}

			if ( p_normal[1] >= 0 )
			{
				l_ptReturn[1] = p_min[1];
			}

			if ( p_normal[2] >= 0 )
			{
				l_ptReturn[2] = p_min[2];
			}

			return l_ptReturn;
		}
	}

	//*************************************************************************************************

	Camera::Camera( SceneSPtr p_pScene, String const & p_strName, Size const & p_size, SceneNodeSPtr p_pNode, eVIEWPORT_TYPE p_eType, eTOPOLOGY p_ePrimitiveType, ePROJECTION_DIRECTION p_eProjectionDirection )
		:	MovableObject( p_pScene, p_pNode, p_strName, eMOVABLE_TYPE_CAMERA )
		,	Renderable< Camera, CameraRenderer >( p_pScene->GetEngine() )
		,	m_pViewport( std::make_shared< Viewport >( p_pScene->GetEngine(), p_size, p_eType ) )
		,	m_ePrimitiveType( p_ePrimitiveType )
		,	m_eProjectionDirection( p_eProjectionDirection )
	{
	}

	Camera::Camera( SceneSPtr p_pScene, String const & p_strName, SceneNodeSPtr p_pNode, ViewportSPtr p_pViewport )
		:	MovableObject( p_pScene, p_pNode, p_strName, eMOVABLE_TYPE_CAMERA )
		,	Renderable< Camera, CameraRenderer >( p_pScene->GetEngine() )
		,	m_pViewport( std::make_shared< Viewport >( *p_pViewport ) )
		,	m_ePrimitiveType( eTOPOLOGY_TRIANGLES )
		,	m_eProjectionDirection( ePROJECTION_DIRECTION_FRONT )
	{
	}

	Camera::Camera( Camera const & p_camera )
		:	MovableObject( p_camera )
		,	Renderable< Camera, CameraRenderer >( p_camera )
		,	m_ePrimitiveType( p_camera.m_ePrimitiveType )
		,	m_eProjectionDirection( p_camera.m_eProjectionDirection )
		,	m_pViewport( std::make_shared< Viewport >( *p_camera.m_pViewport ) )
	{
	}

	Camera::Camera( Camera && p_camera )
		:	MovableObject( std::move( p_camera ) )
		,	Renderable< Camera, CameraRenderer >( std::move( p_camera ) )
		,	m_ePrimitiveType( std::move( p_camera.m_ePrimitiveType ) )
		,	m_eProjectionDirection( std::move( p_camera.m_eProjectionDirection ) )
		,	m_pViewport( std::move( p_camera.m_pViewport ) )
	{
	}

	Camera & Camera::operator =( Camera const & p_camera )
	{
		MovableObject::operator =( p_camera );
		Renderable< Camera, CameraRenderer >::operator =( p_camera );
		m_ePrimitiveType = p_camera.m_ePrimitiveType;
		m_eProjectionDirection = p_camera.m_eProjectionDirection;
		m_pViewport = std::make_shared< Viewport >( *p_camera.m_pViewport );
		return *this;
	}

	Camera & Camera::operator =( Camera && p_camera )
	{
		MovableObject::operator =( std::move( p_camera ) );
		Renderable< Camera, CameraRenderer >::operator =( std::move( p_camera ) );

		if ( this != &p_camera )
		{
			m_ePrimitiveType = std::move( p_camera.m_ePrimitiveType );
			m_eProjectionDirection = std::move( p_camera.m_eProjectionDirection );
			m_pViewport = std::move( p_camera.m_pViewport );
		}

		return *this;
	}

	Camera::~Camera()
	{
	}

	void Camera::ResetOrientation()
	{
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			l_node->SetOrientation( Quaternion::Identity() );
		}
	}

	void Camera::ResetPosition()
	{
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			l_node->SetPosition( Point3r( 0, 0, 0 ) );
		}
	}

	void Camera::Render()
	{
		Pipeline * l_pPipeline = m_pEngine->GetRenderSystem()->GetPipeline();
		bool l_bModified = m_pViewport->Render();
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			Matrix4x4r const & l_mtx = l_node->GetDerivedTransformationMatrix();

			if ( l_bModified || l_node->IsModified() )
			{
				// Express frustum view in world coordinates
				Point3r l_position;
				MtxUtils::get_translate( l_mtx, l_position );

				for ( int i = 0; i < eFRUSTUM_PLANE_COUNT; ++i )
				{
					m_planes[i].Set( l_mtx * m_pViewport->GetFrustumPlane( eFRUSTUM_PLANE( i ) ).GetNormal(), l_position );
				}
			}

			l_pPipeline->MatrixMode( eMTXMODE_VIEW );
			l_pPipeline->LoadIdentity();
			l_pPipeline->PushMatrix();
			l_pPipeline->MultMatrix( l_mtx );
		}

		m_pEngine->GetRenderSystem()->SetCurrentCamera( this );
	}

	void Camera::EndRender()
	{
		m_pEngine->GetRenderSystem()->SetCurrentCamera( NULL );
		m_pEngine->GetRenderSystem()->GetPipeline()->PopMatrix();
	}

	void Camera::Resize( uint32_t p_uiWidth, uint32_t p_uiHeight )
	{
		Resize( Size( p_uiWidth, p_uiHeight ) );
	}

	void Camera::Resize( Size const & p_size )
	{
		m_pViewport->SetSize( p_size );
	}

	bool Camera::Select( SceneSPtr p_pScene, eSELECTION_MODE p_eMode, int p_iX, int p_iY, stSELECT_RESULT & p_stFound )
	{
		bool l_bReturn = false;
		CameraRendererSPtr l_pRenderer = GetRenderer();

		if ( l_pRenderer )
		{
			l_bReturn = l_pRenderer->Select( p_pScene, p_eMode, p_iX, p_iY, p_stFound );
		}

		return l_bReturn;
	}

	eVIEWPORT_TYPE Camera::GetViewportType()const
	{
		return m_pViewport->GetType();
	}

	uint32_t Camera::GetWidth()const
	{
		return m_pViewport->GetWidth();
	}

	uint32_t Camera::GetHeight()const
	{
		return m_pViewport->GetHeight();
	}

	void Camera::SetViewportType( eVIEWPORT_TYPE val )
	{
		m_pViewport->SetType( val );
	}

	bool Camera::IsVisible( CubeBox const & p_box, Matrix4x4r const & p_transformations )const
	{
		bool l_bReturn = true;
		/*
			Point3r l_ptCorners[8];
			l_ptCorners[0] = p_box.GetMin();
			l_ptCorners[1] = p_box.GetMax();

			// Express object box in world coordinates
			l_ptCorners[2] = p_transformations * Point3r( l_ptCorners[0][0], l_ptCorners[1][1], l_ptCorners[0][2] );
			l_ptCorners[3] = p_transformations * Point3r( l_ptCorners[1][0], l_ptCorners[1][1], l_ptCorners[0][2] );
			l_ptCorners[4] = p_transformations * Point3r( l_ptCorners[1][0], l_ptCorners[0][1], l_ptCorners[0][2] );
			l_ptCorners[5] = p_transformations * Point3r( l_ptCorners[0][0], l_ptCorners[1][1], l_ptCorners[1][2] );
			l_ptCorners[6] = p_transformations * Point3r( l_ptCorners[0][0], l_ptCorners[0][1], l_ptCorners[1][2] );
			l_ptCorners[7] = p_transformations * Point3r( l_ptCorners[1][0], l_ptCorners[0][1], l_ptCorners[1][2] );
			l_ptCorners[0] = p_transformations * l_ptCorners[0];
			l_ptCorners[1] = p_transformations * l_ptCorners[1];

			// Retrieve axis aligned box boundaries
			Point3r l_ptMin( l_ptCorners[0] );
			Point3r l_ptMax( l_ptCorners[1] );

			for( int j = 0; j < 8; ++j )
			{
				l_ptMin[0] = std::min( l_ptCorners[j][0], l_ptMin[0] );
				l_ptMin[1] = std::min( l_ptCorners[j][1], l_ptMin[1] );
				l_ptMin[2] = std::min( l_ptCorners[j][2], l_ptMin[2] );

				l_ptMax[0] = std::max( l_ptCorners[j][0], l_ptMax[0] );
				l_ptMax[1] = std::max( l_ptCorners[j][1], l_ptMax[1] );
				l_ptMax[2] = std::max( l_ptCorners[j][2], l_ptMax[2] );
			}

			// Test positive vertex from the axis aligned box to be inside the frustum view.
			for( int i = 0; i < eFRUSTUM_PLANE_COUNT && l_bReturn; ++i )
			{
				if( m_planes[i].Distance( GetVertexP( l_ptMin, l_ptMax, m_planes[i].GetNormal() ) ) < 0 )
				{
					l_bReturn = false;
				}
			}
		*/
		return l_bReturn;
	}

	bool Camera::IsVisible( Point3r const & p_point )const
	{
		bool l_bReturn = true;

		for ( int i = 0; i < 6 && l_bReturn; i++ )
		{
			l_bReturn = m_planes[i].Distance( p_point ) >= 0;
		}

		return l_bReturn;
	}
}
