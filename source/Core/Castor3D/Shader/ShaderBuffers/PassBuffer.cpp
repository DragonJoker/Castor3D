#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, PassBuffer )

namespace castor3d
{
	//*********************************************************************************************

	namespace passbuf
	{
		static uint32_t hash( PassComponentCombineID components
			, TextureCombineID textures )noexcept
		{
			constexpr auto maxPassTypeSize = int( sizeof( PassComponentCombineID ) * 8 );
			constexpr auto maxTexturesSize = int( sizeof( TextureCombineID ) * 8 );
			constexpr auto size = maxPassTypeSize + maxTexturesSize;
			static_assert( size <= 32u );

			auto offset = 0u;
			auto result = uint32_t{};
			result |= uint32_t( components ) << offset;
			offset += maxPassTypeSize;
			result |= uint32_t( textures ) << offset;

			return result;
		}

		static uint32_t hash( Pass const & pass )noexcept
		{
			return hash( pass.getComponentCombineID()
				, pass.getTextureCombineID() );
		}
	}

	//*********************************************************************************************

	VkDeviceSize PassBuffer::PassDataPtr::write( MemChunk const & chunk
		, uint32_t v
		, VkDeviceSize offset )
	{
		auto buffer = m_data.data();
		CU_Require( chunk.askedSize - sizeof( uint32_t ) >= offset );
		offset += chunk.offset;
		CU_Require( m_data.size() - sizeof( uint32_t ) >= offset );
		buffer += offset;
		*reinterpret_cast< uint32_t * >( buffer ) = v;
		return sizeof( uint32_t );
	}

	VkDeviceSize PassBuffer::PassDataPtr::write( MemChunk const & chunk
		, int32_t v
		, VkDeviceSize offset )
	{
		auto buffer = m_data.data();
		CU_Require( chunk.askedSize - sizeof( int32_t ) >= offset );
		offset += chunk.offset;
		CU_Require( m_data.size() - sizeof( int32_t ) >= offset );
		buffer += offset;
		*reinterpret_cast< int32_t * >( buffer ) = v;
		return sizeof( int32_t );
	}

	VkDeviceSize PassBuffer::PassDataPtr::write( MemChunk const & chunk
		, float v
		, VkDeviceSize offset )
	{
		auto buffer = m_data.data();
		CU_Require( chunk.askedSize - sizeof( float ) >= offset );
		offset += chunk.offset;
		CU_Require( m_data.size() - sizeof( float ) >= offset );
		buffer += offset;
		*reinterpret_cast< float * >( buffer ) = v;
		return sizeof( float );
	}

	//*********************************************************************************************

	PassBuffer::PassBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_stride{ uint32_t( engine.getPassComponentsRegister().getPassBufferStride() ) }
		, m_buffer{ engine, device, count * m_stride, cuT( "PassBuffer" ) }
		, m_data{ castor::makeArrayView( m_buffer.getPtr(), count * m_stride ) }
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
			m_connections.emplace_back( pass.onChanged.connect( [this]( Pass const & ppass
				, PassComponentCombineID CU_UnusedParam( oldCombineID )
				, PassComponentCombineID CU_UnusedParam( newCombineID ) )
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

			for ( auto pass : castor::makeArrayView( dirty.begin(), end ) )
			{
				auto it = m_passTypeIndices.emplace( passbuf::hash( *pass )
					, PassTypeData{ uint16_t( m_passTypeIndices.size() )
						, pass->getComponentCombineID()
						, pass->getTextureCombineID() } ).first;
					
				pass->fillBuffer( *this, it->second.index );

				for ( auto & buffer : specifics )
				{
					buffer.second.first.update( *buffer.second.second, *pass );
				}

				pass->reset();
			}

			m_buffer.setCount( uint32_t( m_passes.size() ) );
			m_buffer.setSecondCount( uint32_t( m_passTypeIndices.size() ) );
			m_buffer.upload( commandBuffer );

			for ( auto & buffer : specifics )
			{
				if ( auto & buf = buffer.second.second )
				{
					buf->setCount( uint32_t( m_passes.size() ) );
					buf->upload( commandBuffer );
				}
			}
		}
	}

	void PassBuffer::cleanup()
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_dirty.clear();
	}

	VkDescriptorSetLayoutBinding PassBuffer::createLayoutBinding( uint32_t binding
		, VkShaderStageFlags stages )const
	{
		return m_buffer.createLayoutBinding( binding, stages );
	}

	void PassBuffer::createPassBinding( crg::FramePass & pass, uint32_t binding )const
	{
		return m_buffer.createPassBinding( pass, cuT( "C3D_Materials" ), binding );
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
		return PassBuffer::PassDataPtr{ castor::makeArrayView( m_data.data() + m_stride * index, m_stride ) };
	}

	uint32_t PassBuffer::getMaxPassTypeCount()const
	{
		return std::numeric_limits< PassComponentCombineID >::max();
	}

	uint32_t PassBuffer::getPassTypeIndex( PassComponentCombineID passType
		, TextureCombineID textureCombine )const
	{
		auto hash = passbuf::hash( passType, textureCombine );
		auto it = m_passTypeIndices.find( hash );

		if ( it == m_passTypeIndices.end() )
		{
			CU_Exception( "Pass type index not found in registered ones." );
		}

		return it->second.index;
	}

	std::tuple< PassComponentCombineID, TextureCombineID > PassBuffer::getPassTypeDetails( uint32_t passTypeIndex )const
	{
		auto it = std::find_if( m_passTypeIndices.begin()
			, m_passTypeIndices.end()
			, [&passTypeIndex]( std::unordered_map< uint32_t, PassTypeData >::value_type const & lookup )
			{
				return lookup.second.index == passTypeIndex;
			} );

		if ( it == m_passTypeIndices.end() )
		{
			CU_Exception( "Pass type index not found in registered ones." );
		}

		return { it->second.componentCombine, it->second.textureCombine };
	}
}
