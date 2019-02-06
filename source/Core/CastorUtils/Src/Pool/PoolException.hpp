/*
See LICENSE file in root folder
*/
#ifndef ___CU_PoolException_H___
#define ___CU_PoolException_H___

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
		CU_ScopedEnumBounds( eCommonOutOfMemory )
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

	//!\~english	Specialisation for PoolErrorType::eCommonOutOfMemory.
	//!\~french		Spécialisation pour PoolErrorType::eCommonOutOfMemory.
	template<>
	struct Error< PoolErrorType::eCommonOutOfMemory >
	{
		//!\~english	The error text.
		//!\~french		Le texte de l'erreur.
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

	//!\~english	Specialisation for PoolErrorType::eCommonPoolIsFull.
	//!\~french		Spécialisation pour PoolErrorType::eCommonPoolIsFull.
	template<>
	struct Error< PoolErrorType::eCommonPoolIsFull >
	{
		//!\~english	The error text.
		//!\~french		Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	space	L'espace mémoire.
		 */
		static inline void report( void * space )
		{
			std::cerr << "0x" << std::hex << space << " - " << Error< PoolErrorType::eCommonPoolIsFull >::Text << std::endl;
		}
	};

	//!\~english	Specialisation for PoolErrorType::eCommonMemoryLeaksDetected.
	//!\~french		Spécialisation pour PoolErrorType::eCommonMemoryLeaksDetected.
	template<>
	struct Error< PoolErrorType::eCommonMemoryLeaksDetected >
	{
		//!\~english	The error text.
		//!\~french		Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	size	The memory size.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	size	La taille mémoire.
		 */
		static inline void report( size_t size )
		{
			std::cerr << Error< PoolErrorType::eCommonMemoryLeaksDetected >::Text << ": " << size << "bytes" << std::endl;
		}
	};

	//!\~english	Specialisation for PoolErrorType::eCommonNotFromRange.
	//!\~french		Spécialisation pour PoolErrorType::eCommonNotFromRange.
	template<>
	struct Error< PoolErrorType::eCommonNotFromRange >
	{
		//!\~english	The error text.
		//!\~french		Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	space	L'espace mémoire.
		 */
		static inline void report( void * space )
		{
			std::cerr << "0x" << std::hex << space << " - " << Error< PoolErrorType::eCommonNotFromRange >::Text << std::endl;
		}
	};

	//!\~english	Specialisation for PoolErrorType::eMarkedLeakAddress.
	//!\~french		Spécialisation pour PoolErrorType::eMarkedLeakAddress.
	template<>
	struct Error< PoolErrorType::eMarkedLeakAddress >
	{
		//!\~english	The error text.
		//!\~french		Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	space	L'espace mémoire.
		 */
		static inline void report( void * space )
		{
			std::cerr << "***   " << Error< PoolErrorType::eMarkedLeakAddress >::Text << ": 0x" << std::hex << space << std::endl;
		}
	};

	//!\~english	Specialisation for PoolErrorType::eMarkedDoubleDelete.
	//!\~french		Spécialisation pour PoolErrorType::eMarkedDoubleDelete.
	template<>
	struct Error< PoolErrorType::eMarkedDoubleDelete >
	{
		//!\~english	The error text.
		//!\~french		Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	space	L'espace mémoire.
		 */
		static inline void report( void * space )
		{
			std::cerr << "0x" << std::hex << space << " - " << Error< PoolErrorType::eMarkedDoubleDelete >::Text << std::endl;
		}
	};

	//!\~english	Specialisation for PoolErrorType::eMarkedNotFromPool.
	//!\~french		Spécialisation pour PoolErrorType::eMarkedNotFromPool.
	template<>
	struct Error< PoolErrorType::eMarkedNotFromPool >
	{
		//!\~english	The error text.
		//!\~french		Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	space	L'espace mémoire.
		 */
		static inline void report( void * space )
		{
			std::cerr << "0x" << std::hex << space << " - " << Error< PoolErrorType::eMarkedNotFromPool >::Text << std::endl;
		}
	};

	//!\~english	Specialisation for PoolErrorType::eGrowingNotFromRanges.
	//!\~french		Spécialisation pour PoolErrorType::eGrowingNotFromRanges.
	template<>
	struct Error< PoolErrorType::eGrowingNotFromRanges >
	{
		//!\~english	The error text.
		//!\~french		Le texte de l'erreur.
		CU_API static char const * const Text;
		/**
		 *\~english
		 *\brief		Reports a pool error.
		 *\param[in]	space	The memory space.
		 *\~french
		 *\brief		Rapporte une erreur de pool.
		 *\param[in]	space	L'espace mémoire.
		 */
		static inline void report( void * space )
		{
			std::cerr << "0x" << std::hex << space << " - " << Error< PoolErrorType::eGrowingNotFromRanges >::Text << std::endl;
		}
	};

	//!\~english	Specialisation for PoolErrorType::eSTLAllocatorUnique.
	//!\~french		Spécialisation pour PoolErrorType::eSTLAllocatorUnique.
	template<>
	struct Error< PoolErrorType::eSTLAllocatorUnique >
	{
		//!\~english	The error text.
		//!\~french		Le texte de l'erreur.
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
	 *\param[in]	name		The error text.
	 *\param[in]	params	The error parameters.
	 *\~french
	 *\brief		Rapporte une erreur de pool.
	 *\param[in]	name		Le texte de l'erreur.
	 *\param[in]	params	Les paramètres de l'erreur.
	 */
	template< PoolErrorType ErrorType, typename ... Params >
	static inline void reportError( char const * const name, Params ... params )
	{
		std::cerr << "*** " << name << " *** ";
		Error< ErrorType >::report( params... );
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
		 *\param[in]	file		The file name
		 *\param[in]	function	The function name
		 *\param[in]	line		The line number
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	file		Le nom du fichier
		 *\param[in]	function	Le nom de la fonction
		 *\param[in]	line		Le numéro de ligne
		 */
		PoolMemoryException( char const * file, char const * function, uint32_t line )
			: Exception( Error< ErrorType >::Text, file, function, line )
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
#define CU_PoolException( error ) throw castor::PoolMemoryException< error >( __FILE__, __FUNCTION__, __LINE__ )

#endif
