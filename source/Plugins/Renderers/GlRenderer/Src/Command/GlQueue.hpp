/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Command/Queue.hpp>

namespace gl_renderer
{
	class Queue
		: public renderer::Queue
	{
	public:
		Queue();
		/**
		*\copydoc		renderer::Queue::submit
		*/
		bool submit( renderer::CommandBufferCRefArray const & commandBuffers
			, renderer::SemaphoreCRefArray const & semaphoresToWait
			, renderer::PipelineStageFlagsArray const & semaphoresStage
			, renderer::SemaphoreCRefArray const & semaphoresToSignal
			, renderer::Fence const * fence )const override;
		/**
		*\copydoc		renderer::Queue::present
		*/
		bool present( renderer::SwapChainCRefArray const & swapChains
			, renderer::UInt32Array const & imagesIndex
			, renderer::SemaphoreCRefArray const & semaphoresToWait )const override;
		/**
		*\copydoc		renderer::Queue::waitIdle
		*/
		bool waitIdle()const override;
		/**
		*\copydoc		renderer::Queue::getFamilyIndex
		*/
		inline uint32_t getFamilyIndex()const override
		{
			return 0u;
		}
	};
}
