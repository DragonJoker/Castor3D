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
			auto bindTexture = [this
				, &layout
				, &descriptorSet
				, &index
				, &texIndex]( std::function< uint32_t( TextureConfiguration const & ) > getMask )
			{
				auto it = std::find_if( pass.begin()
					, pass.end()
					, [&getMask]( TextureUnitSPtr unit )
					{
						return getMask( unit->getConfiguration() ) != 0u;
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
			};

			if ( checkFlag( mask, TextureFlag::eAlbedo ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.colourMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eSpecular ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.specularMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eGlossiness ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.glossinessMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eOpacity ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.opacityMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eEmissive ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.emissiveMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eNormal ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.normalMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eHeight ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.heightMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eOcclusion ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.occlusionMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eTransmittance ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.transmittanceMask[0];
					} );
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
		, TextureFlags mask )
	{
		uint32_t texIndex = 0u;

		if ( mask )
		{
			auto bindTexture = [this
				, &layout
				, &writes
				, &index
				, &texIndex]( std::function< uint32_t( TextureConfiguration const & ) > getMask )
			{
				auto it = std::find_if( pass.begin()
					, pass.end()
					, [&getMask]( TextureUnitSPtr unit )
					{
						return getMask( unit->getConfiguration() ) != 0u;
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
			};

			if ( checkFlag( mask, TextureFlag::eAlbedo ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.colourMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eSpecular ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.specularMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eGlossiness ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.glossinessMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eOpacity ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.opacityMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eEmissive ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.emissiveMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eNormal ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.normalMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eHeight ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.heightMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eOcclusion ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.occlusionMask[0];
					} );
			}

			if ( checkFlag( mask, TextureFlag::eTransmittance ) )
			{
				bindTexture( []( TextureConfiguration const & lookup )
					{
						return lookup.transmittanceMask[0];
					} );
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
