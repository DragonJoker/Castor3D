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
				, texture.getTexture()->getDefaultView().getView()
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
					texture.getTexture()->getDefaultView().getView(),
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
		, TextureFlags mask )
	{
		uint32_t texIndex = 0u;

		if ( mask )
		{
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
		else
		{
			for ( auto & unit : pass )
			{
				doBindTexture( *unit
					, layout
					, descriptorSet
					, index
					, texIndex );
			}

			++index;
		}
	}

	void PassRenderNode::fillDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, ashes::WriteDescriptorSetArray & writes
		, TextureFlags mask )
	{
		uint32_t texIndex = 0u;

		if ( mask )
		{
			auto units = pass.getTextureUnits( mask );
			ashes::WriteDescriptorSet write
			{
				index,
				0u,
				uint32_t( units.size() ),
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			};

			for ( auto & unit : units )
			{
				doBindTexture( *unit
					, layout
					, write );
			}

			writes.push_back( write );
			index += uint32_t( units.size() );
		}
		else
		{
			auto count = pass.getNonEnvTextureUnitsCount();

			if ( count )
			{
				ashes::WriteDescriptorSet write
				{
					index,
					0u,
					count,
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				};

				for ( auto & unit : pass )
				{
					if ( unit->getConfiguration().environment == 0u )
					{
						doBindTexture( *unit
							, layout
							, write );
					}
				}

				writes.push_back( write );
				index += count;
			}
		}
	}
}
