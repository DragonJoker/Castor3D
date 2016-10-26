#include "Camera.hpp"

#include "Engine.hpp"

#include "Scene.hpp"
#include "SceneNode.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Viewport.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"

#include <Graphics/CubeBox.hpp>
#include <Graphics/SphereBox.hpp>
#include <Math/TransformationMatrix.hpp>

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
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "camera \"" ) + p_camera.GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< Camera >::CheckError( l_return, "Camera name" );

		if ( l_return )
		{
			l_return = MovableObject::TextWriter{ m_tabs }( p_camera, p_file );
		}

		if ( l_return )
		{
			l_return = Viewport::TextWriter( m_tabs + cuT( "\t" ) )( p_camera.GetViewport(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	Camera::Camera( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node, Viewport && p_viewport )
		: MovableObject{ p_name, p_scene, MovableType::Camera, p_node }
		, m_viewport{ std::move( p_viewport ) }
		, m_frustum{ m_viewport }
	{
		if ( p_scene.GetEngine()->GetRenderSystem()->GetCurrentContext() )
		{
			m_viewport.Initialise();
		}
		else
		{
			p_scene.GetEngine()->PostEvent( MakeInitialiseEvent( m_viewport ) );
		}

		if ( p_node )
		{
			m_notifyIndex = p_node->RegisterObject( std::bind( &Camera::OnNodeChanged, this, std::placeholders::_1 ) );
			OnNodeChanged( *p_node );
		}
	}

	Camera::Camera( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node )
		: Camera{ p_name, p_scene, p_node, Viewport{ *p_scene.GetEngine() } }
	{
	}

	Camera::~Camera()
	{
	}

	void Camera::AttachTo( SceneNodeSPtr p_node )
	{
		MovableObject::AttachTo( p_node );

		if ( p_node )
		{
			m_notifyIndex = p_node->RegisterObject( std::bind( &Camera::OnNodeChanged, this, std::placeholders::_1 ) );
			OnNodeChanged( *p_node );
		}
	}

	void Camera::ResetOrientation()
	{
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			l_node->SetOrientation( Quaternion::identity() );
			OnNodeChanged( *l_node );
		}
	}

	void Camera::ResetPosition()
	{
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			l_node->SetPosition( Point3r( 0, 0, 0 ) );
			OnNodeChanged( *l_node );
		}
	}

	void Camera::Update()
	{
		bool l_modified = m_viewport.Update();
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			if ( l_modified || m_nodeChanged )
			{
				l_node->GetTransformationMatrix();
				auto l_position = l_node->GetDerivedPosition();
				auto const & l_orientation = l_node->GetDerivedOrientation();
				Point3r l_right{ 1.0_r, 0.0_r, 0.0_r };
				Point3r l_up{ 0.0_r, 1.0_r, 0.0_r };
				Point3r l_front{ 0.0_r, 0.0_r, 1.0_r };
				l_orientation.transform( l_right, l_right );
				l_orientation.transform( l_up, l_up );
				l_orientation.transform( l_front, l_front );

				m_frustum.Update( l_position, l_right, l_up, l_front );

				// Update view matrix
				matrix::look_at( m_view, l_position, l_position + l_front, l_up );
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

	void Camera::FillShader( FrameVariableBuffer const & p_sceneBuffer )const
	{
		Point3r l_position = GetParent()->GetDerivedPosition();
		Point3rFrameVariableSPtr l_cameraPos;
		p_sceneBuffer.GetVariable( ShaderProgram::CameraPos, l_cameraPos );

		if ( l_cameraPos )
		{
			l_cameraPos->SetValue( l_position );
		}
	}

	void Camera::OnNodeChanged( SceneNode const & p_node )
	{
		m_nodeChanged = true;
		m_signalChanged( *this );
	}
}
