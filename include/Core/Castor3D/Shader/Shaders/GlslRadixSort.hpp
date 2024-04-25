/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslRadixSort_H___
#define ___C3D_GlslRadixSort_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/ComputeWriter.hpp>

namespace castor3d::shader::radix
{
	enum BindingPoints
	{
		eInputKeys,
		eInputValues,
		eOutputKeys,
		eOutputValues,
	};
	template< uint32_t ValueSizeT >
	inline uint32_t constexpr sortBucketSizeT = 4096u / ValueSizeT;
	/**
	 *\arg	SortBitsT	The number of bits to consider sorting.
	 *\arg	ValueSizeT	The size of the value type.
	 *\arg	ValueT		The value type.
	 */
	template< uint32_t SortBitsT, uint32_t ValueSizeT, typename ValueT >
	static void createShaderT( sdw::ComputeWriter & writer
		, ValueT const & invalidValue )
	{
		static uint32_t constexpr NumThreads = sortBucketSizeT< ValueSizeT >;

		auto inputKeysBuffer = writer.declStorageBuffer( "c3d_inputKeysBuffer"
			, uint32_t( eInputKeys )
			, 0u );
		auto c3d_inputKeys = inputKeysBuffer.declMemberArray< sdw::UInt >( "ik" );
		inputKeysBuffer.end();

		auto inputValuesBuffer = writer.declStorageBuffer( "c3d_inputValuesBuffer"
			, uint32_t( eInputValues )
			, 0u
			, sdw::type::MemoryLayout::eStd430
			, invalidValue.isEnabled() );
		auto c3d_inputValues = inputValuesBuffer.declMemberArray< ValueT >( "iv"
			, invalidValue.isEnabled() );
		inputValuesBuffer.end();

		auto outputKeysBuffer = writer.declStorageBuffer( "c3d_outputKeysBuffer"
			, uint32_t( eOutputKeys )
			, 0u );
		auto c3d_outputKeys = outputKeysBuffer.declMemberArray< sdw::UInt >( "ok" );
		outputKeysBuffer.end();

		auto outputValuesBuffer = writer.declStorageBuffer( "c3d_outputValuesBuffer"
			, uint32_t( eOutputValues )
			, 0u
			, sdw::type::MemoryLayout::eStd430
			, invalidValue.isEnabled() );
		auto c3d_outputValues = outputValuesBuffer.declMemberArray< ValueT >( "ov"
			, invalidValue.isEnabled() );
		outputValuesBuffer.end();

		sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
		auto c3d_numElements = pcb.declMember< sdw::UInt >( "c3d_numElements" );
		pcb.end();

		auto gsKeys = writer.declSharedVariable< sdw::UInt >( "gsKeys", NumThreads ); // A temporary buffer to store the input keys. (NumThreads * sizeof(uint) Bytes)
		auto gsValues = writer.declSharedVariable< ValueT >( "gsValues", NumThreads, invalidValue.isEnabled() ); // A temporary buffer to store the input values. (NumThreads * ValueSizeT Bytes)
		auto gsE = writer.declSharedVariable< sdw::UInt >( "gsE", NumThreads ); // Set a 1 for all false sort keys (b == 0) and a 0 for all true sort keys (b == 1) (NumThreads * sizeof(uint) Bytes)
		auto gsF = writer.declSharedVariable< sdw::UInt >( "gsF", NumThreads ); // Scan the splits. This results in the output index of all false sort keys (b == 0) (NumThreads * sizeof(uint) Bytes)
		auto gsD = writer.declSharedVariable< sdw::UInt >( "gsD", NumThreads ); // The desination index for the ouput key and value. (NumThreads * sizeof(uint) Bytes)
		auto gsTotalFalses = writer.declSharedVariable< sdw::UInt >( "gsTotalFalses" ); // The result of e[NumThreads - 1] + f[NumThreads - 1]; (4 Bytes)

		writer.implementMainT< sdw::VoidT >( NumThreads
			, [&]( sdw::ComputeIn const & in )
			{
				auto const & groupIndex = in.localInvocationIndex;
				auto const & threadIndex = in.globalInvocationID.x();

				// Store the input key and values into shared memory.
				gsKeys[groupIndex] = writer.ternary( threadIndex < c3d_numElements, c3d_inputKeys[threadIndex], sdw::UInt{ UINT_MAX } );
				gsValues[groupIndex] = writer.ternary( threadIndex < c3d_numElements, c3d_inputValues[threadIndex], invalidValue );

				// Loop over the bits starting at the least-significant bit.
				FOR( writer, sdw::UInt, b, 0_u, b < SortBitsT, ++b )
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
					for ( u32 i = 1; i < NumThreads; i <<= 1u )
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
						gsTotalFalses = gsE[NumThreads - 1u] + gsF[NumThreads - 1u];
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
				c3d_outputKeys[threadIndex] = gsKeys[groupIndex];
				c3d_outputValues[threadIndex] = gsValues[groupIndex];
			} );
	}
}

#endif
