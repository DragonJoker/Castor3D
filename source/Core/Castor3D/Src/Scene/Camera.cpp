#include "Camera.hpp"

#include "Engine.hpp"

#include "Scene.hpp"
#include "SceneNode.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Viewport.hpp"

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

	Camera::Camera( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node, Viewport const & p_viewport )
		: MovableObject{ p_name, p_scene, eMOVABLE_TYPE_CAMERA, p_node }
		, m_viewport{ p_viewport }
	{
		p_scene.GetEngine()->PostEvent( MakeInitialiseEvent( m_viewport ) );
	}

	Camera::Camera( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node )
		: Camera{ p_name, p_scene, p_node, Viewport{ *p_scene.GetEngine() } }
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

	void Camera::Render( Pipeline & p_pipeline )
	{
		bool l_modified = m_viewport.Render( p_pipeline );
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			if ( l_modified || l_node->IsModified() )
			{
				auto const & l_position = l_node->GetDerivedPosition();
				auto const & l_orientation = l_node->GetDerivedOrientation();
				Point3r l_right, l_up, l_lookat;
				l_orientation.to_axes( l_right, l_up, l_lookat );
				point::normalise( l_right );
				point::normalise( l_up );
				point::normalise( l_lookat );

				// Rotation
				Matrix4x4r l_rotate { 1.0_r };
				auto & l_col0 = l_rotate[0];
				auto & l_col1 = l_rotate[1];
				auto & l_col2 = l_rotate[2];
				l_col0[0] = l_right[0];
				l_col0[1] = l_up[0];
				l_col0[2] = l_lookat[0];
				l_col0[3] = 0.0_r;
				l_col1[0] = l_right[1];
				l_col1[1] = l_up[1];
				l_col1[2] = l_lookat[1];
				l_col1[3] = 0.0_r;
				l_col2[0] = l_right[2];
				l_col2[1] = l_up[2];
				l_col2[2] = l_lookat[2];
				l_col2[3] = 0.0_r;

				// Translation
				Matrix4x4r l_translate { 1.0_r };
				auto & l_col3 = l_translate[3];
				l_col3[0] = -l_position[0];
				l_col3[1] = -l_position[1];
				l_col3[2] = -l_position[2];
				l_col3[3] = 1.0_r;

				m_view = l_rotate * l_translate;
				//m_view = l_node->GetDerivedTransformationMatrix();

				// Express frustum in view coordinates
				for ( int i = 0; i < eFRUSTUM_PLANE_COUNT; ++i )
				{
					m_planes[i].Set( m_view * m_viewport.GetFrustumPlane( eFRUSTUM_PLANE( i ) ).GetNormal(), l_node->GetDerivedPosition() );
				}
			}

			p_pipeline.SetViewMatrix( m_view );
		}

		GetScene()->GetEngine()->GetRenderSystem()->SetCurrentCamera( this );
	}

	void Camera::EndRender()
	{
		GetScene()->GetEngine()->GetRenderSystem()->SetCurrentCamera( nullptr );
	}

	void Camera::Resize( uint32_t p_width, uint32_t p_height )
	{
		Resize( Size( p_width, p_height ) );
	}

	void Camera::Resize( Size const & p_size )
	{
		m_viewport.Resize( p_size );
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
		m_viewport.UpdateType( val );
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
