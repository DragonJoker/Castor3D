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
		, m_maxCount{ count }
		, m_buffer{ device, count * VkDeviceSize( m_stride ), cuT( "PassBuffer" ) }
		, m_data{ castor::makeArrayView( m_buffer.getPtr(), count * m_stride ) }
	{
	}

	uint32_t PassBuffer::addPass( Pass & pass )
	{
		if ( pass.getId() == 0u )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );

			CU_Require( m_passes.size() < m_maxCount );
			m_passes.emplace_back( &pass );
			pass.setId( m_passID );
			++m_passID;
			m_connections.emplace_back( pass.onChanged.connect( [this]( Pass const & ppass
				, CU_UnusedParam( PassComponentCombineID, oldCombineID )
				, CU_UnusedParam( PassComponentCombineID, newCombineID ) )
				{
					m_dirty.emplace_back( &ppass );
				} ) );
			m_dirty.emplace_back( &pass );
		}

		return pass.getId();
	}

	void PassBuffer::removePass( Pass & pass )noexcept
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
		, UploadData & uploader )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			castor::Vector< Pass const * > dirty;
			castor::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			for ( auto pass : castor::makeArrayView( dirty.begin(), end ) )
			{
				if ( auto it = m_passTypeIndices.emplace( passbuf::hash( *pass )
						, PassTypeData{ uint16_t( m_passTypeIndices.size() )
						, pass->getComponentCombineID()
						, pass->getTextureCombineID() } ).first;
					it->second.index * VkDeviceSize( m_stride ) > m_data.size() )
				{
					log::warn << "Material pass [" << pass->getOwner()->getName() << ", " << pass->getIndex() <<"] is out of buffer boundaries, ignoring it." << std::endl;
				}
				else
				{
					pass->fillBuffer( *this );
				}

				for ( auto const & [name, buffer] : specifics )
				{
					buffer.first.update( *buffer.second, *pass );
				}

				pass->reset();
			}

			auto passCount = std::min( m_maxCount, uint32_t( m_passes.size() ) );
			m_buffer.setCount( passCount );
			m_buffer.setSecondCount( uint32_t( m_passTypeIndices.size() ) );
			m_buffer.upload( uploader );

			for ( auto & [name, buffer] : specifics )
			{
				if ( auto & buf = buffer.second )
				{
					buf->setCount( passCount );
					buf->upload( uploader );
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
		return m_buffer.createPassBinding( pass, binding );
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

		if ( auto index = passID - 1;
			index < m_maxCount )
		{
			return PassBuffer::PassDataPtr{ castor::makeArrayView( m_data.data() + ptrdiff_t( m_stride ) * index, m_stride ) };
		}

		CU_Failure( "Pass ID is out of buffer bounds." );
		static castor::ByteArray dummy{ [this]()
			{
				castor::ByteArray result;
				result.resize( m_stride );
				return result;
			}() };
		return PassBuffer::PassDataPtr{ castor::makeArrayView( dummy.data(), m_stride ) };
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
			, [&passTypeIndex]( castor::UnorderedMap< uint32_t, PassTypeData >::value_type const & lookup )
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
