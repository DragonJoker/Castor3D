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
			l_return = Viewport::TextLoader()( p_camera.GetViewport(), p_file );
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
			l_return = Viewport::BinaryParser( m_path ).Fill( p_obj.GetViewport(), l_chunk );
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
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_VIEWPORT:
					l_return = Viewport::BinaryParser( m_path ).Parse( p_obj.GetViewport(), l_chunk );
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
			Point3r l_return( p_min );

			if ( p_normal[0] >= 0 )
			{
				l_return[0] = p_max[0];
			}

			if ( p_normal[1] >= 0 )
			{
				l_return[1] = p_max[1];
			}

			if ( p_normal[2] >= 0 )
			{
				l_return[2] = p_max[2];
			}

			return l_return;
		}

		Point3r GetVertexN( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
		{
			Point3r l_return( p_max );

			if ( p_normal[0] >= 0 )
			{
				l_return[0] = p_min[0];
			}

			if ( p_normal[1] >= 0 )
			{
				l_return[1] = p_min[1];
			}

			if ( p_normal[2] >= 0 )
			{
				l_return[2] = p_min[2];
			}

			return l_return;
		}
	}

	//*************************************************************************************************

	Camera::Camera( SceneSPtr p_scene, String const & p_name, SceneNodeSPtr p_node, Viewport const & p_viewport )
		: MovableObject( p_scene, p_node, p_name, eMOVABLE_TYPE_CAMERA )
		, m_viewport( p_viewport )
	{
	}

	Camera::Camera( SceneSPtr p_scene, String const & p_name, SceneNodeSPtr p_node )
		: MovableObject( p_scene, p_node, p_name, eMOVABLE_TYPE_CAMERA )
		, m_viewport( Viewport::Ortho( *p_scene->GetOwner(), 0, 1, 0, 1, 0, 1 ) )
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
			l_node->SetOrientation( Quaternion::identity() );
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
		bool l_modified = m_viewport.Render( GetOwner()->GetRenderSystem()->GetPipeline() );
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			if ( l_modified || l_node->IsModified() )
			{
				m_view = l_node->GetDerivedTransformationMatrix();
				//matrix::set_transform( m_view, l_node->GetDerivedPosition() * Point3r( 1, 1, 1 ), Point3r( 1, 1, 1 ), l_node->GetDerivedOrientation() );

				// Express frustum in view coordinates
				for ( int i = 0; i < eFRUSTUM_PLANE_COUNT; ++i )
				{
					m_planes[i].Set( m_view * m_viewport.GetFrustumPlane( eFRUSTUM_PLANE( i ) ).GetNormal(), l_node->GetDerivedPosition() );
				}
			}

			GetOwner()->GetRenderSystem()->GetPipeline().SetViewMatrix( m_view );
		}

		GetOwner()->GetRenderSystem()->SetCurrentCamera( this );
	}

	void Camera::EndRender()
	{
		GetOwner()->GetRenderSystem()->SetCurrentCamera( NULL );
	}

	void Camera::Resize( uint32_t p_width, uint32_t p_height )
	{
		Resize( Size( p_width, p_height ) );
	}

	void Camera::Resize( Size const & p_size )
	{
		m_viewport.SetSize( p_size );
	}

	bool Camera::Select( SceneSPtr p_scene, eSELECTION_MODE p_mode, int p_x, int p_y, stSELECT_RESULT & p_result )
	{
		bool l_return = false;

		return l_return;
	}

	eVIEWPORT_TYPE Camera::GetViewportType()const
	{
		return m_viewport.GetType();
	}

	uint32_t Camera::GetWidth()const
	{
		return m_viewport.GetWidth();
	}

	uint32_t Camera::GetHeight()const
	{
		return m_viewport.GetHeight();
	}

	void Camera::SetViewportType( eVIEWPORT_TYPE val )
	{
		m_viewport.SetType( val );
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
