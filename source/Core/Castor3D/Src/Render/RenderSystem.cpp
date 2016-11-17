#include "RenderSystem.hpp"

#include "Engine.hpp"
#include "ShaderCache.hpp"

#include "Pipeline.hpp"
#include "Viewport.hpp"

#include "Event/Frame/CleanupEvent.hpp"
#include "Overlay/Overlay.hpp"
#include "Overlay/OverlayRenderer.hpp"
#include "Plugin/Plugin.hpp"
#include "Mesh/Submesh.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Shader/ShaderObject.hpp"
#include "Shader/FrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderSystem::RenderSystem( Engine & p_engine, String const & p_name )
		: OwnedBy< Engine >{ p_engine }
		, m_name{ p_name }
		, m_initialised{ false }
		, m_gpuInformations{}
	{
	}

	RenderSystem::~RenderSystem()
	{
		m_mainContext.reset();
	}

	void RenderSystem::Initialise( GpuInformations && p_informations )
	{
		m_gpuInformations = std::move( p_informations );
		DoInitialise();
		m_gpuInformations.UpdateMaxShaderModel();
		REQUIRE( m_gpuInformations.GetMaxShaderModel() >= ShaderModel::eModel2 );
		Logger::LogInfo( cuT( "Vendor: " ) + m_gpuInformations.GetVendor() );
		Logger::LogInfo( cuT( "Renderer: " ) + m_gpuInformations.GetRenderer() );
		Logger::LogInfo( cuT( "Version: " ) + m_gpuInformations.GetVersion() );
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

	GLSL::GlslWriter RenderSystem::CreateGlslWriter()
	{
		return GLSL::GlslWriter{ GLSL::GlslWriterConfig{ m_gpuInformations.GetShaderLanguageVersion(), m_gpuInformations.HasConstantsBuffers(), m_gpuInformations.HasTextureBuffers() } };
	}

	void RenderSystem::SetCurrentContext( Context * p_context )
	{
		m_currentContexts[std::this_thread::get_id()] = p_context;
	}

	Context * RenderSystem::GetCurrentContext()
	{
		Context * l_return{ nullptr };
		auto l_it = m_currentContexts.find( std::this_thread::get_id() );

		if ( l_it != m_currentContexts.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

#if C3D_TRACE_OBJECTS

	void RenderSystem::DoReportTracked()
	{
		for ( auto const & l_decl : m_allocated )
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
