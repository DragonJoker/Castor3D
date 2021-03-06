/*
See LICENSE file in root folder
*/
#ifndef ___CU_DesignModule_HPP___
#define ___CU_DesignModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

#include <iterator>

namespace castor
{
	/**@name Design */
	//@{

	template< class IterT >
	struct IteratorTraits : std::iterator_traits< IterT >
	{
		using iterator_category = typename std::iterator_traits< IterT >::iterator_category;
		static_assert( std::is_convertible< iterator_category, std::random_access_iterator_tag >::value );
	};

	template< class TypeT >
	struct IteratorTraits< TypeT * > : std::iterator_traits< TypeT * >
	{
		using value_type = TypeT;
	};

	template< class FlagTypeT >
	struct FlagIteratorTraitsT
	{
		using difference_type = std::ptrdiff_t;
		using value_type = FlagTypeT;
		using pointer = value_type *;
		using reference = value_type &;
		using iterator_category = std::forward_iterator_tag;
	};
	/**
	\~english
	\brief		Templated class that provide std::array style buffer view.
	\~french
	\brief		Classe template qui fournit une vue sur un tampon, à la manière d'un std::array.
	*/
	template< typename ValueT
		, typename IteratorTraitsT = IteratorTraits< ValueT * > >
	class ArrayView;
	/**
	\~english
	\brief		Struct used to select best way to put type in parameter : 'value' or 'const reference'
	\~french
	\brief		Structure utilisée pour récupéerer la meilleure façon de passer T en paramètre : valeur ou référence constante
	*/
	template< typename T >
	struct CallTraits;
	/**
	\~english
	\brief		Used to have the minimum value of two, at compile time.
	\~french
	\brief		Utilisé pour obtenir la valeur minimale entre deux, à la compilation.
	*/
	template< uint32_t A, uint32_t B, typename Enable = void >
	struct MinValue;
	/**
	\~english
	\brief		Used to have the maximum value of two, at compile time.
	\~french
	\brief		Utilisé pour obtenir la valeur maximale entre deux, à la compilation.
	*/
	template< uint32_t A, uint32_t B, typename Enable = void >
	struct MaxValue;
	/**
	\~english
	*\brief		Connection to a signal.
	\~french
	*\brief		Représente une connexion à un signal.
	*/
	template< typename SignalT >
	class Connection;
	/**
	\~english
	*\brief		Basic signal class
	\~french
	*\brief		Classe basique de signal
	*/
	template< typename Function >
	class Signal;
	/**
	\~english
	\brief		Class used to execute code at scope exit.
	\~french
	\brief		Classe utilisée pour exécuter du code à la sortie d'un scope.
	*/
	template< typename ScopeExitFuncType >
	class ScopeGuard;
	/**
	\~english
	\brief		External resource representation
	\remark		A resource is a collectionnable object. You can change it's name, the Collection is noticed of the change
	\~french
	\brief		Représentation d'une ressource externe
	\remark		Une ressource est un objet collectionnable. Si le nom de la ressource est changé, la Collection est notifiée du changement
	*/
	template< class ResType >
	class Resource;
	/**
	\~english
	\brief		Class used to have an object owned by another one.
	\~french
	\brief		Classe permettant d'avoir un objet controlé par un autre.
	*/
	template< class Owner >
	class OwnedBy;
	/**
	\~english
	\brief		The non-copyable concept implementation
	\remark		Forbids a class which derivates from this one to be copied, either way
	\~french
	\brief		Implémentation du concept de non-copiable
	\remark		Interdit la copie des classes dérivant de celle-ci, de quelque manière que ce soit
	*/
	class NonCopyable;
	/**
	\~english
	\brief		Class for named elements
	\remark		The name type is a template argument so anything can be a name for this class (default is castor::String)
	\~french
	\brief		Classe de base pour les éléments nommés
	\remark		Le nom est un argument template, ainsi n'importe quoi peut être un nom pour cette classe (même si c'est castor::String par défaut)
	*/
	template< typename T = String >
	class NamedBase;
	/**
	\~english
	\brief		Templated class that provide std::array style buffer view.
	\~french
	\brief		Classe template qui fournit une vue sur un tampon, à la manière d'un std::array.
	*/
	template< typename T >
	class GroupChangeTracked;
	/**
	\~english
	\brief		Templated class that provide std::array style buffer view.
	\~french
	\brief		Classe template qui fournit une vue sur un tampon, à la manière d'un std::array.
	*/
	template< typename T >
	class ChangeTracked;
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		11/12/2016
	\~english
	\brief		Allows to declare a scoped variable with an action on construction
				<br />and an action on destruction.
	\remarks	Useful to force execution of code when an exception is thrown.
	\b Example
	@code
		char * buffer = nullptr;
		{
			auto guard = makeBlockGuard( [&buffer, &size]()
			{
				buffer = new char[size + 1];
			},
			[&buffer]()
			{
				delete [] buffer;
			} );
			//
			// ... Code that might throw an exception ...
			//
		}
	@endcode
	\~french
	\brief		Permet de déclarer une variable de scope, avec une action à la construction
				<br />et une action à la destruction.
	\remarks	Utile pour forcer l'exécution de code, si une exception est lancée.
	\b Example
	@code
		char * buffer = nullptr;
		{
			auto guard = makeBlockGuard( [&buffer, &size]()
			{
				buffer = new char[size + 1];
			},
			[&buffer]()
			{
				delete [] buffer;
			} );
			//
			// ... Code pouvant lancer une exception ...
			//
		}
	@endcode
	*/
	template< typename CleanFunc >
	struct BlockGuard;
	/**
	\~english
	\brief		Element collection class
	\remark		A collection class, allowing you to store named objects, removing, finding or adding them as you wish.
	\~french
	\brief		Classe de collection d'éléments
	\remark		Une classe de collection, permettant de stocker des éléments nommés, les enlever, les rechercher.
	*/
	template< typename TObj, typename TKey >
	class Collection;
	/**
	\~english
	\brief		Used to delay initialisation of an object to next use of it.
	\~french
	\brief		Utilisé pour délayer l'initialisation d'un objet à sa prochaine utilisation.
	*/
	template< typename TypeT >
	class DelayedInitialiserT;
	/**
	\~english
	\brief		Unicity exception
	\~french
	\brief		Exception d'unicité
	*/
	class UnicityException;
	/**
	*\brief
	*	Template iterator class on a binary combination of flags.
	*\param FlagType
	*	The scoped enum type.
	*/
	template< typename FlagTypeT
		, typename IteratorTraitsT = FlagIteratorTraitsT< FlagTypeT > >
	struct FlagIterator;
	/**
	*\~english
	*\brief
	*	Template class providing implicit conversion from a scoped enum to
	* 	its integral base type.
	*\remarks
	*	Allows definition of flags, through binary operations (&, |),
	*	for which operands will be of same binary size.
	*\~french
	*\brief
	*	Classe template qui fournit une conversion implicite depuis un scoped enum
	*	vers son type entier de base.
	*\remarks
	*	Permet la définition de flags, au travers des opérations binaires (&, |),
	*	Pour lesquels les opérandes auront la même taille binaire.
	*/
	template< typename FlagType >
	class FlagCombination;
	/**
	\~english
	\brief		Factory concept implementation
	\remark		The classes that can be registered must implement a function of the following form :
				<br />static std::shared_ptr< Obj > create();
	\~french
	\brief		Implémentation du concept de fabrique
	\remark		Les classes pouvant être enregistrées doivent implémenter une fonction de la forme suivante :
				<br />static std::shared_ptr< Obj > create();
	*/
	template< class Obj
		, class Key
		, class PtrType = std::shared_ptr< Obj >
		, typename PFNCreate = std::function< PtrType() >
		, class Predicate = std::less< Key > >
	class Factory;
	/**
	*\~english
	*\brief
	*	Dynamic bitset class, with configurable block type.
	*\~french
	*\brief
	*	Classe de bitset dynamique, avec un type de bloc configurable.
	*/
	template< typename BlockType >
	class DynamicBitsetT;
	/**
	\~english
	\brief		Representation of a Unique instance class
	\remarks	If another instance is to be created, an exception is thrown
	\~french
	\brief		Représentation d'un classe à instance unique
	\remarks	Si une seconde instance est créée, une exception est lancée
	*/
	template< class T >
	class Unique;

	using Named = NamedBase< String >;
	using DynamicBitset = DynamicBitsetT< uint32_t >;

	template< typename T >
	struct IsChangeTrackedT : std::false_type
	{
	};

	template< typename T >
	static inline bool constexpr isChangeTrackedT = IsChangeTrackedT< T >::value;
	//@}
}

#endif
