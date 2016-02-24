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

using namespace Castor;

namespace Castor3D
{
	RenderSystem::RenderSystem( Engine & p_engine, eRENDERER_TYPE p_eRendererType )
		: OwnedBy< Engine >( p_engine )
		, m_rendererType( p_eRendererType )
		, m_initialised( false )
		, m_stereoAvailable( false )
		, m_instancing( false )
		, m_accumBuffer( false )
		, m_nonPowerOfTwoTextures( false )
		, m_currentContext( nullptr )
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
		m_mainContext.reset();
	}

	void RenderSystem::Initialise()
	{
		DoInitialise();
	}

	void RenderSystem::Cleanup()
	{
		if ( m_mainContext )
		{
			m_mainContext->Cleanup();
			m_mainContext.reset();
		}

		DoCleanup();

#if !defined( NDEBUG )

		DoReportTracked();

#endif
	}

	void RenderSystem::RenderAmbientLight( Castor::Colour const & p_clColour, FrameVariableBuffer & p_variableBuffer )
	{
		Point4fFrameVariableSPtr l_variable;
		p_variableBuffer.GetVariable( ShaderProgram::AmbientLight, l_variable );
		Point4f l_ptColour;
		p_clColour.to_rgba( l_ptColour );
		l_variable->SetValue( l_ptColour );
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
		Scene * l_return = nullptr;

		if ( m_stackScenes.size() )
		{
			l_return = m_stackScenes.top();
		}

		return l_return;
	}

	ShaderProgramSPtr RenderSystem::CreateShaderProgram( eSHADER_LANGUAGE p_eLanguage )
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

#if C3D_TRACE_OBJECTS

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

}
