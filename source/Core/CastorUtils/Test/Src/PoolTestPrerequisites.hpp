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
#ifndef ___TEST_PREREQUISITES_H___
#define ___TEST_PREREQUISITES_H___

#include "TestPrerequisites.hpp"

#include <FixedSizeMemoryData.hpp>
#include <FixedSizeMarkedMemoryData.hpp>
#include <FixedGrowingSizeMemoryData.hpp>
#include <FixedGrowingSizeMarkedMemoryData.hpp>

#include <cstdint>
#include <chrono>
#include <iostream>

/** Tout petit objet, ne contenant qu'un int
*/
struct TinyObj
{
	int valueA;
};

/** Petit objet, d�rivant de TinyObj et contenant en plus un double, et un float
*/
struct SmallObj
	: public TinyObj
{
	double valueB;
	float valueC;
};

/** Objet, d�rivant de SmallObj et contenant en plus un double, un float, et un tableau de 1 KO
*/
struct Obj
	: public SmallObj
{
	double valueD;
	float valueE;
	uint8_t valueF[1024];
};

/** Gros objet, d�rivant de Obj et contenant en plus un tableau de 1 MO
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

//! Nombre de int allou�s
static const size_t INT_COUNT = MAX_SIZE / sizeof( int );
//! Nombre de TinyObj allou�s
static const size_t TINYOBJ_COUNT = MAX_SIZE / sizeof( TinyObj );
//! Nombre de SmallObj allou�s
static const size_t SMALLOBJ_COUNT = MAX_SIZE / sizeof( SmallObj );
//! Nombre de Obj allou�s
static const size_t OBJ_COUNT = MAX_SIZE / sizeof( Obj );
//! Nombre de BigObj allou�s
static const size_t BIGOBJ_COUNT = MAX_SIZE / sizeof( BigObj );
//! Nombre de BigObject allou�s
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

/** Compl�te le temps de l'�tape courante, et affiche le message de l'�tape suivante
@param[in] message
Le message de l'�tape suivante
@param[in, out] time
L'index de temps courant, re�oit le nouvel index
*/
inline void NextStep( char const * message, TimePoint & time )
{
	std::cout << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl;
	time = Clock::now();
	std::cout << "  " << message << " ... ";
}

/** Affiche simplement le message de finalisation
@param[in, out] time
L'index de temps courant, re�oit le nouvel index
*/
inline void Finalise( TimePoint & time )
{
	std::cout << std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - time ).count() << "ms" << std::endl;
}

//! Macro pour r�duire l'�criture de tout ce bloc
#define MAKE_POOL_CHECKS( checks, type )\
	checks< type >::Run< int >();\
	checks< type >::Run< TinyObj >();\
	checks< type >::Run< SmallObj >();\
	checks< type >::Run< Obj >();\
	checks< type >::Run< BigObject >();

//! Macro pour r�duire l'�criture de tout ce bloc
#define MAKE_POOL_PTR_CHECKS( checks, type )\
	checks< type >::Run< int *, int * >();\
	checks< type >::Run< TinyObj *, TinyObj * >();\
	checks< type >::Run< SmallObj *, SmallObj * >();\
	checks< type >::Run< Obj *, Obj * >();\
	checks< type >::Run< BigObject *, BigObject * >();

//! Macro pour r�duire l'�criture de tout ce bloc, utilis� pour les tests en m�moire trou�e
#define MAKE_POOL_SCATTERED_CHECKS( checks, type, index )\
	checks< type >::Run< int >( index.m_intIndex );\
	checks< type >::Run< TinyObj >( index.m_tinyObjIndex );\
	checks< type >::Run< SmallObj >( index.m_smallObjIndex );\
	checks< type >::Run< Obj >( index.m_objIndex );\
	checks< type >::Run< BigObject >( index.m_bigObjIndex );

//! Macro pour r�duire l'�criture de tout ce bloc, utilis� pour les tests en m�moire trou�e
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
		Les param�tres de construction d'un objet
		*/
		template< typename Pool, typename ... Params >
		static inline Stored Allocate( Pool & pool, Params ... params )
		{
			return pool.Allocate( params... );
		}

		/** D�salloue un objet
		@param[in] pool
		Le pool d'objets
		@param[in] param
		L'objet � d�sallouer
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
		Les param�tres de construction d'un objet
		*/
		template< typename type, typename ... Params >
		static inline type * Allocate( Params ... params )
		{
			return new type( params... );
		}

		/** D�salloue un objet
		@param[in] param
		L'objet � d�sallouer
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
		Les param�tres de construction d'un objet
		*/
		template< typename type, typename ... Params >
		static inline type * Allocate( Params ... params )
		{
			return new( malloc( sizeof( type ) ) )type( params... );
		}

		/** D�salloue un objet
		@param[in] param
		L'objet � d�sallouer
		*/
		template< typename type >
		static inline void Deallocate( type * param )
		{
			free( param );
		}
	};
}
