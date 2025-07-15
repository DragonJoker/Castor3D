#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"
#include "CCastor3D/Castor3DPlatformTypes.h"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Event/UserInput/UserInputListener.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Scene/Background/Skybox.hpp>

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <ashespp/Core/PlatformWindowHandle.hpp>

namespace
{
	c3d::RenderWindowDesc doLoadSceneFile( c3d::Engine & engine, c3d::Path const & fileName )
	{
		c3d::RenderWindowDesc result;

		if ( c3d::File::fileExists( fileName ) )
		{
			c3d::Logger::logInfo( cuT( "Loading scene file : " ) + fileName );

			if ( fileName.getExtension() == cuT( "cscn" ) || fileName.getExtension() == cuT( "zip" ) )
			{
				try
				{
					c3d::SceneFileParser parser( engine );

					if ( parser.parseFile( fileName ) )
					{
						result = parser.getRenderWindow();
					}
					else
					{
						c3d::Logger::logWarning( cuT( "Can't read scene file" ) );
					}
				}
				catch ( std::exception & exc )
				{
					c3d::Logger::logError( cuT( "Failed to parse the scene file, with following error:" ) + c3d::makeString( exc.what() ) );
				}
			}
		}
		else
		{
			c3d::Logger::logError( cuT( "Scene file doesn't exist: " ) + fileName );
		}

		return result;
	}
}

#ifdef __cplusplus
extern "C"
{
#endif

	static C3DChar const * const ERROR_UNINITIALISED_ENGINE = cuT( "The engine must be initialised" );
	static C3DChar const * const ERROR_UNINITIALISED_ENGOVL = cuT( "The overlay must be initialised" );
	static C3DChar const * const ERROR_UNINITIALISED_ENGSCN = cuT( "The scene must be initialised" );
	static C3DChar const * const ERROR_UNINITIALISED_ENGSPL = cuT( "The sampler must be initialised" );
	static C3DChar const * const ERROR_INITIALISED_ENGINE = cuT( "The engine has already been initialised" );
	static C3DChar const * const ERROR_RENDERER_NOT_LOADED = cuT( "The renderer couldn't be loaded" );
	static C3DChar const * const ERROR_WRONG_FILE_NAME = cuT( "The given file doesn't exist" );

	C3D_CAPIMETHODIMP c3dEngine_create( C3DString appName, bool enableValidation, C3DEngine ** result )
	{
		if ( !result )
			return C3D_POINTER;

		if ( *result )
			return cc3d::reportError( C3D_FAILURE, ERROR_INITIALISED_ENGINE );

		try
		{
			auto engine = c3d::makeUnique< c3d::Engine >( c3d::EngineConfig{ c3d::makeString( appName )
				, c3d::Version{ CCastor3D_VERSION_MAJOR, CCastor3D_VERSION_MINOR, CCastor3D_VERSION_BUILD }
				, enableValidation
				, true } );
			C3D_SafeAlloc( *result, C3DEngine );
			( *result )->internal = c3d::move( engine );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_delete( C3DEngine * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		object->internal.reset();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_initialise( C3DEngine * object, uint32_t fps, bool threaded )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			object->internal->initialise( fps, threaded );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_cleanup( C3DEngine * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			object->internal->cleanup();
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_clearScenes( C3DEngine * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		object->internal->getSceneCache().clear();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_loadRenderer( C3DEngine * object, C3DString type )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			if ( !object->internal->loadRenderer( c3d::makeString( type ) ) )
				return cc3d::reportError( C3D_FAILURE, ERROR_RENDERER_NOT_LOADED );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_loadPlugin( C3DEngine * object, C3DString path )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			object->internal->getPluginCache().loadPlugin( c3d::Path{ c3d::makeString( path ) } );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_loadScene( C3DEngine * object, C3DString name, C3DRenderTarget ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			c3d::Path fileName{ c3d::makeString( name ) };

			if ( !c3d::File::fileExists( fileName ) )
				return cc3d::reportError( C3D_FAILURE, cuT( "Scene file doesn't exist" ) );

			if ( auto desc = doLoadSceneFile( *object->internal, fileName );
				desc.renderTarget && !desc.name.empty() )
			{
				C3D_SafeAlloc( *result, C3DRenderTarget );
				( *result )->internal = desc.renderTarget;
			}
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_loadImageFromFile( C3DEngine * object, C3DString name, C3DString val, C3DImage ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			auto strName = c3d::makeString( name );
			c3d::Path path{ c3d::makeString( val ) };
			c3d::Path pathImage = path;

			if ( !c3d::File::fileExists( pathImage ) )
			{
				pathImage = c3d::Engine::getDataDirectory() / path;
			}

			if ( !c3d::File::fileExists( pathImage ) )
			{
				pathImage = c3d::Engine::getDataDirectory() / cuT( "Texture" ) / path;
			}

			if ( !c3d::File::fileExists( pathImage ) )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_FILE_NAME );

			auto img = object->internal->addNewImage( strName
				, c3d::ImageCreateParams{ pathImage } );
			C3D_SafeAlloc( *result, C3DImage );
			( *result )->internal = img;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_loadImageFromFormat( C3DEngine * object, C3DString name, C3D_PIXEL_FORMAT fmt, C3DSize const * size, C3DImage ** result )
	{
		if ( !object || !result || !size )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			auto strName = c3d::makeString( name );
			auto img = object->internal->addNewImage( strName
				, c3d::ImageCreateParams{ c3d::Size{ size->width, size->height }
					, c3d::PixelFormat( fmt ) } );
			C3D_SafeAlloc( *result, C3DImage );
			( *result )->internal = img;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_loadFontFromFile( C3DEngine * object, C3DString name, C3DString path, uint32_t height, C3DFont ** result )
	{
		if ( !object || !result || !path || !name )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			c3d::String strName = c3d::makeString( name );
			c3d::Path strPath{ c3d::makeString( path ) };
			c3d::Path pathFont = strPath;

			if ( !c3d::File::fileExists( pathFont ) )
			{
				pathFont = c3d::Engine::getDataDirectory() / strPath;
			}

			if ( !c3d::File::fileExists( pathFont ) )
				return cc3d::reportError( C3D_FAILURE, ERROR_WRONG_FILE_NAME );

			auto font = object->internal->addNewFont( strName, height, pathFont );
			C3D_SafeAlloc( *result, C3DFont );
			( *result )->internal = font;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_addScene( C3DEngine * object, C3DScene * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGSCN );

		try
		{
			auto name = val->getInternal()->getName();
			auto own = val->releaseInternal();
			val->setInternal( object->internal->addScene( name, own, true ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_addOverlay( C3DEngine * object, C3DOverlay * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGOVL );

		try
		{
			auto name = val->getInternal()->getName();
			auto own = val->releaseInternal();
			val->setInternal( object->internal->addOverlay( name, own, true ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_addSampler( C3DEngine * object, C3DSampler * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGSPL );

		try
		{
			auto name = val->getInternal()->getName();
			auto own = val->releaseInternal();
			val->setInternal( object->internal->addSampler( name, own, true ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_removeScene( C3DEngine * object, C3DScene * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGSCN );

		try
		{
			auto own = object->internal->removeScene( val->getInternal()->getName(), true );
			if ( !own )
				return cc3d::reportError( C3D_FAILURE, "Scene didn't exist in the scene" );

			val->setInternal( c3d::move( own ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_removeOverlay( C3DEngine * object, C3DOverlay * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGOVL );

		try
		{
			auto own = object->internal->removeOverlay( val->getInternal()->getName(), true );
			if ( !own )
				return cc3d::reportError( C3D_FAILURE, "Overlya didn't exist in the scene" );

			val->setInternal( c3d::move( own ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_removeSampler( C3DEngine * object, C3DSampler * val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );
		if ( !val->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGSPL );

		try
		{
			auto own = object->internal->removeSampler( val->getInternal()->getName(), true );
			if ( !own )
				return cc3d::reportError( C3D_FAILURE, "Sampler didn't exist in the scene" );

			val->setInternal( c3d::move( own ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_getScene( C3DEngine const * object, C3DString name, C3DScene ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			auto res = object->internal->findScene( c3d::makeString( name ) );
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, cuT( "Couldn't find the Scene" ) );

			C3D_SafeAlloc( *result, C3DScene );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_getOverlay( C3DEngine const * object, C3DString name, C3DOverlay ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			auto res = object->internal->findOverlay( c3d::makeString( name ) );
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, cuT( "Couldn't find the Overlay" ) );

			C3D_SafeAlloc( *result, C3DOverlay );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_getSampler( C3DEngine const * object, C3DString name, C3DSampler ** result )
	{
		if ( !object || !name || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			auto res = object->internal->findSampler( c3d::makeString( name ) );
			if ( !res )
				return cc3d::reportError( C3D_FAILURE, cuT( "Couldn't find the Sampler" ) );

			C3D_SafeAlloc( *result, C3DSampler );
			( *result )->setInternal( res );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_renderOneFrame( C3DEngine * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			object->internal->getRenderLoop().renderSyncFrame();
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_startRendering( C3DEngine * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			object->internal->getRenderLoop().beginRendering();
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_pauseRendering( C3DEngine * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			object->internal->getRenderLoop().pause();
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_resumeRendering( C3DEngine * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			object->internal->getRenderLoop().resume();
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_endRendering( C3DEngine * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			object->internal->getRenderLoop().endRendering();
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_registerGuiCallbacks( C3DEngine * object, C3DGuiCallbacks * callbacks )
	{
		if ( !object || !callbacks )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			auto listener = object->internal->getUserInputListener();
			listener->registerClipboardTextAction( [callbacks]( bool set
				, c3d::U32String text )
				{
					if ( set )
					{
						if ( callbacks->onSetClipBoardText )
						{
							callbacks->onSetClipBoardText( callbacks, c3d::makeString( text ).c_str() );
						}
					}
					else
					{
						if ( callbacks->onGetClipBoardText )
						{
							C3DString result;
							callbacks->onGetClipBoardText( callbacks, &result );
							text = c3d::toUtf8U32String( c3d::makeString( result ) );
						}
					}

					return text;
				} );
			listener->registerCursorAction( [callbacks]( c3d::MouseCursor cursor )
				{
					if ( callbacks->onCursorChange )
					{
						callbacks->onCursorChange( callbacks, C3D_MOUSE_CURSOR( cursor ) );
					}
				} );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_unregisterGuiCallbacks( C3DEngine * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			auto listener = object->internal->getUserInputListener();
			listener->unregisterCursorAction();
			listener->unregisterClipboardTextAction();
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_createOverlay( C3DEngine const * object, C3D_OVERLAY_TYPE type, C3DString name, C3DOverlay * parent, C3DOverlay ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			c3d::OverlayUPtr res;
			res = object->internal->createOverlay( c3d::makeString( name )
				, *object->internal
				, c3d::OverlayType( type )
				, parent ? parent->getInternal() : nullptr );
			C3D_SafeAlloc( *result, C3DOverlay );
			( *result )->setInternal( c3d::move( res ) );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_createRenderWindow( C3DEngine const * object, C3DString name, C3DSize const * size, C3DWindowHandle handle, C3DRenderWindow ** result )
	{
		if ( !object || !result || !size )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			auto window = c3d::makeUnique< c3d::RenderWindow >( c3d::makeString( name )
				, *object->internal
				, c3d::Size{ size->width, size->height }
#if defined( _WIN32 )
				, ashes::WindowHandle( c3d::makeRawUnique< ashes::IMswWindowHandle >( ::GetModuleHandle( nullptr ), reinterpret_cast< HWND >( handle.hWnd ) ) ) );
#elif defined( __linux__ )
				, ashes::WindowHandle( c3d::makeRawUnique< ashes::IXWindowHandle >( handle.drawable, handle.display ) ) );
#elif defined( __APPLE__ )
				, ashes::WindowHandle( c3d::makeRawUnique< ashes::IMacOsWindowHandle >( handle.view ) ) );
#endif
			C3D_SafeAlloc( *result, C3DRenderWindow );
			( *result )->internal = c3d::move( window );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_createSampler( C3DEngine const * object, C3DString name, C3DSampler ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			auto res = object->internal->createSampler( c3d::makeString( name ), *object->internal );
			C3D_SafeAlloc( *result, C3DSampler );
			( *result )->setInternal( c3d::move( res ) );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_createScene( C3DEngine const * object, C3DString name, C3DScene ** result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );

		try
		{
			auto res = object->internal->createScene( c3d::makeString( name ), *object->internal );
			C3D_SafeAlloc( *result, C3DScene );
			( *result )->setInternal( c3d::move( res ) );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dEngine_createSkybox( C3DEngine const * object, C3DScene * scene, C3DSkybox ** result )
	{
		if ( !object || !scene || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGINE );
		if ( !scene->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_ENGSCN );

		try
		{
			auto res = c3d::makeUnique< c3d::SkyboxBackground >( *object->internal, *scene->getInternal() );
			C3D_SafeAlloc( *result, C3DSkybox );
			( *result )->internal = c3d::move( res );
		}
		C3D_CatchCommonExceptions()

			return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3d_getPluginsDirectory( C3DChar * result, size_t resultSize )
	{
		if ( !result )
			return C3D_POINTER;

		cc3d::copyStringN( c3d::Engine::getPluginsDirectory(), result, resultSize );
		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3d_getEngineDirectory( C3DChar * result, size_t resultSize )
	{
		if ( !result )
			return C3D_POINTER;

		cc3d::copyStringN( c3d::Engine::getEngineDirectory(), result, resultSize );
		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3d_getDataDirectory( C3DChar * result, size_t resultSize )
	{
		if ( !result )
			return C3D_POINTER;

		cc3d::copyStringN( c3d::Engine::getDataDirectory(), result, resultSize );
		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
