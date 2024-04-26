/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslBitonicSort_H___
#define ___C3D_GlslBitonicSort_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/ComputeWriter.hpp>

namespace castor3d::shader
{
	/**
	 *\arg	ValueSizeT	The size of the value type.
	 */
	template< uint32_t ValueSizeT >
	struct BitonicSortT
	{
		static uint32_t constexpr bucketSize{ 4096u / ValueSizeT };

		uint32_t threadsCount;

		BitonicSortT( sdw::ComputeWriter & writer
			, uint32_t batchesPerPass )
			: threadsCount{ bucketSize / ( batchesPerPass << 1u ) }
			, m_batchSize{ writer.declConstant( "gBatchSize", sdw::UInt{ bucketSize / batchesPerPass } ) }
			, m_numThreads{ writer.declConstant( "gNumThreads", sdw::UInt{ threadsCount } ) }
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
			, [[maybe_unused]] sdw::UInt threadIndex
			, sdw::Array< sdw::UInt > const & inputKeys
			, sdw::Array< sdw::UInt > const & outputKeys
			, sdw::Array< ValueT > const & inputValues
			, sdw::Array< ValueT > const & outputValues
			, ValueT const & invalidValue )
		{
			auto gsKeys = writer.declSharedVariable< sdw::UInt >( "gsKeys", bucketSize );
			auto gsValues = writer.declSharedVariable< ValueT >( "gsValues", bucketSize, inputValues.isEnabled() );

			auto bitInsert0 = [&]( sdw::UInt const & value
					, sdw::UInt const & bit )
				{
					return ( ( ( m_maxUInt << bit ) & value ) << 1u ) | ( ~( m_maxUInt << bit ) & value );
				};

			auto bitonicSort = [&]()
				{
					auto batchSizeLog = writer.declLocale( "batchSizeLog", writer.cast< sdw::UInt >( findMSB( m_batchSize ) ) );

					// we process a power of two number of elements, 
					auto passCount = writer.declLocale( "passCount", 1u + writer.cast< sdw::UInt >( findMSB( elementCount - 1u ) ) );
					auto roundedElementCount = writer.declLocale( "roundedElementCount", 1u << passCount );
					auto batchCount = writer.declLocale( "batchCount", ( roundedElementCount + m_batchSize - 1u ) >> batchSizeLog );
					// Load data into shared memory. Pad missing values with max ints.

					FOR( writer, sdw::UInt, batch, 0_u, batch < batchCount, ++batch )
					{
						// each thread loads a pair of values per batch.
						auto i1 = writer.declLocale( "i1", groupIndex + batch * m_batchSize );
						auto i2 = writer.declLocale( "i2", i1 + ( m_batchSize >> 1u ) );
						gsKeys[i1] = writer.ternary( i1 < elementCount, inputKeys[elementOffset + i1], m_maxUInt );
						gsKeys[i2] = writer.ternary( i2 < elementCount, inputKeys[elementOffset + i2], m_maxUInt );
						gsValues[i1] = writer.ternary( i1 < elementCount, inputValues[elementOffset + i1], invalidValue );
						gsValues[i2] = writer.ternary( i2 < elementCount, inputValues[elementOffset + i2], invalidValue );
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

						IF( writer, i1 < elementCount )
						{
							outputKeys[elementOffset + i1] = gsKeys[i1];
							outputValues[elementOffset + i1] = gsValues[i1];
						}
						FI
						IF( writer, i2 < elementCount )
						{
							outputKeys[elementOffset + i2] = gsKeys[i2];
							outputValues[elementOffset + i2] = gsValues[i2];
						}
						FI
					}
					ROF
				};
			elementCount = min( sdw::UInt{ bucketSize }, elementCount );

			IF( writer, elementCount > 1_u )
			{
				bitonicSort();
			}
			FI
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
		sdw::UInt m_batchSize;
		sdw::UInt m_numThreads;
		sdw::UInt m_maxUInt;
	};
}

#endif
