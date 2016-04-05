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
#ifndef ___CU_POOL_EXCEPTION_H___
#define ___CU_POOL_EXCEPTION_H___

#include "CastorUtilsPrerequisites.hpp"
#include "Exception.hpp"

#include <exception>
#include <cassert>
#include <iostream>

namespace Castor
{
	/*!
	\~english
	\brief		Pool errors enumeration.
	\~french
	\brief		Enumération des erreurs de pool.
	*/
	typedef enum ePOOL_ERROR_TYPE
	{
		ePOOL_ERROR_TYPE_COMMON_OUT_OF_MEMORY,
		ePOOL_ERROR_TYPE_COMMON_POOL_IS_FULL,
		ePOOL_ERROR_TYPE_COMMON_MEMORY_LEAKS_DETECTED,
		ePOOL_ERROR_TYPE_COMMON_NOT_FROM_RANGE,
		ePOOL_ERROR_TYPE_MARKED_LEAK_ADDRESS,
		ePOOL_ERROR_TYPE_MARKED_DOUBLE_DELETE,
		ePOOL_ERROR_TYPE_MARKED_NOT_FROM_POOL,
		ePOOL_ERROR_TYPE_GROWING_NOT_FROM_RANGES,
		ePOOL_ERROR_TYPE_STL_ALLOCATOR_UNIQUE,
	}	eERROR_TYPE;

	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2015
	\~english
	\brief		Error text and report function.
	\~french
	\brief		Texte et fonction de report d'erreur.
	*/
	template< eERROR_TYPE ErrorType > struct Error;

	//!\~english Specialisation for ePOOL_ERROR_TYPE_COMMON_OUT_OF_MEMORY.	\~french Spécialisation pour ePOOL_ERROR_TYPE_COMMON_OUT_OF_MEMORY.
	template<>
	struct Error< ePOOL_ERROR_TYPE_COMMON_OUT_OF_MEMORY >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 */
		static inline void Report()
		{
			std::cerr << Error< ePOOL_ERROR_TYPE_COMMON_OUT_OF_MEMORY >::Text << std::endl;
		}
	};

	//!\~english Specialisation for ePOOL_ERROR_TYPE_COMMON_POOL_IS_FULL.	\~french Spécialisation pour ePOOL_ERROR_TYPE_COMMON_POOL_IS_FULL.
	template<>
	struct Error< ePOOL_ERROR_TYPE_COMMON_POOL_IS_FULL >
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
		static inline void Report( void * p_space )
		{
			std::cerr << "0x" << std::hex << p_space << " - " << Error< ePOOL_ERROR_TYPE_COMMON_POOL_IS_FULL >::Text << std::endl;
		}
	};

	//!\~english Specialisation for ePOOL_ERROR_TYPE_COMMON_MEMORY_LEAKS_DETECTED.	\~french Spécialisation pour ePOOL_ERROR_TYPE_COMMON_MEMORY_LEAKS_DETECTED.
	template<>
	struct Error< ePOOL_ERROR_TYPE_COMMON_MEMORY_LEAKS_DETECTED >
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
		static inline void Report( size_t p_size )
		{
			std::cerr << Error< ePOOL_ERROR_TYPE_COMMON_MEMORY_LEAKS_DETECTED >::Text << ": " << p_size << "bytes" << std::endl;
		}
	};

	//!\~english Specialisation for ePOOL_ERROR_TYPE_COMMON_NOT_FROM_RANGE.	\~french Spécialisation pour ePOOL_ERROR_TYPE_COMMON_NOT_FROM_RANGE.
	template<>
	struct Error< ePOOL_ERROR_TYPE_COMMON_NOT_FROM_RANGE >
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
		static inline void Report( void * p_space )
		{
			std::cerr << "0x" << std::hex << p_space << " - " << Error< ePOOL_ERROR_TYPE_COMMON_NOT_FROM_RANGE >::Text << std::endl;
		}
	};

	//!\~english Specialisation for ePOOL_ERROR_TYPE_MARKED_LEAK_ADDRESS.	\~french Spécialisation pour ePOOL_ERROR_TYPE_MARKED_LEAK_ADDRESS.
	template<>
	struct Error< ePOOL_ERROR_TYPE_MARKED_LEAK_ADDRESS >
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
		static inline void Report( void * p_space )
		{
			std::cerr << "***   " << Error< ePOOL_ERROR_TYPE_MARKED_LEAK_ADDRESS >::Text << ": 0x" << std::hex << p_space << std::endl;
		}
	};

	//!\~english Specialisation for ePOOL_ERROR_TYPE_MARKED_DOUBLE_DELETE.	\~french Spécialisation pour ePOOL_ERROR_TYPE_MARKED_DOUBLE_DELETE.
	template<>
	struct Error< ePOOL_ERROR_TYPE_MARKED_DOUBLE_DELETE >
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
		static inline void Report( void * p_space )
		{
			std::cerr << "0x" << std::hex << p_space << " - " << Error< ePOOL_ERROR_TYPE_MARKED_DOUBLE_DELETE >::Text << std::endl;
		}
	};

	//!\~english Specialisation for ePOOL_ERROR_TYPE_MARKED_NOT_FROM_POOL.	\~french Spécialisation pour ePOOL_ERROR_TYPE_MARKED_NOT_FROM_POOL.
	template<>
	struct Error< ePOOL_ERROR_TYPE_MARKED_NOT_FROM_POOL >
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
		static inline void Report( void * p_space )
		{
			std::cerr << "0x" << std::hex << p_space << " - " << Error< ePOOL_ERROR_TYPE_MARKED_NOT_FROM_POOL >::Text << std::endl;
		}
	};

	//!\~english Specialisation for ePOOL_ERROR_TYPE_GROWING_NOT_FROM_RANGES.	\~french Spécialisation pour ePOOL_ERROR_TYPE_GROWING_NOT_FROM_RANGES.
	template<>
	struct Error< ePOOL_ERROR_TYPE_GROWING_NOT_FROM_RANGES >
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
		static inline void Report( void * p_space )
		{
			std::cerr << "0x" << std::hex << p_space << " - " << Error< ePOOL_ERROR_TYPE_GROWING_NOT_FROM_RANGES >::Text << std::endl;
		}
	};

	//!\~english Specialisation for ePOOL_ERROR_TYPE_STL_ALLOCATOR_UNIQUE.	\~french Spécialisation pour ePOOL_ERROR_TYPE_STL_ALLOCATOR_UNIQUE.
	template<>
	struct Error< ePOOL_ERROR_TYPE_STL_ALLOCATOR_UNIQUE >
	{
		//!\~english The error text.	\~french Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 */
		static inline void Report()
		{
			std::cerr << Error< ePOOL_ERROR_TYPE_STL_ALLOCATOR_UNIQUE >::Text << std::endl;
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
	template< eERROR_TYPE ErrorType, typename ... Params >
	static inline void ReportError( char const * const p_name, Params ... p_params )
	{
		std::cerr << "*** " << p_name << " *** ";
		Error< ErrorType >::Report( p_params... );
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
	template< eERROR_TYPE ErrorType >
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
\brief		Macro to ease the use of Castor::PoolException
\~french
\brief		Macro définie pour faciliter l'utilisation de Castor::PoolException
*/
#define POOL_EXCEPTION( error ) throw Castor::PoolMemoryException< error >( __FILE__, __FUNCTION__, __LINE__ )

#endif
