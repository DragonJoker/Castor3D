#include "Castor3D/Render/PostEffect/PostEffect.hpp"

#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementSmartPtr( castor3d, PostEffect )

namespace castor3d
{
	PostEffect::PostEffect( castor::String const & name
		, castor::String const & groupName
		, castor::String const & fullName
		, RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & CU_UnusedParam( parameters )
		, uint32_t passesCount
		, Kind kind )
		: castor::OwnedBy< RenderSystem >{ renderSystem }
		, castor::Named{ name }
		, m_fullName{ fullName }
		, m_renderTarget{ renderTarget }
		, m_graph{ m_renderTarget.getGraph().createPassGroup( groupName ) }
		, m_passesCount{ passesCount }
		, m_kind{ kind }
		, m_enabled{ true }
	{
	}

	PostEffect::~PostEffect()
	{
	}

	bool PostEffect::writeInto( castor::StringStream & file, castor::String const & tabs )
	{
		return doWriteInto( file, tabs );
	}

	bool PostEffect::initialise( castor3d::RenderDevice const & device
		, Texture const & source
		, Texture const & target
		, crg::FramePass const & previousPass )
	{
		m_source = &source;
		return doInitialise( device
			, source
			, target
			, previousPass );
	}

	void PostEffect::cleanup( castor3d::RenderDevice const & device )
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
