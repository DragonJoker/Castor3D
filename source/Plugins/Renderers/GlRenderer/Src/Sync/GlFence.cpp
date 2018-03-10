/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Sync/GlFence.hpp"

#include "Core/GlDevice.hpp"

namespace gl_renderer
{
	enum GlFenceWaitResult
	{
		GL_WAIT_RESULT_ALREADY_SIGNALED = 0x911A,
		GL_WAIT_RESULT_CONDITION_SATISFIED = 0x911C,
		GL_WAIT_RESULT_TIMEOUT_EXPIRED = 0x911B,
	};

	Fence::Fence( renderer::Device const & device
		, renderer::FenceCreateFlags flags )
		: renderer::Fence{ device, flags }
	{
	}

	Fence::~Fence()
	{
		if ( m_fence )
		{
			glLogCall( gl::DeleteSync, m_fence );
		}
	}

	renderer::WaitResult Fence::wait( uint32_t timeout )const
	{
		if ( !m_fence )
		{
			m_fence = glLogCall( gl::FenceSync, GL_WAIT_FLAG_SYNC_GPU_COMMANDS_COMPLETE, 0u );
		}

		auto res = glLogCall( gl::ClientWaitSync, m_fence, GL_WAIT_FLAG_SYNC_FLUSH_COMMANDS_BIT, timeout );
		return ( res == GL_WAIT_RESULT_ALREADY_SIGNALED || res == GL_WAIT_RESULT_CONDITION_SATISFIED )
			? renderer::WaitResult::eSuccess
			: ( res == GL_WAIT_RESULT_TIMEOUT_EXPIRED
				? renderer::WaitResult::eTimeOut
				: renderer::WaitResult::eError );
	}

	void Fence::reset()const
	{
		if ( m_fence )
		{
			glLogCall( gl::DeleteSync, m_fence );
			m_fence = nullptr;
		}
	}
}
