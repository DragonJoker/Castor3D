#include "Castor3D/Scene/Camera.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, Camera )

namespace c3d
{
	namespace camera
	{
		static CU_ImplementAttributeParserBlock( parserCameraParent, CameraContext )
		{
			auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
			SceneNodeRPtr parent = blockContext->scene->scene->findSceneNode( name );

			if ( parent )
			{
				while ( parent->getParent()
					&& parent->getParent() != blockContext->scene->scene->getObjectRootNode()
					&& parent->getParent() != blockContext->scene->scene->getCameraRootNode() )
				{
					parent = parent->getParent();
				}

				if ( !parent->getParent()
					|| parent->getParent() == blockContext->scene->scene->getObjectRootNode() )
				{
					parent->attachTo( *blockContext->scene->scene->getCameraRootNode() );
				}

				blockContext->parentNode = parent;
			}
			else
			{
				CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCameraViewport, CameraContext )
		{
			blockContext->viewport = makeUnique< Viewport >( *getEngine( *blockContext ) );
			blockContext->viewport->setPerspective( 0.0_degrees, 1, 0, 1 );
		}
		CU_EndAttributePushBlock( CSCNSection::eViewport, blockContext )

		static CU_ImplementAttributeParserBlock( parserCameraPrimitive, CameraContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->primitiveType = VkPrimitiveTopology( params[0]->get< uint32_t >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCameraEnd, CameraContext )
		{
			if ( blockContext->viewport )
			{
				auto node = blockContext->parentNode;

				if ( !node )
				{
					node = blockContext->scene->scene->getCameraRootNode();
				}

				auto camera = blockContext->scene->scene->addNewCamera( blockContext->name
					, *blockContext->scene->scene
					, *node
					, c3d::move( *blockContext->viewport.release() ) );
				camera->setHdrConfig( c3d::move( blockContext->hdrConfig ) );
				camera->setColourGradingConfig( c3d::move( blockContext->colourGradingConfig ) );
				log::info << "Loaded camera [" << camera->getName() << "]" << std::endl;
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserViewportType, CameraContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->viewport->updateType( ViewportType( params[0]->get< uint32_t >() ) );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportLeft, CameraContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->viewport->updateLeft( params[0]->get< float >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportRight, CameraContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->viewport->updateRight( params[0]->get< float >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportTop, CameraContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->viewport->updateTop( params[0]->get< float >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportBottom, CameraContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->viewport->updateBottom( params[0]->get< float >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportNear, CameraContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->viewport->updateNear( params[0]->get< float >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportFar, CameraContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->viewport->updateFar( params[0]->get< float >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportFovY, CameraContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->viewport->updateFovY( Angle::fromDegrees( params[0]->get< float >() ) );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportAspectRatio, CameraContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				blockContext->viewport->updateRatio( params[0]->get< float >() );
		}
		CU_EndAttribute()
	}

	Camera::Camera( String const & name
		, Scene & scene
		, SceneNode & node
		, Viewport viewport
		, bool ownProjMtx )
		: MovableObject{ name, scene, MovableType::eCamera, node }
		, m_viewport{ c3d::move( viewport ) }
		, m_frustum{ m_viewport }
		, m_ownProjection{ ownProjMtx }
	{
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

	Camera::Camera( String const & name
		, CameraCreateInfo const & createInfo )
		: Camera{ name
			, *createInfo.scene
			, *createInfo.parentNode
			, createInfo.viewport ? *createInfo.viewport : Viewport{ *createInfo.scene->getEngine() }
			, createInfo.ownProjMtx }
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
			Point3f right{ 1.0, 0.0, 0.0 };
			Point3f up{ 0.0, 1.0, 0.0 };
			orientation.transform( right, right );
			orientation.transform( up, up );
			Point3f front{ point::cross( right, up ) };
			up = point::cross( front, right );

			// Update view matrix
			matrix::lookAt( m_view, position, position + front, up );
			updateFrustum();
			onGpuChanged( *this );
		}
	}

	void Camera::setProjection( Matrix4x4f const & projection )
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
		auto const & sceneNode = *geometry.getParent();
		auto transform = geometry.getGlobalTransform();
		auto result = m_frustum.isVisible( geometry.getBoundingSphere( submesh )
				, transform
				, sceneNode.getDerivedScale() )
			&& m_frustum.isVisible( geometry.getBoundingBox( submesh )
				, transform );
		return result;
	}

	void Camera::cloneInto( Camera & output )const
	{
		output.m_frustum = m_frustum;
		output.m_view = m_view;
		output.m_hdrConfig = m_hdrConfig;
		output.m_colourGradingConfig = m_colourGradingConfig;
		output.m_ownProjection = m_ownProjection;
		output.m_projection = m_projection;
		m_viewport.cloneInto( output.m_viewport );
	}

	void Camera::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< CameraContext > cameraCtx{ result, CSCNSection::eCamera, CSCNSection::eScene };
		BlockParserContextT< CameraContext > viewportCtx{ result, CSCNSection::eViewport, CSCNSection::eCamera };

		cameraCtx.addParser( cuT( "parent" ), camera::parserCameraParent, { makeParameter< ParameterType::eName >() } );
		cameraCtx.addParser( cuT( "primitive" ), camera::parserCameraPrimitive, { makeParameter< ParameterType::eCheckedText, VkPrimitiveTopology >() } );
		cameraCtx.addPushParser( cuT( "viewport" ), CSCNSection::eViewport, camera::parserCameraViewport );
		cameraCtx.addPopParser( cuT( "}" ), camera::parserCameraEnd );

		viewportCtx.addParser( cuT( "type" ), camera::parserViewportType, { makeParameter< ParameterType::eCheckedText, ViewportType >() } );
		viewportCtx.addParser( cuT( "left" ), camera::parserViewportLeft, { makeParameter< ParameterType::eFloat >() } );
		viewportCtx.addParser( cuT( "right" ), camera::parserViewportRight, { makeParameter< ParameterType::eFloat >() } );
		viewportCtx.addParser( cuT( "top" ), camera::parserViewportTop, { makeParameter< ParameterType::eFloat >() } );
		viewportCtx.addParser( cuT( "bottom" ), camera::parserViewportBottom, { makeParameter< ParameterType::eFloat >() } );
		viewportCtx.addParser( cuT( "near" ), camera::parserViewportNear, { makeParameter< ParameterType::eFloat >() } );
		viewportCtx.addParser( cuT( "far" ), camera::parserViewportFar, { makeParameter< ParameterType::eFloat >() } );
		viewportCtx.addParser( cuT( "fov_y" ), camera::parserViewportFovY, { makeParameter< ParameterType::eFloat >() } );
		viewportCtx.addParser( cuT( "aspect_ratio" ), camera::parserViewportAspectRatio, { makeParameter< ParameterType::eFloat >() } );
		viewportCtx.addDefaultPopParser();
	}

	Matrix4x4f Camera::getRescaledProjection( Size const & renderSize
		, float scale
		, bool safeBanded )const
	{
		if ( m_ownProjection )
		{
			return m_projection;
		}

		return scale == 1.0f
			? ( safeBanded
				? m_viewport.getSafeBandedProjection( renderSize )
				: m_viewport.getProjection() )
			: ( safeBanded
				? m_viewport.getRescaledSafeBandedProjection( renderSize, scale )
				: m_viewport.getRescaledProjection( scale ) );
	}

	float Camera::getProjectionScale( Size const & renderSize )const
	{
		float const scale = std::abs( 2.0f * ( getFovY() * 0.5f ).tan() );
		return std::abs( float( renderSize.getHeight() ) / scale );
	}
}
