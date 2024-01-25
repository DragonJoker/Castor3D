#include "Castor3D/Scene/Camera.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

CU_ImplementSmartPtr( castor3d, Camera )

namespace castor3d
{
	Camera::Camera( castor::String const & name
		, Scene & scene
		, SceneNode & node
		, Viewport viewport
		, bool ownProjMtx )
		: MovableObject{ name, scene, MovableType::eCamera, node }
		, m_viewport{ castor::move( viewport ) }
		, m_frustum{ m_viewport }
		, m_ownProjection{ ownProjMtx }
	{
	}

	Camera::Camera( castor::String const & name
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

	void Camera::updateFrustum()
	{
		if ( !m_ownProjection )
		{
			m_frustum.update( m_view );
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
			m_viewport.update();
			auto position = node->getDerivedPosition();
			auto const & orientation = node->getDerivedOrientation();
			castor::Point3f right{ 1.0, 0.0, 0.0 };
			castor::Point3f up{ 0.0, 1.0, 0.0 };
			orientation.transform( right, right );
			orientation.transform( up, up );
			castor::Point3f front{ castor::point::cross( right, up ) };
			up = castor::point::cross( front, right );

			// Update view matrix
			castor::matrix::lookAt( m_view, position, position + front, up );
			updateFrustum();
			onGpuChanged( *this );
		}
	}

	void Camera::setProjection( castor::Matrix4x4f const & projection )
	{
		CU_Require( m_ownProjection );

		if ( m_projection != projection )
		{
			m_projection = projection;
			markDirty();
		}
	}

	bool Camera::isVisible( Geometry const & geometry, Submesh const & submesh )const
	{
		auto & sceneNode = *geometry.getParent();
		auto result = m_frustum.isVisible( geometry.getBoundingSphere( submesh )
				, sceneNode.getDerivedTransformationMatrix()
				, sceneNode.getDerivedScale() )
			&& m_frustum.isVisible( geometry.getBoundingBox( submesh )
				, sceneNode.getDerivedTransformationMatrix() );
		return result;
	}

	castor::Matrix4x4f Camera::getRescaledProjection( float scale
		, bool safeBanded )const
	{
		if ( m_ownProjection )
		{
			return m_projection;
		}

		return scale == 1.0f
			? ( safeBanded
				? m_viewport.getSafeBandedProjection()
				: m_viewport.getProjection() )
			: ( safeBanded
				? m_viewport.getRescaledSafeBandedProjection( scale )
				: m_viewport.getRescaledProjection( scale ) );
	}
}
