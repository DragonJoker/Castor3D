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

	CU_DeclareSmartPtr( Camera );
	CU_DeclareSmartPtr( Viewport );
	CU_DeclareSmartPtr( IViewportImpl );
	CU_DeclareSmartPtr( Ray );

	//! Camera pointer map, sorted by name
	CU_DeclareMap( castor::String, CameraSPtr, CameraPtrStr );
	//! Camera pointer array
	CU_DeclareVector( CameraSPtr, CameraPtr );

	//@}
}

#endif
