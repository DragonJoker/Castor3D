#include "MainWindow.h"

#include <GLFW/glfw3.h>
#if defined( _WIN32 )
#	include <Windows.h>
#else
#	include <dirent.h>
#	define min(a, b) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif
#include <tinyfiledialogs.h>
#include <string.h>

static const uint32_t windowWidth = 1024u;
static const uint32_t windowHeight = 768u;

bool doLoadPlugins( C3DEngine * engine );
bool doSelectSceneFile( C3DChar * result, size_t resultSize );
bool doLoadScene( C3DEngine * engine, MainWindow * window, C3DString fileName );
void doUnloadScene( C3DEngine * engine, MainWindow * window );

void onGetClipBoardText( C3DGuiCallbacks *, C3DString * text );
void onSetClipBoardText( C3DGuiCallbacks *, C3DString text );
void onCursorChange( C3DGuiCallbacks *, C3D_MOUSE_CURSOR cursor );

static GLFWcursor * cursors[C3D_MOUSE_CURSOR_COUNT];
static GLFWwindow * glfwMainWindow = NULL;

int main( int argc, char const * const * argv )
{
	C3DChar sceneFile[MAX_PATH_SIZE] = { 0 };
	clock_t current = { 0 };
	C3DGuiCallbacks callbacks = { 0 };
	MainWindow window = { 0 };
	C3DLogger * logger = NULL;
	C3DEngine * engine = NULL;
	int result = 0;

	c3dLogger_create( C3D_LOG_TYPE_DEBUG, &logger );
	c3dLogger_setFileName( logger, "CCastorViewer.log", C3D_LOG_TYPE_COUNT );
	c3dLogger_logInfo( logger, "CCastorViewer - Start" );

	if ( c3dEngine_create( "CastorViewerSharp", true, &engine ) != C3D_OK )
		goto cleanup;

	if ( !doLoadPlugins( engine ) )
		goto cleanup;

	callbacks.onCursorChange = onCursorChange;
	callbacks.onGetClipBoardText = onGetClipBoardText;
	callbacks.onSetClipBoardText = onSetClipBoardText;
	if ( c3dEngine_registerGuiCallbacks( engine, &callbacks ) != C3D_OK )
		goto cleanup;

	if ( !doSelectSceneFile( sceneFile, MAX_PATH_SIZE * sizeof( C3DChar ) ) )
		goto cleanup;

	glfwInit();
	cursors[C3D_MOUSE_CURSOR_ARROW] = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
	cursors[C3D_MOUSE_CURSOR_HAND] = glfwCreateStandardCursor( GLFW_POINTING_HAND_CURSOR );
	cursors[C3D_MOUSE_CURSOR_TEXT] = glfwCreateStandardCursor( GLFW_IBEAM_CURSOR );
	cursors[C3D_MOUSE_CURSOR_SIZE_WE] = glfwCreateStandardCursor( GLFW_RESIZE_EW_CURSOR );
	cursors[C3D_MOUSE_CURSOR_SIZE_NS] = glfwCreateStandardCursor( GLFW_RESIZE_NS_CURSOR );
	cursors[C3D_MOUSE_CURSOR_SIZE_NWSE] = glfwCreateStandardCursor( GLFW_RESIZE_NWSE_CURSOR );
	cursors[C3D_MOUSE_CURSOR_SIZE_NESW] = glfwCreateStandardCursor( GLFW_RESIZE_NESW_CURSOR );
	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	glfwMainWindow = glfwCreateWindow( ( int )windowWidth, ( int )windowHeight, "CCastorViewer", NULL, NULL );

	windowCreate( glfwMainWindow, engine, &window );

	if ( doLoadScene( engine, &window, sceneFile ) )
	{
		while ( !glfwWindowShouldClose( glfwMainWindow ) )
		{
			glfwPollEvents();
			current = clock();
			for ( TIMER_ID i = IDT_MOTION; i < IDT_COUNT; ++i )
			{
				if ( window.timers[i].started
					&& ( ( ( double )( current - window.timers[i].startTime ) ) * 1000.0 ) / CLOCKS_PER_SEC >= 30.0 )
				{
					window.timers[i].callback( &window );
					window.timers[i].startTime = current;
				}
			}
		}

		doUnloadScene( engine , &window );
		windowDestroy( &window );
	}

	glfwDestroyCursor( cursors[C3D_MOUSE_CURSOR_ARROW] );
	glfwDestroyCursor( cursors[C3D_MOUSE_CURSOR_HAND] );
	glfwDestroyCursor( cursors[C3D_MOUSE_CURSOR_TEXT] );
	glfwDestroyCursor( cursors[C3D_MOUSE_CURSOR_SIZE_WE] );
	glfwDestroyCursor( cursors[C3D_MOUSE_CURSOR_SIZE_NS] );
	glfwDestroyCursor( cursors[C3D_MOUSE_CURSOR_SIZE_NWSE] );
	glfwDestroyCursor( cursors[C3D_MOUSE_CURSOR_SIZE_NESW] );

	glfwTerminate();

	goto cleanup;

end:
	return result;

cleanup:
	if ( engine )
	{
		c3dEngine_cleanup( engine );
		c3dEngine_delete( engine );
		engine = NULL;
	}

	c3dLogger_logInfo( logger, "CCastorViewer - Exit" );
	c3dLogger_delete( logger );

	goto end;
}

static bool stringEndsWith( char const * const val, size_t valLen, char const * const suffix, size_t suffixSize )
{
	size_t suffixStart;
	size_t suffixLen = strnlen( suffix, suffixSize );
	if ( suffixLen > valLen )
		return false;

	suffixStart = valLen - suffixLen;
	for ( size_t i = 0; i < suffixLen; ++i )
	{
		if ( val[suffixStart + i] != suffix[i] )
			return false;
	}

	return true;
}

bool doLoadPlugins( C3DEngine * engine )
{
	C3DChar path[MAX_PATH_SIZE] = { 0 };
	C3DChar fullPath[MAX_PATH_SIZE] = { 0 };
	size_t remainingSize = 0;
	size_t pathSize = 0;
	size_t len = 0;
#ifdef _WIN32
	C3DChar lookupPath[MAX_PATH_SIZE] = { 0 };
#else
	DIR * dir = NULL;
	struct dirent * dirent = NULL;
#endif

	c3d_getPluginsDirectory( path, MAX_PATH_SIZE );
	pathSize = strnlen( path, MAX_PATH_SIZE );

#ifdef _WIN32

	strncpy_s( lookupPath, MAX_PATH_SIZE * sizeof( C3DChar ), path, pathSize );
	strncat_s( lookupPath, MAX_PATH_SIZE * sizeof( C3DChar ), "\\*", 2U );

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFileA( lookupPath, &ffd );
	if ( INVALID_HANDLE_VALUE == hFind )
		return false;

	do
	{
		len = strnlen( ffd.cFileName, MAX_PATH_SIZE );
		if ( ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0
			|| len >= MAX_PATH_SIZE
			|| pathSize + 1U + len >= MAX_PATH_SIZE
			|| !stringEndsWith( ffd.cFileName, len, ".dll", 4U ) )
			continue;

		memset( fullPath, 0, MAX_PATH_SIZE );
		remainingSize = MAX_PATH_SIZE;
		strncpy_s( fullPath, MAX_PATH_SIZE * sizeof( C3DChar ), path, min( pathSize, remainingSize ) );
		remainingSize -= pathSize;
		strncat_s( fullPath, MAX_PATH_SIZE * sizeof( C3DChar ), "\\", min( 1U, remainingSize ) );
		remainingSize -= 1U;
		strncat_s( fullPath, MAX_PATH_SIZE * sizeof( C3DChar ), ffd.cFileName, min( len, remainingSize ) );
		c3dEngine_loadPlugin( engine, fullPath );
	}
	while ( FindNextFile( hFind, &ffd ) != 0 );
	FindClose( hFind );

#elif __linux__

	if ( ( dir = opendir( path ) ) == NULL )
		return false;

	while ( ( dirent = readdir( dir ) ) != NULL )
	{
		len = strnlen( dirent->d_name, 256 );
		if ( dirent->d_type == DT_DIR
			|| len >= MAX_PATH_SIZE
			|| pathSize + 1U + len >= MAX_PATH_SIZE
			|| !stringEndsWith( dirent->d_name, len, ".so", 3U ) )
			continue;

		memset( fullPath, 0, MAX_PATH_SIZE );
		remainingSize = MAX_PATH_SIZE;
		strncpy( fullPath, path, min( pathSize, remainingSize ) );
		remainingSize -= pathSize;
		strncat( fullPath, "/", min( 1U, remainingSize ) );
		remainingSize -= 1U;
		strncat( fullPath, dirent->d_name, min( len, remainingSize ) );
		c3dEngine_loadPlugin( engine, fullPath );
	}

	closedir( dir );
	
#elif __APPLE__

	if ( ( dir = opendir( path ) ) == NULL )
		return false;

	while ( ( dirent = readdir( dir ) ) != NULL )
	{
		len = strnlen( dirent->d_name, 256 );
		if ( dirent->d_type == DT_DIR
			|| len >= MAX_PATH_SIZE
			|| pathSize + 1U + len >= MAX_PATH_SIZE
			|| !stringEndsWith( dirent->d_name, len, ".dylib", 6U ) )
			continue;

		memset( fullPath, 0, MAX_PATH_SIZE );
		remainingSize = MAX_PATH_SIZE;
		strncpy( fullPath, path, min( pathSize, remainingSize ) );
		remainingSize -= pathSize;
		strncat( fullPath, "/", min( 1U, remainingSize ) );
		remainingSize -= 1U;
		strncat( fullPath, dirent->d_name, min( len, remainingSize ) );
		c3dEngine_loadPlugin( engine, fullPath );
	}

	closedir( dir );

#endif

	return c3dEngine_loadRenderer( engine, "vk" ) == C3D_OK
		&& c3dEngine_initialise( engine, 1000, 1 ) == C3D_OK;
}

bool doSelectSceneFile( C3DChar * result, size_t resultSize )
{
	size_t len = 0;
	char const * filterPatterns[2] = { "*.cscn", "*.zip" };
	const char * selectedFile = tinyfd_openFileDialog( "Select a scene file to load"
		, NULL
		, 2, filterPatterns, "Scene files"
		, 0 );

	if ( selectedFile )
	{
		len = strnlen( selectedFile, MAX_PATH_SIZE );
		if ( len > 0 && len < MAX_PATH_SIZE )
		{
			strncpy( result, selectedFile, resultSize < len ? resultSize : len );
			return true;
		}
	}

	return false;
}

bool doLoadScene( C3DEngine * engine, MainWindow * window, C3DString filename )
{
	if ( window->scene != NULL )
		doUnloadScene( engine, window );

	c3dEngine_startRendering( engine );

	if ( c3dEngine_loadScene( engine, filename, &window->renderTarget ) != C3D_OK
		|| window->renderTarget == NULL )
		return false;

	if ( c3dRenderWindow_initialise( window->renderWindow, window->renderTarget ) != C3D_OK
		|| c3dRenderTarget_getScene( window->renderTarget, &window->scene ) != C3D_OK
		|| c3dRenderTarget_getCamera( window->renderTarget, &window->camera ) != C3D_OK
		|| c3dCamera_getNode( window->camera, &window->cameraNode ) != C3D_OK )
		return false;

	window->nodeState = stateCreate( window->cameraNode );

	return true;
}

void doUnloadScene( C3DEngine * engine, MainWindow * window )
{
	c3dEngine_endRendering( engine );
	stateDestroy( &window->nodeState );

	if ( window->renderWindow )
	{
		c3dRenderWindow_cleanup( window->renderWindow );
	}

	if ( window->cameraNode )
	{
		c3dSceneNode_delete( window->cameraNode );
		window->cameraNode = NULL;
	}

	if ( window->camera )
	{
		c3dCamera_delete( window->camera );
		window->camera = NULL;
	}

	if ( window->scene )
	{
		c3dScene_delete( window->scene );
		window->scene = NULL;
	}

	if ( window->renderTarget )
	{
		c3dRenderTarget_delete( window->renderTarget );
		window->renderTarget = NULL;
	}
}

void onGetClipBoardText( C3DGuiCallbacks * callbacks, C3DString * text )
{
	//return Clipboard.GetText();
}

void onSetClipBoardText( C3DGuiCallbacks * callbacks, C3DString text )
{
	//Clipboard.SetText(text);
}

void onCursorChange( C3DGuiCallbacks * callbacks, C3D_MOUSE_CURSOR cursor )
{
	if ( glfwMainWindow )
		glfwSetCursor( glfwMainWindow, cursors[cursor] );
}
