#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/NormalsComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/PositionsComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/TangentsComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/TexcoordsComponent.hpp"
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
		static std::vector< ashes::PipelineVertexInputStateCreateInfo > createVertexLayout( SubmeshFlags const & submeshFlags
			, ShaderFlags const & shaderFlags
			, bool hasTextures
			, uint32_t & currentLocation )
		{
			std::vector< ashes::PipelineVertexInputStateCreateInfo > result;

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphPositions ) )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { MorphComponent::PosBindingPoint
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, MorphComponent::PosBindingPoint
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result .emplace_back( 0u, bindings, attributes );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphNormals )
				&& checkFlag( shaderFlags, ShaderFlag::eNormal ) )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { MorphComponent::NmlBindingPoint
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, MorphComponent::NmlBindingPoint
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result.emplace_back( 0u, bindings, attributes );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphTangents )
				&& checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { MorphComponent::TanBindingPoint
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, MorphComponent::TanBindingPoint
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result.emplace_back( 0u, bindings, attributes );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphTexcoords )
				&& hasTextures )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { MorphComponent::TexBindingPoint
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, MorphComponent::TexBindingPoint
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result.emplace_back( 0u, bindings, attributes );
			}

			return result;
		}
	}

	castor::String const MorphComponent::Name = cuT( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, PosBindingPoint }
	{
		if ( submesh.hasComponent( PositionsComponent::Name ) )
		{
			m_flags |= SubmeshFlag::eMorphPositions;
		}

		if ( submesh.hasComponent( NormalsComponent::Name ) )
		{
			m_flags |= SubmeshFlag::eMorphNormals;
		}

		if ( submesh.hasComponent( TangentsComponent::Name ) )
		{
			m_flags |= SubmeshFlag::eMorphTangents;
		}

		if ( submesh.hasComponent( TexcoordsComponent::Name ) )
		{
			m_flags |= SubmeshFlag::eMorphTexcoords;
		}
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
			auto layoutIt = m_layouts.find( hash );

			if ( layoutIt == m_layouts.end() )
			{
				layoutIt = m_layouts.emplace( hash
					, smshcompmorph::createVertexLayout( submeshFlags
						, shaderFlags
						, !mask.empty()
						, currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.back().vertexAttributeDescriptions.back().location + 1u;
			}

			layouts.insert( layouts.end()
				, layoutIt->second.begin()
				, layoutIt->second.end() );
		}
	}

	SubmeshComponentSPtr MorphComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< MorphComponent >( submesh );
		result->m_flags = m_flags;
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
	}
}
