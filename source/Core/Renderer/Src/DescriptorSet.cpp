/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "DescriptorSet.hpp"

#include "DescriptorSetLayoutBinding.hpp"
#include "DescriptorSetPool.hpp"
#include "Sampler.hpp"
#include "Texture.hpp"

namespace renderer
{
	DescriptorSet::DescriptorSet( DescriptorSetPool const & pool )
	{
		pool.allocate( 1u );
	}
}
