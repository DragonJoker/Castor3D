#include "Castor3D/Shader/Shaders/GlslOneSweepSort.hpp"

namespace c3d::shader
{
	namespace sort
	{
		sdw::UInt32 const & toUInt( sdw::UInt32 const & in )
		{
			return in;
		}

		sdw::UInt32 toUInt( sdw::Int const & in )
		{
			auto & writer = sdw::findWriterMandat( in );
			return writer.cast< sdw::UInt32 >( in ^ 0x80000000_i );
		}

		//Radix Tricks by Michael Herf
		//http://stereopsis.com/radix.html
		sdw::UInt32 toUInt( sdw::Float const & in )
		{
			auto & writer = sdw::findWriterMandat( in );
			sdw::UInt32 mask = writer.cast< sdw::UInt32 >( -( writer.cast< sdw::Int >( sdw::floatBitsToUInt( in ) >> 31_u ) ) | 0x80000000_i );
			return sdw::floatBitsToUInt( in ) ^ mask;
		}

		sdw::Boolean isPartialDispatch( sdw::UInt const & isPartialMask )
		{
			return ( isPartialMask & 1_u ) != 0_u;
		}

		sdw::UInt flattenGid( sdw::U32Vec3 const & gid, sdw::UInt const & isPartialMask )
		{
			sdw::ShaderWriter & writer = sdw::findWriterMandat( gid, isPartialMask );
			return writer.ternary( isPartialDispatch( isPartialMask )
				, gid.x() + ( isPartialMask >> 1_u ) * c3d::sweep::maxDispatchDim
				, gid.x() + gid.y() * c3d::sweep::maxDispatchDim );
		}

		Helpers::Helpers( sdw::ShaderWriter & writer
			, sdw::UInt32 const & threadBlocks
			, sdw::UInt32 const & radixShift
			, u32 keysPerThread
			, u32 threadsPerGroup
			, u32 partitionSize )
			: m_writer{ writer }
			, m_threadBlocks{ threadBlocks }
			, m_radixShift{ radixShift }
			, m_keysPerThread{ keysPerThread }
			, m_threadsPerGroup{ threadsPerGroup }
			, m_partitionSize{ partitionSize }
		{
		}

		sdw::UInt32 Helpers::currentPass()const
		{
			return m_radixShift >> 3u;
		}

		sdw::UInt32 Helpers::passHistOffset( sdw::UInt32 const & index )const
		{
			return ( ( currentPass() * m_threadBlocks ) + index ) << c3d::sweep::radixLog;
		}

		sdw::UInt32 Helpers::getWaveIndex( sdw::UInt32 const & gtid
			, sdw::UInt32 const & subgroupSize )const
		{
			return gtid / subgroupSize;
		}

		sdw::UInt32 Helpers::extractDigit( sdw::UInt32 const & key )const
		{
			return key >> m_radixShift & c3d::sweep::radixMask;
		}

		sdw::UInt32 Helpers::extractPackedIndex( sdw::UInt32 const & key )const
		{
			return key >> ( m_radixShift + 1_u ) & c3d::sweep::halfMask;
		}

		sdw::UInt32 Helpers::extractPackedShift( sdw::UInt32 const & key )const
		{
			return m_writer.ternary( ( key >> sdw::UInt32{ m_radixShift } & 1_u ) != 0_u, 16_u, 0_u );
		}

		sdw::UInt32 Helpers::extractPackedValue( sdw::UInt32 const & packed
			, sdw::UInt32 const & key )const
		{
			return packed >> extractPackedShift( key ) & 0xffff_u;
		}

		sdw::UInt32 Helpers::deviceOffsetWGE16( sdw::UInt32 const & gtid
			, sdw::UInt32 const & subgroupSize
			, sdw::UInt32 const & subgroupInvocationID
			, sdw::UInt32 const & partIndex )const
		{
			return sharedOffsetWGE16( gtid, subgroupSize, subgroupInvocationID ) + partIndex * m_partitionSize;
		}

		sdw::UInt32 Helpers::deviceOffsetWLT16( sdw::UInt32 const & gtid
			, sdw::UInt32 const & subgroupSize
			, sdw::UInt32 const & subgroupInvocationID
			, sdw::UInt32 const & partIndex
			, sdw::UInt32 const & serialIterations )const
		{
			return sharedOffsetWLT16( gtid, subgroupSize, subgroupInvocationID, serialIterations ) + partIndex * m_partitionSize;
		}

		sdw::UInt32 Helpers::waveHistsSizeWGE16( sdw::UInt32 const & subgroupSize )const
		{
			return m_threadsPerGroup / subgroupSize * c3d::sweep::radix;
		}

		sdw::UInt32 Helpers::waveHistsSizeWLT16()const
		{
			return sdw::UInt32{ c3d::sweep::totalSharedMem };
		}

		sdw::UInt32 Helpers::subPartSizeWGE16( sdw::UInt32 const & subgroupSize )const
		{
			return m_keysPerThread * subgroupSize;
		}

		sdw::UInt32 Helpers::sharedOffsetWGE16( sdw::UInt32 const & gtid
			, sdw::UInt32 const & subgroupSize
			, sdw::UInt32 const & subgroupInvocationID )const
		{
			return subgroupInvocationID + getWaveIndex( gtid, subgroupSize ) * subPartSizeWGE16( subgroupSize );
		}

		sdw::UInt32 Helpers::subPartSizeWLT16( sdw::UInt32 const & subgroupSize
			, sdw::UInt32 const & serialIterations )const
		{
			return m_keysPerThread * subgroupSize * serialIterations;
		}

		sdw::UInt32 Helpers::sharedOffsetWLT16( sdw::UInt32 const & gtid
			, sdw::UInt32 const & subgroupSize
			, sdw::UInt32 const & subgroupInvocationID
			, sdw::UInt32 const & serialIterations )const
		{
			return subgroupInvocationID
				+ ( getWaveIndex( gtid, subgroupSize ) / serialIterations * subPartSizeWLT16( subgroupSize, serialIterations ) )
				+ ( getWaveIndex( gtid, subgroupSize ) % serialIterations * subgroupSize );
		}
	}
}
