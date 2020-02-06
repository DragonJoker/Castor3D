﻿#include "FrameBufferAttachment.hpp"

using namespace Castor;

namespace Castor3D
{
	FrameBufferAttachment::FrameBufferAttachment( AttachmentType p_type )
		: m_type( p_type )
		, m_point( AttachmentPoint::eNone )
		, m_index( 0 )
	{
	}

	FrameBufferAttachment::~FrameBufferAttachment()
	{
	}

	void FrameBufferAttachment::Attach( AttachmentPoint p_eAttachment )
	{
		Attach( p_eAttachment, 0 );
	}

	void FrameBufferAttachment::Attach( AttachmentPoint p_eAttachment
		, uint8_t p_index )
	{
		m_index = p_index;
		m_point = p_eAttachment;
		DoAttach();
	}

	void FrameBufferAttachment::Detach()
	{
		DoDetach();
		m_point = AttachmentPoint::eNone;
		m_index = 0;
	}

	void FrameBufferAttachment::Clear( BufferComponent p_component )const
	{
		REQUIRE( ( p_component == BufferComponent::eColour && m_point == AttachmentPoint::eColour )
			|| ( p_component == BufferComponent::eDepth
				&& ( m_point == AttachmentPoint::eDepth
					|| m_point == AttachmentPoint::eDepthStencil ) )
			|| ( p_component == BufferComponent::eStencil
				&& ( m_point == AttachmentPoint::eStencil
					|| m_point == AttachmentPoint::eDepthStencil ) ) );
		DoClear( p_component );
	}
}
