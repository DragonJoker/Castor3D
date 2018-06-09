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
	namespace
	{
		void doBindTexture( TextureUnitSPtr texture
			, renderer::DescriptorSetLayout const & layout
			, renderer::DescriptorSet & descriptorSet
			, uint32_t & index )
		{
			if ( texture )
			{
				descriptorSet.createBinding( layout.getBinding( index++ )
					, texture->getTexture()->getDefaultView()
					, texture->getSampler()->getSampler() );
			}
		}

		void doBindTexture( TextureUnitSPtr texture
			, renderer::DescriptorSetLayout const & layout
			, renderer::WriteDescriptorSetArray & writes
			, uint32_t & index )
		{
			if ( texture )
			{
				writes.push_back( renderer::WriteDescriptorSet
					{
						index++,
						0u,
						1u,
						renderer::DescriptorType::eCombinedImageSampler,
						{
							{
								texture->getSampler()->getSampler(),
								texture->getTexture()->getDefaultView(),
								renderer::ImageLayout::eShaderReadOnlyOptimal
							},
						},
						{},
						{}
					} );
			}
		}
	}

	PassRenderNode::PassRenderNode( Pass & pass )
		: pass{ pass }
	{
	}

	void PassRenderNode::fillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, renderer::DescriptorSet & descriptorSet
		, bool opacityOnly )
	{
		if ( opacityOnly )
		{
			doBindTexture( pass.getTextureUnit( TextureChannel::eOpacity )
				, layout
				, descriptorSet
				, index );
		}
		else
		{
			doBindTexture( pass.getTextureUnit( TextureChannel::eDiffuse )
				, layout
				, descriptorSet
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eSpecular )
				, layout
				, descriptorSet
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eGloss )
				, layout
				, descriptorSet
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eNormal )
				, layout
				, descriptorSet
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eOpacity )
				, layout
				, descriptorSet
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eHeight )
				, layout
				, descriptorSet
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eAmbientOcclusion )
				, layout
				, descriptorSet
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eEmissive )
				, layout
				, descriptorSet
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eTransmittance )
				, layout
				, descriptorSet
				, index );
		}
	}

	void PassRenderNode::fillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, renderer::WriteDescriptorSetArray & writes
		, bool opacityOnly )
	{
		if ( opacityOnly )
		{
			doBindTexture( pass.getTextureUnit( TextureChannel::eOpacity )
				, layout
				, writes
				, index );
		}
		else
		{
			doBindTexture( pass.getTextureUnit( TextureChannel::eDiffuse )
				, layout
				, writes
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eSpecular )
				, layout
				, writes
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eGloss )
				, layout
				, writes
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eNormal )
				, layout
				, writes
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eOpacity )
				, layout
				, writes
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eHeight )
				, layout
				, writes
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eAmbientOcclusion )
				, layout
				, writes
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eEmissive )
				, layout
				, writes
				, index );
			doBindTexture( pass.getTextureUnit( TextureChannel::eTransmittance )
				, layout
				, writes
				, index );
		}
	}
}
