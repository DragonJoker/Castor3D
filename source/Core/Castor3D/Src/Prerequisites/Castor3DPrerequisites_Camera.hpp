/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_CAMERA_H___
#define ___C3D_PREREQUISITES_CAMERA_H___

namespace castor3d
{
	/**@name Camera */
	//@{

	class Camera;
	class Viewport;
	class IViewportImpl;
	class Ray;

	DECLARE_SMART_PTR( Camera );
	DECLARE_SMART_PTR( Viewport );
	DECLARE_SMART_PTR( IViewportImpl );
	DECLARE_SMART_PTR( Ray );

	//! Camera pointer map, sorted by name
	DECLARE_MAP( castor::String, CameraSPtr, CameraPtrStr );
	//! Camera pointer array
	DECLARE_VECTOR( CameraSPtr, CameraPtr );

	//@}
}

#endif
