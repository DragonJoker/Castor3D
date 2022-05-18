#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
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
			, uint32_t & currentBinding
			, uint32_t & currentLocation )
		{
			std::vector< ashes::PipelineVertexInputStateCreateInfo > result;

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphPositions ) )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { currentBinding
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, currentBinding
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result .emplace_back( 0u, bindings, attributes );
				++currentBinding;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphNormals )
				&& checkFlag( shaderFlags, ShaderFlag::eNormal ) )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { currentBinding
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, currentBinding
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result.emplace_back( 0u, bindings, attributes );
				++currentBinding;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphTangents )
				&& checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { currentBinding
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, currentBinding
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result.emplace_back( 0u, bindings, attributes );
				++currentBinding;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphTexcoords0 )
				&& hasTextures )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { currentBinding
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, currentBinding
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result.emplace_back( 0u, bindings, attributes );
				++currentBinding;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphTexcoords1 )
				&& hasTextures )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { currentBinding
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, currentBinding
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result.emplace_back( 0u, bindings, attributes );
				++currentBinding;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphTexcoords2 )
				&& hasTextures )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { currentBinding
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, currentBinding
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result.emplace_back( 0u, bindings, attributes );
				++currentBinding;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphTexcoords3 )
				&& hasTextures )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { currentBinding
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, currentBinding
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result.emplace_back( 0u, bindings, attributes );
				++currentBinding;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphColours ) )
			{
				ashes::VkVertexInputBindingDescriptionArray bindings;
				ashes::VkVertexInputAttributeDescriptionArray attributes;
				bindings.push_back( { currentBinding
					, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } );
				attributes.push_back( { currentLocation++
					, currentBinding
					, VK_FORMAT_R32G32B32_SFLOAT
					, 0u } );
				result.emplace_back( 0u, bindings, attributes );
				++currentBinding;
			}

			return result;
		}
	}

	castor::String const MorphComponent::Name = cuT( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, Id }
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

		if ( submesh.hasComponent( Texcoords0Component::Name ) )
		{
			m_flags |= SubmeshFlag::eMorphTexcoords0;
		}

		if ( submesh.hasComponent( Texcoords1Component::Name ) )
		{
			m_flags |= SubmeshFlag::eMorphTexcoords1;
		}

		if ( submesh.hasComponent( Texcoords2Component::Name ) )
		{
			m_flags |= SubmeshFlag::eMorphTexcoords2;
		}

		if ( submesh.hasComponent( Texcoords3Component::Name ) )
		{
			m_flags |= SubmeshFlag::eMorphTexcoords3;
		}

		if ( submesh.hasComponent( ColoursComponent::Name ) )
		{
			m_flags |= SubmeshFlag::eMorphColours;
		}
	}

	void MorphComponent::gather( ShaderFlags const & shaderFlags
		, ProgramFlags const & programFlags
		, SubmeshFlags const & submeshFlags
		, MaterialRPtr material
		, TextureFlagsArray const & mask
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation )
	{
		if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
		{
			auto hash = std::hash< SubmeshFlags::BaseType >{}( submeshFlags );
			hash = castor::hashCombine( hash, shaderFlags.value() );
			hash = castor::hashCombine( hash, mask.empty() );
			hash = castor::hashCombine( hash, currentBinding );
			hash = castor::hashCombine( hash, currentLocation );
			auto layoutIt = m_layouts.find( hash );

			if ( layoutIt == m_layouts.end() )
			{
				layoutIt = m_layouts.emplace( hash
					, smshcompmorph::createVertexLayout( submeshFlags
						, shaderFlags
						, checkFlag( programFlags, ProgramFlag::eForceTexCoords ) || !mask.empty()
						, currentBinding
						, currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.back().vertexAttributeDescriptions.back().location + 1u;
				currentBinding = layoutIt->second.back().vertexAttributeDescriptions.back().binding + 1u;
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

	SubmeshFlags MorphComponent::getSubmeshFlags( Pass const * pass )const
	{
		if ( !pass )
		{
			return m_flags;
		}

		auto maxTexCoordIndex = pass->getMaxTexCoordSet();
		auto result = m_flags;

		if ( maxTexCoordIndex == 3u )
		{
			return result;
		}

		remFlag( result, SubmeshFlag::eMorphTexcoords3 );

		if ( maxTexCoordIndex == 2u )
		{
			return result;
		}

		remFlag( result, SubmeshFlag::eMorphTexcoords2 );

		if ( maxTexCoordIndex == 1u )
		{
			return result;
		}

		remFlag( result, SubmeshFlag::eMorphTexcoords1 );
		return result;
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
