/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslBitonicSort_H___
#define ___C3D_GlslBitonicSort_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/ComputeWriter.hpp>

namespace castor3d::shader
{
	struct BitonicSort
	{
		static uint32_t constexpr BlockSize = 1024u;
		static uint32_t constexpr BatchesPerPass = 8u;
		static uint32_t constexpr ValuesPerThread = BatchesPerPass << 1u;
		static uint32_t constexpr NumThreads = BlockSize / ValuesPerThread;

		sdw::UInt m_batchSize;
		sdw::UInt m_numThreads;
		sdw::UInt m_maxUInt;

		BitonicSort( sdw::ComputeWriter & writer )
			: m_batchSize{ writer.declConstant( "gBatchSize", sdw::UInt{ BlockSize / BatchesPerPass } ) }
			, m_numThreads{ writer.declConstant( "gNumThreads", sdw::UInt{ NumThreads } ) }
			, m_maxUInt{ writer.declConstant( "gMaxUInt", 0xFFFFFFFF_u ) }
		{
		}
		/**
		 *\arg	ValueT		The value type.
		 */
		template< typename ValueT >
		void sortT( sdw::ComputeWriter & writer
			, sdw::UInt const & elementOffset
			, sdw::UInt const & elementCount
			, sdw::Array< sdw::UInt > const & inKeys
			, sdw::Array< sdw::UInt > const & outKeys
			, sdw::UInt const & localInvocationIndex
			, sdw::Array< ValueT > const & inValues
			, sdw::Array< ValueT > const & outValues
			, ValueT const & invalidValue )
		{
			auto gsKeys = writer.declSharedVariable< sdw::UInt >( "gsKeys", BlockSize );
			auto gsValues = writer.declSharedVariable< ValueT >( "gsValues", BlockSize, inValues.isEnabled() );

			auto bitInsert0 = [&]( sdw::UInt const & value
					, sdw::UInt const & bit )
				{
					return ( ( ( m_maxUInt << bit ) & value ) << 1u ) | ( ~( m_maxUInt << bit ) & value );
				};

			auto bitonicSort = [&]( sdw::UInt const & groupIndex
					, sdw::UInt const & offset
					, sdw::UInt const & numElements )
				{
					// start with simple version, do everything in group shared memory
					auto batchSizeLog = writer.declLocale( "batchSizeLog", writer.cast< sdw::UInt >( findMSB( m_batchSize ) ) );

					// we process a power of two number of elements, 
					auto passCount = writer.declLocale( "passCount", 1u + writer.cast< sdw::UInt >( findMSB( numElements - 1u ) ) );
					auto roundedElementCount = writer.declLocale( "roundedElementCount", 1u << passCount );
					auto batchCount = writer.declLocale( "batchCount", ( roundedElementCount + m_batchSize - 1u ) >> batchSizeLog );
					// Load data into shared memory. Pad missing values with max ints.

					FOR( writer, sdw::UInt, batch, 0_u, batch < batchCount, ++batch )
					{
						// each thread loads a pair of values per batch.
						auto i1 = writer.declLocale( "i1", groupIndex + batch * m_batchSize );
						auto i2 = writer.declLocale( "i2", i1 + ( m_batchSize >> 1u ) );
						gsKeys[i1] = writer.ternary( i1 < numElements, inKeys[offset + i1], m_maxUInt );
						gsKeys[i2] = writer.ternary( i2 < numElements, inKeys[offset + i2], m_maxUInt );
						gsValues[i1] = writer.ternary( i1 < numElements, inValues[offset + i1], invalidValue );
						gsValues[i2] = writer.ternary( i2 < numElements, inValues[offset + i2], invalidValue );
					}
					ROF

					shader::groupMemoryBarrierWithGroupSync( writer );
	
					// Each loop iteration produces blocks of size k that are monotonic (alternatively increasing and decreasing)
					// thus, producing blocks of size 2*k that are bitonic.
					// as a result, the last pass produces a single block sorted in ascending order
					FOR( writer, sdw::UInt, pass, 0_u, pass < passCount, ++pass )
					{
						auto k = writer.declLocale( "k", 1_u << ( pass + 1u ) );
						// Each iteration compares and optionally swap elements in pairs exactly once for each element
						FOR( writer, sdw::UInt, subPass, 0_u, subPass <= pass, ++subPass )
						{
							FOR( writer, sdw::UInt, batch, 0_u, batch < batchCount, ++batch )
							{
								auto indexBit = writer.declLocale( "indexBit", groupIndex + batch * m_numThreads );
								auto relPass = writer.declLocale( "relPass", pass - subPass );
								auto indexFirst = writer.declLocale( "indexFirst", bitInsert0( indexBit, relPass ) );
								auto indexSecond = writer.declLocale( "indexSecond", indexFirst | ( 1u << relPass ) );
								auto keyFirst = writer.declLocale( "keyFirst", gsKeys[indexFirst] );
								auto keySecond = writer.declLocale( "keySecond", gsKeys[indexSecond] );
								auto valFirst = writer.declLocale( "valFirst", gsValues[indexFirst] );
								auto valSecond = writer.declLocale( "valSecond", gsValues[indexSecond] );
								shader::groupMemoryBarrierWithGroupSync( writer );

								IF( writer, writer.ternary( ( indexFirst & k ) == 0_u, 1_u, 0_u ) ^ writer.ternary( keyFirst <= keySecond, 1_u, 0_u ) )
								{
									gsKeys[indexFirst] = keySecond;
									gsKeys[indexSecond] = keyFirst;
									gsValues[indexFirst] = valSecond;
									gsValues[indexSecond] = valFirst;
								}
								FI

								shader::groupMemoryBarrierWithGroupSync( writer );
							}
							ROF
						}
						ROF
					}
					ROF

					// Now commit the results to global memory.
					FOR( writer, sdw::UInt, batch, 0_u, batch < batchCount, ++batch )
					{
						auto i1 = writer.declLocale( "i1", groupIndex + batch * m_batchSize );
						auto i2 = writer.declLocale( "i2", i1 + ( m_batchSize >> 1u ) );

						IF( writer, i1 < numElements )
						{
							outKeys[offset + i1] = gsKeys[i1];
							outValues[offset + i1] = gsValues[i1];
						}
						FI
						IF( writer, i2 < numElements )
						{
							outKeys[offset + i2] = gsKeys[i2];
							outValues[offset + i2] = gsValues[i2];
						}
						FI
					}
					ROF
				};

			IF( writer, elementCount > 1_u )
			{
				bitonicSort( localInvocationIndex, elementOffset, elementCount );
			}
			FI
		}
		/**
		 *\arg	ValueT		The value type.
		 */
		template< typename ValueT >
		void sortT( sdw::ComputeWriter & writer
			, sdw::UInt const & elementOffset
			, sdw::UInt const & elementCount
			, sdw::Array< sdw::UInt > const & inKeys
			, sdw::Array< sdw::UInt > const & outKeys
			, sdw::UInt const & localInvocationIndex
			, ValueT const & invalidValue )
		{
			sdw::Array< ValueT > inValues = writer.declGlobalArray< ValueT >( "c3d_dummyInValues", 1u, false );
			sdw::Array< ValueT > outValues = writer.declGlobalArray< ValueT >( "c3d_dummyOutValues", 1u, false );
			sortT( writer, elementOffset, elementCount
				, inKeys, outKeys, localInvocationIndex
				, inValues, outValues, invalidValue );
		}
	};
}

#endif
