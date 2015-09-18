#include <cstring>

namespace
{
	template< typename T1, typename T2, uint32_t C1, uint32_t C2 > struct PtOperators
	{
		static inline Castor::Point< T1, C1 > add( Castor::Point< T1, C1 > const & p_ptA, Castor::Point< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer += p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > sub( Castor::Point< T1, C1 > const & p_ptA, Castor::Point< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer -= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > mul( Castor::Point< T1, C1 > const & p_ptA, Castor::Point< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer *= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > div( Castor::Point< T1, C1 > const & p_ptA, Castor::Point< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer /= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > add( Castor::Point< T1, C1 > const & p_ptA, T2 * p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer += p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > sub( Castor::Point< T1, C1 > const & p_ptA, T2 * p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer -= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > mul( Castor::Point< T1, C1 > const & p_ptA, T2 * p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer *= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > div( Castor::Point< T1, C1 > const & p_ptA, T2 * p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer /= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > add( Castor::Point< T1, C1 > const & p_ptA, T2 const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );

			for ( uint32_t i = 0; i < C1; ++i )
			{
				l_ptReturn[i] += p_ptB;
			}

			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > sub( Castor::Point< T1, C1 > const & p_ptA, T2 const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );

			for ( uint32_t i = 0; i < C1; ++i )
			{
				l_ptReturn[i] -= p_ptB;
			}

			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > mul( Castor::Point< T1, C1 > const & p_ptA, T2 const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );

			for ( uint32_t i = 0; i < C1; ++i )
			{
				l_ptReturn[i] *= p_ptB;
			}

			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > div( Castor::Point< T1, C1 > const & p_ptA, T2 const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );

			for ( uint32_t i = 0; i < C1; ++i )
			{
				l_ptReturn[i] /= p_ptB;
			}

			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > add( Castor::Coords< T1, C1 > const & p_ptA, Castor::Point< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer += p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > sub( Castor::Coords< T1, C1 > const & p_ptA, Castor::Point< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer -= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > mul( Castor::Coords< T1, C1 > const & p_ptA, Castor::Point< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer *= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > div( Castor::Coords< T1, C1 > const & p_ptA, Castor::Point< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer /= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > add( Castor::Coords< T1, C1 > const & p_ptA, T2 * p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer += p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > sub( Castor::Coords< T1, C1 > const & p_ptA, T2 * p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer -= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > mul( Castor::Coords< T1, C1 > const & p_ptA, T2 * p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer *= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > div( Castor::Coords< T1, C1 > const & p_ptA, T2 * p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB, p_ptB + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer /= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > add( Castor::Coords< T1, C1 > const & p_ptA, T2 const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );

			for ( uint32_t i = 0; i < C1; ++i )
			{
				l_ptReturn[i] += p_ptB;
			}

			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > sub( Castor::Coords< T1, C1 > const & p_ptA, T2 const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );

			for ( uint32_t i = 0; i < C1; ++i )
			{
				l_ptReturn[i] -= p_ptB;
			}

			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > mul( Castor::Coords< T1, C1 > const & p_ptA, T2 const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );

			for ( uint32_t i = 0; i < C1; ++i )
			{
				l_ptReturn[i] *= p_ptB;
			}

			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > div( Castor::Coords< T1, C1 > const & p_ptA, T2 const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );

			for ( uint32_t i = 0; i < C1; ++i )
			{
				l_ptReturn[i] /= p_ptB;
			}

			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > add( Castor::Coords< T1, C1 > const & p_ptA, Castor::Coords< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer += p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > sub( Castor::Coords< T1, C1 > const & p_ptA, Castor::Coords< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer -= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > mul( Castor::Coords< T1, C1 > const & p_ptA, Castor::Coords< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer *= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > div( Castor::Coords< T1, C1 > const & p_ptA, Castor::Coords< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer /= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > add( Castor::Point< T1, C1 > const & p_ptA, Castor::Coords< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer += p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > sub( Castor::Point< T1, C1 > const & p_ptA, Castor::Coords< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer -= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > mul( Castor::Point< T1, C1 > const & p_ptA, Castor::Coords< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer *= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}

		static inline Castor::Point< T1, C1 > div( Castor::Point< T1, C1 > const & p_ptA, Castor::Coords< T2, C2 > const & p_ptB )
		{
			Castor::Point< T1, C1 > l_ptReturn( p_ptA );
			T1 * l_pBuffer = l_ptReturn.ptr();
			std::for_each( p_ptB.const_ptr(), p_ptB.const_ptr() + std::min( C1, C2 ), [&]( T2 p_pVal )
			{
				*l_pBuffer /= p_pVal;
				l_pBuffer++;
			} );
			return l_ptReturn;
		}
	};

	template< typename T1, typename T2 > struct PtOperators< T1, T2, 4, 4 >
	{
		static inline Castor::Point< T1, 4 > add( Castor::Point< T1, 4 > const & p_ptA, Castor::Point< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > sub( Castor::Point< T1, 4 > const & p_ptA, Castor::Point< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > mul( Castor::Point< T1, 4 > const & p_ptA, Castor::Point< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > div( Castor::Point< T1, 4 > const & p_ptA, Castor::Point< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > add( Castor::Point< T1, 4 > const & p_ptA, Castor::Coords< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > sub( Castor::Point< T1, 4 > const & p_ptA, Castor::Coords< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > mul( Castor::Point< T1, 4 > const & p_ptA, Castor::Coords< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > div( Castor::Point< T1, 4 > const & p_ptA, Castor::Coords< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > add( Castor::Point< T1, 4 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > sub( Castor::Point< T1, 4 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > mul( Castor::Point< T1, 4 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > div( Castor::Point< T1, 4 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > add( Castor::Point< T1, 4 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB, p_ptA[2] + p_ptB, p_ptA[3] + p_ptB );
		}

		static inline Castor::Point< T1, 4 > sub( Castor::Point< T1, 4 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB, p_ptA[2] - p_ptB, p_ptA[3] - p_ptB );
		}

		static inline Castor::Point< T1, 4 > mul( Castor::Point< T1, 4 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB, p_ptA[2] * p_ptB, p_ptA[3] * p_ptB );
		}

		static inline Castor::Point< T1, 4 > div( Castor::Point< T1, 4 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB, p_ptA[2] / p_ptB, p_ptA[3] / p_ptB );
		}

		static inline Castor::Point< T1, 4 > add( Castor::Coords< T1, 4 > const & p_ptA, Castor::Point< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > sub( Castor::Coords< T1, 4 > const & p_ptA, Castor::Point< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > mul( Castor::Coords< T1, 4 > const & p_ptA, Castor::Point< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > div( Castor::Coords< T1, 4 > const & p_ptA, Castor::Point< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > add( Castor::Coords< T1, 4 > const & p_ptA, Castor::Coords< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > sub( Castor::Coords< T1, 4 > const & p_ptA, Castor::Coords< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > mul( Castor::Coords< T1, 4 > const & p_ptA, Castor::Coords< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > div( Castor::Coords< T1, 4 > const & p_ptA, Castor::Coords< T2, 4 > const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > add( Castor::Coords< T1, 4 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2], p_ptA[3] + p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > sub( Castor::Coords< T1, 4 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2], p_ptA[3] - p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > mul( Castor::Coords< T1, 4 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2], p_ptA[3] * p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > div( Castor::Coords< T1, 4 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2], p_ptA[3] / p_ptB[3] );
		}

		static inline Castor::Point< T1, 4 > add( Castor::Coords< T1, 4 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB, p_ptA[2] + p_ptB, p_ptA[3] + p_ptB );
		}

		static inline Castor::Point< T1, 4 > sub( Castor::Coords< T1, 4 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB, p_ptA[2] - p_ptB, p_ptA[3] - p_ptB );
		}

		static inline Castor::Point< T1, 4 > mul( Castor::Coords< T1, 4 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB, p_ptA[2] * p_ptB, p_ptA[3] * p_ptB );
		}

		static inline Castor::Point< T1, 4 > div( Castor::Coords< T1, 4 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 4 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB, p_ptA[2] / p_ptB, p_ptA[3] / p_ptB );
		}
	};

	template< typename T1, typename T2 > struct PtOperators< T1, T2, 3, 3 >
	{
		static inline Castor::Point< T1, 3 > add( Castor::Point< T1, 3 > const & p_ptA, Castor::Point< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > sub( Castor::Point< T1, 3 > const & p_ptA, Castor::Point< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > mul( Castor::Point< T1, 3 > const & p_ptA, Castor::Point< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > div( Castor::Point< T1, 3 > const & p_ptA, Castor::Point< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > add( Castor::Point< T1, 3 > const & p_ptA, Castor::Coords< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > sub( Castor::Point< T1, 3 > const & p_ptA, Castor::Coords< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > mul( Castor::Point< T1, 3 > const & p_ptA, Castor::Coords< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > div( Castor::Point< T1, 3 > const & p_ptA, Castor::Coords< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > add( Castor::Point< T1, 3 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > sub( Castor::Point< T1, 3 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > mul( Castor::Point< T1, 3 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > div( Castor::Point< T1, 3 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > add( Castor::Point< T1, 3 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB, p_ptA[2] + p_ptB );
		}

		static inline Castor::Point< T1, 3 > sub( Castor::Point< T1, 3 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB, p_ptA[2] - p_ptB );
		}

		static inline Castor::Point< T1, 3 > mul( Castor::Point< T1, 3 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB, p_ptA[2] * p_ptB );
		}

		static inline Castor::Point< T1, 3 > div( Castor::Point< T1, 3 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB, p_ptA[2] / p_ptB );
		}

		static inline Castor::Point< T1, 3 > add( Castor::Coords< T1, 3 > const & p_ptA, Castor::Point< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > sub( Castor::Coords< T1, 3 > const & p_ptA, Castor::Point< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > mul( Castor::Coords< T1, 3 > const & p_ptA, Castor::Point< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > div( Castor::Coords< T1, 3 > const & p_ptA, Castor::Point< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > add( Castor::Coords< T1, 3 > const & p_ptA, Castor::Coords< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > sub( Castor::Coords< T1, 3 > const & p_ptA, Castor::Coords< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > mul( Castor::Coords< T1, 3 > const & p_ptA, Castor::Coords< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > div( Castor::Coords< T1, 3 > const & p_ptA, Castor::Coords< T2, 3 > const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > add( Castor::Coords< T1, 3 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1], p_ptA[2] + p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > sub( Castor::Coords< T1, 3 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1], p_ptA[2] - p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > mul( Castor::Coords< T1, 3 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1], p_ptA[2] * p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > div( Castor::Coords< T1, 3 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1], p_ptA[2] / p_ptB[2] );
		}

		static inline Castor::Point< T1, 3 > add( Castor::Coords< T1, 3 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB, p_ptA[2] + p_ptB );
		}

		static inline Castor::Point< T1, 3 > sub( Castor::Coords< T1, 3 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB, p_ptA[2] - p_ptB );
		}

		static inline Castor::Point< T1, 3 > mul( Castor::Coords< T1, 3 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB, p_ptA[2] * p_ptB );
		}

		static inline Castor::Point< T1, 3 > div( Castor::Coords< T1, 3 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 3 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB, p_ptA[2] / p_ptB );
		}
	};

	template< typename T1, typename T2 > struct PtOperators< T1, T2, 2, 2 >
	{
		static inline Castor::Point< T1, 2 > add( Castor::Point< T1, 2 > const & p_ptA, Castor::Point< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > sub( Castor::Point< T1, 2 > const & p_ptA, Castor::Point< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > mul( Castor::Point< T1, 2 > const & p_ptA, Castor::Point< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > div( Castor::Point< T1, 2 > const & p_ptA, Castor::Point< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > add( Castor::Point< T1, 2 > const & p_ptA, Castor::Coords< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > sub( Castor::Point< T1, 2 > const & p_ptA, Castor::Coords< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > mul( Castor::Point< T1, 2 > const & p_ptA, Castor::Coords< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > div( Castor::Point< T1, 2 > const & p_ptA, Castor::Coords< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > add( Castor::Point< T1, 2 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > sub( Castor::Point< T1, 2 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > mul( Castor::Point< T1, 2 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > div( Castor::Point< T1, 2 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > add( Castor::Point< T1, 2 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB );
		}

		static inline Castor::Point< T1, 2 > sub( Castor::Point< T1, 2 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB );
		}

		static inline Castor::Point< T1, 2 > mul( Castor::Point< T1, 2 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB );
		}

		static inline Castor::Point< T1, 2 > div( Castor::Point< T1, 2 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB );
		}

		static inline Castor::Point< T1, 2 > add( Castor::Coords< T1, 2 > const & p_ptA, Castor::Point< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > sub( Castor::Coords< T1, 2 > const & p_ptA, Castor::Point< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > mul( Castor::Coords< T1, 2 > const & p_ptA, Castor::Point< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > div( Castor::Coords< T1, 2 > const & p_ptA, Castor::Point< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > add( Castor::Coords< T1, 2 > const & p_ptA, Castor::Coords< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > sub( Castor::Coords< T1, 2 > const & p_ptA, Castor::Coords< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > mul( Castor::Coords< T1, 2 > const & p_ptA, Castor::Coords< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > div( Castor::Coords< T1, 2 > const & p_ptA, Castor::Coords< T2, 2 > const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > add( Castor::Coords< T1, 2 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] + p_ptB[0], p_ptA[1] + p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > sub( Castor::Coords< T1, 2 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] - p_ptB[0], p_ptA[1] - p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > mul( Castor::Coords< T1, 2 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] * p_ptB[0], p_ptA[1] * p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > div( Castor::Coords< T1, 2 > const & p_ptA, T2 * p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] / p_ptB[0], p_ptA[1] / p_ptB[1] );
		}

		static inline Castor::Point< T1, 2 > add( Castor::Coords< T1, 2 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] + p_ptB, p_ptA[1] + p_ptB );
		}

		static inline Castor::Point< T1, 2 > sub( Castor::Coords< T1, 2 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] - p_ptB, p_ptA[1] - p_ptB );
		}

		static inline Castor::Point< T1, 2 > mul( Castor::Coords< T1, 2 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] * p_ptB, p_ptA[1] * p_ptB );
		}

		static inline Castor::Point< T1, 2 > div( Castor::Coords< T1, 2 > const & p_ptA, T2 const & p_ptB )
		{
			return Castor::Point< T1, 2 >( p_ptA[0] / p_ptB, p_ptA[1] / p_ptB );
		}
	};
}

namespace Castor
{
//*************************************************************************************************

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, Count >::div( p_ptA, p_ptB );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator +( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::add( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator -( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::sub( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator *( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::mul( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator /( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::div( p_pt, p_coords );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator *( Coords< T, Count > const & p_pt, T const & p_coord )
	{
		return PtOperators< T, T, Count, Count >::mul( p_pt, p_coord );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator /( Coords< T, Count > const & p_pt, T const & p_coord )
	{
		return PtOperators< T, T, Count, Count >::div( p_pt, p_coord );
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
	Point< T, Count >::BinaryLoader::BinaryLoader()
		:	Loader< Point< T, Count >, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
	{
	}

	template< typename T, uint32_t Count >
	bool Point< T, Count >::BinaryLoader::operator()( Point< T, Count > & p_object, BinaryFile & p_file )
	{
		bool l_return = true;

		for ( uint32_t i = 0; i < Count && l_return; ++i )
		{
			l_return = p_file.Read( p_object[i] ) == sizeof( T );
		}

		return l_return;
	}

	template< typename T, uint32_t Count >
	bool Point< T, Count >::BinaryLoader::operator()( Point< T, Count > const & p_object, BinaryFile & p_file )
	{
		bool l_return = true;

		for ( uint32_t i = 0; i < Count && l_return; ++i )
		{
			l_return = p_file.Write( p_object[i] ) == sizeof( T );
		}

		return l_return;
	}

//*************************************************************************************************

	template< typename T, uint32_t Count >
	Point< T, Count >::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Point< T, Count >, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	template< typename T, uint32_t Count >
	bool Point< T, Count >::TextLoader::operator()( Point< T, Count > & p_object, TextFile & p_file )
	{
		String l_strWord;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			if ( p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t" ) ) > 0 )
			{
				StringStream l_streamWord( l_strWord );
				l_streamWord >> p_object[i];
			}

			xchar l_cDump;
			p_file.ReadChar( l_cDump );
		}

		return true;
	}

	template< typename T, uint32_t Count >
	bool Point< T, Count >::TextLoader::operator()( Point< T, Count > const & p_object, TextFile & p_file )
	{
		StringStream l_streamWord;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			if ( !l_streamWord.str().empty() )
			{
				l_streamWord << cuT( " " );
			}

			l_streamWord << p_object[i];
		}

		bool l_return = p_file.Print( 1024, cuT( "%s" ), l_streamWord.str().c_str() ) > 0;
		return l_return;
	}

//*************************************************************************************************

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
#if CASTOR_HAS_VARIADIC_TEMPLATES
	namespace
	{
		template< typename T, uint32_t Count, uint32_t Index, typename U, typename ... Values > void construct( Point< T, Count > & p_ptResult, U p_current, Values ... );
		template< typename T, uint32_t Count, uint32_t Index > void construct( Point< T, Count > & p_ptResult )
		{
		}
		template< typename T, uint32_t Count, uint32_t Index, typename U > void construct( Point< T, Count > & p_ptResult, U p_last )
		{
			if ( Index < Count )
			{
				p_ptResult[Index] = T( p_last );
			}
		}
		template< typename T, uint32_t Count, uint32_t Index, typename U, typename ... Values > void construct( Point< T, Count > & p_ptResult, U p_current, Values ... p_values )
		{
			if ( Index < Count )
			{
				p_ptResult[Index] = T( p_current );
				construct < T, Count, Index + 1, Values... > ( p_ptResult, p_values... );
			}
		}
	}
	template< typename T, uint32_t Count >
	template< typename ... Values >
	Point< T, Count >::Point( Values ... p_values )
	{
		std::memset( m_coords, 0, binary_size );
		construct< T, Count, 0, Values... >( *this, p_values... );
	}
#else
	template< typename T, uint32_t Count >
	Point< T, Count >::Point()
	{
		std::memset( m_coords, 0, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const & p_vA, T const & p_vB )
	{
		std::memset( m_coords, 0, binary_size );
		m_coords[0] = p_vA;
		m_coords[1] = p_vB;
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const & p_vA, T const & p_vB, T const & p_vC )
	{
		std::memset( m_coords, 0, binary_size );
		m_coords[0] = p_vA;
		m_coords[1] = p_vB;
		m_coords[2] = p_vC;
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const & p_vA, T const & p_vB, T const & p_vC, T const & p_vD )
	{
		std::memset( m_coords, 0, binary_size );
		m_coords[0] = p_vA;
		m_coords[1] = p_vB;
		m_coords[2] = p_vC;
		m_coords[3] = p_vD;
	}

	template< typename T, uint32_t Count >
	template< typename U, typename V >
	Point< T, Count >::Point( U const & p_vA, V const & p_vB )
	{
		std::memset( m_coords, 0, binary_size );
		m_coords[0] = T( p_vA );
		m_coords[1] = T( p_vB );
	}

	template< typename T, uint32_t Count >
	template< typename U, typename V, typename W >
	Point< T, Count >::Point( U const & p_vA, V const & p_vB, W const & p_vC )
	{
		std::memset( m_coords, 0, binary_size );
		m_coords[0] = T( p_vA );
		m_coords[1] = T( p_vB );
		m_coords[2] = T( p_vC );
	}

	template< typename T, uint32_t Count >
	template< typename U, typename V, typename W, typename X >
	Point< T, Count >::Point( U const & p_vA, V const & p_vB, W const & p_vC, X const & p_vD )
	{
		std::memset( m_coords, 0, binary_size );
		m_coords[0] = T( p_vA );
		m_coords[1] = T( p_vB );
		m_coords[2] = T( p_vC );
		m_coords[3] = T( p_vD );
	}
#endif
	template< typename T, uint32_t Count >
	template< typename U >
	Point< T, Count >::Point( Point< U, Count > const & p_pt )
	{
		if ( std::is_same< T, U >::value )
		{
			std::memcpy( m_coords, p_pt.m_coords, binary_size );
		}
		else
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				m_coords[i] = static_cast< T >( p_pt.m_coords[i] );
			}
		}
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Point< T, Count > const & p_pt )
	{
		std::memcpy( m_coords, p_pt.m_coords, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Point< T, Count > && p_pt )
	{
		std::memcpy( m_coords, p_pt.m_coords, binary_size );
		std::memset( p_pt.m_coords, 0, binary_size );
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
		std::memcpy( m_coords, p_pt.m_coords, binary_size );
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
			std::memcpy( m_coords, p_pt.m_coords, binary_size );
			std::memset( p_pt.m_coords, 0, binary_size );
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
			CASTOR_ASSERT( false );
			throw std::range_error( "Point subscript out of range" );
		}

		return m_coords[p_pos];
	}

	template< typename T, uint32_t Count >
	T & Point< T, Count >::at( uint32_t p_pos )
	{
		if ( p_pos >= Count )
		{
			CASTOR_ASSERT( false );
			throw std::range_error( "Point subscript out of range" );
		}

		return m_coords[p_pos];
	}

//*************************************************************************************************

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		bool l_return = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && l_return; i++ )
		{
			l_return = Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return l_return;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::div( p_ptA, p_ptB );
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
		return PtOperators< T, U, Count, Count >::add( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator -( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::sub( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator *( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::mul( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator /( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::div( p_pt, p_coords );
	}
	template< typename T, uint32_t Count, typename U >
	inline Point< T, Count > operator *( Point< T, Count > const & p_pt, U const & p_coord )
	{
		return PtOperators< T, U, Count, Count >::mul( p_pt, p_coord );
	}
	template< typename T, uint32_t Count, typename U >
	inline Point< T, Count > operator /( Point< T, Count > const & p_pt, U const & p_coord )
	{
		return PtOperators< T, U, Count, Count >::div( p_pt, p_coord );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		bool l_return = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && l_return; i++ )
		{
			l_return = Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return l_return;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::div( p_ptA, p_ptB );
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
		bool l_return = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && l_return; i++ )
		{
			l_return = Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return l_return;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator +( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator -( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator *( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator /( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::div( p_ptA, p_ptB );
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
		Point< T, Count > l_ptReturn;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			l_ptReturn[i] = -p_pt[i];
		}

		return l_ptReturn;
	}

	//*************************************************************************************************
	namespace point
	{
		template< typename T, uint32_t Count >
		inline void negate( Point< T, Count > & p_ptPoint )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				p_ptPoint[i] = -p_ptPoint[i];
			}
		}

		template< typename T, uint32_t Count >
		void normalise( Point< T, Count > & p_ptPoint )
		{
			T l_tLength = T( distance( p_ptPoint ) );

			if ( !Policy< T >::is_null( l_tLength ) )
			{
				p_ptPoint /= l_tLength;
			}
		}

		template< typename T, uint32_t Count >
		Point< T, Count > get_normalised( Point< T, Count > const & p_ptPoint )
		{
			Point< T, Count > l_ptReturn( p_ptPoint );
			normalise( l_ptReturn );
			return l_ptReturn;
		}

		template< typename T, uint32_t Count >
		T dot( Point< T, Count > const & p_ptA, Point< T, Count > const & p_ptB )
		{
			T l_tReturn;
			Policy< T >::init( l_tReturn );

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_tReturn += p_ptA[i] * p_ptB[i];
			}

			return l_tReturn;
		}

		template< typename T, uint32_t Count >
		double cos_theta( Point< T, Count > const & p_ptA, Point< T, Count > const & p_ptB )
		{
			double l_dReturn = double( distance( p_ptA ) * distance( p_ptB ) );

			if ( l_dReturn != 0 )
			{
				l_dReturn = dot( p_ptA, p_ptB ) / l_dReturn;
			}
			else
			{
				l_dReturn = dot( p_ptA, p_ptB );
			}

			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance_squared( Point< T, Count > const & p_ptPoint )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn += p_ptPoint[i] * p_ptPoint[i];
			}

			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance( Point< T, Count > const & p_ptPoint )
		{
			return sqrt( distance_squared( p_ptPoint ) );
		}

		template< typename T, uint32_t Count >
		inline double distance_manhattan( Point< T, Count > const & p_ptPoint )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn += abs( p_ptPoint[i] );
			}

			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance_minkowski( Point< T, Count > const & p_ptPoint, double p_dOrder )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn += pow( double( abs( p_ptPoint[i] ) ), p_dOrder );
			}

			l_dReturn = pow( l_dReturn, 1.0 / p_dOrder );
			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance_chebychev( Point< T, Count > const & p_ptPoint )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn = std::max( l_dReturn, double( abs( p_ptPoint[i] ) ) );
			}

			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		T dot( Point< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
		{
			T l_tReturn = T();

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_tReturn += p_ptA[i] * p_ptB[i];
			}

			return l_tReturn;
		}

		template< typename T, uint32_t Count >
		double cos_theta( Point< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
		{
			double l_dReturn = double( distance( p_ptA ) * distance( p_ptB ) );

			if ( l_dReturn != 0 )
			{
				l_dReturn = dot( p_ptA, p_ptB ) / l_dReturn;
			}
			else
			{
				l_dReturn = dot( p_ptA, p_ptB );
			}

			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		T dot( Coords< T, Count > const & p_ptA, Point< T, Count > const & p_ptB )
		{
			T l_tReturn;
			Policy< T >::init( l_tReturn );

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_tReturn += p_ptA[i] * p_ptB[i];
			}

			return l_tReturn;
		}

		template< typename T, uint32_t Count >
		double cos_theta( Coords< T, Count > const & p_ptA, Point< T, Count > const & p_ptB )
		{
			double l_dReturn = double( distance( p_ptA ) * distance( p_ptB ) );

			if ( l_dReturn != 0 )
			{
				l_dReturn = dot( p_ptA, p_ptB ) / l_dReturn;
			}
			else
			{
				l_dReturn = dot( p_ptA, p_ptB );
			}

			return l_dReturn;
		}
	}
}

//*************************************************************************************************

template< typename T, uint32_t Count >
inline Castor::String & operator << ( Castor::String & p_strOut, Castor::Point< T, Count > const & p_pt )
{
	Castor::StringStream l_streamOut;

	if ( Count )
	{
		l_streamOut << p_pt[0];

		for ( uint32_t i = 0; i < Count; i++ )
		{
			l_streamOut << cuT( "\t" ) << p_pt[i];
		}
	}

	p_strOut += l_streamOut.str();
	return p_strOut;
}

template< typename T, uint32_t Count >
inline Castor::String & operator >> ( Castor::String & p_strIn, Castor::Point< T, Count > & p_pt )
{
	Castor::StringStream l_streamIn( p_strIn );

	for ( uint32_t i = 0; i < Count; i++ )
	{
		l_streamIn >> p_pt[i];
	}

	p_strIn = l_streamIn.str();
	return p_strIn;
}

template< typename T, uint32_t Count, typename CharType >
inline std::basic_ostream< CharType > & operator << ( std::basic_ostream< CharType > & p_streamOut, Castor::Point< T, Count > const & p_pt )
{
	if ( Count )
	{
		p_streamOut << p_pt[0];

		for ( uint32_t i = 0; i < Count; i++ )
		{
			p_streamOut << "\t" << p_pt[i];
		}
	}

	return p_streamOut;
}
template< typename T, uint32_t Count, typename CharType >
inline std::basic_istream< CharType > & operator >> ( std::basic_istream< CharType > & p_streamIn, Castor::Point< T, Count > & p_pt )
{
	for ( uint32_t i = 0; i < Count; i++ )
	{
		p_streamIn >> p_pt[i];
	}

	return p_streamIn;
}

//*************************************************************************************************
