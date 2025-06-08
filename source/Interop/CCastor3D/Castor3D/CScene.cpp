#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/BillboardCache.hpp>
#include <Castor3D/Cache/SceneNodeCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Model/Mesh/MeshGenerator.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Background/Image.hpp>
#include <Castor3D/Scene/Background/Skybox.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/LightGroup.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_SCENE = cuT( "The scene must be initialised" );
	static const C3DString ERROR_NULL_PARENT_NODE = cuT( "The node must not be null." );
	static const C3DString ERROR_UNINITIALISED_SCENEGEOM = cuT( "The geometry must be initialised." );
	static const C3DString ERROR_UNINITIALISED_SCENECAM = cuT( "The camera must be initialised." );
	static const C3DString ERROR_UNINITIALISED_SCENELGT = cuT( "The light must be initialised." );
	static const C3DString ERROR_UNINITIALISED_SCENELGTGRP = cuT( "The light group must be initialised." );
	static const C3DString ERROR_UNINITIALISED_SCENEMSH = cuT( "The mesh must be initialised." );
	static const C3DString ERROR_WRONG_SCENE_FILE_NAME = cuT( "The file doesn't exist." );

	C3D_CAPIMETHODIMP c3dScene_delete( C3DScene * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			cc3d::reportWarning( ERROR_UNINITIALISED_SCENE );

		if ( object->getInternal() )
			object->getInternal()->cleanup();

		object->releaseInternal();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getBackgroundColour( C3DScene const * object, C3DRgbColour * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		*result = cc3d::convert( object->getInternal()->getBackgroundColour() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_setBackgroundColour( C3DScene * object, C3DRgbColour const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		object->getInternal()->setBackgroundColour( cc3d::convert( *val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_setBackgroundImage( C3DScene * object, C3DString filePath )
	{
		if ( !object || !filePath )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		castor::Path path{ castor::makeString( filePath ) };
		if ( !castor::File::fileExists( path ) )
			return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_SCENE_FILE_NAME );

		auto background = castor::makeUnique< castor3d::ImageBackground >( *object->getInternal()->getEngine(), *object->getInternal(), castor::String{} );
		background->setImage( path.getPath(), path.getFileName( true ) );
		object->getInternal()->setBackground( castor::ptrRefCast< castor3d::SceneBackground >( background ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_setBackgroundSkybox( C3DScene * object, C3DSkybox * skybox )
	{
		if ( !object || !skybox )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		object->getInternal()->setBackground( castor::ptrRefCast< castor3d::SceneBackground >( skybox->internal ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getName( C3DScene const * object, C3DString * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		return cc3d::copyString( object->getInternal()->getName(), result );
	}

	C3D_CAPIMETHODIMP c3dScene_getAmbientLight( C3DScene const * object, C3DRgbColour * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		*result = cc3d::convert( object->getInternal()->getAmbientLight() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_setAmbientLight( C3DScene * object, C3DRgbColour const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		object->getInternal()->setAmbientLight( cc3d::convert( *val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getRootNode( C3DScene const * object, C3DSceneNode ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->getRootNode();
			C3D_SafeAlloc( *result, C3DSceneNode );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getObjectRootNode( C3DScene const * object, C3DSceneNode ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->getObjectRootNode();
			C3D_SafeAlloc( *result, C3DSceneNode );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getCameraRootNode( C3DScene const * object, C3DSceneNode ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->getCameraRootNode();
			C3D_SafeAlloc( *result, C3DSceneNode );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_addNode( C3DScene * object, C3DSceneNode * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_NULL_PARENT_NODE );

		try
		{
			auto name = val->getInternal()->getName();
			auto own = val->releaseInternal();
			val->setInternal( object->getInternal()->addSceneNode( name, own, true ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_addGeometry( C3DScene * object, C3DGeometry * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENEGEOM );

		try
		{
			auto own = val->releaseInternal();
			val->setInternal( object->getInternal()->addGeometry( castor::move( own ) ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_addCamera( C3DScene * object, C3DCamera * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENECAM );

		try
		{
			auto name = val->getInternal()->getName();
			auto own = val->releaseInternal();
			val->setInternal( object->getInternal()->addCamera( name, own, true ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_addLight( C3DScene * object, C3DLight * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENELGT );

		try
		{
			auto name = val->getInternal()->getName();
			auto own = val->releaseInternal();
			val->setInternal( object->getInternal()->addLight( name, own, true ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_addLightGroup( C3DScene * object, C3DLightGroup * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENELGTGRP );

		try
		{
			auto name = val->getInternal()->getName();
			auto own = val->releaseInternal();
			val->setInternal( object->getInternal()->addLightGroup( name, own, true ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_addMesh( C3DScene * object, C3DMesh * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENEMSH );

		try
		{
			auto name = val->getInternal()->getName();
			auto own = val->releaseInternal();
			val->setInternal( object->getInternal()->addMesh( name, own, true ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_removeNode( C3DScene * object, C3DSceneNode * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_NULL_PARENT_NODE );

		try
		{
			auto own = object->getInternal()->removeSceneNode( val->getInternal()->getName(), true );
			if ( !own )
				return cc3d::reportError( C3D_FAILURE, "SceneNode didn't exist in the scene" );

			val->setInternal( castor::move( own ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_removeGeometry( C3DScene * object, C3DGeometry * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENEGEOM );

		try
		{
			auto own = object->getInternal()->removeGeometry( val->getInternal()->getName(), true );
			if ( !own )
				return cc3d::reportError( C3D_FAILURE, "Geometry didn't exist in the scene" );

			val->setInternal( castor::move( own ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_removeCamera( C3DScene * object, C3DCamera * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENECAM );

		try
		{
			auto own = object->getInternal()->removeCamera( val->getInternal()->getName(), true );
			if ( !own )
				return cc3d::reportError( C3D_FAILURE, "Camera didn't exist in the scene" );

			val->setInternal( castor::move( own ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_removeLight( C3DScene * object, C3DLight * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENELGT );

		try
		{
			auto own = object->getInternal()->removeLight( val->getInternal()->getName(), true );
			if ( !own )
				return cc3d::reportError( C3D_FAILURE, "Light didn't exist in the scene" );

			val->setInternal( castor::move( own ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_removeLightGroup( C3DScene * object, C3DLightGroup * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENELGTGRP );

		try
		{
			auto own = object->getInternal()->removeLightGroup( val->getInternal()->getName(), true );
			if ( !own )
				return cc3d::reportError( C3D_FAILURE, "LightGroup didn't exist in the scene" );

			val->setInternal( castor::move( own ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_removeMesh( C3DScene * object, C3DMesh * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENEMSH );

		try
		{
			auto own = object->getInternal()->removeMesh( val->getInternal()->getName(), true );
			if ( !own )
				return cc3d::reportError( C3D_FAILURE, "Mesh didn't exist in the scene" );

			val->setInternal( castor::move( own ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getNode( C3DScene const * object, C3DString name, C3DSceneNode ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->findSceneNode( castor::makeString( name ) );
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, cuT( "Couldn't find the SceneNode" ) );

			C3D_SafeAlloc( *result, C3DSceneNode );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getGeometry( C3DScene const * object, C3DString name, C3DGeometry ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->findGeometry( castor::makeString( name ) );
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, cuT( "Couldn't find the Geometry" ) );

			C3D_SafeAlloc( *result, C3DGeometry );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getCamera( C3DScene const * object, C3DString name, C3DCamera ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->findCamera( castor::makeString( name ) );
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, cuT( "Couldn't find the Camera" ) );

			C3D_SafeAlloc( *result, C3DCamera );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getLight( C3DScene const * object, C3DString name, C3DLight ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->findLight( castor::makeString( name ) );
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, cuT( "Couldn't find the Light" ) );

			C3D_SafeAlloc( *result, C3DLight );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getLightGroup( C3DScene const * object, C3DString name, C3DLightGroup ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->findLightGroup( castor::makeString( name ) );
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, cuT( "Couldn't find the LightGroup" ) );

			C3D_SafeAlloc( *result, C3DLightGroup );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_getMesh( C3DScene const * object, C3DString name, C3DMesh ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->findMesh( castor::makeString( name ) );
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, cuT( "Couldn't find the Mesh" ) );

			C3D_SafeAlloc( *result, C3DMesh );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_createNode( C3DScene const * object, C3DString name, C3DSceneNode const * parent, C3DSceneNode ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			if ( auto res = object->getInternal()->createSceneNode( castor::makeString( name ), *object->getInternal() ) )
			{
				if ( parent )
				{
					res->attachTo( *parent->getInternal() );
				}
				else
				{
					res->attachTo( *object->getInternal()->getObjectRootNode() );
				}

				C3D_SafeAlloc( *result, C3DSceneNode );
				( *result )->setInternal( castor::move( res ) );
			}
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_createGeometry( C3DScene const * object, C3DString name, C3DMesh const * mesh, C3DSceneNode const * parent, C3DGeometry ** result )
	{
		if ( !object || !name || !parent || !result || !mesh )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->createGeometry( castor::makeString( name )
				, *object->getInternal()
				, *parent->getInternal()
				, mesh->getInternal() );
			C3D_SafeAlloc( *result, C3DGeometry );
			( *result )->setInternal( castor::move( res ) );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_createCamera( C3DScene const * object, C3DString name, uint32_t ww, uint32_t wh, C3DSceneNode const * parent, C3DCamera ** result )
	{
		if ( !object || !name || !parent || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			castor3d::Viewport viewport{ *object->getInternal()->getEngine() };
			viewport.setPerspective( castor::Angle::fromDegrees( 120.0f ), 4.0f / 3.0f, 0.1f, 1000.0f );
			viewport.resize( castor::Size{ ww, wh } );
			auto res = object->getInternal()->createCamera( castor::makeString( name )
				, *object->getInternal()
				, *parent->getInternal()
				, std::move( viewport ) );
			C3D_SafeAlloc( *result, C3DCamera );
			( *result )->setInternal( castor::move( res ) );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_createLight( C3DScene const * object, C3DString name, C3DSceneNode const * parent, C3D_LIGHT_TYPE type, C3DLight ** result )
	{
		if ( !object || !name || !parent || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->createLight( castor::makeString( name )
				, *object->getInternal()
				, *parent->getInternal()
				, object->getInternal()->getLightsFactory()
				, castor3d::LightType( type ) );
			C3D_SafeAlloc( *result, C3DLight );
			( *result )->setInternal( castor::move( res ) );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_createLightGroup( C3DScene const * object, C3DString name, C3D_LIGHT_TYPE type, C3DLightGroup ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->createLightGroup( castor::makeString( name )
				, *object->getInternal()
				, object->getInternal()->getLightsFactory()
				, castor3d::LightType( type ) );
			C3D_SafeAlloc( *result, C3DLightGroup );
			( *result )->setInternal( castor::move( res ) );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dScene_createMesh( C3DScene const * object, C3DString type, C3DString name, C3DMesh ** result )
	{
		if ( !object || !name || !type || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SCENE );

		try
		{
			auto res = object->getInternal()->createMesh( name, *object->getInternal() );
			object->getInternal()->getEngine()->getMeshFactory().create( castor::makeString( name ) )->generate( *res, castor3d::Parameters{} );
			C3D_SafeAlloc( *result, C3DMesh );
			( *result )->setInternal( castor::move( res ) );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
