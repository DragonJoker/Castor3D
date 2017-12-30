#include "Camera.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"

#include <Graphics/BoundingBox.hpp>
#include <Graphics/BoundingSphere.hpp>

using namespace castor;

namespace castor3d
{
	Camera::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Camera >{ tabs }
	{
	}

	bool Camera::TextWriter::operator()( Camera const & camera, TextFile & file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing Camera " ) + camera.getName() );
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "camera \"" ) + camera.getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< Camera >::checkError( result, "Camera name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( camera, file );
		}

		if ( result )
		{
			result = Viewport::TextWriter( m_tabs + cuT( "\t" ) )( camera.getViewport(), file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	Camera::Camera( String const & name
		, Scene & scene
		, SceneNodeSPtr node
		, Viewport && viewport
		, bool invertX )
		: MovableObject{ name, scene, MovableType::eCamera, node }
		, m_viewport{ std::move( viewport ) }
		, m_frustum{ m_viewport }
		, m_invertX{ invertX }
	{
		if ( scene.getEngine()->getRenderSystem()->getCurrentContext() )
		{
			m_viewport.initialise();
		}
		else
		{
			scene.getListener().postEvent( makeInitialiseEvent( m_viewport ) );
		}

		if ( node )
		{
			m_notifyIndex = node->onChanged.connect( std::bind( &Camera::onNodeChanged, this, std::placeholders::_1 ) );
			onNodeChanged( *node );
		}
	}

	Camera::Camera( String const & name
		, Scene & scene
		, SceneNodeSPtr node
		, bool invertX )
		: Camera{ name
			, scene
			, node
			, Viewport{ *scene.getEngine() }
			, invertX }
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
				Point3r front{ point::cross( right, up ) };
				up = point::cross( front, right );

				// Update view matrix
				matrix::lookAt( m_view, position, position + front, up );

				if ( m_invertX )
				{
					matrix::scale( m_view, Point3f{ -1.0f, 1.0f, 1.0f } );
				}

				m_frustum.update( position, right, up, front );
				m_nodeChanged = false;
			}
		}
	}

	void Camera::apply()const
	{
		m_viewport.apply();
	}

	void Camera::resize( uint32_t width, uint32_t height )
	{
		resize( Size( width, height ) );
	}

	void Camera::resize( Size const & size )
	{
		m_viewport.resize( size );
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

	void Camera::setViewportType( ViewportType value )
	{
		m_viewport.updateType( value );
	}

	bool Camera::isVisible( Geometry const & geometry, Submesh const & submesh )const
	{
		auto & sceneNode = *geometry.getParent();
		return m_frustum.isVisible( geometry.getBoundingSphere( submesh )
				, sceneNode.getDerivedTransformationMatrix()
				, sceneNode.getDerivedScale() )
			&& m_frustum.isVisible( geometry.getBoundingBox( submesh )
				, sceneNode.getDerivedTransformationMatrix() );
	}

	bool Camera::isVisible( BoundingBox const & box
		, Matrix4x4r const & transformations )const
	{
		return m_frustum.isVisible( box
			, transformations );
	}

	bool Camera::isVisible( castor::BoundingSphere const & sphere
		, Matrix4x4r const & transformations
		, Point3r const & scale )const
	{
		return m_frustum.isVisible( sphere
			, transformations
			, scale );
	}

	bool Camera::isVisible( Point3r const & point )const
	{
		return m_frustum.isVisible( point );
	}

	void Camera::onNodeChanged( SceneNode const & node )
	{
		m_nodeChanged = true;
		onChanged( *this );
	}
}
