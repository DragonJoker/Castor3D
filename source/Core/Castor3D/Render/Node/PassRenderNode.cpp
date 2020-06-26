#include "Castor3D/Render/Node/PassRenderNode.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		void doBindTexture( TextureUnit const & texture
			, ashes::DescriptorSetLayout const & layout
			, ashes::DescriptorSet & descriptorSet
			, uint32_t index
			, uint32_t & texIndex )
		{
			descriptorSet.createBinding( layout.getBinding( index )
				, texture.getTexture()->getDefaultView().getSampledView()
				, texture.getSampler()->getSampler()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, texIndex++ );
		}

		void doBindTexture( TextureUnit const & texture
			, ashes::DescriptorSetLayout const & layout
			, ashes::WriteDescriptorSet & write )
		{
			write.imageInfo.push_back( 
				{
					texture.getSampler()->getSampler(),
					texture.getTexture()->getDefaultView().getSampledView(),
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				} );
		}
	}

	PassRenderNode::PassRenderNode( Pass & pass )
		: pass{ pass }
	{
	}

	void PassRenderNode::fillDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, ashes::DescriptorSet & descriptorSet
		, TextureFlagsArray const & mask )
	{
		fillDescriptor( layout, index, descriptorSet, merge( mask ) );
	}

	void PassRenderNode::fillDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, ashes::WriteDescriptorSetArray & writes
		, TextureFlagsArray const & mask )
	{
		fillDescriptor( layout, index, writes, merge( mask ) );
	}

	void PassRenderNode::fillDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, ashes::DescriptorSet & descriptorSet
		, TextureFlags mask )
	{
		CU_Assert( mask, "Unexpected empty mask ?" );
		uint32_t texIndex = 0u;
		auto units = pass.getTextureUnits( mask );

		for ( auto & unit : units )
		{
			doBindTexture( *unit
				, layout
				, descriptorSet
				, index
				, texIndex );
		}

		++index;
	}

	void PassRenderNode::fillDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, ashes::WriteDescriptorSetArray & writes
		, TextureFlags mask )
	{
		CU_Assert( mask, "Unexpected empty mask ?" );
		auto units = pass.getTextureUnits( mask );
		auto count = uint32_t( units.size() );

		if ( count )
		{
			ashes::WriteDescriptorSet write
			{
				index,
				0u,
				count,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			};

			for ( auto & unit : units )
			{
				doBindTexture( *unit
					, layout
					, write );
			}

			writes.push_back( write );
			index += count;
		}
	}
}
