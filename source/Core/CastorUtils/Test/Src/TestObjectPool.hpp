/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___TEST_SINGLE_THREADED_POOL_H___
#define ___TEST_SINGLE_THREADED_POOL_H___

#include "PoolTestPrerequisites.hpp"

#include <Pool/PoolManagedObject.hpp>

#include <random>

namespace Testing
{
	namespace Pools
	{
		template< typename Object, castor::MemoryDataType MemDataType >
		using MyObjectPool = castor::ObjectPool< Object, MemDataType >;

		template< castor::MemoryDataType MemDataType >
		struct SFixedChecks
		{
			template< typename type >
			static void run()
			{
				static const uint32_t obj_count = AllocPolicies::PoolPolicy< type >::Count;
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					MyObjectPool< type, MemDataType > pool( obj_count );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type >::allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type >::deallocate( pool, object );
					}
				}
				finalise( time );
			}
		};

		template< castor::MemoryDataType MemDataType >
		struct SAllDeallChecks
		{
			template< typename type >
			static void run()
			{
				static const uint32_t obj_count = AllocPolicies::PoolPolicy< type >::Count;
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					MyObjectPool< type, MemDataType > pool( obj_count );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer = AllocPolicies::PoolPolicy< type >::allocate( pool );
						AllocPolicies::PoolPolicy< type >::deallocate( pool, *buffer );
						*buffer++ = AllocPolicies::PoolPolicy< type >::allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type >::deallocate( pool, object );
					}
				}
				finalise( time );
			}
		};

		template< castor::MemoryDataType MemDataType >
		struct SScatteredChecks
		{
			template< typename type >
			static void run( std::vector< size_t > const & index )
			{
				static const uint32_t obj_count = AllocPolicies::PoolPolicy< type >::Count;
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					MyObjectPool< type, MemDataType > pool( obj_count );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type >::allocate( pool );
					}

					NextStep( "Random deallocations", time );

					for ( size_t i : index )
					{
						AllocPolicies::PoolPolicy< type >::deallocate( pool, array[i] );
					}

					NextStep( "Reallocations", time );

					for ( size_t i : index )
					{
						array[i] = AllocPolicies::PoolPolicy< type >::allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type >::deallocate( pool, object );
					}
				}
				finalise( time );
			}
		};

		template< castor::MemoryDataType MemDataType >
		struct SVariableChecks
		{
			template< typename type >
			static void run()
			{
				static const uint32_t obj_count = AllocPolicies::PoolPolicy< type >::Count;
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					MyObjectPool< type, MemDataType > pool( obj_count / 3 );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type >::allocate( pool );
					}

					NextStep( "double size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type >::allocate( pool );
					}

					NextStep( "Triple size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type >::allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type >::deallocate( pool, object );
					}
				}
				finalise( time );
			}
		};

		template< castor::MemoryDataType MemDataType >
		struct SUniqueFixedChecks
		{
			template< typename type >
			using ManagedObject = castor::PoolManagedObject< type, MemDataType >;

			template < typename type >
			static void run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				using Type = ManagedObject< type >;
				using Stored = Type *;
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< Stored > array( obj_count );
					auto buffer = array.data();
					Type::MyUniqueObjectPool::create( obj_count );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = new Type;
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						delete object;
					}

					Type::MyUniqueObjectPool::destroy();
				}
				finalise( time );
			}
		};
	}

	namespace Traditional
	{
		template< typename Policy >
		struct SFixedChecks
		{
			template < typename type >
			static void run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = Policy::template allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						Policy::template deallocate< type >( object );
					}
				}
				finalise( time );
			}
		};

		template< typename Policy >
		struct SAllDeallChecks
		{
			template < typename type >
			static void run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer = Policy::template allocate< type >();
						Policy::template deallocate< type >( *buffer );
						*buffer++ = Policy::template allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						Policy::template deallocate< type >( object );
					}
				}
				finalise( time );
			}
		};

		template< typename Policy >
		struct SScatteredChecks
		{
			template < typename type >
			static void run( std::vector< size_t > const & index )
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					type ** buffer = arraybuffer.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = Policy::template allocate< type >();
					}

					NextStep( "Random deallocations", time );

					for ( size_t i : index )
					{
						Policy::template deallocate< type >( arraybuffer[i] );
					}

					NextStep( "Reallocations", time );

					for ( size_t i : index )
					{
						arraybuffer[i] = Policy::template allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::template deallocate< type >( object );
					}
				}
				finalise( time );
			}
		};

		template< typename Policy >
		struct SVariableChecks
		{
			template < typename type >
			static void run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template allocate< type >();
					}

					NextStep( "double size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template allocate< type >();
					}

					NextStep( "Triple size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						Policy::template deallocate< type >( object );
					}
				}
				finalise( time );
			}
		};
	}

	namespace PlacementNew
	{
		template< typename Policy >
		struct SFixedChecks
		{
			template < typename type  >
			static void run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = Policy::template allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::template deallocate< type >( object );
					}
				}
				finalise( time );
			}
		};

		template< typename Policy >
		struct SAllDeallChecks
		{
			template < typename type  >
			static void run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer = Policy::template allocate< type >( data );
						Policy::template deallocate( *buffer );
						*buffer++ = Policy::template allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::template deallocate< type >( object );
					}
				}
				finalise( time );
			}
		};

		template< typename Policy >
		struct SScatteredChecks
		{
			template < typename type  >
			static void run( std::vector< size_t > const & index )
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = Policy::template allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Random deallocations", time );

					for ( size_t i : index )
					{
						Policy::template deallocate< type >( arraybuffer[i] );
					}

					NextStep( "Reallocations", time );

					for ( size_t i : index )
					{
						arraybuffer[i] = Policy::template allocate< type >( arraybuffer[i] );
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::template deallocate< type >( object );
					}
				}
				finalise( time );
			}
		};

		template< typename Policy >
		struct SVariableChecks
		{
			template < typename type >
			static void run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "double size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Triple size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::template deallocate( object );
					}
				}
				finalise( time );
			}
		};
	}

	namespace FixedSizePerformance
	{
		template< typename Checker >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Fixed Size Performance checks" << std::endl;
			std::cout << " " << typeid( Checker ).name() << std::endl << std::endl;
			TimePoint time = Clock::now();

			Checker::template run< int >();
			Checker::template run< TinyObj >();
			Checker::template run< SmallObj >();
			Checker::template run< Obj >();
			Checker::template run< BigObject >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}

		template< castor::MemoryDataType MemDataType >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Fixed Size Performance checks" << std::endl;
			std::cout << " Pools::SFixedChecks< " << castor::MemoryDataNamer< MemDataType >::Name << " >" << std::endl << std::endl;
			TimePoint time = Clock::now();

			Pools::SFixedChecks< MemDataType >::template run< int >();
			Pools::SFixedChecks< MemDataType >::template run< TinyObj >();
			Pools::SFixedChecks< MemDataType >::template run< SmallObj >();
			Pools::SFixedChecks< MemDataType >::template run< Obj >();
			Pools::SFixedChecks< MemDataType >::template run< BigObject >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}
	}

	namespace AllDeallPerformance
	{
		template< typename Checker >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "allocate/Dealloc Performance checks" << std::endl;
			std::cout << " " << typeid( Checker ).name() << std::endl << std::endl;
			TimePoint time = Clock::now();

			Checker::template run< int >();
			Checker::template run< TinyObj >();
			Checker::template run< SmallObj >();
			Checker::template run< Obj >();
			Checker::template run< BigObject >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}

		template< castor::MemoryDataType MemDataType >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "allocate/Dealloc Performance checks" << std::endl;
			std::cout << " Pools::SAllDeallChecks< " << castor::MemoryDataNamer< MemDataType >::Name << " >" << std::endl << std::endl;
			TimePoint time = Clock::now();

			Pools::SAllDeallChecks< MemDataType >::template run< int >();
			Pools::SAllDeallChecks< MemDataType >::template run< TinyObj >();
			Pools::SAllDeallChecks< MemDataType >::template run< SmallObj >();
			Pools::SAllDeallChecks< MemDataType >::template run< Obj >();
			Pools::SAllDeallChecks< MemDataType >::template run< BigObject >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}
	}

	namespace ScatteredMemoryPerformance
	{
		struct Index
		{
			Index()
			{
				doInitialiseIndex( m_intIndex, INT_COUNT / 3 );
				doInitialiseIndex( m_tinyObjIndex, TINYOBJ_COUNT / 3 );
				doInitialiseIndex( m_smallObjIndex, SMALLOBJ_COUNT / 3 );
				doInitialiseIndex( m_objIndex, OBJ_COUNT / 3 );
				doInitialiseIndex( m_bigObjIndex, BIGOBJ_COUNT / 3 );
			}

			std::vector< size_t > m_intIndex;
			std::vector< size_t > m_tinyObjIndex;
			std::vector< size_t > m_smallObjIndex;
			std::vector< size_t > m_objIndex;
			std::vector< size_t > m_bigObjIndex;

		private:
			void doInitialiseIndex( std::vector< size_t > & index, size_t count )
			{
				std::default_random_engine engine;
				std::uniform_int_distribution< size_t > distribution( 1, 3 );
				index.reserve( count );

				for ( size_t i = 0; i < count; i += distribution( engine ) )
				{
					index.push_back( i );
				}
			}
		};

		template< typename Checker >
		void Checks( Index const & index )
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Scattered Memory Performance checks" << std::endl;
			std::cout << " " << typeid( Checker ).name() << std::endl << std::endl;
			TimePoint time = Clock::now();

			Checker::template run< int >( index.m_intIndex );
			Checker::template run< TinyObj >( index.m_tinyObjIndex );
			Checker::template run< SmallObj >( index.m_smallObjIndex );
			Checker::template run< Obj >( index.m_objIndex );
			Checker::template run< BigObject >( index.m_bigObjIndex );

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}

		template< castor::MemoryDataType MemDataType >
		void Checks( Index const & index )
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Scattered Memory Performance checks" << std::endl;
			std::cout << " Pools::SScatteredChecks< " << castor::MemoryDataNamer< MemDataType >::Name << " >" << std::endl << std::endl;
			TimePoint time = Clock::now();

			Pools::SScatteredChecks< MemDataType >::template run< int >( index.m_intIndex );
			Pools::SScatteredChecks< MemDataType >::template run< TinyObj >( index.m_tinyObjIndex );
			Pools::SScatteredChecks< MemDataType >::template run< SmallObj >( index.m_smallObjIndex );
			Pools::SScatteredChecks< MemDataType >::template run< Obj >( index.m_objIndex );
			Pools::SScatteredChecks< MemDataType >::template run< BigObject >( index.m_bigObjIndex );

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}
	}

	namespace VariableSizePerformance
	{
		template< typename Checker >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Variable Size Performance checks" << std::endl;
			std::cout << " " << typeid( Checker ).name() << std::endl << std::endl;
			TimePoint time = Clock::now();

			Checker::template run< int >();
			Checker::template run< TinyObj >();
			Checker::template run< SmallObj >();
			Checker::template run< Obj >();
			Checker::template run< BigObject >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}

		template< castor::MemoryDataType MemDataType >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Variable Size Performance checks" << std::endl;
			std::cout << " Pools::SVariableChecks< " << castor::MemoryDataNamer< MemDataType >::Name << " >" << std::endl << std::endl;
			TimePoint time = Clock::now();

			Pools::SVariableChecks< MemDataType >::template run< int >();
			Pools::SVariableChecks< MemDataType >::template run< TinyObj >();
			Pools::SVariableChecks< MemDataType >::template run< SmallObj >();
			Pools::SVariableChecks< MemDataType >::template run< Obj >();
			Pools::SVariableChecks< MemDataType >::template run< BigObject >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}
	}

	namespace UniqueObjectPool
	{
		template< typename Checker >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Unique Pool Fixed Size Performance checks" << std::endl;
			std::cout << " " << typeid( Checker ).name() << std::endl << std::endl;
			TimePoint time = Clock::now();

			Checker::template run< TinyObj >();
			Checker::template run< SmallObj >();
			Checker::template run< Obj >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}

		template< castor::MemoryDataType MemDataType >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Unique Pool Fixed Size Performance checks" << std::endl;
			std::cout << " Pools::SFixedChecks< " << castor::MemoryDataNamer< MemDataType >::Name << " >" << std::endl << std::endl;
			TimePoint time = Clock::now();

			Pools::SUniqueFixedChecks< MemDataType >::template run< TinyObj >();
			Pools::SUniqueFixedChecks< MemDataType >::template run< SmallObj >();
			Pools::SUniqueFixedChecks< MemDataType >::template run< Obj >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}
	}
}

#endif
