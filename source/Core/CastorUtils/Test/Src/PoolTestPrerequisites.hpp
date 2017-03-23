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
#ifndef ___TEST_PREREQUISITES_H___
#define ___TEST_PREREQUISITES_H___

#include "CastorUtilsTestPrerequisites.hpp"

#include <Pool/FixedSizeMemoryData.hpp>
#include <Pool/FixedSizeMarkedMemoryData.hpp>
#include <Pool/FixedGrowingSizeMemoryData.hpp>
#include <Pool/FixedGrowingSizeMarkedMemoryData.hpp>

#include <cstdint>
#include <chrono>
#include <iostream>

/** Tout petit objet, ne contenant qu'un int
*/
struct TinyObj
{
	int valueA;
};

/** Petit objet, dérivant de TinyObj et contenant en plus un double, et un float
*/
struct SmallObj
	: public TinyObj
{
	double valueB;
	float valueC;
};

/** Objet, dérivant de SmallObj et contenant en plus un double, un float, et un tableau de 1 KO
*/
struct Obj
	: public SmallObj
{
	double valueD;
	float valueE;
	uint8_t valueF[1024];
};

/** Gros objet, dérivant de Obj et contenant en plus un tableau de 1 MO
*/
struct BigObj
	: public Obj
{
	uint8_t valueG[1024 * 1024];

	inline BigObj( double d, float f )
	{
		valueD = d;
		valueE = f;
	}
};

/** Gros objet, contenant un tableau de 1 MO
*/
struct BigObject
{
	uint8_t value[1024 * 1024];
};

/** La taille maximale allouée (par le pool et autres)
*/
#if !defined( NDEBUG )
static const size_t MAX_SIZE = 1024;
#else
static const size_t MAX_SIZE = 1024 * 1024 * 128;
#endif

//! Nombre de int alloués
static const size_t INT_COUNT = MAX_SIZE / sizeof( int );
//! Nombre de TinyObj alloués
static const size_t TINYOBJ_COUNT = MAX_SIZE / sizeof( TinyObj );
//! Nombre de SmallObj alloués
static const size_t SMALLOBJ_COUNT = MAX_SIZE / sizeof( SmallObj );
//! Nombre de Obj alloués
static const size_t OBJ_COUNT = MAX_SIZE / sizeof( Obj );
//! Nombre de BigObj alloués
static const size_t BIGOBJ_COUNT = MAX_SIZE / sizeof( BigObj );
//! Nombre de BigObject alloués
static const size_t BIGOBJECT_COUNT = MAX_SIZE / sizeof( BigObject );

typedef std::chrono::high_resolution_clock Clock;
typedef Clock::time_point TimePoint;

/** Affiche simplement le message  d'initialisation
@return
L'index de temps courant
*/
template< typename type, size_t obj_count >
inline TimePoint Initialise()
{
	std::cout << "  ****************************************" << std::endl;
	std::cout << "  " << obj_count << " " << typeid( type ).name() << std::endl;
	TimePoint time = Clock::now();
	std::cout << "  Initialisation ... ";
	return time;
}

/** Complàte le temps de l'étape courante, et affiche le message de l'étape suivante
@param[in] message
Le message de l'étape suivante
@param[in, out] time
L'index de temps courant, reçoit le nouvel index
*/
inline void NextStep( char const * message, TimePoint & time )
{
	std::cout << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl;
	time = Clock::now();
	std::cout << "  " << message << " ... ";
}

/** Affiche simplement le message de finalisation
@param[in, out] time
L'index de temps courant, reçoit le nouvel index
*/
inline void Finalise( TimePoint & time )
{
	std::cout << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl;
}

//! Macro pour réduire l'écriture de tout ce bloc
#define MAKE_POOL_CHECKS( checks, type )\
	checks< type >::Run< int >();\
	checks< type >::Run< TinyObj >();\
	checks< type >::Run< SmallObj >();\
	checks< type >::Run< Obj >();\
	checks< type >::Run< BigObject >();

//! Macro pour réduire l'écriture de tout ce bloc
#define MAKE_POOL_PTR_CHECKS( checks, type )\
	checks< type >::Run< int *, int * >();\
	checks< type >::Run< TinyObj *, TinyObj * >();\
	checks< type >::Run< SmallObj *, SmallObj * >();\
	checks< type >::Run< Obj *, Obj * >();\
	checks< type >::Run< BigObject *, BigObject * >();

//! Macro pour réduire l'écriture de tout ce bloc, utilisé pour les tests en mémoire trouée
#define MAKE_POOL_SCATTERED_CHECKS( checks, type, index )\
	checks< type >::Run< int >( index.m_intIndex );\
	checks< type >::Run< TinyObj >( index.m_tinyObjIndex );\
	checks< type >::Run< SmallObj >( index.m_smallObjIndex );\
	checks< type >::Run< Obj >( index.m_objIndex );\
	checks< type >::Run< BigObject >( index.m_bigObjIndex );

//! Macro pour réduire l'écriture de tout ce bloc, utilisé pour les tests en mémoire trouée
#define MAKE_POOL_PTR_SCATTERED_CHECKS( checks, type, index )\
	checks< type >::Run< int *, int * >( index.m_intIndex );\
	checks< type >::Run< TinyObj *, TinyObj * >( index.m_tinyObjIndex );\
	checks< type >::Run< SmallObj *, SmallObj * >( index.m_smallObjIndex );\
	checks< type >::Run< Obj *, Obj * >( index.m_objIndex );\
	checks< type >::Run< BigObject *, BigObject * >( index.m_bigObjIndex );

#endif

namespace AllocPolicies
{
	/** Politique d'allocation, pour les pool contenant des types non pointeurs
	*/
	template< typename Type, typename Stored = Type * >
	struct PoolPolicy
	{
		/** Alloue un objet, retourne un pointeur sur cet objet
		@param[in] pool
		Le pool d'objets
		@param[in] params
		Les paramètres de construction d'un objet
		*/
		template< typename Pool, typename ... Params >
		static inline Stored Allocate( Pool & pool, Params ... params )
		{
			return pool.Allocate( params... );
		}

		/** Désalloue un objet
		@param[in] pool
		Le pool d'objets
		@param[in] param
		L'objet à désallouer
		*/
		template< typename Pool >
		static inline void Deallocate( Pool & pool, Stored param )
		{
			pool.Deallocate( param );
		}

		//! Le nombre d'objets allouables
		static const size_t Count = MAX_SIZE / sizeof( Type );
	};

	/** Politique d'allocation, en utilisant le couple new/delete
	*/
	struct SNewDeletePolicy
	{
		/** Alloue un objet, retourne un pointeur sur cet objet
		@param[in] params
		Les paramètres de construction d'un objet
		*/
		template< typename type, typename ... Params >
		static inline type * Allocate( Params ... params )
		{
			return new type( params... );
		}

		/** Désalloue un objet
		@param[in] param
		L'objet à désallouer
		*/
		template< typename type >
		static inline void Deallocate( type * param )
		{
			delete param;
		}
	};

	/** Politique d'allocation, en utilisant le couple free/malloc
	*/
	struct SMallocFreePolicy
	{
		/** Alloue un objet, retourne un pointeur sur cet objet
		@param[in] params
		Les paramètres de construction d'un objet
		*/
		template< typename type, typename ... Params >
		static inline type * Allocate( Params ... params )
		{
			return new( malloc( sizeof( type ) ) )type( params... );
		}

		/** Désalloue un objet
		@param[in] param
		L'objet à désallouer
		*/
		template< typename type >
		static inline void Deallocate( type * param )
		{
			free( param );
		}
	};

	/** Politique d'allocation, en utilisant le couple new/delete
	*/
	struct SPlacementNewPolicy
	{
		/** Alloue un objet, retourne un pointeur sur cet objet
		@param[in] params
		Les paramètres de construction d'un objet
		*/
		template< typename type, typename ... Params >
		static inline type * Allocate( void * mem, Params ... params )
		{
			return new( mem ) type( params... );
		}

		/** Désalloue un objet
		@param[in] param
		L'objet à désallouer
		*/
		template< typename type >
		static inline void Deallocate( type * param )
		{
			param->~type();
		}
	};
}
