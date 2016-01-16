/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
		using ObjectPool = Castor::ObjectPool< Object, MemDataType >;

		template< Castor::eMEMDATA_TYPE MemDataType >
		struct SFixedChecks
		{
			template< typename type, typename stored = type *, size_t obj_count = AllocPolicies::PoolPolicy< type, stored >::Count >
			static void Run()
			{
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< stored > array( obj_count );
					stored * buffer = array.data();
					ObjectPool< type, MemDataType > pool( obj_count );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type, stored >::Allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type, stored >::Deallocate( pool, object );
					}
				}
				Finalise( time );
			}
		};

		template< Castor::eMEMDATA_TYPE MemDataType >
		struct SScatteredChecks
		{
			template< typename type, typename stored = type *, size_t obj_count = AllocPolicies::PoolPolicy< type, stored >::Count >
			static void Run( std::vector< size_t > const & index )
			{
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< stored > array( obj_count );
					stored * buffer = array.data();
					ObjectPool< type, MemDataType > pool( obj_count );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type, stored >::Allocate( pool );
					}

					NextStep( "Random deallocations", time );

					for ( size_t i : index )
					{
						AllocPolicies::PoolPolicy< type, stored >::Deallocate( pool, array[i] );
					}

					NextStep( "Reallocations", time );

					for ( size_t i : index )
					{
						array[i] = AllocPolicies::PoolPolicy< type, stored >::Allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type, stored >::Deallocate( pool, object );
					}
				}
				Finalise( time );
			}
		};

		template< Castor::eMEMDATA_TYPE MemDataType >
		struct SVariableChecks
		{
			template< typename type, typename stored = type *, size_t obj_count = AllocPolicies::PoolPolicy< type, stored >::Count >
			static void Run()
			{
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< stored > array( obj_count );
					stored * buffer = array.data();
					ObjectPool< type, MemDataType > pool( obj_count / 3 );
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type, stored >::Allocate( pool );
					}

					NextStep( "Double size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type, stored >::Allocate( pool );
					}

					NextStep( "Triple size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = AllocPolicies::PoolPolicy< type, stored >::Allocate( pool );
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						AllocPolicies::PoolPolicy< type, stored >::Deallocate( pool, object );
					}
				}
				Finalise( time );
			}
		};

		template< Castor::eMEMDATA_TYPE MemDataType >
		struct SUniqueFixedChecks
		{
			template< typename type >
			using ManagedObject = Castor::ManagedObject< type, MemDataType >;

			template< typename type, size_t obj_count = AllocPolicies::PoolPolicy< type, ManagedObject< type > * >::Count >
			static void Run()
			{
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
			template< typename type, size_t obj_count = MAX_SIZE / sizeof( type ) >
			static void Run()
			{
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = Policy::Allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						Policy::Deallocate( object );
					}
				}
				Finalise( time );
			}
		};

		template< typename Policy >
		struct SScatteredChecks
		{
			template< typename type, size_t obj_count = MAX_SIZE / sizeof( type ) >
			static void Run( std::vector< size_t > const & index )
			{
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					type ** buffer = arraybuffer.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = Policy::Allocate< type >();
					}

					NextStep( "Random deallocations", time );

					for ( size_t i : index )
					{
						Policy::Deallocate( arraybuffer[i] );
					}

					NextStep( "Reallocations", time );

					for ( size_t i : index )
					{
						arraybuffer[i] = Policy::Allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						Policy::Deallocate( object );
					}
				}
				Finalise( time );
			}
		};

		template< typename Policy >
		struct SVariableChecks
		{
			template< typename type, size_t obj_count = MAX_SIZE / sizeof( type ) >
			static void Run()
			{
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > array( obj_count );
					type ** buffer = array.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::Allocate< type >();
					}

					NextStep( "Double size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::Allocate< type >();
					}

					NextStep( "Triple size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = Policy::Allocate< type >();
					}

					NextStep( "Deallocation", time );

					for ( auto object : array )
					{
						Policy::Deallocate( object );
					}
				}
				Finalise( time );
			}
		};
	}

	namespace PlacementNew
	{
		struct SFixedChecks
		{
			template< typename type, size_t obj_count = MAX_SIZE / sizeof( type ) >
			static void Run()
			{
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = new( data )type();
						data += sizeof( type );
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						object->~type();
					}
				}
				Finalise( time );
			}
		};

		struct SScatteredChecks
		{
			template< typename type, size_t obj_count = MAX_SIZE / sizeof( type ) >
			static void Run( std::vector< size_t > const & index )
			{
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count; ++i )
					{
						*buffer++ = new( data )type();
						data += sizeof( type );
					}

					NextStep( "Random deallocations", time );

					for ( size_t i : index )
					{
						arraybuffer[i]->~type();
					}

					NextStep( "Reallocations", time );

					for ( size_t i : index )
					{
						arraybuffer[i] = new( arraybuffer[i] )type();
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						object->~type();
					}
				}
				Finalise( time );
			}
		};

		struct SVariableChecks
		{
			template< typename type, size_t obj_count = MAX_SIZE / sizeof( type ) >
			static void Run()
			{
				TimePoint time = Initialise< type, obj_count >();
				{
					std::vector< type * > arraybuffer( obj_count );
					std::vector< uint8_t > arraydata( obj_count * sizeof( type ) );
					type ** buffer = arraybuffer.data();
					uint8_t * data = arraydata.data();
					NextStep( "Allocation", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = new( data )type();
						data += sizeof( type );
					}

					NextStep( "Double size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = new( data )type();
						data += sizeof( type );
					}

					NextStep( "Triple size", time );

					for ( size_t i = 0; i < obj_count / 3; ++i )
					{
						*buffer++ = new( data )type();
						data += sizeof( type );
					}

					NextStep( "Deallocation", time );

					for ( auto object : arraybuffer )
					{
						object->~type();
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

			Checker::Run< int >();
			Checker::Run< TinyObj >();
			Checker::Run< SmallObj >();
			Checker::Run< Obj >();
			Checker::Run< BigObject >();

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

			Pools::SFixedChecks< MemDataType >::Run< int, int * >();
			Pools::SFixedChecks< MemDataType >::Run< TinyObj, TinyObj * >();
			Pools::SFixedChecks< MemDataType >::Run< SmallObj, SmallObj * >();
			Pools::SFixedChecks< MemDataType >::Run< Obj, Obj * >();
			Pools::SFixedChecks< MemDataType >::Run< BigObject, BigObject * >();

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

			Checker::Run< int >( index.m_intIndex );
			Checker::Run< TinyObj >( index.m_tinyObjIndex );
			Checker::Run< SmallObj >( index.m_smallObjIndex );
			Checker::Run< Obj >( index.m_objIndex );
			Checker::Run< BigObject >( index.m_bigObjIndex );

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

			Pools::SScatteredChecks< MemDataType >::Run< int, int * >( index.m_intIndex );
			Pools::SScatteredChecks< MemDataType >::Run< TinyObj, TinyObj * >( index.m_tinyObjIndex );
			Pools::SScatteredChecks< MemDataType >::Run< SmallObj, SmallObj * >( index.m_smallObjIndex );
			Pools::SScatteredChecks< MemDataType >::Run< Obj, Obj * >( index.m_objIndex );
			Pools::SScatteredChecks< MemDataType >::Run< BigObject, BigObject * >( index.m_bigObjIndex );

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

			Checker::Run< int >();
			Checker::Run< TinyObj >();
			Checker::Run< SmallObj >();
			Checker::Run< Obj >();
			Checker::Run< BigObject >();

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

			Pools::SVariableChecks< MemDataType >::Run< int, int * >();
			Pools::SVariableChecks< MemDataType >::Run< TinyObj, TinyObj * >();
			Pools::SVariableChecks< MemDataType >::Run< SmallObj, SmallObj * >();
			Pools::SVariableChecks< MemDataType >::Run< Obj, Obj * >();
			Pools::SVariableChecks< MemDataType >::Run< BigObject, BigObject * >();

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

			Checker::Run< TinyObj >();
			Checker::Run< SmallObj >();
			Checker::Run< Obj >();

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

			Pools::SUniqueFixedChecks< MemDataType >::Run< TinyObj >();
			Pools::SUniqueFixedChecks< MemDataType >::Run< SmallObj >();
			Pools::SUniqueFixedChecks< MemDataType >::Run< Obj >();

			std::cout << "  ****************************************" << std::endl << std::endl;
			std::cout << "Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl << std::endl;
			std::cout << "********************************************************************************" << std::endl;
			time = Clock::now();
			std::cout << std::endl;
		}
	}
}

#endif
