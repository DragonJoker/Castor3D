#include "Castor3D/Render/RenderLoopSync.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Design/BlockGuard.hpp>

namespace castor3d
{
	namespace rendlpsnc
	{
		static const char * CALL_START_RENDERING = "Can't call beginRendering in a synchronous render loop";
		static const char * CALL_END_RENDERING = "Can't call endRendering in a synchronous render loop";
		static const char * CALL_PAUSE_RENDERING = "Can't call Pause in a synchronous render loop";
		static const char * CALL_RESUME_RENDERING = "Can't call Resume in a synchronous render loop";
		static const char * RLS_UNKNOWN_EXCEPTION = "Unknown exception";
	}

	RenderLoopSync::RenderLoopSync( Engine & engine
		, uint32_t wantedFPS )
		: RenderLoop( engine, wantedFPS, true )
		, m_active( true )
	{
	}

	RenderLoopSync::~RenderLoopSync()
	{
		cleanup();
	}

	void RenderLoopSync::beginRendering()
	{
		CU_Exception( rendlpsnc::CALL_START_RENDERING );
	}

	void RenderLoopSync::renderSyncFrame( castor::Milliseconds tslf )
	{
		if ( m_active )
		{
			try
			{
				doRenderFrame( tslf );
			}
			catch ( castor::Exception & exc )
			{
				log::error << exc.getFullDescription() << std::endl;
				m_active = false;
			}
			catch ( std::exception & exc )
			{
				log::error << exc.what() << std::endl;
				m_active = false;
			}
			catch ( ... )
			{
				log::error << rendlpsnc::RLS_UNKNOWN_EXCEPTION << std::endl;
				m_active = false;
			}
		}
	}

	void RenderLoopSync::pause()
	{
		CU_Exception( rendlpsnc::CALL_PAUSE_RENDERING );
	}

	void RenderLoopSync::resume()
	{
		CU_Exception( rendlpsnc::CALL_RESUME_RENDERING );
	}

	void RenderLoopSync::endRendering()
	{
		CU_Exception( rendlpsnc::CALL_END_RENDERING );
	}
}
