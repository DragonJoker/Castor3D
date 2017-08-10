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
#ifndef ___CU_POOL_EXCEPTION_H___
#define ___CU_POOL_EXCEPTION_H___

#include "CastorUtilsPrerequisites.hpp"
#include "Exception/Exception.hpp"

#include <exception>
#include <cassert>
#include <iostream>

namespace castor
{
	/*!
	\~english
	\brief		Pool errors enumeration.
	\~french
	\brief		Enumération des erreurs de pool.
	*/
	enum class PoolErrorType
	{
		eCommonOutOfMemory,
		eCommonPoolIsFull,
		eCommonMemoryLeaksDetected,
		eCommonNotFromRange,
		eMarkedLeakAddress,
		eMarkedDoubleDelete,
		eMarkedNotFromPool,
		eGrowingNotFromRanges,
		eSTLAllocatorUnique,
		CASTOR_SCOPED_ENUM_BOUNDS( eCommonOutOfMemory )
	};

	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2015
	\~english
	\brief		Error text and report function.
	\~french
	\brief		Texte et fonction de report d'erreur.
	*/
	template< PoolErrorType ErrorType > struct Error;

	//!\~english Specialisation for PoolErrorType::eCommonOutOfMemory.	\~french Spécialisation pour PoolErrorType::eCommonOutOfMemory.
	template<>
	struct Error< PoolErrorType::eCommonOutOfMemory >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 */
		static inline void report()
		{
			std::cerr << Error< PoolErrorType::eCommonOutOfMemory >::Text << std::endl;
		}
	};

	//!\~english Specialisation for PoolErrorType::eCommonPoolIsFull.	\~french Spécialisation pour PoolErrorType::eCommonPoolIsFull.
	template<>
	struct Error< PoolErrorType::eCommonPoolIsFull >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	p_space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	p_space	L'espace mémoire.
		 */
		static inline void report( void * p_space )
		{
			std::cerr << "0x" << std::hex << p_space << " - " << Error< PoolErrorType::eCommonPoolIsFull >::Text << std::endl;
		}
	};

	//!\~english Specialisation for PoolErrorType::eCommonMemoryLeaksDetected.	\~french Spécialisation pour PoolErrorType::eCommonMemoryLeaksDetected.
	template<>
	struct Error< PoolErrorType::eCommonMemoryLeaksDetected >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	p_size	The memory size.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	p_size	La taille mémoire.
		 */
		static inline void report( size_t p_size )
		{
			std::cerr << Error< PoolErrorType::eCommonMemoryLeaksDetected >::Text << ": " << p_size << "bytes" << std::endl;
		}
	};

	//!\~english Specialisation for PoolErrorType::eCommonNotFromRange.	\~french Spécialisation pour PoolErrorType::eCommonNotFromRange.
	template<>
	struct Error< PoolErrorType::eCommonNotFromRange >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	p_space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	p_space	L'espace mémoire.
		 */
		static inline void report( void * p_space )
		{
			std::cerr << "0x" << std::hex << p_space << " - " << Error< PoolErrorType::eCommonNotFromRange >::Text << std::endl;
		}
	};

	//!\~english Specialisation for PoolErrorType::eMarkedLeakAddress.	\~french Spécialisation pour PoolErrorType::eMarkedLeakAddress.
	template<>
	struct Error< PoolErrorType::eMarkedLeakAddress >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	p_space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	p_space	L'espace mémoire.
		 */
		static inline void report( void * p_space )
		{
			std::cerr << "***   " << Error< PoolErrorType::eMarkedLeakAddress >::Text << ": 0x" << std::hex << p_space << std::endl;
		}
	};

	//!\~english Specialisation for PoolErrorType::eMarkedDoubleDelete.	\~french Spécialisation pour PoolErrorType::eMarkedDoubleDelete.
	template<>
	struct Error< PoolErrorType::eMarkedDoubleDelete >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	p_space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	p_space	L'espace mémoire.
		 */
		static inline void report( void * p_space )
		{
			std::cerr << "0x" << std::hex << p_space << " - " << Error< PoolErrorType::eMarkedDoubleDelete >::Text << std::endl;
		}
	};

	//!\~english Specialisation for PoolErrorType::eMarkedNotFromPool.	\~french Spécialisation pour PoolErrorType::eMarkedNotFromPool.
	template<>
	struct Error< PoolErrorType::eMarkedNotFromPool >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	p_space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	p_space	L'espace mémoire.
		 */
		static inline void report( void * p_space )
		{
			std::cerr << "0x" << std::hex << p_space << " - " << Error< PoolErrorType::eMarkedNotFromPool >::Text << std::endl;
		}
	};

	//!\~english Specialisation for PoolErrorType::eGrowingNotFromRanges.	\~french Spécialisation pour PoolErrorType::eGrowingNotFromRanges.
	template<>
	struct Error< PoolErrorType::eGrowingNotFromRanges >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	p_space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	p_space	L'espace mémoire.
		 */
		static inline void report( void * p_space )
		{
			std::cerr << "0x" << std::hex << p_space << " - " << Error< PoolErrorType::eGrowingNotFromRanges >::Text << std::endl;
		}
	};

	//!\~english Specialisation for PoolErrorType::eSTLAllocatorUnique.	\~french Spécialisation pour PoolErrorType::eSTLAllocatorUnique.
	template<>
	struct Error< PoolErrorType::eSTLAllocatorUnique >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 */
		static inline void report()
		{
			std::cerr << Error< PoolErrorType::eSTLAllocatorUnique >::Text << std::endl;
		}
	};
	/**
	 *\~english
	 *\brief		Reports a pool error.
	 *\param[in]	p_name		The error text.
	 *\param[in]	p_params	The error parameters.
	 *\~french
	 *\brief		Rapporte une erreur de pool.
	 *\param[in]	p_name		Le texte de l'erreur.
	 *\param[in]	p_params	Les paramètres de l'erreur.
	 */
	template< PoolErrorType ErrorType, typename ... Params >
	static inline void reportError( char const * const p_name, Params ... p_params )
	{
		std::cerr << "*** " << p_name << " *** ";
		Error< ErrorType >::report( p_params... );
	}
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2015
	\~english
	\brief		Pool exception.
	\~french
	\brief		Exception de pool.
	*/
	template< PoolErrorType ErrorType >
	class PoolMemoryException
		: public Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	p_file		The file name
		 *\param[in]	p_function	The function name
		 *\param[in]	p_line		The line number
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_file		Le nom du fichier
		 *\param[in]	p_function	Le nom de la fonction
		 *\param[in]	p_line		Le numéro de ligne
		 */
		PoolMemoryException( char const * p_file, char const * p_function, uint32_t p_line )
			: Exception( Error< ErrorType >::Text, p_file, p_function, p_line )
		{
		}
	};
}
/*!
\author 	Sylvain DOREMUS
\date 		03/01/2011
\~english
\brief		Macro to ease the use of castor::PoolException
\~french
\brief		Macro définie pour faciliter l'utilisation de castor::PoolException
*/
#define POOL_EXCEPTION( error ) throw castor::PoolMemoryException< error >( __FILE__, __FUNCTION__, __LINE__ )

#endif
