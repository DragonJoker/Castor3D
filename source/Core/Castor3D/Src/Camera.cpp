#include "Camera.hpp"

#include "Engine.hpp"
#include "Pipeline.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include "SceneNode.hpp"
#include "Viewport.hpp"

#include <TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	bool Camera::TextLoader::operator()( Camera const & p_camera, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "\tcamera \"" ) + p_camera.GetName() + cuT( "\"\n\t{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = MovableObject::TextLoader()( p_camera, p_file );
		}

		if ( l_return )
		{
			l_return = Viewport::TextLoader()( * p_camera.GetViewport(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	Camera::BinaryParser::BinaryParser( Path const & p_path )
		:	MovableObject::BinaryParser( p_path )
	{
	}

	bool Camera::BinaryParser::Fill( Camera const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_CAMERA );

		if ( l_return )
		{
			l_return = MovableObject::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( uint8_t( p_obj.GetPrimitiveType() ), eCHUNK_TYPE_CAMERA_PRIMITIVES, l_chunk );
		}

		if ( l_return )
		{
			l_return = Viewport::BinaryParser( m_path ).Fill( *p_obj.GetViewport(), l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool Camera::BinaryParser::Parse( Camera & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		uint8_t l_type;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_CAMERA_PRIMITIVES:
					l_return = DoParseChunk( l_type, l_chunk );

					if ( l_return )
					{
						p_obj.SetPrimitiveType( eTOPOLOGY( l_type ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT:
					l_return = Viewport::BinaryParser( m_path ).Parse( *p_obj.GetViewport(), l_chunk );
					break;

				default:
					l_return = MovableObject::BinaryParser( m_path ).Parse( p_obj, l_chunk );
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

	Camera::Camera( SceneSPtr p_pScene, String const & p_strName, SceneNodeSPtr p_pNode, ViewportSPtr p_pViewport, eTOPOLOGY p_ePrimitiveType )
		: MovableObject( p_pScene, p_pNode, p_strName, eMOVABLE_TYPE_CAMERA )
		, m_pEngine( p_pScene->GetEngine() )
		, m_pViewport( std::make_shared< Viewport >( *p_pViewport ) )
		, m_ePrimitiveType( eTOPOLOGY_TRIANGLES )
	{
	}

	Camera::Camera( SceneSPtr p_pScene, String const & p_strName, SceneNodeSPtr p_pNode, Size const & p_size, eVIEWPORT_TYPE p_eType, eTOPOLOGY p_ePrimitiveType )
		: Camera( p_pScene, p_strName, p_pNode, std::make_shared< Viewport >( p_pScene->GetEngine(), p_size, p_eType ), p_ePrimitiveType )
	{
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
		bool l_bModified = m_pViewport->Render();
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			if ( l_bModified || l_node->IsModified() )
			{
				Point3r l_position = l_node->GetDerivedPosition();
				//Point3r l_u( 0, 1, 0 );
				//Point3r l_n( 0, 0, 1 );
				//point::normalise( l_node->GetDerivedOrientation().Transform( l_u, l_u ) );
				//point::normalise( l_node->GetDerivedOrientation().Transform( l_n, l_n ) );
				//Point3r l_v = l_n ^ l_u;
				//Matrix4x4r l_rotate( 1 );
				//l_rotate[0][0] = l_u[0]; l_rotate[0][1] = l_u[1]; l_rotate[0][2] = l_u[2]; l_rotate[0][3] = 0.0f;
				//l_rotate[1][0] = l_v[0]; l_rotate[1][1] = l_v[1]; l_rotate[1][2] = l_v[2]; l_rotate[1][3] = 0.0f;
				//l_rotate[2][0] = l_n[0]; l_rotate[2][1] = l_n[1]; l_rotate[2][2] = l_n[2]; l_rotate[2][3] = 0.0f;
				//Matrix4x4r l_translate( 1 );
				//matrix::set_translate( l_translate, l_position );
				//m_view = l_rotate * l_translate;
				matrix::set_transform( m_view, l_position, Point3r( 1, 1, 1 ), l_node->GetDerivedOrientation() );

				// Express frustum in view coordinates

				for ( int i = 0; i < eFRUSTUM_PLANE_COUNT; ++i )
				{
					m_planes[i].Set( m_view * m_pViewport->GetFrustumPlane( eFRUSTUM_PLANE( i ) ).GetNormal(), l_position );
				}
			}

			m_pEngine->GetRenderSystem()->GetPipeline().SetViewMatrix( m_view );
		}

		m_pEngine->GetRenderSystem()->SetCurrentCamera( this );
	}

	void Camera::EndRender()
	{
		m_pEngine->GetRenderSystem()->SetCurrentCamera( NULL );
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
		bool l_return = false;

		return l_return;
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
		bool l_return = true;
		//Point3r l_ptCorners[8];
		//l_ptCorners[0] = p_box.GetMin();
		//l_ptCorners[1] = p_box.GetMax();

		//// Express object box in world coordinates
		//l_ptCorners[2] = p_transformations * Point3r( l_ptCorners[0][0], l_ptCorners[1][1], l_ptCorners[0][2] );
		//l_ptCorners[3] = p_transformations * Point3r( l_ptCorners[1][0], l_ptCorners[1][1], l_ptCorners[0][2] );
		//l_ptCorners[4] = p_transformations * Point3r( l_ptCorners[1][0], l_ptCorners[0][1], l_ptCorners[0][2] );
		//l_ptCorners[5] = p_transformations * Point3r( l_ptCorners[0][0], l_ptCorners[1][1], l_ptCorners[1][2] );
		//l_ptCorners[6] = p_transformations * Point3r( l_ptCorners[0][0], l_ptCorners[0][1], l_ptCorners[1][2] );
		//l_ptCorners[7] = p_transformations * Point3r( l_ptCorners[1][0], l_ptCorners[0][1], l_ptCorners[1][2] );
		//l_ptCorners[0] = p_transformations * l_ptCorners[0];
		//l_ptCorners[1] = p_transformations * l_ptCorners[1];

		//// Retrieve axis aligned box boundaries
		//Point3r l_ptMin( l_ptCorners[0] );
		//Point3r l_ptMax( l_ptCorners[1] );

		//for( int j = 0; j < 8; ++j )
		//{
		//	l_ptMin[0] = std::min( l_ptCorners[j][0], l_ptMin[0] );
		//	l_ptMin[1] = std::min( l_ptCorners[j][1], l_ptMin[1] );
		//	l_ptMin[2] = std::min( l_ptCorners[j][2], l_ptMin[2] );

		//	l_ptMax[0] = std::max( l_ptCorners[j][0], l_ptMax[0] );
		//	l_ptMax[1] = std::max( l_ptCorners[j][1], l_ptMax[1] );
		//	l_ptMax[2] = std::max( l_ptCorners[j][2], l_ptMax[2] );
		//}

		//// Test positive vertex from the axis aligned box to be inside the frustum view.
		//for( int i = 0; i < eFRUSTUM_PLANE_COUNT && l_return; ++i )
		//{
		//	if( m_planes[i].Distance( GetVertexP( l_ptMin, l_ptMax, m_planes[i].GetNormal() ) ) < 0 )
		//	{
		//		l_return = false;
		//	}
		//}

		return l_return;
	}

	bool Camera::IsVisible( Point3r const & p_point )const
	{
		bool l_return = true;

		for ( int i = 0; i < 6 && l_return; i++ )
		{
			l_return = m_planes[i].Distance( p_point ) >= 0;
		}

		return l_return;
	}
}
