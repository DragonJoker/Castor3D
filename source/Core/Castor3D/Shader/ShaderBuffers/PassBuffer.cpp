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

		static uint32_t constexpr count()noexcept
		{
			constexpr auto maxPassTypeSize = castor::getBitSize( MaxPassTypes );
			constexpr auto maxPassFlagsSize = castor::getBitSize( uint32_t( PassFlag::eAllVisibility ) );
			constexpr auto maxTextureFlagsSize = castor::getBitSize( uint32_t( TextureFlag::eAll ) );
			constexpr auto offset = maxPassTypeSize + maxPassFlagsSize + maxTextureFlagsSize;
			static_assert( offset < 31u );
			constexpr auto result = 0x000000001u << offset;

			return result;
		}

		static uint32_t hash( PassTypeID passType
			, PassFlags passFlags
			, TextureFlags textureFlags )noexcept
		{
			constexpr auto maxPassTypeSize = castor::getBitSize( MaxPassTypes );
			constexpr auto maxPassFlagsSize = castor::getBitSize( uint32_t( PassFlag::eAllVisibility ) );
			constexpr auto maxTextureFlagsSize = castor::getBitSize( uint32_t( TextureFlag::eAll ) );
			constexpr auto size = maxPassTypeSize + maxPassFlagsSize + maxTextureFlagsSize;
			static_assert( size < 31u );

			auto offset = 0u;
			auto result = uint32_t( textureFlags & TextureFlag::eAll ) << offset;
			offset += maxTextureFlagsSize;
			result |= uint32_t( passFlags & PassFlag::eAllVisibility ) << offset;
			offset += maxPassFlagsSize;
			result |= uint32_t( passType ) << offset;
			return result;
		}

		static uint32_t hash( Pass const & pass )noexcept
		{
			return hash( pass.getTypeID()
				, pass.getPassFlags()
				, pass.getTextures() );
		}

		static std::tuple< PassTypeID, PassFlags, TextureFlags > unhash( uint32_t hash )noexcept
		{
			constexpr auto maxPassTypeSize = castor::getBitSize( MaxPassTypes );
			constexpr auto maxPassTypeMask = ( 0x1u << uint32_t( maxPassTypeSize ) ) - 1u;
			constexpr auto maxPassFlagsSize = castor::getBitSize( uint32_t( PassFlag::eAllVisibility ) );
			constexpr auto maxPassFlagsMask = ( 0x1u << uint32_t( maxPassFlagsSize ) ) - 1u;
			constexpr auto maxTextureFlagsSize = castor::getBitSize( uint32_t( TextureFlag::eAll ) );
			constexpr auto maxTextureFlagsMask = ( 0x1u << uint32_t( maxTextureFlagsSize ) ) - 1u;

			auto offset = 0u;
			auto textureFlags = TextureFlags( uint16_t( ( hash >> offset ) & maxTextureFlagsMask ) );
			offset += maxTextureFlagsSize;
			auto passFlags = PassFlags( uint16_t( ( hash >> offset ) & maxPassFlagsMask ) );
			offset += maxPassFlagsSize;
			auto passTypeID = PassTypeID( uint16_t( ( hash >> offset ) & maxPassTypeMask ) );
			return { passTypeID, passFlags, textureFlags };
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

	void PassBuffer::update( SpecificsBuffers const & specifics
		, ashes::CommandBuffer const & commandBuffer )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			std::vector< Pass const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin()
				, end
				, [this, &specifics]( Pass const * pass )
				{
					auto it = m_passTypeIndices.emplace( passbuf::hash( *pass )
						, uint16_t( m_passTypeIndices.size() ) ).first;
					pass->fillBuffer( *this, it->second );

					for ( auto & buffer : specifics )
					{
						buffer.second.first.update( *buffer.second.second, *pass );
					}
				} );
			m_buffer.setCount( uint32_t( m_passes.size() ) );
			m_buffer.setSecondaryCount( uint32_t( m_passTypeIndices.size() ) );
			m_buffer.upload( commandBuffer );

			for ( auto & buffer : specifics )
			{
				buffer.second.second->setCount( uint32_t( m_passes.size() ) );
				buffer.second.second->upload( commandBuffer );
			}
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
		result.passId = &data.passId;
		result.lighting = &data.lighting;
		result.passCount = &data.passCount;

		return result;
	}

	uint32_t PassBuffer::getMaxPassTypeCount()const
	{
		return passbuf::count();
	}

	uint32_t PassBuffer::getPassTypeIndex( PassTypeID passType
		, PassFlags passFlags
		, TextureFlags textureFlags )const
	{
		auto hash = passbuf::hash( passType, passFlags, textureFlags );
		auto it = m_passTypeIndices.find( hash );

		if ( it == m_passTypeIndices.end() )
		{
			CU_Exception( "Pass type index not found in registered ones." );
		}

		return it->second;
	}

	std::tuple< PassTypeID, PassFlags, TextureFlags > PassBuffer::getPassTypeDetails( uint32_t passTypeIndex )const
	{
		auto it = std::find_if( m_passTypeIndices.begin()
			, m_passTypeIndices.end()
			, [&passTypeIndex]( std::unordered_map< uint64_t, uint16_t >::value_type const & lookup )
			{
				return lookup.second == passTypeIndex;
			} );

		if ( it == m_passTypeIndices.end() )
		{
			CU_Exception( "Pass type index not found in registered ones." );
		}

		return passbuf::unhash( it->first );
	}
}
