/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslRadixSort_H___
#define ___C3D_GlslRadixSort_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/ComputeWriter.hpp>

namespace castor3d::shader
{
	/**
	 *\arg	ValueSizeT	The size of the value type.
	 */
	template< uint32_t ValueSizeT >
	struct RadixSortT
	{
		static uint32_t constexpr bucketSize{ 1024u / ValueSizeT };
		uint32_t threadsCount{ bucketSize };
		/**
		 *\param	sortBits	The number of bits to consider sorting.
		 */
		RadixSortT( sdw::ComputeWriter & writer
			, uint32_t sortBits )
			: m_sortBits{ sortBits }
			, m_maxUInt{ writer.declConstant( "gMaxUInt", 0xFFFFFFFF_u ) }
		{
		}
		/**
		 *\arg	ValueT		The value type.
		 */
		template< typename ValueT >
		void sortT( sdw::ShaderWriter & writer
			, sdw::UInt elementOffset
			, sdw::UInt elementCount
			, sdw::UInt groupIndex
			, sdw::UInt threadIndex
			, sdw::Array< sdw::UInt > const & inputKeys
			, sdw::Array< sdw::UInt > const & outputKeys
			, sdw::Array< ValueT > const & inputValues
			, sdw::Array< ValueT > const & outputValues
			, ValueT const & invalidValue )const
		{
			auto gsKeys = writer.declSharedVariable< sdw::UInt >( "gsKeys", threadsCount ); // A temporary buffer to store the input keys. (threadsCount * sizeof(uint) Bytes)
			auto gsValues = writer.declSharedVariable< ValueT >( "gsValues", threadsCount, inputValues.isEnabled() ); // A temporary buffer to store the input values. (threadsCount * ValueSizeT Bytes)
			auto gsE = writer.declSharedVariable< sdw::UInt >( "gsE", threadsCount ); // Set a 1 for all false sort keys (b == 0) and a 0 for all true sort keys (b == 1) (threadsCount * sizeof(uint) Bytes)
			auto gsF = writer.declSharedVariable< sdw::UInt >( "gsF", threadsCount ); // Scan the splits. This results in the output index of all false sort keys (b == 0) (threadsCount * sizeof(uint) Bytes)
			auto gsD = writer.declSharedVariable< sdw::UInt >( "gsD", threadsCount ); // The desination index for the ouput key and value. (threadsCount * sizeof(uint) Bytes)
			auto gsTotalFalses = writer.declSharedVariable< sdw::UInt >( "gsTotalFalses" ); // The result of e[threadsCount - 1] + f[threadsCount - 1]; (4 Bytes)

			// Store the input key and values into shared memory.
			gsKeys[groupIndex] = writer.ternary( threadIndex < elementCount, inputKeys[elementOffset + threadIndex], m_maxUInt );
			gsValues[groupIndex] = writer.ternary( threadIndex < elementCount, inputValues[elementOffset + threadIndex], invalidValue );

			// Loop over the bits starting at the least-significant bit.
			FOR( writer, sdw::UInt, b, 0_u, b < m_sortBits, ++b )
			{
				// 1. In a temporary buffer in shared memory, we set a 1 for all false 
				//    sort keys (b = 0) and a 0 for all true sort keys.
				gsE[groupIndex] = writer.ternary( ( ( gsKeys[groupIndex] >> b ) & 1_u ) == 0_u
					, 1_u
					, 0_u );

				// Sync group shared memory writes.
				shader::groupMemoryBarrierWithGroupSync( writer );

				IF( writer, groupIndex == 0_u )
				{
					gsF[groupIndex] = 0_u;
				}
				ELSE
				{
					gsF[groupIndex] = gsE[groupIndex - 1_u];
				}
				FI

				// Sync group shared memory writes.
				shader::groupMemoryBarrierWithGroupSync( writer );
				auto temp = writer.declLocale( "temp", 0_u );

				// 2. We then scan (prefix sum) this buffer. This is the enumerate operation; 
				//    each false sort key now contains its destination address in the scan 
				//    output, which we will call f. These first two steps are equivalent to 
				//    a stream compaction operation on all false sort keys.
				for ( u32 i = 1; i < bucketSize; i <<= 1u )
				{
					temp = gsF[groupIndex];

					IF( writer, groupIndex > i )
					{
						temp += gsF[groupIndex - i];
					}
					FI

					// Sync group shared memory reads before writes.
					shader::groupMemoryBarrierWithGroupSync( writer );

					gsF[groupIndex] = temp;

					// Sync group shared memory writes.
					shader::groupMemoryBarrierWithGroupSync( writer );
				}

				// 3. The last element in the scan's output now contains the total 
				//    number of false sort keys. We write this value to a shared 
				//    variable, gs_TotalFalses.
				IF ( writer, groupIndex == 0_u )
				{
					gsTotalFalses = gsE[bucketSize - 1u] + gsF[bucketSize - 1u];
				}
				FI

				// Sync group shared memory writes.
				shader::groupMemoryBarrierWithGroupSync( writer );

				// 4. Now we compute the destination address for the true sort keys. For 
				// a sort key at index i, this address is t = i - f + totalFalses. We 
				// then select between t and f depending on the value of b to get the 
				// destination address d of each fragment.
				gsD[groupIndex] = writer.ternary( gsE[groupIndex] == 1u
					, gsF[groupIndex]
					, groupIndex - gsF[groupIndex] + gsTotalFalses );

				// 5. Finally, we scatter the original sort keys to destination address 
				//    d. The scatter pattern is a perfect permutation of the input, so 
				//    we see no write conflicts with this scatter.
				auto key = writer.declLocale( "key", gsKeys[groupIndex] );
				auto value = writer.declLocale( "value", gsValues[groupIndex], invalidValue.isEnabled() );

				// Sync group shared memory reads before writes.
				shader::groupMemoryBarrierWithGroupSync( writer );

				gsKeys[gsD[groupIndex]] = key;
				gsValues[gsD[groupIndex]] = value;

				// Sync group shared memory writes.
				shader::groupMemoryBarrierWithGroupSync( writer );
			}
			ROF

			// Now commit the results to global memory.
			outputKeys[elementOffset + threadIndex] = gsKeys[groupIndex];
			outputValues[elementOffset + threadIndex] = gsValues[groupIndex];
		}
		/**
		 *\arg	ValueT		The value type.
		 */
		template< typename ValueT >
		void sortT( sdw::ComputeWriter & writer
			, sdw::UInt elementOffset
			, sdw::UInt elementCount
			, sdw::UInt groupIndex
			, sdw::UInt threadIndex
			, sdw::Array< sdw::UInt > const & inputKeys
			, sdw::Array< sdw::UInt > const & outputKeys
			, ValueT const & invalidValue )
		{
			sdw::Array< ValueT > inputValues = writer.declGlobalArray< ValueT >( "c3d_dummyInValues", 1u, false );
			sdw::Array< ValueT > outputValues = writer.declGlobalArray< ValueT >( "c3d_dummyOutValues", 1u, false );
			sortT( writer
				, elementOffset, elementCount
				, groupIndex, threadIndex
				, inputKeys, outputKeys
				, inputValues, outputValues
				, invalidValue );
		}

	private:
		uint32_t m_sortBits;
		sdw::UInt m_maxUInt;
	};
}

#endif
