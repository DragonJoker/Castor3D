#include "RenderSystem.hpp"

#include "Render/Context.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	RenderSystem::RenderSystem( Engine & engine, String const & p_name )
		: OwnedBy< Engine >{ engine }
		, m_name{ p_name }
		, m_initialised{ false }
		, m_gpuInformations{}
	{
	}

	RenderSystem::~RenderSystem()
	{
		m_mainContext.reset();
	}

	void RenderSystem::initialise( GpuInformations && p_informations )
	{
		m_gpuInformations = std::move( p_informations );
		doInitialise();
		m_gpuInformations.updateMaxShaderModel();
		REQUIRE( m_gpuInformations.getMaxShaderModel() >= ShaderModel::eModel2 );
		Logger::logInfo( cuT( "Vendor: " ) + m_gpuInformations.getVendor() );
		Logger::logInfo( cuT( "Renderer: " ) + m_gpuInformations.getRenderer() );
		Logger::logInfo( cuT( "Version: " ) + m_gpuInformations.getVersion() );
	}

	void RenderSystem::cleanup()
	{
		if ( m_mainContext )
		{
			m_mainContext->cleanup();
			m_mainContext.reset();
		}

		doCleanup();

#if C3D_TRACE_OBJECTS

		m_tracker.reportTracked();

#endif
	}

	void RenderSystem::pushScene( Scene * p_scene )
	{
		m_stackScenes.push( p_scene );
	}

	void RenderSystem::popScene()
	{
		m_stackScenes.pop();
	}

	Scene * RenderSystem::getTopScene()
	{
		Scene * result = nullptr;

		if ( m_stackScenes.size() )
		{
			result = m_stackScenes.top();
		}

		return result;
	}

	glsl::GlslWriter RenderSystem::createGlslWriter()
	{
		return glsl::GlslWriter{ glsl::GlslWriterConfig{ m_gpuInformations.getShaderLanguageVersion(), m_gpuInformations.hasConstantsBuffers(), m_gpuInformations.hasTextureBuffers() } };
	}

	void RenderSystem::setCurrentContext( Context * p_context )
	{
		m_currentContexts[std::this_thread::get_id()] = p_context;
	}

	Context * RenderSystem::getCurrentContext()
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
