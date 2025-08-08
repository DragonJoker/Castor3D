#include "Castor3D/Render/PostEffect/PostEffect.hpp"

#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementSmartPtr( c3d, PostEffect )

namespace c3d
{
	PostEffect::PostEffect( String const & name
		, String const & groupName
		, String const & fullName
		, RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, CU_UnusedParam( Parameters const &, parameters )
		, uint32_t passesCount
		, Kind kind )
		: OwnedBy< RenderSystem >{ renderSystem }
		, Named{ name }
		, m_fullName{ fullName }
		, m_renderTarget{ renderTarget }
		, m_graph{ m_renderTarget.getGraph().createPassGroup( toUtf8( groupName ) ) }
		, m_passesCount{ passesCount }
		, m_kind{ kind }
	{
	}

	bool PostEffect::writeInto( StringStream & file, String const & tabs )
	{
		return doWriteInto( file, tabs );
	}

	bool PostEffect::initialise( RenderDevice const & device
		, Texture const & source
		, Texture & target )
	{
		m_source = &source;
		return doInitialise( device
			, source
			, target );
	}

	void PostEffect::cleanup( RenderDevice const & device )
	{
		doCleanup( device );
	}

	bool PostEffect::update( CpuUpdater & updater
		, Texture const & source )
	{
		m_passIndex = ( &source == m_source ) ? 0u : 1u;
		doCpuUpdate( updater );
		return isEnabled();
	}

	void PostEffect::update( GpuUpdater & updater )
	{
		doGpuUpdate( updater );
	}

	void PostEffect::doCpuUpdate( CpuUpdater & updater )
	{
	}

	void PostEffect::doGpuUpdate( GpuUpdater & updater )
	{
	}
}
