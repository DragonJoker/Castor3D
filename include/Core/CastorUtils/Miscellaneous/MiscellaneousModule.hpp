/*
See LICENSE file in root folder
*/
#ifndef ___CU_MiscellaneousModule_HPP___
#define ___CU_MiscellaneousModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

namespace castor
{
	/**@name Miscellaneous */
	//@{
	/**
	\~english
	\brief		Helper class, used to time a block's execution time
	\remark		Call the macro CU_Time() at the beginning of a block to have a console output when leaving that block
	\~french
	\brief		Classe permettant de mesurer le temps d'exécution d'un bloc
	\remark		Appelez la macro CU_Time() au début d'un bloc pour avoir une entrée dans la console en sortie du bloc
	*/
	class BlockTimer;
	/**
	\~english
	\brief		Helper class, used to track entering and leaving a block
	\remark		Call the macro CU_Track() at the beginning of a block to have a console output when entering or leaving that block
	\~french
	\brief		Classe permettant de tracer les entrées et sorties de bloc
	\remark		Appelez la macro CU_Track() au début d'un bloc pour avoir une entrée dans la console en entrée et en sortie du bloc
	*/
	class BlockTracker;
	/**
	\~english
	\brief		Retrieves the basic CPU informations.
	\~french
	\brief		Récupère les informations sur le CPU.
	*/
	class CpuInformations;
	/**
	\~english
	\brief		Platform independant library
	\remark		Loads a library and gives access to it's functions in a platform independant way
	\~french
	\brief		Bibliothèque dynamique (Dll, shared lib)
	\remark		Charge une bibliothèque et permet l'accès a ses fonction de manière indépendante de l'OS
	*/
	class DynamicLibrary;
	/**
	\~english
	\brief		Millisecond precise timer representation
	\~french
	\brief		Représentation d'un timer précis à la milliseconde
	*/
	class PreciseTimer;
	/**
	\~english
	\brief 		String functions namespace
	\~french
	\brief 		Espace de nom regroupant des fonctions sur les chaînes de caractères
	*/
	namespace string
	{
		/**
		\~english
		\brief 		Utf8 functions namespace
		\~french
		\brief 		Espace de nom regroupant des fonctions sur les chaînes de caractères en UTF-8
		*/
		namespace utf8
		{
			/**
			\~english
			\brief		An iterator, to an UTF-8 string
			\~french
			\brief		Un itérateur sur une chaîne UTF-8
			*/
			class iterator;
			/**
			\~english
			\brief		An iterator, to an UTF-8 string
			\~french
			\brief		Un itérateur sur une chaîne UTF-8
			*/
			class const_iterator;
		}
	}

	static const String cuEmptyString;

	CU_DeclareSmartPtr( DynamicLibrary );
	//@}
}

#endif
