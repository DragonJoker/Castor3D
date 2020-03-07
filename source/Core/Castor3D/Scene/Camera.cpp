#include "Castor3D/Scene/Camera.hpp"

#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#if C3D_DebugFrustum
extern bool C3D_DebugFrustumDisplay;
#endif

using namespace castor;

namespace castor3d
{
	Camera::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Camera >{ tabs }
	{
	}

	bool Camera::TextWriter::operator()( Camera const & camera, TextFile & file )
	{
		log::info << m_tabs << cuT( "Writing Camera " ) << camera.getName() << std::endl;
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
		, SceneNode & node
		, Viewport viewport
		, bool ownProjMtx )
		: MovableObject{ name, scene, MovableType::eCamera, node }
		, m_viewport{ std::move( viewport ) }
		, m_frustum{ m_viewport }
		, m_ownProjection{ ownProjMtx }
	{
		m_notifyIndex = node.onChanged.connect( [this]( SceneNode const & node )
			{
				onNodeChanged( node );
			} );
		onNodeChanged( node );
	}

	Camera::Camera( String const & name
		, Scene & scene
		, SceneNode & node
		, bool ownProjMtx )
		: Camera{ name
			, scene
			, node
			, Viewport{ *scene.getEngine() }
			, ownProjMtx }
	{
	}

	Camera::~Camera()
	{
		m_notifyIndex.disconnect();
	}

	void Camera::attachTo( SceneNode & node )
	{
		if ( &node != getParent() )
		{
			MovableObject::attachTo( node );
			m_notifyIndex = node.onChanged.connect( [this]( SceneNode const & node )
				{
					onNodeChanged( node );
				} );
			onNodeChanged( node );
		}
	}

	void Camera::updateFrustum()
	{
		if ( !m_ownProjection )
		{
			m_frustum.update( m_viewport.getProjection(), m_view );
		}
		else
		{
			m_frustum.update( m_projection, m_view );
		}
	}

	void Camera::update()
	{
		auto node = getParent();

		if ( node )
		{
			bool modified = m_viewport.update();

			if ( modified || m_nodeChanged )
			{
				auto position = node->getDerivedPosition();
				auto const & orientation = node->getDerivedOrientation();
				castor::Point3f right{ 1.0, 0.0, 0.0 };
				castor::Point3f up{ 0.0, 1.0, 0.0 };
				orientation.transform( right, right );
				orientation.transform( up, up );
				castor::Point3f front{ point::cross( right, up ) };
				up = point::cross( front, right );

				// Update view matrix
				matrix::lookAt( m_view, position, position + front, up );
				updateFrustum();
				m_nodeChanged = false;
			}
		}
	}

	void Camera::setProjection( castor::Matrix4x4f const & projection )
	{
		CU_Require( m_ownProjection );

		if ( m_projection != projection )
		{
			m_projection = projection;
			updateFrustum();
			onChanged( *this );
		}
	}

	bool Camera::isVisible( Geometry const & geometry, Submesh const & submesh )const
	{
#if C3D_DebugFrustum
		//C3D_DebugFrustumDisplay = geometry.getName() == cuT( "buisson_droit_8" );

		if ( C3D_DebugFrustumDisplay )
		{
			std::clog << cuT( "Geometry: " ) << geometry.getName() << cuT( " - Submesh: " ) << submesh.getId();
		}
#endif
		auto & sceneNode = *geometry.getParent();
		auto result = m_frustum.isVisible( geometry.getBoundingSphere( submesh )
				, sceneNode.getDerivedTransformationMatrix()
				, sceneNode.getDerivedScale() )
			&& m_frustum.isVisible( geometry.getBoundingBox( submesh )
				, sceneNode.getDerivedTransformationMatrix() );
#if C3D_DebugFrustum
		if ( C3D_DebugFrustumDisplay )
		{
			std::clog << std::endl;
		}
#endif
		return result;
	}

	void Camera::onNodeChanged( SceneNode const & node )
	{
		m_nodeChanged = true;
		onChanged( *this );
	}
}
