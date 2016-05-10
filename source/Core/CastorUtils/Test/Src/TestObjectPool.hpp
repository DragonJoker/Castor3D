/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___TEST_SINGLE_THREADED_POOL_H___
#define ___TEST_SINGLE_THREADED_POOL_H___

#include "PoolTestPrerequisites.hpp"

#include <PoolManagedObject.hpp>

#include <random>

namespace Testing
{
	namespace Pools
	{
		template< typename Object, Castor::eMEMDATA_TYPE MemDataType >
		using MyObjectPool = Castor::ObjectPool< Object, MemDataType >;

		template< Castor::eMEMDATA_TYPE MemDataType >
		struct SFixedChecks
		{
			template< typename type >
			static void Run()
			{
				static const uint32_t obj_count = AllocPolicies::PoolPolicy< type >::Count;
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					MyObjectPool< type, MemDataType > pool( obj_count );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type >::Allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type >::Deallocate( pool, object );
					}
				}
				Finalise( time );
			}
		};

		template< Castor::eMEMDATA_TYPE MemDataType >
		struct SAllDeallChecks
		{
			template< typename type >
			static void Run()
			{
				static const uint32_t obj_count = AllocPolicies::PoolPolicy< type >::Count;
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					MyObjectPool< type, MemDataType > pool( obj_count );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer = AllocPolicies::PoolPolicy< type >::Allocate( pool );
						AllocPolicies::PoolPolicy< type >::Deallocate( pool, *buffer );
						*buffer++ = AllocPolicies::PoolPolicy< type >::Allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type >::Deallocate( pool, object );
					}
				}
				Finalise( time );
			}
		};

		template< Castor::eMEMDATA_TYPE MemDataType >
		struct SScatteredChecks
		{
			template< typename type >
			static void Run( std::vector< size_t > const & index )
			{
				static const uint32_t obj_count = AllocPolicies::PoolPolicy< type >::Count;
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					MyObjectPool< type, MemDataType > pool( obj_count );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type >::Allocate( pool );
					}

					NextStep( "Random deallocations", time );

					for ( size_t i : index )
					{
						AllocPolicies::PoolPolicy< type >::Deallocate( pool, array[i] );
					}

					NextStep( "Reallocations", time );

					for ( size_t i : index )
					{
						array[i] = AllocPolicies::PoolPolicy< type >::Allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type >::Deallocate( pool, object );
					}
				}
				Finalise( time );
			}
		};

		template< Castor::eMEMDATA_TYPE MemDataType >
		struct SVariableChecks
		{
			template< typename type >
			static void Run()
			{
				static const uint32_t obj_count = AllocPolicies::PoolPolicy< type >::Count;
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					MyObjectPool< type, MemDataType > pool( obj_count / 3 );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type >::Allocate( pool );
					}

					NextStep( "Double size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type >::Allocate( pool );
					}

					NextStep( "Triple size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type >::Allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type >::Deallocate( pool, object );
					}
				}
				Finalise( time );
			}
		};

		template< Castor::eMEMDATA_TYPE MemDataType >
		struct SUniqueFixedChecks
		{
			template< typename type >
			using ManagedObject = Castor::PoolManagedObject< type, MemDataType >;

			template < typename type >
			static void Run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				using Type = ManagedObject< type >;
				using Stored = Type *;
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< Stored > array( obj_count );
					auto buffer = array.data();
					Type::MyUniqueObjectPool::Create( obj_count );
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

					Type::MyUniqueObjectPool::Destroy();
				}
				Finalise( time );
			}
		};
	}

	namespace Traditional
	{
		template< typename Policy >
		struct SFixedChecks
		{
			template < typename type >
			static void Run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = Policy::template Allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						Policy::template Deallocate< type >( object );
					}
				}
				Finalise( time );
			}
		};

		template< typename Policy >
		struct SAllDeallChecks
		{
			template < typename type >
			static void Run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer = Policy::template Allocate< type >();
						Policy::template Deallocate< type >( *buffer );
						*buffer++ = Policy::template Allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						Policy::template Deallocate< type >( object );
					}
				}
				Finalise( time );
			}
		};

		template< typename Policy >
		struct SScatteredChecks
		{
			template < typename type >
			static void Run( std::vector< size_t > const & index )
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					type ** buffer = arraybuffer.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = Policy::template Allocate< type >();
					}

					NextStep( "Random deallocations", time );

					for ( size_t i : index )
					{
						Policy::template Deallocate< type >( arraybuffer[i] );
					}

					NextStep( "Reallocations", time );

					for ( size_t i : index )
					{
						arraybuffer[i] = Policy::template Allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::template Deallocate< type >( object );
					}
				}
				Finalise( time );
			}
		};

		template< typename Policy >
		struct SVariableChecks
		{
			template < typename type >
			static void Run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template Allocate< type >();
					}

					NextStep( "Double size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template Allocate< type >();
					}

					NextStep( "Triple size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template Allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						Policy::template Deallocate< type >( object );
					}
				}
				Finalise( time );
			}
		};
	}

	namespace PlacementNew
	{
		template< typename Policy >
		struct SFixedChecks
		{
			template < typename type  >
			static void Run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = Policy::template Allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::template Deallocate< type >( object );
					}
				}
				Finalise( time );
			}
		};

		template< typename Policy >
		struct SAllDeallChecks
		{
			template < typename type  >
			static void Run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer = Policy::template Allocate< type >( data );
						Policy::template Deallocate( *buffer );
						*buffer++ = Policy::template Allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::template Deallocate< type >( object );
					}
				}
				Finalise( time );
			}
		};

		template< typename Policy >
		struct SScatteredChecks
		{
			template < typename type  >
			static void Run( std::vector< size_t > const & index )
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = Policy::template Allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Random deallocations", time );

					for ( size_t i : index )
					{
						Policy::template Deallocate< type >( arraybuffer[i] );
					}

					NextStep( "Reallocations", time );

					for ( size_t i : index )
					{
						arraybuffer[i] = Policy::template Allocate< type >( arraybuffer[i] );
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::template Deallocate< type >( object );
					}
				}
				Finalise( time );
			}
		};

		template< typename Policy >
		struct SVariableChecks
		{
			template < typename type >
			static void Run()
			{
				static const uint32_t obj_count = MAX_SIZE / sizeof( type );
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template Allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Double size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template Allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Triple size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::template Allocate< type >( data );
						data += sizeof( type );
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::template Deallocate( object );
					}
				}
				Finalise( time );
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

			Checker::template Run< int >();
			Checker::template Run< TinyObj >();
			Checker::template Run< SmallObj >();
			Checker::template Run< Obj >();
			Checker::template Run< BigObject >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}

		template< Castor::eMEMDATA_TYPE MemDataType >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Fixed Size Performance checks" << std::endl;
			std::cout << " Pools::SFixedChecks< " << Castor::MemoryDataNamer< MemDataType >::Name << " >" << std::endl << std::endl;
			TimePoint time = Clock::now();

			Pools::SFixedChecks< MemDataType >::template Run< int >();
			Pools::SFixedChecks< MemDataType >::template Run< TinyObj >();
			Pools::SFixedChecks< MemDataType >::template Run< SmallObj >();
			Pools::SFixedChecks< MemDataType >::template Run< Obj >();
			Pools::SFixedChecks< MemDataType >::template Run< BigObject >();

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
			std::cout << "Alloc/Dealloc Performance checks" << std::endl;
			std::cout << " " << typeid( Checker ).name() << std::endl << std::endl;
			TimePoint time = Clock::now();

			Checker::template Run< int >();
			Checker::template Run< TinyObj >();
			Checker::template Run< SmallObj >();
			Checker::template Run< Obj >();
			Checker::template Run< BigObject >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}

		template< Castor::eMEMDATA_TYPE MemDataType >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Alloc/Dealloc Performance checks" << std::endl;
			std::cout << " Pools::SAllDeallChecks< " << Castor::MemoryDataNamer< MemDataType >::Name << " >" << std::endl << std::endl;
			TimePoint time = Clock::now();

			Pools::SAllDeallChecks< MemDataType >::template Run< int >();
			Pools::SAllDeallChecks< MemDataType >::template Run< TinyObj >();
			Pools::SAllDeallChecks< MemDataType >::template Run< SmallObj >();
			Pools::SAllDeallChecks< MemDataType >::template Run< Obj >();
			Pools::SAllDeallChecks< MemDataType >::template Run< BigObject >();

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
				DoInitialiseIndex( m_intIndex, INT_COUNT / 3 );
				DoInitialiseIndex( m_tinyObjIndex, TINYOBJ_COUNT / 3 );
				DoInitialiseIndex( m_smallObjIndex, SMALLOBJ_COUNT / 3 );
				DoInitialiseIndex( m_objIndex, OBJ_COUNT / 3 );
				DoInitialiseIndex( m_bigObjIndex, BIGOBJ_COUNT / 3 );
			}

			std::vector< size_t > m_intIndex;
			std::vector< size_t > m_tinyObjIndex;
			std::vector< size_t > m_smallObjIndex;
			std::vector< size_t > m_objIndex;
			std::vector< size_t > m_bigObjIndex;

		private:
			void DoInitialiseIndex( std::vector< size_t > & index, size_t count )
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

			Checker::template Run< int >( index.m_intIndex );
			Checker::template Run< TinyObj >( index.m_tinyObjIndex );
			Checker::template Run< SmallObj >( index.m_smallObjIndex );
			Checker::template Run< Obj >( index.m_objIndex );
			Checker::template Run< BigObject >( index.m_bigObjIndex );

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}

		template< Castor::eMEMDATA_TYPE MemDataType >
		void Checks( Index const & index )
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Scattered Memory Performance checks" << std::endl;
			std::cout << " Pools::SScatteredChecks< " << Castor::MemoryDataNamer< MemDataType >::Name << " >" << std::endl << std::endl;
			TimePoint time = Clock::now();

			Pools::SScatteredChecks< MemDataType >::template Run< int >( index.m_intIndex );
			Pools::SScatteredChecks< MemDataType >::template Run< TinyObj >( index.m_tinyObjIndex );
			Pools::SScatteredChecks< MemDataType >::template Run< SmallObj >( index.m_smallObjIndex );
			Pools::SScatteredChecks< MemDataType >::template Run< Obj >( index.m_objIndex );
			Pools::SScatteredChecks< MemDataType >::template Run< BigObject >( index.m_bigObjIndex );

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

			Checker::template Run< int >();
			Checker::template Run< TinyObj >();
			Checker::template Run< SmallObj >();
			Checker::template Run< Obj >();
			Checker::template Run< BigObject >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}

		template< Castor::eMEMDATA_TYPE MemDataType >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Variable Size Performance checks" << std::endl;
			std::cout << " Pools::SVariableChecks< " << Castor::MemoryDataNamer< MemDataType >::Name << " >" << std::endl << std::endl;
			TimePoint time = Clock::now();

			Pools::SVariableChecks< MemDataType >::template Run< int >();
			Pools::SVariableChecks< MemDataType >::template Run< TinyObj >();
			Pools::SVariableChecks< MemDataType >::template Run< SmallObj >();
			Pools::SVariableChecks< MemDataType >::template Run< Obj >();
			Pools::SVariableChecks< MemDataType >::template Run< BigObject >();

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

			Checker::template Run< TinyObj >();
			Checker::template Run< SmallObj >();
			Checker::template Run< Obj >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}

		template< Castor::eMEMDATA_TYPE MemDataType >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Unique Pool Fixed Size Performance checks" << std::endl;
			std::cout << " Pools::SFixedChecks< " << Castor::MemoryDataNamer< MemDataType >::Name << " >" << std::endl << std::endl;
			TimePoint time = Clock::now();

			Pools::SUniqueFixedChecks< MemDataType >::template Run< TinyObj >();
			Pools::SUniqueFixedChecks< MemDataType >::template Run< SmallObj >();
			Pools::SUniqueFixedChecks< MemDataType >::template Run< Obj >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}
	}
}

#endif
