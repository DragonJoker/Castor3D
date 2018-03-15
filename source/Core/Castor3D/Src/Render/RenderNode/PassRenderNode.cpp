#include "PassRenderNode.hpp"

#include "Material/Pass.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Castor3DPrerequisites.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>

using namespace castor;

namespace castor3d
{
	PassRenderNode::PassRenderNode( Pass & pass )
		: pass{ pass }
	{
	}

	void PassRenderNode::fillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t index
		, renderer::DescriptorSet & descriptorSet )
	{
		for ( auto & texture : pass )
		{
			descriptorSet.createBinding( layout.getBinding( index++ )
				, texture->getTexture()->getDefaultView()
				, texture->getSampler()->getSampler() );
		}
	}
}
