#include "Camera.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Scene/Scene.hpp"

#include <Graphics/CubeBox.hpp>
#include <Graphics/SphereBox.hpp>

using namespace castor;

namespace castor3d
{
	Camera::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Camera >{ p_tabs }
	{
	}

	bool Camera::TextWriter::operator()( Camera const & p_camera, TextFile & p_file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing Camera " ) + p_camera.getName() );
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "camera \"" ) + p_camera.getName() + cuT( "\"\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< Camera >::checkError( result, "Camera name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_camera, p_file );
		}

		if ( result )
		{
			result = Viewport::TextWriter( m_tabs + cuT( "\t" ) )( p_camera.getViewport(), p_file );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	Camera::Camera( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node, Viewport && p_viewport )
		: MovableObject{ p_name, p_scene, MovableType::eCamera, p_node }
		, m_viewport{ std::move( p_viewport ) }
		, m_frustum{ m_viewport }
	{
		if ( p_scene.getEngine()->getRenderSystem()->getCurrentContext() )
		{
			m_viewport.initialise();
		}
		else
		{
			p_scene.getListener().postEvent( MakeInitialiseEvent( m_viewport ) );
		}

		if ( p_node )
		{
			m_notifyIndex = p_node->onChanged.connect( std::bind( &Camera::onNodeChanged, this, std::placeholders::_1 ) );
			onNodeChanged( *p_node );
		}
	}

	Camera::Camera( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node )
		: Camera{ p_name, p_scene, p_node, Viewport{ *p_scene.getEngine() } }
	{
	}

	Camera::~Camera()
	{
		m_notifyIndex.disconnect();
	}

	void Camera::attachTo( SceneNodeSPtr node )
	{
		if ( node != getParent() )
		{
			MovableObject::attachTo( node );

			if ( node )
			{
				m_notifyIndex = node->onChanged.connect( std::bind( &Camera::onNodeChanged, this, std::placeholders::_1 ) );
				onNodeChanged( *node );
			}
		}
	}

	void Camera::update()
	{
		bool modified = m_viewport.update();
		SceneNodeSPtr node = getParent();

		if ( node )
		{
			if ( modified || m_nodeChanged )
			{
				node->getTransformationMatrix();
				auto position = node->getDerivedPosition();
				auto const & orientation = node->getDerivedOrientation();
				Point3r right{ 1.0_r, 0.0_r, 0.0_r };
				Point3r up{ 0.0_r, 1.0_r, 0.0_r };
				orientation.transform( right, right );
				orientation.transform( up, up );
				Point3r front{ right ^ up };
				up = front ^ right;

				m_frustum.update( position, right, up, front );

				// Update view matrix
				matrix::lookAt( m_view, position, position + front, up );
				m_nodeChanged = false;
			}
		}
	}

	void Camera::apply()const
	{
		m_viewport.apply();
	}

	void Camera::resize( uint32_t p_width, uint32_t p_height )
	{
		resize( Size( p_width, p_height ) );
	}

	void Camera::resize( Size const & p_size )
	{
		m_viewport.resize( p_size );
	}

	ViewportType Camera::getViewportType()const
	{
		return m_viewport.getType();
	}

	uint32_t Camera::getWidth()const
	{
		return m_viewport.getWidth();
	}

	uint32_t Camera::getHeight()const
	{
		return m_viewport.getHeight();
	}

	void Camera::setViewportType( ViewportType val )
	{
		m_viewport.updateType( val );
	}

	bool Camera::isVisible( CubeBox const & p_box, Matrix4x4r const & p_transformations )const
	{
		return m_frustum.isVisible( p_box, p_transformations );
	}

	bool Camera::isVisible( castor::SphereBox const & p_box, castor::Matrix4x4r const & p_transformations )const
	{
		return m_frustum.isVisible( p_box, p_transformations );
	}

	bool Camera::isVisible( Point3r const & p_point )const
	{
		return m_frustum.isVisible( p_point );
	}

	void Camera::onNodeChanged( SceneNode const & p_node )
	{
		m_nodeChanged = true;
		onChanged( *this );
	}
}
