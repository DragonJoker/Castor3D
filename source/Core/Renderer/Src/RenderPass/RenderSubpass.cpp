/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "RenderPass/RenderSubpass.hpp"

#include "Core/Device.hpp"
#include "RenderPass/RenderSubpassState.hpp"

namespace renderer
{
	RenderSubpass::RenderSubpass( PipelineBindPoint pipelineBindPoint
		, RenderSubpassState const & state
		, AttachmentReferenceArray const & inputAttaches
		, AttachmentReferenceArray const & colourAttaches
		, AttachmentReferenceArray const & resolveAttaches
		, AttachmentReference const * depthAttach
		, UInt32Array const & reserveAttaches )
		: m_description{
			0u,
			pipelineBindPoint,
			inputAttaches,
			colourAttaches,
			resolveAttaches,
			std::nullopt,
			reserveAttaches
		}
		, m_neededState{ state }
	{
		if ( depthAttach )
		{
			m_description.depthStencilAttachment = *depthAttach;
		}
	}

	RenderSubpass::RenderSubpass( PipelineBindPoint pipelineBindPoint
		, RenderSubpassState const & state
		, AttachmentReferenceArray const & colourAttaches )
		: RenderSubpass( pipelineBindPoint
			, state
			, AttachmentReferenceArray{}
			, colourAttaches
			, AttachmentReferenceArray{}
			, nullptr
			, UInt32Array{} )
	{
	}

	RenderSubpass::RenderSubpass( PipelineBindPoint pipelineBindPoint
		, RenderSubpassState const & state
		, AttachmentReferenceArray const & colourAttaches
		, AttachmentReference const & depthAttach )
		: RenderSubpass( pipelineBindPoint
			, state
			, AttachmentReferenceArray{}
			, colourAttaches
			, AttachmentReferenceArray{}
			, &depthAttach
			, UInt32Array{} )
	{
	}
}
