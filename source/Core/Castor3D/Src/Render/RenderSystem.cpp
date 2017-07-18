#include "RenderSystem.hpp"

#include "Render/Context.hpp"

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

#if C3D_TRACE_OBJECTS

		m_tracker.ReportTracked();

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
		Scene * result = nullptr;

		if ( m_stackScenes.size() )
		{
			result = m_stackScenes.top();
		}

		return result;
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
		Context * result{ nullptr };
		auto it = m_currentContexts.find( std::this_thread::get_id() );

		if ( it != m_currentContexts.end() )
		{
			result = it->second;
		}

		return result;
	}
}
