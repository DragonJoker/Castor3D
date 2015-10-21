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
#include "Viewport.hpp"

#include <Debug.hpp>

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

RenderSystem::RenderSystem( Engine & p_engine, eRENDERER_TYPE p_eRendererType )
	: OwnedBy< Engine >( p_engine )
	, m_eRendererType( p_eRendererType )
	, m_bInitialised( false )
	, m_bStereoAvailable( false )
	, m_bInstancing( false )
	, m_bAccumBuffer( false )
	, m_bNonPowerOfTwoTextures( false )
	, m_pCurrentContext( nullptr )
	, m_pCurrentCamera( nullptr )
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

void RenderSystem::PushScene( Scene * p_scene )
{
	m_stackScenes.push( p_scene );
}

void RenderSystem::PopScene()
{
	m_stackScenes.pop();
}

Scene * RenderSystem::GetTopScene()
{
	Scene * l_return = NULL;

	if ( m_stackScenes.size() )
	{
		l_return = m_stackScenes.top();
	}

	return l_return;
}

ShaderProgramBaseSPtr RenderSystem::CreateShaderProgram( eSHADER_LANGUAGE p_eLanguage )
{
	auto l_lock = Castor::make_unique_lock( m_mutex );

	return CreateShaderProgram();
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

	std::stringstream l_ptr;
	l_ptr.width( 16 );
	l_ptr.fill( '0' );
	l_ptr << std::hex << std::right << uint64_t( p_object );
	std::stringstream l_type;
	l_type.width( 20 );
	l_type << std::left << p_type;
	std::stringstream l_name;
	l_name << "(" << m_id << ") " << l_type.str() << " [0x" << l_ptr.str() << "]";

	if ( l_return )
	{
		StringStream l_stream;
		Debug::ShowBacktrace( l_stream );
		m_allocated.push_back( { ++m_id, p_type, p_object, p_file, p_line, 1, l_stream.str() } );
		Castor::Logger::LogDebug( l_name );
		p_name = l_name.str();
	}
	else
	{
		if ( l_it->m_ref > 0 )
		{
			Castor::Logger::LogDebug( std::stringstream() << "Rereferencing object: " << l_type.str() << " [0x" << l_ptr.str() << "] => " << l_it->m_ref );
		}
		else
		{
			Castor::Logger::LogDebug( l_name );
		}

		++l_it->m_ref;
	}

	return l_return;
}

bool RenderSystem::DoTrack( Named * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name )
{
	return DoTrack( reinterpret_cast< void * >( p_object ), p_type + ": " + string::string_cast< char >( p_object->GetName() ), p_file, p_line, p_name );
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
		std::stringstream l_type;
		l_type.width( 20 );
		l_type << std::left << l_it->m_name;

		if ( !--l_it->m_ref )
		{
			p_declaration = *l_it;
			l_return = true;
			Castor::Logger::LogWarning( std::stringstream() << "Released " << l_type.str() << " [0x" << l_ptr.str() << "] => " << p_declaration.m_ref );
		}
		else if ( l_it->m_ref < 0 )
		{
			Castor::Logger::LogError( std::stringstream() << "Trying to release an already released object: " << l_type.str() << " [0x" << l_ptr.str() << "] => " << l_it->m_ref );
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
		if ( l_decl.m_ref > 0 )
		{
			std::stringstream l_stream;
			l_stream << "Leaked 0x" << std::hex << l_decl.m_object << std::dec << " (" << l_decl.m_name << "), from file " << l_decl.m_file << ", line " << l_decl.m_line << std::endl;
			l_stream << string::string_cast< char >( l_decl.m_stack ) << std::endl;
			Castor::Logger::LogError( l_stream.str() );
		}
	}
}

#endif
