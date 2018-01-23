#include "RendererPrerequisites.hpp"

namespace renderer
{
	bool isDepthStencilFormat( PixelFormat format )
	{
		return format == PixelFormat::eD24S8;
	}

	bool isStencilFormat( PixelFormat format )
	{
		return format == PixelFormat::eS8;
	}

	bool isDepthFormat( PixelFormat format )
	{
		return format == PixelFormat::eD16
			|| format == PixelFormat::eD32F;
	}

	ImageAspectFlags getAspectMask( renderer::PixelFormat format )
	{
		return isDepthStencilFormat( format )
			? renderer::ImageAspectFlags( renderer::ImageAspectFlag::eDepth | renderer::ImageAspectFlag::eStencil )
			: isDepthFormat( format )
				? renderer::ImageAspectFlags( renderer::ImageAspectFlag::eDepth )
				: isStencilFormat( format )
					? renderer::ImageAspectFlags( renderer::ImageAspectFlag::eStencil )
					: renderer::ImageAspectFlags( renderer::ImageAspectFlag::eColour );
	}
}
