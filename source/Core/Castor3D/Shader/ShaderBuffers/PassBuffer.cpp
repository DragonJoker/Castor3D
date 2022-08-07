#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

CU_ImplementCUSmartPtr( castor3d, PassBuffer )

namespace castor3d
{
	//*********************************************************************************************

	namespace passbuf
	{
		static PassBuffer::PassesData bindData( uint8_t * buffer
			, uint32_t count )
		{
			return castor::makeArrayView( reinterpret_cast< PassBuffer::PassData * >( buffer )
				, reinterpret_cast< PassBuffer::PassData * >( buffer ) + count );
		}

		static uint64_t getHash( Pass const & pass )
		{
			constexpr auto maxTexturesSize = castor::getBitSize( MaxPassTextures );
			constexpr auto maxChannelsSize = castor::getBitSize( uint32_t( TextureFlag::eAll ) );
			auto offset = 0u;
			uint64_t result = uint64_t( pass.getTextures() ) << offset;
			offset += maxChannelsSize;
			result |= uint64_t( pass.getTextureUnitsCount() ) << offset;
			offset += maxTexturesSize;
			result |= uint64_t( pass.getTypeID() ) << offset;
			return result;
		}
	}

	//*********************************************************************************************

	PassBuffer::PassBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * DataSize, cuT( "PassBuffer" ) }
		, m_data{ passbuf::bindData( m_buffer.getPtr(), count ) }
	{
	}

	uint32_t PassBuffer::addPass( Pass & pass )
	{
		if ( pass.getId() == 0u )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );

			CU_Require( m_passes.size() < MaxMaterialsCount );
			m_passes.emplace_back( &pass );
			pass.setId( m_passID++ );
			m_connections.emplace_back( pass.onChanged.connect( [this]( Pass const & ppass )
				{
					m_dirty.emplace_back( &ppass );
				} ) );
			m_dirty.emplace_back( &pass );
		}

		return pass.getId();
	}

	void PassBuffer::removePass( Pass & pass )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		auto id = pass.getId() - 1u;
		CU_Require( id < m_passes.size() );
		CU_Require( &pass == m_passes[id] );
		auto it = m_passes.erase( m_passes.begin() + id );
		m_connections.erase( m_connections.begin() + id );
		++id;

		while ( it != m_passes.end() )
		{
			( *it )->setId( id );
			++it;
			++id;
		}

		pass.setId( 0u );
		m_passID--;
	}

	void PassBuffer::update( ashes::CommandBuffer const & commandBuffer )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			std::vector< Pass const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin()
				, end
				, [this]( Pass const * pass )
				{
					auto it = m_passTypeIndices.emplace( passbuf::getHash( *pass )
						, uint16_t( m_passTypeIndices.size() ) ).first;
					pass->fillBuffer( *this, it->second );
				} );
			m_buffer.upload( commandBuffer );
		}
	}

	VkDescriptorSetLayoutBinding PassBuffer::createLayoutBinding( uint32_t binding
		, VkShaderStageFlags stages )const
	{
		return m_buffer.createLayoutBinding( binding, stages );
	}

	void PassBuffer::createPassBinding( crg::FramePass & pass, uint32_t binding )const
	{
		return m_buffer.createPassBinding( pass, cuT( "Materials" ), binding );
	}

	void PassBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}

	ashes::WriteDescriptorSet PassBuffer::getBinding( uint32_t binding )const
	{
		return m_buffer.getBinding( binding );
	}

	PassBuffer::PassDataPtr PassBuffer::getData( uint32_t passID )
	{
		CU_Require( passID > 0 );
		auto index = passID - 1;
		PassDataPtr result{};

		auto & data = m_data[index];
		result.colourDiv = &data.colourDiv;
		result.specDiv = &data.specDiv;
		result.edgeWidth = &data.edgeWidth;
		result.depthFactor = &data.depthFactor;
		result.normalFactor = &data.normalFactor;
		result.objectFactor = &data.objectFactor;
		result.edgeColour = &data.edgeColour;
		result.specific = &data.specific;
		result.index = &data.index;
		result.emissive = &data.emissive;
		result.alphaRef = &data.alphaRef;
		result.sssProfileIndex = &data.sssProfileIndex;
		result.transmission = &data.transmission;
		result.opacity = &data.opacity;
		result.refractionRatio = &data.refractionRatio;
		result.hasRefraction = &data.hasRefraction;
		result.hasReflection = &data.hasReflection;
		result.bwAccumulationOperator = &data.bwAccumulationOperator;
		result.textures = &data.textures;
		result.textureCount = &data.textureCount;
		result.passTypeIndex = &data.passTypeIndex;

		return result;
	}
}
