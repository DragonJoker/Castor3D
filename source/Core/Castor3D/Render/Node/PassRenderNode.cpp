#include "Castor3D/Render/RenderNode/PassRenderNode.hpp"

#include "Castor3D/Material/Pass.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Castor3DPrerequisites.hpp"
#include "Castor3D/Texture/Sampler.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

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
				, texture.getTexture()->getDefaultView()
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
					texture.getTexture()->getDefaultView(),
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
		, bool opacityOnly )
	{
		uint32_t texIndex = 0u;

		if ( opacityOnly )
		{
			auto it = std::find_if( pass.begin()
				, pass.end()
				, []( TextureUnitSPtr unit )
				{
					return unit->getConfiguration().opacityMask[0] != 0u;
				} );

			if ( it != pass.end() )
			{
				doBindTexture( *( *it )
					, layout
					, descriptorSet
					, index
					, texIndex );
				++index;
			}
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
		, bool opacityOnly )
	{
		uint32_t texIndex = 0u;

		if ( opacityOnly )
		{
			auto it = std::find_if( pass.begin()
				, pass.end()
				, []( TextureUnitSPtr unit )
				{
					return unit->getConfiguration().opacityMask[0] != 0u;
				} );

			if ( it != pass.end() )
			{
				ashes::WriteDescriptorSet write
				{
					index,
					0u,
					1u,
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				};
				doBindTexture( *( *it )
					, layout
					, write );
				writes.push_back( write );
				++index;
			}
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
