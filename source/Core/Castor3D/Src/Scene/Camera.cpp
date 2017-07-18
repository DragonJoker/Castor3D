#include "Camera.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Scene/Scene.hpp"

#include <Graphics/CubeBox.hpp>
#include <Graphics/SphereBox.hpp>

using namespace Castor;

namespace Castor3D
{
	Camera::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Camera >{ p_tabs }
	{
	}

	bool Camera::TextWriter::operator()( Camera const & p_camera, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing Camera " ) + p_camera.GetName() );
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "camera \"" ) + p_camera.GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< Camera >::CheckError( result, "Camera name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_camera, p_file );
		}

		if ( result )
		{
			result = Viewport::TextWriter( m_tabs + cuT( "\t" ) )( p_camera.GetViewport(), p_file );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	Camera::Camera( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node, Viewport && p_viewport )
		: MovableObject{ p_name, p_scene, MovableType::eCamera, p_node }
		, m_viewport{ std::move( p_viewport ) }
		, m_frustum{ m_viewport }
	{
		if ( p_scene.GetEngine()->GetRenderSystem()->GetCurrentContext() )
		{
			m_viewport.Initialise();
		}
		else
		{
			p_scene.GetListener().PostEvent( MakeInitialiseEvent( m_viewport ) );
		}

		if ( p_node )
		{
			m_notifyIndex = p_node->onChanged.connect( std::bind( &Camera::OnNodeChanged, this, std::placeholders::_1 ) );
			OnNodeChanged( *p_node );
		}
	}

	Camera::Camera( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node )
		: Camera{ p_name, p_scene, p_node, Viewport{ *p_scene.GetEngine() } }
	{
	}

	Camera::~Camera()
	{
		m_notifyIndex.disconnect();
	}

	void Camera::AttachTo( SceneNodeSPtr p_node )
	{
		MovableObject::AttachTo( p_node );

		if ( p_node )
		{
			m_notifyIndex = p_node->onChanged.connect( std::bind( &Camera::OnNodeChanged, this, std::placeholders::_1 ) );
			OnNodeChanged( *p_node );
		}
	}

	void Camera::ResetOrientation()
	{
		SceneNodeSPtr node = GetParent();

		if ( node )
		{
			node->SetOrientation( Quaternion::identity() );
			OnNodeChanged( *node );
		}
	}

	void Camera::ResetPosition()
	{
		SceneNodeSPtr node = GetParent();

		if ( node )
		{
			node->SetPosition( Point3r( 0, 0, 0 ) );
			OnNodeChanged( *node );
		}
	}

	void Camera::Update()
	{
		bool modified = m_viewport.Update();
		SceneNodeSPtr node = GetParent();

		if ( node )
		{
			if ( modified || m_nodeChanged )
			{
				node->GetTransformationMatrix();
				auto position = node->GetDerivedPosition();
				auto const & orientation = node->GetDerivedOrientation();
				Point3r right{ 1.0_r, 0.0_r, 0.0_r };
				Point3r up{ 0.0_r, 1.0_r, 0.0_r };
				orientation.transform( right, right );
				orientation.transform( up, up );
				Point3r front{ right ^ up };
				up = front ^ right;

				m_frustum.Update( position, right, up, front );

				// Update view matrix
				matrix::look_at( m_view, position, position + front, up );
				m_nodeChanged = false;
			}
		}
	}

	void Camera::Apply()const
	{
		m_viewport.Apply();
	}

	void Camera::Resize( uint32_t p_width, uint32_t p_height )
	{
		Resize( Size( p_width, p_height ) );
	}

	void Camera::Resize( Size const & p_size )
	{
		m_viewport.Resize( p_size );
	}

	ViewportType Camera::GetViewportType()const
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

	void Camera::SetViewportType( ViewportType val )
	{
		m_viewport.UpdateType( val );
	}

	bool Camera::IsVisible( CubeBox const & p_box, Matrix4x4r const & p_transformations )const
	{
		return m_frustum.IsVisible( p_box, p_transformations );
	}

	bool Camera::IsVisible( Castor::SphereBox const & p_box, Castor::Matrix4x4r const & p_transformations )const
	{
		return m_frustum.IsVisible( p_box, p_transformations );
	}

	bool Camera::IsVisible( Point3r const & p_point )const
	{
		return m_frustum.IsVisible( p_point );
	}

	void Camera::OnNodeChanged( SceneNode const & p_node )
	{
		m_nodeChanged = true;
		onChanged( *this );
	}
}
