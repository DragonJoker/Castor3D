#include "Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/BonedVertex.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, PassMasksComponent )

namespace castor3d
{
	//*********************************************************************************************

	namespace passflags
	{
		static ashes::PipelineVertexInputStateCreateInfo createVertexLayout( uint32_t & currentBinding
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { currentBinding
				, sizeof( castor::Point4ui ), VK_VERTEX_INPUT_RATE_VERTEX } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ 1u, { currentLocation++
				, currentBinding
				, VK_FORMAT_R32G32B32A32_UINT
				, 0u } };
			++currentBinding;
			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}

		static uint32_t convert( uint8_t a, uint8_t b, uint8_t c, uint8_t d )
		{
			return ( uint32_t( d ) << 24u )
				| ( uint32_t( c ) << 16u )
				| ( uint32_t( b ) << 8u )
				| ( uint32_t( a ) << 0u );
		}

		static castor::Point4uiArray convert( std::vector< PassMasks > const & src )
		{
			castor::Point4uiArray result;
			result.reserve( src.size() );

			for ( auto & value : src )
			{
				auto a = convert( value.data[0], value.data[1], value.data[2], value.data[3] );
				auto b = convert( value.data[4], value.data[5], value.data[6], value.data[7] );
				auto c = convert( value.data[8], value.data[9], value.data[10], value.data[11] );
				auto d = convert( value.data[12], value.data[13], value.data[14], 0u );
				result.push_back( castor::Point4ui{ a, b, c, d } );
			}

			return result;
		}
	}

	//*********************************************************************************************

	void PassMasksComponent::SurfaceShader::fillSurfaceType( sdw::type::Struct & type
		, uint32_t * index )const
	{
		if ( index )
		{
			static_cast< sdw::type::IOStruct & >( type ).declMember( "passMasks", ast::type::Kind::eVec4U, ast::type::NotArray, ( *index )++ );
		}
		else
		{
			static_cast< sdw::type::BaseStruct & >( type ).declMember( "passMasks", ast::type::Kind::eVec4U, ast::type::NotArray );
		}
	}

	//*********************************************************************************************

	void PassMasksComponent::ComponentData::copy( SubmeshComponentDataRPtr data )const
	{
		auto result = static_cast< ComponentData * >( data );
		result->m_data = m_data;
	}

	void PassMasksComponent::ComponentData::gather( PipelineFlags const & flags
		, Pass const & pass
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation )
	{
		if ( flags.enablePassMasks() )
		{
			auto hash = std::hash< uint32_t >{}( currentBinding );
			hash = castor::hashCombine( hash, currentLocation );
			auto layoutIt = m_layouts.find( hash );

			if ( layoutIt == m_layouts.end() )
			{
				layoutIt = m_layouts.emplace( hash
					, passflags::createVertexLayout( currentBinding
						, currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
				currentBinding = layoutIt->second.vertexAttributeDescriptions.back().binding + 1u;
			}

			layouts.emplace_back( layoutIt->second );
		}
	}

	void PassMasksComponent::ComponentData::addDatas( PassMasks const * const begin
		, PassMasks const * const end )
	{
		m_data.insert( m_data.end(), begin, end );
	}

	void PassMasksComponent::ComponentData::addDatas( std::vector< PassMasks > const & boneData )
	{
		addDatas( boneData.data(), boneData.data() + boneData.size() );
	}

	bool PassMasksComponent::ComponentData::doInitialise( RenderDevice const & device )
	{
		return true;
	}

	void PassMasksComponent::ComponentData::doCleanup( RenderDevice const & device )
	{
		m_data.clear();
	}

	void PassMasksComponent::ComponentData::doUpload( UploadData & uploader )
	{
		auto count = uint32_t( m_data.size() );
		auto & offsets = m_submesh.getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshData::ePassMasks );

		if ( count && buffer.hasData() )
		{
			CU_Require( buffer.getCount< castor::Point4ui >() == count );
			m_up = passflags::convert( m_data );
			uploader.pushUpload( m_up.data()
				, m_up.size() * sizeof( castor::Point4ui )
				, buffer.getBuffer()
				, buffer.getOffset()
				, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		}
	}

	//*********************************************************************************************

	castor::String const PassMasksComponent::TypeName = C3D_MakeSubmeshComponentName( "passmasks" );

	PassMasksComponent::PassMasksComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName
			, std::make_unique< ComponentData >( submesh ) }
	{
	}

	SubmeshComponentUPtr PassMasksComponent::clone( Submesh & submesh )const
	{
		auto result = castor::makeUnique< PassMasksComponent >( submesh );
		result->getData().copy( &getData() );
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	//*********************************************************************************************
}
