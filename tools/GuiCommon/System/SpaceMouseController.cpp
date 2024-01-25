/*
See LICENSE file in root folder
*/
#include "GuiCommon/System/SpaceMouseController.hpp"

CU_ImplementSmartPtr( GuiCommon, I3DController )

#if defined( GC_HasSpaceMouseSDK )

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <SpaceMouse/CNavigation3D.hpp>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <CastorUtils/Miscellaneous/BlockTracker.hpp>
#include <CastorUtils/Math/Speed.hpp>

#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

namespace GuiCommon
{
	namespace spcmse
	{
		static castor::Point3f & convert( navlib::point_t const & in
			, castor::Point3f & out )
		{
			return out = { in.x, in.y, in.z };
		}

		static castor::Point3f & convert( navlib::vector_t const & in
			, castor::Point3f & out )
		{
			return out = { in.x, in.y, in.z };
		}

		static navlib::point_t & convert( castor::Point3f const & in
			, navlib::point_t & out )
		{
			return out = { in->x, in->y, in->z };
		}

		static navlib::vector_t & convert( castor::Point3f const & in
			, navlib::vector_t & out )
		{
			return out = { in->x, in->y, in->z };
		}

		static navlib::matrix_t & convert( castor::Matrix4x4d const & in
			, navlib::matrix_t & out )
		{
			std::copy_n( in.constPtr(), 16u, out.begin() );
			return out;
		}

		static navlib::matrix_t & convert( castor::Matrix4x4f const & in
			, navlib::matrix_t & out )
		{
			return convert( castor::Matrix4x4d{ in }, out );
		}

		static castor::Matrix4x4f & convert( navlib::matrix_t const & in
			, castor::Matrix4x4f & out )
		{
			castor::Matrix4x4d mtx;
			std::copy_n( &in.m00, 16u, mtx.ptr() );
			return out = mtx;
		}

		class SpaceMouseController
			: public TDx::SpaceMouse::Navigation3D::CNavigation3D
			, public I3DController
		{
		public:
			SpaceMouseController( castor::String const & appName
				, castor3d::FrameListener & listener )
				: m_listener{ listener }
			{
				try
				{
					PutProfileHint( castor::toUtf8( appName ).c_str() );
					PutFrameTimingSource( TimingSource::SpaceMouse );
					EnableNavigation( true );
					Write( getMotionModelSettingsName(), "FreeCamera" );
					Write( getLockHorizonSettingsName(), true );
					Write( getMoveObjectsSettingsName(), false );
					m_connected = true;
				}
				catch ( std::exception & exc )
				{
					castor::Logger::logError( cuT( "Couldn't initialise space mouse controller: " ) + castor::makeString( exc.what() ) );
				}
				catch ( ... )
				{
					castor::Logger::logError( cuT( "Couldn't initialise space mouse controller: Unknown error" ) );
				}
			}

			~SpaceMouseController()noexcept override
			{
				if ( m_connected )
				{
					EnableNavigation( false );
				}
			}

			static castor::MbString const & getSettingsName()
			{
				static castor::MbString const result{ navlib::settings_k };
				return result;
			}

			static castor::MbString const & getMotionModelSettingsName()
			{
				static castor::MbString const result{ getSettingsName() + ".MotionModel" };
				return result;
			}

			static castor::MbString const & getLockHorizonSettingsName()
			{
				static castor::MbString const result{ getSettingsName() + ".LockHorizon" };
				return result;
			}

			static castor::MbString const & getMoveObjectsSettingsName()
			{
				static castor::MbString const result{ getSettingsName() + ".MoveObjects" };
				return result;
			}

			void reset()override
			{
				if ( m_camera )
				{
					m_cameraMatrix = m_camera->getParent()->getTransformationMatrix();
				}

				if ( m_geometry )
				{
					m_geometryMatrix = m_geometry->getParent()->getTransformationMatrix();
				}
			}

			void setCamera( castor3d::CameraRPtr camera )override
			{
				m_camera = camera;
				reset();
			}

			void setGeometry( castor3d::GeometryRPtr geometry )override
			{
				m_geometry = geometry;
				reset();
			}

			void setSpeedFactor( float value )override
			{
				m_speedFactor = value;
			}

			void multSpeedFactor( float value )override
			{
				m_speedFactor *= value;
			}

			void invertXTranslate( bool invert )override
			{
				m_axisScale->x = invert ? -1.0f : 1.0f;
			}

			void invertYTranslate( bool invert )override
			{
				m_axisScale->y = invert ? 1.0f : -1.0f;
			}

			void invertZTranslate( bool invert )override
			{
				m_axisScale->z = invert ? 1.0f : -1.0f;
			}

		public:
			static castor::Matrix4x4d getCoordinateSystem()
			{
				static castor::Matrix4x4d const cs{ castor::Array< double, 16u >
					{ 1.0, 0.0, 0.0, 0.0
					, 0.0, 1.0, 0.0, 0.0
					, 0.0, 0.0, 1.0, 0.0
					, 0.0, 0.0, 0.0, 1.0 } };
				return cs;
			}
			/**
			*\name ISpace3D overrides.
			*/
			/**@{*/
			long GetCoordinateSystem( navlib::matrix_t & affine )const override
			{
				convert( getCoordinateSystem(), affine );
				return 0;
			}

			long GetFrontView( navlib::matrix_t & affine )const override
			{
				convert( getCoordinateSystem().getInverse(), affine );
				return 0;
			}
			/**@}*/

		public:
			/**
			*\name IState overrides.
			*/
			/**@{*/
			long SetTransaction( long transaction )override
			{
				if ( isActive() )
				{
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long SetMotionFlag( bool motion )override
			{
				if ( isActive() )
				{
					m_moving = motion;
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}
			/**@}*/

		public:
			/**
			*\name IView overrides.
			*/
			/**@{*/
			long GetCameraMatrix( navlib::matrix_t & matrix )const override
			{
				if ( isCameraActive() )
				{
					m_cameraMatrix = m_camera->getParent()->getTransformationMatrix();
					convert( m_cameraMatrix, matrix );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::no_data_available );
			}

			long GetPointerPosition( navlib::point_t & position )const override
			{
				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long GetViewConstructionPlane( navlib::plane_t & plane ) const
			{
				if ( isCameraActive() )
				{
					// if it is a 2D projection then return the plane parallel to the view
					// at the model position
					auto translation = m_camera->getParent()->getPosition();

					if ( m_camera->getViewport().getType() == castor3d::ViewportType::eOrtho )
					{
						// The camera's z-axis
						auto zAxis = castor::Point3f{ m_camera->getParent()->getTransformationMatrix().getRow( 2u ) };
						plane = { { zAxis->x, zAxis->y, zAxis->z }, -castor::point::dot( zAxis, translation ) };
					}
					else
					{
						plane = { { 0.0, 0.0, 1.0 }, -translation->z };
					}

					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::no_data_available );
			}

			long GetViewExtents( navlib::box_t & bbox )const override
			{
				if ( isCameraActive() )
				{
					auto & extents = m_camera->getFrustum().getBoundingBox();
					convert( extents.getMin(), bbox.min );
					convert( extents.getMax(), bbox.max );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long GetViewFOV( double & fov )const override
			{
				if ( isCameraActive()
					&& ( m_camera->getViewportType() == castor3d::ViewportType::ePerspective
						|| m_camera->getViewportType() == castor3d::ViewportType::eFrustum ) )
				{
					auto & viewport = m_camera->getViewport();

					if ( m_camera->getViewportType() == castor3d::ViewportType::eFrustum )
					{
						double halfHeight = ( double( viewport.getTop() ) - viewport.getBottom() ) / 2.0;
						fov = ( castor::atand( halfHeight / viewport.getNear() ) * 2.0 ).radians();
					}
					else
					{
						fov = viewport.getFovY().radians();
					}

					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long GetViewFrustum( navlib::frustum_t & frustum )const override
			{
				if ( isCameraActive()
					&& ( m_camera->getViewportType() == castor3d::ViewportType::ePerspective
						|| m_camera->getViewportType() == castor3d::ViewportType::eFrustum ) )
				{
					auto & viewport = m_camera->getViewport();

					if ( m_camera->getViewportType() == castor3d::ViewportType::eFrustum )
					{
						frustum = { viewport.getLeft(), viewport.getRight()
							, viewport.getBottom(), viewport.getTop()
							, viewport.getNear(), viewport.getFar() };
					}
					else
					{
						double halfHeight = viewport.getNear() * ( viewport.getFovY() * 0.5 ).tand();
						double aspect = viewport.getRatio();
						double halfWidth = halfHeight * aspect;
						frustum = { -halfWidth, halfWidth
							, -halfHeight, halfHeight
							, viewport.getNear(), viewport.getFar() };
					}

					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long GetIsViewPerspective( navlib::bool_t & perspective )const override
			{
				if ( isCameraActive() )
				{
					perspective = ( m_camera->getViewportType() == castor3d::ViewportType::ePerspective
							|| m_camera->getViewportType() == castor3d::ViewportType::eFrustum )
						? 1u
						: 0u;
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::no_data_available );
			}

			long GetIsViewRotatable( navlib::bool_t & rotatable )const override
			{
				if ( isCameraActive() )
				{
					rotatable = 1u;
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long SetCameraMatrix( navlib::matrix_t const & matrix )override
			{
				if ( isCameraActive() )
				{
					loadMatrix( matrix, *m_camera->getParent(), m_cameraMatrix );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long SetViewExtents( const navlib::box_t & extents )override
			{
				if ( isCameraActive()
					&& m_camera->getViewportType() == castor3d::ViewportType::eOrtho )
				{
					auto & viewport = m_camera->getViewport();
					double scale = viewport.getProjectionScale();
					viewport.setOrtho( float( extents.min.x / scale ), float( extents.max.x / scale )
						, float( extents.min.y / scale ), float( extents.max.y / scale )
						, viewport.getNear(), viewport.getFar() );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long SetViewFOV( double fov )override
			{
				if ( isCameraActive()
					&& ( m_camera->getViewportType() == castor3d::ViewportType::ePerspective
						|| m_camera->getViewportType() == castor3d::ViewportType::eFrustum ) )
				{
					auto & viewport = m_camera->getViewport();
					viewport.setPerspective( castor::Angle::fromDegrees( fov )
						, viewport.getRatio()
						, viewport.getNear(), viewport.getFar() );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long SetViewFrustum( const navlib::frustum_t & frustum )override
			{
				if ( isCameraActive()
					&& ( m_camera->getViewportType() == castor3d::ViewportType::ePerspective
						|| m_camera->getViewportType() == castor3d::ViewportType::eFrustum ) )
				{
					auto & viewport = m_camera->getViewport();
					viewport.setFrustum( float( frustum.left ), float( frustum.right )
						, float( frustum.bottom ), float( frustum.top )
						, float( frustum.nearVal ), float( frustum.farVal ) );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}
			/**@}*/

		public:
			/**
			*\name IModel overrides.
			*/
			/**@{*/
			long GetModelExtents( navlib::box_t & bbox )const override
			{
				if ( isGeometryActive() )
				{
					auto & extents = m_geometry->getBoundingBox();
					convert( extents.getMin(), bbox.min );
					convert( extents.getMax(), bbox.max );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long GetSelectionExtents( navlib::box_t & bbox )const override
			{
				if ( isGeometryActive() )
				{
					auto & extents = m_geometry->getBoundingBox();
					convert( extents.getMin(), bbox.min );
					convert( extents.getMax(), bbox.max );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long GetSelectionTransform( navlib::matrix_t & matrix )const override
			{
				return navlib::make_result_code( navlib::navlib_errc::function_not_supported );
			}

			long GetIsSelectionEmpty( navlib::bool_t & empty )const override
			{
				empty = isGeometryActive() ? 0u : 1u;
				return 0;
			}

			long SetSelectionTransform( const navlib::matrix_t & matrix )override
			{
				return navlib::make_result_code( navlib::navlib_errc::function_not_supported );
			}

			long GetUnitsToMeters( double & meters )const override
			{
				meters = 1.0;
				return 0;
			}

			long GetFloorPlane( navlib::plane_t & floor )const override
			{
				floor = { { 0, 1, 0 }, 0.0 };
				return 0;
			}
			/**@}*/

		public:
			/**
			*\name IPivot overrides.
			*/
			/**@{*/
			long GetPivotPosition( navlib::point_t & position )const override
			{
				return navlib::make_result_code( navlib::navlib_errc::function_not_supported );
			}

			long IsUserPivot( navlib::bool_t & userPivot )const override
			{
				return navlib::make_result_code( navlib::navlib_errc::function_not_supported );
			}

			long SetPivotPosition( const navlib::point_t & position )override
			{
				return navlib::make_result_code( navlib::navlib_errc::function_not_supported );
			}

			long GetPivotVisible( navlib::bool_t & visible )const override
			{
				return navlib::make_result_code( navlib::navlib_errc::function_not_supported );
			}

			long SetPivotVisible( bool visible )override
			{
				return navlib::make_result_code( navlib::navlib_errc::function_not_supported );
			}
			/**@}*/

		public:
			/**
			*\name IEvent overrides.
			*/
			/**@{*/
			long SetActiveCommand( std::string commandId )override
			{
				return navlib::make_result_code( navlib::navlib_errc::function_not_supported );
			}
			/**@}*/

		public:
			/**
			*\name IHit overrides.
			*/
			/**@{*/
			long GetHitLookAt( navlib::point_t & position )const override
			{
				if ( isCameraActive() )
				{
					castor::Matrix4x4d nodeMtx{ m_camera->getParent()->getTransformationMatrix() };
					castor::Point3f pos;
					castor::Point3f scl;
					castor::Quaternion rotation;
					castor::matrix::decompose( nodeMtx, pos, scl, rotation );
					castor::Point3f front{ 0.0, 0.0, 1.0 };
					rotation.transform( front, front );
					position.x = front->x;
					position.y = front->y;
					position.z = front->z;

					if ( pos != front )
					{
						return 0;
					}
				}

				return navlib::make_result_code( navlib::navlib_errc::no_data_available );
			}

			long SetHitAperture( double aperture )override
			{
				if ( isCameraActive() )
				{
					m_lookAperture = float( aperture );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long SetHitDirection( const navlib::vector_t & direction )override
			{
				if ( isCameraActive() )
				{
					convert( direction, m_lookDirection );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long SetHitLookFrom( const navlib::point_t & eye )override
			{
				if ( isCameraActive() )
				{
					convert( eye, m_lookPosition );
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}

			long SetHitSelectionOnly( bool onlySelection )override
			{
				if ( isCameraActive() )
				{
					m_hitSelectionOnly = onlySelection;
					return 0;
				}

				return navlib::make_result_code( navlib::navlib_errc::invalid_operation );
			}
			/**@}*/

		private:
			bool isCameraActive()const noexcept
			{
				return m_camera != nullptr;
			}

			bool isGeometryActive()const noexcept
			{
				return m_geometry != nullptr;
			}

			bool isActive()const noexcept
			{
				return isCameraActive()
					|| isGeometryActive();
			}

			void loadMatrix( navlib::matrix_t const & matrix
				, castor3d::SceneNode & node
				, castor::Matrix4x4f & savedMatrix )
			{
				castor::Matrix4x4f nodeMtx;
				convert( matrix, nodeMtx );
				auto localTransform = savedMatrix.getInverse() * nodeMtx;
				savedMatrix = nodeMtx;

				castor::Point3f position;
				castor::Point3f scaling;
				castor::Quaternion orientation;
				castor::matrix::decompose( nodeMtx
					, position
					, scaling
					, orientation );

				// Retrieve transform relative to previous one.
				castor::Point3f translate;
				castor::Point3f scale;
				castor::Quaternion rotation;
				castor::matrix::decompose( localTransform
					, translate
					, scale
					, rotation );

				// Invert X and Z translation
				translate *= m_axisScale;

				// Recompute to full transform
				orientation.transform( translate, translate );

				m_listener.postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
					, [this, &node, translate, rotation]()
					{
						node.translate( translate * m_speedFactor * 5.0f );
						node.rotate( rotation );
					} ) );
			}

		private:
			bool m_connected{};
			castor3d::FrameListener & m_listener;
			castor3d::CameraRPtr m_camera{};
			castor3d::GeometryRPtr m_geometry{};
			float m_lookAperture{};
			castor::Point3f m_lookDirection{};
			castor::Point3f m_lookPosition{};
			bool m_hitSelectionOnly{};
			bool m_moving{ true };
			mutable castor::Matrix4x4f m_cameraMatrix;
			mutable castor::Matrix4x4f m_geometryMatrix;
			float m_speedFactor{ 1.0f };
			castor::Point3f m_axisScale{ -1.0, 1.0, -1.0 };
		};
	}

	I3DControllerUPtr I3DController::create( castor::String const & appName
		, castor3d::FrameListener & listener )
	{
		return castor::makeUniqueDerived< I3DController, spcmse::SpaceMouseController >( appName, listener );
	}
}
#else

namespace GuiCommon
{
	I3DControllerUPtr I3DController::create( castor::String const & appName
		, castor3d::FrameListener & listener )
	{
		return nullptr;
	}
}


#endif
