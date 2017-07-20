namespace Testing
{
	namespace detail
	{
		template< typename T1, typename T2, uint32_t C1, uint32_t C2 > struct PtOperators
		{
			static inline Testing::Point< T1, C1 > add( Testing::Point< T1, C1 > const & p_ptA, Testing::Point< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer += p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Point< T1, C1 > const & p_ptA, Testing::Point< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer -= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Point< T1, C1 > const & p_ptA, Testing::Point< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer *= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Point< T1, C1 > const & p_ptA, Testing::Point< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer /= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Point< T1, C1 > const & p_ptA, T2 * p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer += p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Point< T1, C1 > const & p_ptA, T2 * p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer -= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Point< T1, C1 > const & p_ptA, T2 * p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer *= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Point< T1, C1 > const & p_ptA, T2 * p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer /= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Point< T1, C1 > const & p_ptA, T2 const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] += p_ptB;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Point< T1, C1 > const & p_ptA, T2 const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] -= p_ptB;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Point< T1, C1 > const & p_ptA, T2 const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] *= p_ptB;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Point< T1, C1 > const & p_ptA, T2 const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] /= p_ptB;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Coords< T1, C1 > const & p_ptA, Testing::Point< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer += p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Coords< T1, C1 > const & p_ptA, Testing::Point< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer -= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Coords< T1, C1 > const & p_ptA, Testing::Point< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer *= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Coords< T1, C1 > const & p_ptA, Testing::Point< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer /= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Coords< T1, C1 > const & p_ptA, T2 * p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer += p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Coords< T1, C1 > const & p_ptA, T2 * p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer -= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Coords< T1, C1 > const & p_ptA, T2 * p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer *= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Coords< T1, C1 > const & p_ptA, T2 * p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer /= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Coords< T1, C1 > const & p_ptA, T2 const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] += p_ptB;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Coords< T1, C1 > const & p_ptA, T2 const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] -= p_ptB;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Coords< T1, C1 > const & p_ptA, T2 const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] *= p_ptB;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Coords< T1, C1 > const & p_ptA, T2 const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] /= p_ptB;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Coords< T1, C1 > const & p_ptA, Testing::Coords< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer += p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Coords< T1, C1 > const & p_ptA, Testing::Coords< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer -= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Coords< T1, C1 > const & p_ptA, Testing::Coords< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer *= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Coords< T1, C1 > const & p_ptA, Testing::Coords< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer /= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Point< T1, C1 > const & p_ptA, Testing::Coords< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer += p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Point< T1, C1 > const & p_ptA, Testing::Coords< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer -= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Point< T1, C1 > const & p_ptA, Testing::Coords< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer *= p_pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Point< T1, C1 > const & p_ptA, Testing::Coords< T2, C2 > const & p_ptB )
			{
				Testing::Point< T1, C1 > result( p_ptA );
				T1 * pBuffer = result.ptr();
				std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
				{
					*pBuffer /= p_pVal;
					pBuffer++;
				} );
				return result;
			}
		};

		template< typename T1, typename T2 > struct PtOperators< T1, T2, 4, 4 >
		{
			static inline Testing::Point< T1, 4 > add( Testing::Point< T1, 4 > const & p_ptA, Testing::Point< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Point< T1, 4 > const & p_ptA, Testing::Point< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Point< T1, 4 > const & p_ptA, Testing::Point< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Point< T1, 4 > const & p_ptA, Testing::Point< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Point< T1, 4 > const & p_ptA, Testing::Coords< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Point< T1, 4 > const & p_ptA, Testing::Coords< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Point< T1, 4 > const & p_ptA, Testing::Coords< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Point< T1, 4 > const & p_ptA, Testing::Coords< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Point< T1, 4 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Point< T1, 4 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Point< T1, 4 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Point< T1, 4 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Point< T1, 4 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB, p_ptA[2] + p_ptB, p_ptA[3] + p_ptB );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Point< T1, 4 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB, p_ptA[2] - p_ptB, p_ptA[3] - p_ptB );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Point< T1, 4 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB, p_ptA[2] * p_ptB, p_ptA[3] * p_ptB );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Point< T1, 4 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB, p_ptA[2] / p_ptB, p_ptA[3] / p_ptB );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Coords< T1, 4 > const & p_ptA, Testing::Point< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Coords< T1, 4 > const & p_ptA, Testing::Point< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Coords< T1, 4 > const & p_ptA, Testing::Point< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Coords< T1, 4 > const & p_ptA, Testing::Point< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Coords< T1, 4 > const & p_ptA, Testing::Coords< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Coords< T1, 4 > const & p_ptA, Testing::Coords< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Coords< T1, 4 > const & p_ptA, Testing::Coords< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Coords< T1, 4 > const & p_ptA, Testing::Coords< T2, 4 > const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Coords< T1, 4 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Coords< T1, 4 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Coords< T1, 4 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Coords< T1, 4 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Coords< T1, 4 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB, p_ptA[2] + p_ptB, p_ptA[3] + p_ptB );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Coords< T1, 4 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB, p_ptA[2] - p_ptB, p_ptA[3] - p_ptB );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Coords< T1, 4 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB, p_ptA[2] * p_ptB, p_ptA[3] * p_ptB );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Coords< T1, 4 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 4 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB, p_ptA[2] / p_ptB, p_ptA[3] / p_ptB );
			}
		};

		template< typename T1, typename T2 > struct PtOperators< T1, T2, 3, 3 >
		{
			static inline Testing::Point< T1, 3 > add( Testing::Point< T1, 3 > const & p_ptA, Testing::Point< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Point< T1, 3 > const & p_ptA, Testing::Point< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Point< T1, 3 > const & p_ptA, Testing::Point< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Point< T1, 3 > const & p_ptA, Testing::Point< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Point< T1, 3 > const & p_ptA, Testing::Coords< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Point< T1, 3 > const & p_ptA, Testing::Coords< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Point< T1, 3 > const & p_ptA, Testing::Coords< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Point< T1, 3 > const & p_ptA, Testing::Coords< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Point< T1, 3 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Point< T1, 3 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Point< T1, 3 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Point< T1, 3 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Point< T1, 3 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB, p_ptA[2] + p_ptB );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Point< T1, 3 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB, p_ptA[2] - p_ptB );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Point< T1, 3 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB, p_ptA[2] * p_ptB );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Point< T1, 3 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB, p_ptA[2] / p_ptB );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Coords< T1, 3 > const & p_ptA, Testing::Point< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Coords< T1, 3 > const & p_ptA, Testing::Point< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Coords< T1, 3 > const & p_ptA, Testing::Point< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Coords< T1, 3 > const & p_ptA, Testing::Point< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Coords< T1, 3 > const & p_ptA, Testing::Coords< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Coords< T1, 3 > const & p_ptA, Testing::Coords< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Coords< T1, 3 > const & p_ptA, Testing::Coords< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Coords< T1, 3 > const & p_ptA, Testing::Coords< T2, 3 > const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Coords< T1, 3 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Coords< T1, 3 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Coords< T1, 3 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Coords< T1, 3 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Coords< T1, 3 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB, p_ptA[2] + p_ptB );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Coords< T1, 3 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB, p_ptA[2] - p_ptB );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Coords< T1, 3 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB, p_ptA[2] * p_ptB );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Coords< T1, 3 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 3 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB, p_ptA[2] / p_ptB );
			}
		};

		template< typename T1, typename T2 > struct PtOperators< T1, T2, 2, 2 >
		{
			static inline Testing::Point< T1, 2 > add( Testing::Point< T1, 2 > const & p_ptA, Testing::Point< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Point< T1, 2 > const & p_ptA, Testing::Point< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Point< T1, 2 > const & p_ptA, Testing::Point< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Point< T1, 2 > const & p_ptA, Testing::Point< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Point< T1, 2 > const & p_ptA, Testing::Coords< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Point< T1, 2 > const & p_ptA, Testing::Coords< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Point< T1, 2 > const & p_ptA, Testing::Coords< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Point< T1, 2 > const & p_ptA, Testing::Coords< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Point< T1, 2 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Point< T1, 2 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Point< T1, 2 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Point< T1, 2 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Point< T1, 2 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Point< T1, 2 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Point< T1, 2 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Point< T1, 2 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Coords< T1, 2 > const & p_ptA, Testing::Point< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Coords< T1, 2 > const & p_ptA, Testing::Point< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Coords< T1, 2 > const & p_ptA, Testing::Point< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Coords< T1, 2 > const & p_ptA, Testing::Point< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Coords< T1, 2 > const & p_ptA, Testing::Coords< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Coords< T1, 2 > const & p_ptA, Testing::Coords< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Coords< T1, 2 > const & p_ptA, Testing::Coords< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Coords< T1, 2 > const & p_ptA, Testing::Coords< T2, 2 > const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Coords< T1, 2 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Coords< T1, 2 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Coords< T1, 2 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Coords< T1, 2 > const & p_ptA, T2 * p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Coords< T1, 2 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Coords< T1, 2 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Coords< T1, 2 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Coords< T1, 2 > const & p_ptA, T2 const & p_ptB )
			{
				return Testing::Point< T1, 2 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB );
			}
		};
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords()
		:	m_coords( NULL	)
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( T * p_pValues )
		:	m_coords( p_pValues	)
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Coords< T, Count > const & p_pt )
		:	m_coords( p_pt.m_coords )
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Coords< T, Count > && p_pt )
		:	m_coords( NULL	)
	{
		m_coords		= std::move( p_pt.m_coords );
		p_pt.m_coords	= NULL;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count >::~Coords()
	{
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( Coords< T, Count > const & p_pt )
	{
		m_coords = p_pt.m_coords;
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( Coords< T, Count > && p_pt )
	{
		if ( this != & p_pt )
		{
			m_coords		= std::move( p_pt.m_coords );
			p_pt.m_coords	= NULL;
		}

		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( Point< T, Count > const & p_pt )
	{
		memcpy( m_coords, p_pt.const_ptr(), binary_size );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( T * p_pValues )
	{
		m_coords = p_pValues;
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator +=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this + p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator -=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this - p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this * p_pt );
		return *this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this / p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator +=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this + p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator -=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this - p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this * p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this / p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator +=( U const * p_coords )
	{
		*this = ( *this + p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator -=( U const * p_coords )
	{
		*this = ( *this - p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( U const * p_coords )
	{
		*this = ( *this * p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( U const * p_coords )
	{
		*this = ( *this / p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( T const & p_coord )
	{
		*this = ( *this * p_coord );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( T const & p_coord )
	{
		*this = ( *this / p_coord );
		return * this;
	}

	template< typename T, uint32_t Count >
	void Coords< T, Count >::swap( Coords< T, Count > & p_pt )
	{
		std::swap( m_coords, p_pt.m_coords );
	}

	template< typename T, uint32_t Count >
	void Coords< T, Count >::flip()
	{
		for ( uint32_t i = 0; i < Count / 2; i++ )
		{
			std::swap( m_coords[i], m_coords[Count - 1 - i] );
		}
	}

	template< typename T, uint32_t Count >
	inline void Coords< T, Count >::to_values( T * p_pResult )const
	{
		if ( m_coords )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				p_pResult[i] = m_coords[i];
			}
		}
	}

	template< typename T, uint32_t Count >
	T const & Coords< T, Count >::at( uint32_t p_pos )const
	{
		if ( p_pos >= Count )
		{
			throw std::range_error( "Coords subscript out of range" );
		}

		return m_coords[p_pos];
	}

	template< typename T, uint32_t Count >
	T & Coords< T, Count >::at( uint32_t p_pos )
	{
		if ( p_pos >= Count )
		{
			throw std::range_error( "Coords subscript out of range" );
		}

		return m_coords[p_pos];
	}

//*************************************************************************************************

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = Castor::Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, Count >::div( p_ptA, p_ptB );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator +( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return detail::PtOperators< T, U, Count, Count >::add( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator -( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return detail::PtOperators< T, U, Count, Count >::sub( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator *( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return detail::PtOperators< T, U, Count, Count >::mul( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator /( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return detail::PtOperators< T, U, Count, Count >::div( p_pt, p_coords );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator *( Coords< T, Count > const & p_pt, T const & p_coord )
	{
		return detail::PtOperators< T, T, Count, Count >::mul( p_pt, p_coord );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator /( Coords< T, Count > const & p_pt, T const & p_coord )
	{
		return detail::PtOperators< T, T, Count, Count >::div( p_pt, p_coord );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator ^( Coords< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB )
	{
		return Point< T, 3 >(
				   ( p_ptA[1] * p_ptB[2] ) - ( p_ptA[2] * p_ptB[1] ),
				   ( p_ptA[2] * p_ptB[0] ) - ( p_ptA[0] * p_ptB[2] ),
				   ( p_ptA[0] * p_ptB[1] ) - ( p_ptA[1] * p_ptB[0] )
			   );
	}
//*************************************************************************************************

	template< typename T, uint32_t Count >
	Point< T, Count >::Point()
	{
		std::memset( m_coords, 0, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const * p_pValues )
	{
		if ( !p_pValues )
		{
			std::memset( m_coords, 0, binary_size );
		}
		else
		{
			std::memcpy( m_coords, p_pValues, binary_size );
		}
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const & p_vA, T const & p_vB )
	{
		m_coords[0] = p_vA;
		m_coords[1] = p_vB;
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const & p_vA, T const & p_vB, T const & p_vC )
	{
		m_coords[0] = p_vA;
		m_coords[1] = p_vB;
		m_coords[2] = p_vC;
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const & p_vA, T const & p_vB, T const & p_vC, T const & p_vD )
	{
		m_coords[0] = p_vA;
		m_coords[1] = p_vB;
		m_coords[2] = p_vC;
		m_coords[3] = p_vD;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	Point< T, Count >::Point( U const * p_pValues )
	{
		if ( !p_pValues )
		{
			std::memset( m_coords, 0, binary_size );
		}
		else
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				m_coords[i] = p_pValues[i];
			}
		}
	}

	template< typename T, uint32_t Count >
	template< typename U, typename V >
	Point< T, Count >::Point( U const & p_vA, V const & p_vB )
	{
		m_coords[0] = T( p_vA );
		m_coords[1] = T( p_vB );
	}

	template< typename T, uint32_t Count >
	template< typename U, typename V, typename W >
	Point< T, Count >::Point( U const & p_vA, V const & p_vB, W const & p_vC )
	{
		m_coords[0] = T( p_vA );
		m_coords[1] = T( p_vB );
		m_coords[2] = T( p_vC );
	}

	template< typename T, uint32_t Count >
	template< typename U, typename V, typename W, typename X >
	Point< T, Count >::Point( U const & p_vA, V const & p_vB, W const & p_vC, X const & p_vD )
	{
		m_coords[0] = T( p_vA );
		m_coords[1] = T( p_vB );
		m_coords[2] = T( p_vC );
		m_coords[3] = T( p_vD );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Point< T, Count > const & p_pt )
	{
		memcpy( m_coords, p_pt.m_coords, binary_size );
		/*
				for( uint32_t i = 0; i < Count; i++ )
				{
					m_coords[i] = p_pt.m_coords[i];
				}
		*/
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Point< T, Count > && p_pt )
	{
		memcpy( m_coords, p_pt.m_coords, binary_size );
		memset( p_pt.m_coords, 0, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Coords< T, Count > const & p_pt )
	{
		if ( !p_pt.const_ptr() )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				m_coords[i] = T();
			}
		}
		else
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				m_coords[i] = p_pt[i];
			}
		}
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count >::~Point()
	{
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator =( Point< T, Count > const & p_pt )
	{
		memcpy( m_coords, p_pt.m_coords, binary_size );
		/*
				for( uint32_t i = 0; i < Count; i++ )
				{
					m_coords[i] = p_pt.m_coords[i];
				}
		*/
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator =( Point< T, Count > && p_pt )
	{
		if ( this != & p_pt )
		{
			memcpy( m_coords, p_pt.m_coords, binary_size );
			memset( p_pt.m_coords, 0, binary_size );
		}

		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator +=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this + p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator -=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this - p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator *=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this * p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator /=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this / p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator +=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this + p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator -=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this - p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator *=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this * p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator /=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this / p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator +=( U const * p_coords )
	{
		*this = ( *this + p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator -=( U const * p_coords )
	{
		*this = ( *this - p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator *=( U const * p_coords )
	{
		*this = ( *this * p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator /=( U const * p_coords )
	{
		*this = ( *this / p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator *=( T const & p_coord )
	{
		*this = ( *this * p_coord );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator /=( T const & p_coord )
	{
		*this = ( *this / p_coord );
		return * this;
	}

	template< typename T, uint32_t Count >
	void Point< T, Count >::swap( Point< T, Count > & p_pt )
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			std::swap( m_coords[i], p_pt.m_coords[i] );
		}
	}

	template< typename T, uint32_t Count >
	void Point< T, Count >::flip()
	{
		for ( uint32_t i = 0; i < Count / 2; i++ )
		{
			std::swap( m_coords[i], m_coords[Count - 1 - i] );
		}
	}

	template< typename T, uint32_t Count >
	inline void Point< T, Count >::to_values( T * p_pResult )const
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			p_pResult[i] = m_coords[i];
		}
	}

	template< typename T, uint32_t Count >
	T const & Point< T, Count >::at( uint32_t p_pos )const
	{
		if ( p_pos >= Count )
		{
			throw std::range_error( "Point subscript out of range" );
		}

		return m_coords[p_pos];
	}

	template< typename T, uint32_t Count >
	T & Point< T, Count >::at( uint32_t p_pos )
	{
		if ( p_pos >= Count )
		{
			throw std::range_error( "Point subscript out of range" );
		}

		return m_coords[p_pos];
	}

//*************************************************************************************************

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = Castor::Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::div( p_ptA, p_ptB );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator ^( Point< T, 3 > const & p_ptA, Point< U, 3 > const & p_ptB )
	{
		return Point< T, 3 >(
				   ( p_ptA[1] * p_ptB[2] ) - ( p_ptA[2] * p_ptB[1] ),
				   ( p_ptA[2] * p_ptB[0] ) - ( p_ptA[0] * p_ptB[2] ),
				   ( p_ptA[0] * p_ptB[1] ) - ( p_ptA[1] * p_ptB[0] )
			   );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator +( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return detail::PtOperators< T, U, Count, Count >::add( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator -( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return detail::PtOperators< T, U, Count, Count >::sub( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator *( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return detail::PtOperators< T, U, Count, Count >::mul( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator /( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return detail::PtOperators< T, U, Count, Count >::div( p_pt, p_coords );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator *( Point< T, Count > const & p_pt, T const & p_coord )
	{
		return detail::PtOperators< T, T, Count, Count >::mul( p_pt, p_coord );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator /( Point< T, Count > const & p_pt, T const & p_coord )
	{
		return detail::PtOperators< T, T, Count, Count >::div( p_pt, p_coord );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = Castor::Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::div( p_ptA, p_ptB );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator ^( Coords< T, 3 > const & p_ptA, Point< U, 3 > const & p_ptB )
	{
		return Point< T, 3 >(
				   ( p_ptA[1] * p_ptB[2] ) - ( p_ptA[2] * p_ptB[1] ),
				   ( p_ptA[2] * p_ptB[0] ) - ( p_ptA[0] * p_ptB[2] ),
				   ( p_ptA[0] * p_ptB[1] ) - ( p_ptA[1] * p_ptB[0] )
			   );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = Castor::Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return detail::PtOperators< T, U, Count, _Count >::div( p_ptA, p_ptB );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator ^( Point< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB )
	{
		return Point< T, 3 >(
				   ( p_ptA[1] * p_ptB[2] ) - ( p_ptA[2] * p_ptB[1] ),
				   ( p_ptA[2] * p_ptB[0] ) - ( p_ptA[0] * p_ptB[2] ),
				   ( p_ptA[0] * p_ptB[1] ) - ( p_ptA[1] * p_ptB[0] )
			   );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator -( Point< T, Count > const & p_pt )
	{
		Point< T, Count > result;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			result[i] = -p_pt[i];
		}

		return result;
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	inline void point::negate( Point< T, Count > & p_point )
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			p_point[i] = -p_point[i];
		}
	}

	template< typename T, uint32_t Count >
	void point::normalise( Point< T, Count > & p_point )
	{
		T tLength = T( distance( p_point ) );

		if ( !Castor::Policy< T >::is_null( tLength ) )
		{
			p_point /= tLength;
		}
	}

	template< typename T, uint32_t Count >
	Point< T, Count > point::get_normalised( Point< T, Count > const & p_point )
	{
		Point< T, Count > result( p_point );
		normalise( result );
		return result;
	}

	template< typename T, uint32_t Count >
	T point::dot( Point< T, Count > const & p_ptA, Point< T, Count > const & p_ptB )
	{
		T tReturn;
		Castor::Policy< T >::init( tReturn );

		for ( uint32_t i = 0; i < Count; i++ )
		{
			tReturn += p_ptA[i] * p_ptB[i];
		}

		return tReturn;
	}

	template< typename T, uint32_t Count >
	double point::cos_theta( Point< T, Count > const & p_ptA, Point< T, Count > const & p_ptB )
	{
		double dReturn = double( distance( p_ptA ) * distance( p_ptB ) );

		if ( dReturn != 0 )
		{
			dReturn = dot( p_ptA, p_ptB ) / dReturn;
		}
		else
		{
			dReturn = dot( p_ptA, p_ptB );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distance_squared( Point< T, Count > const & p_point )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += p_point[i] * p_point[i];
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distance( Point< T, Count > const & p_point )
	{
		return sqrt( distance_squared( p_point ) );
	}

	template< typename T, uint32_t Count >
	inline double point::distance_manhattan( Point< T, Count > const & p_point )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += abs( p_point[i] );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distance_minkowski( Point< T, Count > const & p_point, double p_dOrder )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += pow( double( abs( p_point[i] ) ), p_dOrder );
		}

		dReturn = pow( dReturn, 1.0 / p_dOrder );
		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distance_chebychev( Point< T, Count > const & p_point )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn = std::max( dReturn, double( abs( p_point[i] ) ) );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	T point::dot( Point< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
	{
		T tReturn = T();

		for ( uint32_t i = 0; i < Count; i++ )
		{
			tReturn += p_ptA[i] * p_ptB[i];
		}

		return tReturn;
	}

	template< typename T, uint32_t Count >
	double point::cos_theta( Point< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
	{
		double dReturn = double( distance( p_ptA ) * distance( p_ptB ) );

		if ( dReturn != 0 )
		{
			dReturn = dot( p_ptA, p_ptB ) / dReturn;
		}
		else
		{
			dReturn = dot( p_ptA, p_ptB );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	T point::dot( Coords< T, Count > const & p_ptA, Point< T, Count > const & p_ptB )
	{
		T tReturn;
		Castor::Policy< T >::init( tReturn );

		for ( uint32_t i = 0; i < Count; i++ )
		{
			tReturn += p_ptA[i] * p_ptB[i];
		}

		return tReturn;
	}

	template< typename T, uint32_t Count >
	double point::cos_theta( Coords< T, Count > const & p_ptA, Point< T, Count > const & p_ptB )
	{
		double dReturn = double( distance( p_ptA ) * distance( p_ptB ) );

		if ( dReturn != 0 )
		{
			dReturn = dot( p_ptA, p_ptB ) / dReturn;
		}
		else
		{
			dReturn = dot( p_ptA, p_ptB );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	T point::dot( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
	{
		T tReturn;
		Castor::Policy< T >::init( tReturn );

		for ( uint32_t i = 0; i < Count; i++ )
		{
			tReturn += p_ptA[i] * p_ptB[i];
		}

		return tReturn;
	}

	template< typename T, uint32_t Count >
	double point::cos_theta( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
	{
		double dReturn = double( distance( p_ptA ) * distance( p_ptB ) );

		if ( dReturn != 0 )
		{
			dReturn = dot( p_ptA, p_ptB ) / dReturn;
		}
		else
		{
			dReturn = dot( p_ptA, p_ptB );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	inline void point::negate( Coords< T, Count > & p_point )
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			p_point[i] = -p_point[i];
		}
	}

	template< typename T, uint32_t Count >
	void point::normalise( Coords< T, Count > & p_point )
	{
		T tLength = T( distance( p_point ) );

		if ( !Castor::Policy< T >::is_null( tLength ) )
		{
			p_point /= tLength;
		}
	}

	template< typename T, uint32_t Count >
	double point::distance_squared( Coords< T, Count > const & p_point )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += p_point[i] * p_point[i];
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distance( Coords< T, Count > const & p_point )
	{
		return sqrt( distance_squared( p_point ) );
	}

	template< typename T, uint32_t Count >
	inline double point::distance_manhattan( Coords< T, Count > const & p_point )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += abs( p_point[i] );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distance_minkowski( Coords< T, Count > const & p_point, double p_dOrder )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += pow( double( abs( p_point[i] ) ), p_dOrder );
		}

		dReturn = pow( dReturn, 1.0 / p_dOrder );
		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distance_chebychev( Coords< T, Count > const & p_point )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn = std::max( dReturn, double( abs( p_point[i] ) ) );
		}

		return dReturn;
	}

//*************************************************************************************************
}

//*************************************************************************************************
