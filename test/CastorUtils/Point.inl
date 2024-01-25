namespace Testing
{
	namespace detail
	{
		template< typename T1, typename T2, uint32_t C1, uint32_t C2 > struct PtOperators
		{
			static inline Testing::Point< T1, C1 > add( Testing::Point< T1, C1 > const & lhs, Testing::Point< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer += pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Point< T1, C1 > const & lhs, Testing::Point< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer -= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Point< T1, C1 > const & lhs, Testing::Point< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer *= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Point< T1, C1 > const & lhs, Testing::Point< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer /= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Point< T1, C1 > const & lhs, T2 * rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs, rhs + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer += pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Point< T1, C1 > const & lhs, T2 * rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs, rhs + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer -= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Point< T1, C1 > const & lhs, T2 * rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs, rhs + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer *= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Point< T1, C1 > const & lhs, T2 * rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs, rhs + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer /= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Point< T1, C1 > const & lhs, T2 const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] += rhs;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Point< T1, C1 > const & lhs, T2 const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] -= rhs;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Point< T1, C1 > const & lhs, T2 const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] *= rhs;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Point< T1, C1 > const & lhs, T2 const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] /= rhs;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Coords< T1, C1 > const & lhs, Testing::Point< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer += pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Coords< T1, C1 > const & lhs, Testing::Point< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer -= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Coords< T1, C1 > const & lhs, Testing::Point< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer *= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Coords< T1, C1 > const & lhs, Testing::Point< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer /= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Coords< T1, C1 > const & lhs, T2 * rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs, rhs + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer += pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Coords< T1, C1 > const & lhs, T2 * rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs, rhs + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer -= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Coords< T1, C1 > const & lhs, T2 * rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs, rhs + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer *= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Coords< T1, C1 > const & lhs, T2 * rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs, rhs + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer /= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Coords< T1, C1 > const & lhs, T2 const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] += rhs;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Coords< T1, C1 > const & lhs, T2 const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] -= rhs;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Coords< T1, C1 > const & lhs, T2 const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] *= rhs;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Coords< T1, C1 > const & lhs, T2 const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );

				for ( uint32_t i = 0; i < C1; ++i )
				{
					result[i] /= rhs;
				}

				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Coords< T1, C1 > const & lhs, Testing::Coords< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer += pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Coords< T1, C1 > const & lhs, Testing::Coords< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer -= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Coords< T1, C1 > const & lhs, Testing::Coords< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer *= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Coords< T1, C1 > const & lhs, Testing::Coords< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer /= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > add( Testing::Point< T1, C1 > const & lhs, Testing::Coords< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer += pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > sub( Testing::Point< T1, C1 > const & lhs, Testing::Coords< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer -= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > mul( Testing::Point< T1, C1 > const & lhs, Testing::Coords< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer *= pVal;
					pBuffer++;
				} );
				return result;
			}

			static inline Testing::Point< T1, C1 > div( Testing::Point< T1, C1 > const & lhs, Testing::Coords< T2, C2 > const & rhs )
			{
				Testing::Point< T1, C1 > result( lhs );
				T1 * pBuffer = result.ptr();
				std::for_each( rhs.constPtr(), rhs.constPtr() + std::min( C1, C2 ), [&]( T2 pVal )
				{
					*pBuffer /= pVal;
					pBuffer++;
				} );
				return result;
			}
		};

		template< typename T1, typename T2 > struct PtOperators< T1, T2, 4, 4 >
		{
			static inline Testing::Point< T1, 4 > add( Testing::Point< T1, 4 > const & lhs, Testing::Point< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Point< T1, 4 > const & lhs, Testing::Point< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Point< T1, 4 > const & lhs, Testing::Point< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Point< T1, 4 > const & lhs, Testing::Point< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Point< T1, 4 > const & lhs, Testing::Coords< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Point< T1, 4 > const & lhs, Testing::Coords< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Point< T1, 4 > const & lhs, Testing::Coords< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Point< T1, 4 > const & lhs, Testing::Coords< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Point< T1, 4 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Point< T1, 4 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Point< T1, 4 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Point< T1, 4 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Point< T1, 4 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] + rhs, lhs[1] + rhs, lhs[2] + rhs, lhs[3] + rhs );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Point< T1, 4 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] - rhs, lhs[1] - rhs, lhs[2] - rhs, lhs[3] - rhs );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Point< T1, 4 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Point< T1, 4 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Coords< T1, 4 > const & lhs, Testing::Point< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Coords< T1, 4 > const & lhs, Testing::Point< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Coords< T1, 4 > const & lhs, Testing::Point< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Coords< T1, 4 > const & lhs, Testing::Point< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Coords< T1, 4 > const & lhs, Testing::Coords< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Coords< T1, 4 > const & lhs, Testing::Coords< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Coords< T1, 4 > const & lhs, Testing::Coords< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Coords< T1, 4 > const & lhs, Testing::Coords< T2, 4 > const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Coords< T1, 4 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3] );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Coords< T1, 4 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3] );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Coords< T1, 4 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3] );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Coords< T1, 4 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3] );
			}

			static inline Testing::Point< T1, 4 > add( Testing::Coords< T1, 4 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] + rhs, lhs[1] + rhs, lhs[2] + rhs, lhs[3] + rhs );
			}

			static inline Testing::Point< T1, 4 > sub( Testing::Coords< T1, 4 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] - rhs, lhs[1] - rhs, lhs[2] - rhs, lhs[3] - rhs );
			}

			static inline Testing::Point< T1, 4 > mul( Testing::Coords< T1, 4 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs );
			}

			static inline Testing::Point< T1, 4 > div( Testing::Coords< T1, 4 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 4 >( lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs );
			}
		};

		template< typename T1, typename T2 > struct PtOperators< T1, T2, 3, 3 >
		{
			static inline Testing::Point< T1, 3 > add( Testing::Point< T1, 3 > const & lhs, Testing::Point< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Point< T1, 3 > const & lhs, Testing::Point< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Point< T1, 3 > const & lhs, Testing::Point< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Point< T1, 3 > const & lhs, Testing::Point< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Point< T1, 3 > const & lhs, Testing::Coords< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Point< T1, 3 > const & lhs, Testing::Coords< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Point< T1, 3 > const & lhs, Testing::Coords< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Point< T1, 3 > const & lhs, Testing::Coords< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Point< T1, 3 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Point< T1, 3 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Point< T1, 3 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Point< T1, 3 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Point< T1, 3 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] + rhs, lhs[1] + rhs, lhs[2] + rhs );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Point< T1, 3 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] - rhs, lhs[1] - rhs, lhs[2] - rhs );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Point< T1, 3 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Point< T1, 3 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Coords< T1, 3 > const & lhs, Testing::Point< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Coords< T1, 3 > const & lhs, Testing::Point< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Coords< T1, 3 > const & lhs, Testing::Point< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Coords< T1, 3 > const & lhs, Testing::Point< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Coords< T1, 3 > const & lhs, Testing::Coords< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Coords< T1, 3 > const & lhs, Testing::Coords< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Coords< T1, 3 > const & lhs, Testing::Coords< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Coords< T1, 3 > const & lhs, Testing::Coords< T2, 3 > const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Coords< T1, 3 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2] );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Coords< T1, 3 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2] );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Coords< T1, 3 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2] );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Coords< T1, 3 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2] );
			}

			static inline Testing::Point< T1, 3 > add( Testing::Coords< T1, 3 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] + rhs, lhs[1] + rhs, lhs[2] + rhs );
			}

			static inline Testing::Point< T1, 3 > sub( Testing::Coords< T1, 3 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] - rhs, lhs[1] - rhs, lhs[2] - rhs );
			}

			static inline Testing::Point< T1, 3 > mul( Testing::Coords< T1, 3 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs );
			}

			static inline Testing::Point< T1, 3 > div( Testing::Coords< T1, 3 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 3 >( lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs );
			}
		};

		template< typename T1, typename T2 > struct PtOperators< T1, T2, 2, 2 >
		{
			static inline Testing::Point< T1, 2 > add( Testing::Point< T1, 2 > const & lhs, Testing::Point< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] + rhs[0], lhs[1] + rhs[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Point< T1, 2 > const & lhs, Testing::Point< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] - rhs[0], lhs[1] - rhs[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Point< T1, 2 > const & lhs, Testing::Point< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] * rhs[0], lhs[1] * rhs[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Point< T1, 2 > const & lhs, Testing::Point< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] / rhs[0], lhs[1] / rhs[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Point< T1, 2 > const & lhs, Testing::Coords< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] + rhs[0], lhs[1] + rhs[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Point< T1, 2 > const & lhs, Testing::Coords< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] - rhs[0], lhs[1] - rhs[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Point< T1, 2 > const & lhs, Testing::Coords< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] * rhs[0], lhs[1] * rhs[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Point< T1, 2 > const & lhs, Testing::Coords< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] / rhs[0], lhs[1] / rhs[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Point< T1, 2 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] + rhs[0], lhs[1] + rhs[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Point< T1, 2 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] - rhs[0], lhs[1] - rhs[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Point< T1, 2 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] * rhs[0], lhs[1] * rhs[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Point< T1, 2 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] / rhs[0], lhs[1] / rhs[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Point< T1, 2 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] + rhs, lhs[1] + rhs );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Point< T1, 2 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] - rhs, lhs[1] - rhs );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Point< T1, 2 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] * rhs, lhs[1] * rhs );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Point< T1, 2 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] / rhs, lhs[1] / rhs );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Coords< T1, 2 > const & lhs, Testing::Point< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] + rhs[0], lhs[1] + rhs[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Coords< T1, 2 > const & lhs, Testing::Point< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] - rhs[0], lhs[1] - rhs[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Coords< T1, 2 > const & lhs, Testing::Point< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] * rhs[0], lhs[1] * rhs[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Coords< T1, 2 > const & lhs, Testing::Point< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] / rhs[0], lhs[1] / rhs[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Coords< T1, 2 > const & lhs, Testing::Coords< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] + rhs[0], lhs[1] + rhs[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Coords< T1, 2 > const & lhs, Testing::Coords< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] - rhs[0], lhs[1] - rhs[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Coords< T1, 2 > const & lhs, Testing::Coords< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] * rhs[0], lhs[1] * rhs[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Coords< T1, 2 > const & lhs, Testing::Coords< T2, 2 > const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] / rhs[0], lhs[1] / rhs[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Coords< T1, 2 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] + rhs[0], lhs[1] + rhs[1] );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Coords< T1, 2 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] - rhs[0], lhs[1] - rhs[1] );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Coords< T1, 2 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] * rhs[0], lhs[1] * rhs[1] );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Coords< T1, 2 > const & lhs, T2 * rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] / rhs[0], lhs[1] / rhs[1] );
			}

			static inline Testing::Point< T1, 2 > add( Testing::Coords< T1, 2 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] + rhs, lhs[1] + rhs );
			}

			static inline Testing::Point< T1, 2 > sub( Testing::Coords< T1, 2 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] - rhs, lhs[1] - rhs );
			}

			static inline Testing::Point< T1, 2 > mul( Testing::Coords< T1, 2 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] * rhs, lhs[1] * rhs );
			}

			static inline Testing::Point< T1, 2 > div( Testing::Coords< T1, 2 > const & lhs, T2 const & rhs )
			{
				return Testing::Point< T1, 2 >( lhs[0] / rhs, lhs[1] / rhs );
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
	Coords< T, Count >::Coords( T * rhs )
		:	m_coords( rhs )
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Coords< T, Count > const & rhs )
		:	m_coords( rhs.m_coords )
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Coords< T, Count > && rhs )
		:	m_coords( NULL	)
	{
		m_coords		= castor::move( rhs.m_coords );
		rhs.m_coords	= NULL;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count >::~Coords()
	{
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( Coords< T, Count > const & rhs )
	{
		m_coords = rhs.m_coords;
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( Coords< T, Count > && rhs )
	{
		if ( this != &rhs )
		{
			m_coords		= castor::move( rhs.m_coords );
			rhs.m_coords	= NULL;
		}

		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( Point< T, Count > const & rhs )
	{
		memcpy( m_coords, rhs.constPtr(), binary_size );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( T * rhs )
	{
		m_coords = rhs;
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator +=( Coords< U, _Count > const & rhs )
	{
		*this = ( *this + rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator -=( Coords< U, _Count > const & rhs )
	{
		*this = ( *this - rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( Coords< U, _Count > const & rhs )
	{
		*this = ( *this * rhs );
		return *this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( Coords< U, _Count > const & rhs )
	{
		*this = ( *this / rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator +=( Point< U, _Count > const & rhs )
	{
		*this = ( *this + rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator -=( Point< U, _Count > const & rhs )
	{
		*this = ( *this - rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( Point< U, _Count > const & rhs )
	{
		*this = ( *this * rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( Point< U, _Count > const & rhs )
	{
		*this = ( *this / rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator +=( U const * rhs )
	{
		*this = ( *this + rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator -=( U const * rhs )
	{
		*this = ( *this - rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( U const * rhs )
	{
		*this = ( *this * rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( U const * rhs )
	{
		*this = ( *this / rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( T const & rhs )
	{
		*this = ( *this * rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( T const & rhs )
	{
		*this = ( *this / rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	void Coords< T, Count >::swap( Coords< T, Count > & rhs )
	{
		castor::swap( m_coords, rhs.m_coords );
	}

//*************************************************************************************************

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Coords< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = castor::Policy< T >::equals( lhs[i], rhs[i] );
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Coords< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return !( lhs == rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Coords< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::add( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Coords< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::sub( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Coords< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Coords< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, Count >::div( lhs, rhs );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator +( Coords< T, Count > const & lhs, U const * rhs )
	{
		return detail::PtOperators< T, U, Count, Count >::add( lhs, rhs );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator -( Coords< T, Count > const & lhs, U const * rhs )
	{
		return detail::PtOperators< T, U, Count, Count >::sub( lhs, rhs );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator *( Coords< T, Count > const & lhs, U const * rhs )
	{
		return detail::PtOperators< T, U, Count, Count >::mul( lhs, rhs );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator /( Coords< T, Count > const & lhs, U const * rhs )
	{
		return detail::PtOperators< T, U, Count, Count >::div( lhs, rhs );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator *( Coords< T, Count > const & lhs, T const & rhs )
	{
		return detail::PtOperators< T, T, Count, Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator /( Coords< T, Count > const & lhs, T const & rhs )
	{
		return detail::PtOperators< T, T, Count, Count >::div( lhs, rhs );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator ^( Coords< T, 3 > const & lhs, Coords< U, 3 > const & rhs )
	{
		return Point< T, 3 >(
				   ( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				   ( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				   ( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
			   );
	}
//*************************************************************************************************

	template< typename T, uint32_t Count >
	Point< T, Count >::Point()
	{
		std::memset( m_coords, 0, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const * pValues )
	{
		if ( !pValues )
		{
			std::memset( m_coords, 0, binary_size );
		}
		else
		{
			std::memcpy( m_coords, pValues, binary_size );
		}
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const & a, T const & b )
	{
		m_coords[0] = a;
		m_coords[1] = b;
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const & a, T const & b, T const & c )
	{
		m_coords[0] = a;
		m_coords[1] = b;
		m_coords[2] = c;
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const & a, T const & b, T const & c, T const & d )
	{
		m_coords[0] = a;
		m_coords[1] = b;
		m_coords[2] = c;
		m_coords[3] = d;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	Point< T, Count >::Point( U const * rhs )
	{
		if ( !rhs )
		{
			std::memset( m_coords, 0, binary_size );
		}
		else
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				m_coords[i] = rhs[i];
			}
		}
	}

	template< typename T, uint32_t Count >
	template< typename U, typename V >
	Point< T, Count >::Point( U const & a, V const & b )
	{
		m_coords[0] = T( a );
		m_coords[1] = T( b );
	}

	template< typename T, uint32_t Count >
	template< typename U, typename V, typename W >
	Point< T, Count >::Point( U const & a, V const & b, W const & c )
	{
		m_coords[0] = T( a );
		m_coords[1] = T( b );
		m_coords[2] = T( c );
	}

	template< typename T, uint32_t Count >
	template< typename U, typename V, typename W, typename X >
	Point< T, Count >::Point( U const & a, V const & b, W const & c, X const & d )
	{
		m_coords[0] = T( a );
		m_coords[1] = T( b );
		m_coords[2] = T( c );
		m_coords[3] = T( d );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Point< T, Count > const & rhs )
	{
		memcpy( m_coords, rhs.m_coords, binary_size );
		/*
				for( uint32_t i = 0; i < Count; i++ )
				{
					m_coords[i] = rhs.m_coords[i];
				}
		*/
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Point< T, Count > && rhs )
	{
		memcpy( m_coords, rhs.m_coords, binary_size );
		memset( rhs.m_coords, 0, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Coords< T, Count > const & rhs )
	{
		if ( !rhs.constPtr() )
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
				m_coords[i] = rhs[i];
			}
		}
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count >::~Point()
	{
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator =( Point< T, Count > const & rhs )
	{
		memcpy( m_coords, rhs.m_coords, binary_size );
		/*
				for( uint32_t i = 0; i < Count; i++ )
				{
					m_coords[i] = rhs.m_coords[i];
				}
		*/
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator =( Point< T, Count > && rhs )
	{
		if ( this != &rhs )
		{
			memcpy( m_coords, rhs.m_coords, binary_size );
			memset( rhs.m_coords, 0, binary_size );
		}

		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator +=( Point< U, _Count > const & rhs )
	{
		*this = ( *this + rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator -=( Point< U, _Count > const & rhs )
	{
		*this = ( *this - rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator *=( Point< U, _Count > const & rhs )
	{
		*this = ( *this * rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator /=( Point< U, _Count > const & rhs )
	{
		*this = ( *this / rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator +=( Coords< U, _Count > const & rhs )
	{
		*this = ( *this + rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator -=( Coords< U, _Count > const & rhs )
	{
		*this = ( *this - rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator *=( Coords< U, _Count > const & rhs )
	{
		*this = ( *this * rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator /=( Coords< U, _Count > const & rhs )
	{
		*this = ( *this / rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator +=( U const * rhs )
	{
		*this = ( *this + rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator -=( U const * rhs )
	{
		*this = ( *this - rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator *=( U const * rhs )
	{
		*this = ( *this * rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator /=( U const * rhs )
	{
		*this = ( *this / rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator *=( T const & rhs )
	{
		*this = ( *this * rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator /=( T const & rhs )
	{
		*this = ( *this / rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	void Point< T, Count >::swap( Point< T, Count > & rhs )
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			castor::swap( m_coords[i], rhs.m_coords[i] );
		}
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = castor::Policy< T >::equals( lhs[i], rhs[i] );
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return !( lhs == rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::add( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::sub( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::div( lhs, rhs );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator ^( Point< T, 3 > const & lhs, Point< U, 3 > const & rhs )
	{
		return Point< T, 3 >(
				   ( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				   ( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				   ( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
			   );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator +( Point< T, Count > const & lhs, U const * rhs )
	{
		return detail::PtOperators< T, U, Count, Count >::add( lhs, rhs );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator -( Point< T, Count > const & lhs, U const * rhs )
	{
		return detail::PtOperators< T, U, Count, Count >::sub( lhs, rhs );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator *( Point< T, Count > const & lhs, U const * rhs )
	{
		return detail::PtOperators< T, U, Count, Count >::mul( lhs, rhs );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator /( Point< T, Count > const & lhs, U const * rhs )
	{
		return detail::PtOperators< T, U, Count, Count >::div( lhs, rhs );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator *( Point< T, Count > const & lhs, T const & rhs )
	{
		return detail::PtOperators< T, T, Count, Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator /( Point< T, Count > const & lhs, T const & rhs )
	{
		return detail::PtOperators< T, T, Count, Count >::div( lhs, rhs );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = castor::Policy< T >::equals( lhs[i], rhs[i] );
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return !( lhs == rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::add( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::sub( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::div( lhs, rhs );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator ^( Coords< T, 3 > const & lhs, Point< U, 3 > const & rhs )
	{
		return Point< T, 3 >(
				   ( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				   ( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				   ( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
			   );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = castor::Policy< T >::equals( lhs[i], rhs[i] );
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return !( lhs == rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::add( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::sub( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return detail::PtOperators< T, U, Count, _Count >::div( lhs, rhs );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator ^( Point< T, 3 > const & lhs, Coords< U, 3 > const & rhs )
	{
		return Point< T, 3 >(
				   ( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				   ( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				   ( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
			   );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator -( Point< T, Count > const & rhs )
	{
		Point< T, Count > result;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			result[i] = -rhs[i];
		}

		return result;
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	inline void point::negate( Point< T, Count > & rhs )
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			rhs[i] = -rhs[i];
		}
	}

	template< typename T, uint32_t Count >
	void point::normalise( Point< T, Count > & rhs )
	{
		T tLength = T( distance( rhs ) );

		if ( !castor::Policy< T >::isNull( tLength ) )
		{
			rhs /= tLength;
		}
	}

	template< typename T, uint32_t Count >
	Point< T, Count > point::getNormalised( Point< T, Count > const & rhs )
	{
		Point< T, Count > result( rhs );
		normalise( result );
		return result;
	}

	template< typename T, uint32_t Count >
	T point::dot( Point< T, Count > const & lhs, Point< T, Count > const & rhs )
	{
		T tReturn;
		castor::Policy< T >::initialise( tReturn );

		for ( uint32_t i = 0; i < Count; i++ )
		{
			tReturn += lhs[i] * rhs[i];
		}

		return tReturn;
	}

	template< typename T, uint32_t Count >
	double point::cosTheta( Point< T, Count > const & lhs, Point< T, Count > const & rhs )
	{
		double dReturn = double( distance( lhs ) * distance( rhs ) );

		if ( dReturn != 0 )
		{
			dReturn = dot( lhs, rhs ) / dReturn;
		}
		else
		{
			dReturn = dot( lhs, rhs );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distanceSquared( Point< T, Count > const & rhs )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += rhs[i] * rhs[i];
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distance( Point< T, Count > const & rhs )
	{
		return sqrt( distanceSquared( rhs ) );
	}

	template< typename T, uint32_t Count >
	inline double point::distanceManhattan( Point< T, Count > const & rhs )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += abs( rhs[i] );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distanceMinkowski( Point< T, Count > const & rhs, double order )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += pow( double( abs( rhs[i] ) ), order );
		}

		dReturn = pow( dReturn, 1.0 / order );
		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distanceChebychev( Point< T, Count > const & rhs )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn = std::max( dReturn, double( abs( rhs[i] ) ) );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	T point::dot( Point< T, Count > const & lhs, Coords< T, Count > const & rhs )
	{
		T tReturn = T();

		for ( uint32_t i = 0; i < Count; i++ )
		{
			tReturn += lhs[i] * rhs[i];
		}

		return tReturn;
	}

	template< typename T, uint32_t Count >
	double point::cosTheta( Point< T, Count > const & lhs, Coords< T, Count > const & rhs )
	{
		double dReturn = double( distance( lhs ) * distance( rhs ) );

		if ( dReturn != 0 )
		{
			dReturn = dot( lhs, rhs ) / dReturn;
		}
		else
		{
			dReturn = dot( lhs, rhs );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	T point::dot( Coords< T, Count > const & lhs, Point< T, Count > const & rhs )
	{
		T tReturn;
		castor::Policy< T >::initialise( tReturn );

		for ( uint32_t i = 0; i < Count; i++ )
		{
			tReturn += lhs[i] * rhs[i];
		}

		return tReturn;
	}

	template< typename T, uint32_t Count >
	double point::cosTheta( Coords< T, Count > const & lhs, Point< T, Count > const & rhs )
	{
		double dReturn = double( distance( lhs ) * distance( rhs ) );

		if ( dReturn != 0 )
		{
			dReturn = dot( lhs, rhs ) / dReturn;
		}
		else
		{
			dReturn = dot( lhs, rhs );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	T point::dot( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs )
	{
		T tReturn;
		castor::Policy< T >::initialise( tReturn );

		for ( uint32_t i = 0; i < Count; i++ )
		{
			tReturn += lhs[i] * rhs[i];
		}

		return tReturn;
	}

	template< typename T, uint32_t Count >
	double point::cosTheta( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs )
	{
		double dReturn = double( distance( lhs ) * distance( rhs ) );

		if ( dReturn != 0 )
		{
			dReturn = dot( lhs, rhs ) / dReturn;
		}
		else
		{
			dReturn = dot( lhs, rhs );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	inline void point::negate( Coords< T, Count > & rhs )
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			rhs[i] = -rhs[i];
		}
	}

	template< typename T, uint32_t Count >
	void point::normalise( Coords< T, Count > & rhs )
	{
		T tLength = T( distance( rhs ) );

		if ( !castor::Policy< T >::isNull( tLength ) )
		{
			rhs /= tLength;
		}
	}

	template< typename T, uint32_t Count >
	double point::distanceSquared( Coords< T, Count > const & rhs )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += rhs[i] * rhs[i];
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distance( Coords< T, Count > const & rhs )
	{
		return sqrt( distanceSquared( rhs ) );
	}

	template< typename T, uint32_t Count >
	inline double point::distanceManhattan( Coords< T, Count > const & rhs )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += abs( rhs[i] );
		}

		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distanceMinkowski( Coords< T, Count > const & rhs, double order )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn += pow( double( abs( rhs[i] ) ), order );
		}

		dReturn = pow( dReturn, 1.0 / order );
		return dReturn;
	}

	template< typename T, uint32_t Count >
	double point::distanceChebychev( Coords< T, Count > const & rhs )
	{
		double dReturn = 0.0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			dReturn = std::max( dReturn, double( abs( rhs[i] ) ) );
		}

		return dReturn;
	}

//*************************************************************************************************
}

//*************************************************************************************************
