#include "RenderSystem.hpp"
#include "CleanupEvent.hpp"
#include "FrameVariableBuffer.hpp"
#include "PointFrameVariable.hpp"
#include "Pipeline.hpp"
#include "Engine.hpp"
#include "Plugin.hpp"
#include "ShaderManager.hpp"
#include "ShaderProgram.hpp"
#include "FrameVariable.hpp"
#include "Submesh.hpp"
#include "Sampler.hpp"
#include "Overlay.hpp"
#include "OverlayRenderer.hpp"
#include "ShaderPlugin.hpp"
#include "Viewport.hpp"

#include <Debug.hpp>

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

RenderSystem::RenderSystem( Engine * p_pEngine, eRENDERER_TYPE p_eRendererType )
	: m_pEngine( p_pEngine )
	, m_useMultiTexturing( false )
	, m_bInitialised( false )
	, m_useShaders( false )
	, m_forceShaders( false )
	, m_eRendererType( p_eRendererType )
	, m_bStereoAvailable( false )
	, m_pCurrentContext( NULL )
	, m_pCurrentCamera( NULL )
	, m_bInstancing( false )
	, m_bAccumBuffer( false )
	, m_bNonPowerOfTwoTextures( false )
{
	m_useShader[eSHADER_TYPE_VERTEX] = false;
	m_useShader[eSHADER_TYPE_PIXEL] = false;
	m_useShader[eSHADER_TYPE_GEOMETRY] = false;
	m_useShader[eSHADER_TYPE_HULL] = false;
	m_useShader[eSHADER_TYPE_DOMAIN] = false;

	m_pipeline = std::make_unique< Pipeline >( *this );
}

RenderSystem::~RenderSystem()
{
	m_pipeline.reset();
}

void RenderSystem::Initialise()
{
	DoInitialise();
}

void RenderSystem::Cleanup()
{
	DoCleanup();

#if !defined( NDEBUG )

	DoReportTracked();

#endif
}

void RenderSystem::RenderAmbientLight( Castor::Colour const & p_clColour, FrameVariableBuffer & p_variableBuffer )
{
	Point4fFrameVariableSPtr l_pVariable;
	p_variableBuffer.GetVariable( ShaderProgramBase::AmbientLight, l_pVariable );
	Point4f l_ptColour;
	p_clColour.to_rgba( l_ptColour );
	l_pVariable->SetValue( l_ptColour );
}

void RenderSystem::PushScene( Scene * p_pScene )
{
	m_stackScenes.push( p_pScene );
}

void RenderSystem::PopScene()
{
	m_stackScenes.pop();
}

Scene * RenderSystem::GetTopScene()
{
	Scene * l_pReturn = NULL;

	if ( m_stackScenes.size() )
	{
		l_pReturn = m_stackScenes.top();
	}

	return l_pReturn;
}

ShaderProgramBaseSPtr RenderSystem::CreateShaderProgram( eSHADER_LANGUAGE p_eLanguage )
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	ShaderProgramBaseSPtr l_pReturn;

	if ( p_eLanguage == eSHADER_LANGUAGE_GLSL || p_eLanguage == eSHADER_LANGUAGE_HLSL )
	{
		l_pReturn = CreateShaderProgram();
	}
	else
	{
		ShaderPluginSPtr l_pPlugin = m_pEngine->GetShaderPlugin( p_eLanguage );

		if ( l_pPlugin )
		{
			l_pReturn = l_pPlugin->CreateShader( this );
		}
	}

	return l_pReturn;
}

Camera * RenderSystem::GetCurrentCamera()const
{
	return m_pCurrentCamera;
}

void RenderSystem::SetCurrentCamera( Camera * p_pCamera )
{
	m_pCurrentCamera = p_pCamera;
}

#if !defined( NDEBUG )

bool RenderSystem::DoTrack( void * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name )
{
	auto && l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [p_object]( ObjectDeclaration const & l_object )
	{
		return p_object == l_object.m_object;
	} );

	bool l_return = l_it == m_allocated.end();

	if ( l_return )
	{
		StringStream l_stream;
		Debug::ShowBacktrace( l_stream );
		m_allocated.push_back( { ++m_id, p_type, p_object, p_file, p_line, 1, l_stream.str() } );
		std::stringstream l_ptr;
		l_ptr.width( 16 );
		l_ptr.fill( '0' );
		l_ptr << std::hex << std::right << uint64_t( p_object );
		std::stringstream l_type;
		l_type.width( 20 );
		l_type << std::left << p_type;
		std::stringstream l_name;
		l_name << "(" << m_id << ") " << l_type.str() << " [0x" << l_ptr.str() << "]";
		Castor::Logger::LogDebug( l_name );
		p_name = l_name.str();
	}
	else
	{
		++l_it->m_ref;
	}

	return l_return;
}

bool RenderSystem::DoTrack( Named * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name )
{
	return DoTrack( reinterpret_cast< void * >( p_object ), p_type + ": " + string::to_str( p_object->GetName() ), p_file, p_line, p_name );
}

bool RenderSystem::DoUntrack( void * p_object, ObjectDeclaration & p_declaration )
{
	auto l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [&p_object]( ObjectDeclaration p_decl )
	{
		return p_object == p_decl.m_object;
	} );

	bool l_return = false;
	char l_szName[1024] = { 0 };
	std::stringstream l_ptr;
	l_ptr.width( 16 );
	l_ptr.fill( '0' );
	l_ptr << std::hex << std::right << uint64_t( p_object );

	if ( l_it != m_allocated.end() )
	{
		if ( !--l_it->m_ref )
		{
			p_declaration = *l_it;
			m_allocated.erase( l_it );
			l_return = true;
			std::stringstream l_type;
			l_type.width( 20 );
			l_type << std::left << p_declaration.m_name;
			Castor::Logger::LogWarning( std::stringstream() << "Released " << l_type.str() << " [0x" << l_ptr.str() << "] => " << p_declaration.m_ref );
		}
	}
	else
	{
		Castor::Logger::LogWarning( std::stringstream() << "Untracked [0x" << l_ptr.str() << cuT( "]" ) );
	}

	return l_return;
}

bool RenderSystem::DoUntrack( Named * p_object, ObjectDeclaration & p_declaration )
{
	return DoUntrack( reinterpret_cast< void * >( p_object ), p_declaration );
}

void RenderSystem::DoReportTracked()
{
	for ( auto && l_decl : m_allocated )
	{
		std::stringstream l_stream;
		l_stream << "Leaked 0x" << std::hex << l_decl.m_object << std::dec << " (" << l_decl.m_name << "), from file " << l_decl.m_file << ", line " << l_decl.m_line << std::endl;
		l_stream << string::to_str( l_decl.m_stack ) << std::endl;
		Castor::Logger::LogError( l_stream.str() );
	}
}

#endif
