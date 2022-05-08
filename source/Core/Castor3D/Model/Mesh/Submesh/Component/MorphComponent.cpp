#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

namespace castor3d
{
	namespace smshcompmorph
	{
		static ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout( SubmeshFlags const & submeshFlags
			, ShaderFlags const & shaderFlags
			, bool hasTextures
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { MorphComponent::BindingPoint
				, sizeof( InterleavedVertex ), VK_VERTEX_INPUT_RATE_VERTEX } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ 1u, { currentLocation++
				, MorphComponent::BindingPoint
				, VK_FORMAT_R32G32B32_SFLOAT
				, offsetof( InterleavedVertex, pos ) } };

			if ( checkFlag( shaderFlags, ShaderFlag::eNormal ) )
			{
				attributes.push_back( { currentLocation++
					, MorphComponent::BindingPoint
					, VK_FORMAT_R32G32B32_SFLOAT
					, offsetof( InterleavedVertex, nml ) } );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
			{
				attributes.push_back( { currentLocation++
					, MorphComponent::BindingPoint
					, VK_FORMAT_R32G32B32_SFLOAT
					, offsetof( InterleavedVertex, tan ) } );
			}

			if ( hasTextures )
			{
				attributes.push_back( { currentLocation++
					, MorphComponent::BindingPoint
					, VK_FORMAT_R32G32B32_SFLOAT
					, offsetof( InterleavedVertex, tex ) } );
			}

			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}
	}

	castor::String const MorphComponent::Name = cuT( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, BindingPoint }
	{
	}

	void MorphComponent::gather( ShaderFlags const & shaderFlags
		, SubmeshFlags const & submeshFlags
		, MaterialRPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, TextureFlagsArray const & mask
		, uint32_t & currentLocation )
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::eMorphing ) )
		{
			auto hash = std::hash< SubmeshFlags::BaseType >{}( submeshFlags );
			hash = castor::hashCombine( hash, shaderFlags.value() );
			hash = castor::hashCombine( hash, mask.empty() );
			hash = castor::hashCombine( hash, currentLocation );
			auto layoutIt = m_animLayouts.find( hash );

			if ( layoutIt == m_animLayouts.end() )
			{
				layoutIt = m_animLayouts.emplace( hash
					, smshcompmorph::doCreateVertexLayout( submeshFlags
						, shaderFlags
						, !mask.empty(), currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
			}

			buffers.emplace_back( getOwner()->getBufferOffsets().getMorphBuffer() );
			offsets.emplace_back( 0u );
			layouts.emplace_back( layoutIt->second );
		}
	}

	SubmeshComponentSPtr MorphComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< MorphComponent >( submesh );
		result->m_data = m_data;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	bool MorphComponent::doInitialise( RenderDevice const & device )
	{
		return true;
	}

	void MorphComponent::doCleanup( RenderDevice const & device )
	{
	}

	void MorphComponent::doUpload()
	{
		if ( getOwner()->hasBufferOffsets() )
		{
			if ( getOwner()->getBufferOffsets().hasVertices() )
			{
				auto & offsets = getOwner()->getBufferOffsets();
				std::copy( m_data.begin()
					, m_data.end()
					, offsets.getMorphData< InterleavedVertex >().begin() );
				offsets.markMorphDirty();
			}
		}
	}
}
