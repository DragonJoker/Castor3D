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
#ifndef ___C3D_MATERIAL_CACHE_H___
#define ___C3D_MATERIAL_CACHE_H___

#include "Cache/Cache.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for Material.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour Material.
	*/
	template< typename KeyType >
	struct CacheTraits< Material, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Material >( KeyType const &, MaterialType ) >;
		using Merger = std::function< void( CacheBase< Material, KeyType > const &
			, castor::Collection< Material, KeyType > &
			, std::shared_ptr< Material > ) >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Material collection, with additional functions
	\~french
	\brief		Collection de matériaux, avec des fonctions additionnelles
	*/
	template<>
	class Cache< Material, castor::String >
		: public CacheBase< Material, castor::String >
	{
	public:
		using MyCacheType = CacheBase< Material, castor::String >;
		using MyCacheTraits = typename MyCacheType::MyCacheTraits;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	produce		The element producer.
		 *\param[in]	initialise	The element initialiser.
		 *\param[in]	clean		The element cleaner.
		 *\param[in]	merge		The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	produce		Le créateur d'objet.
		 *\param[in]	initialise	L'initialiseur d'objet.
		 *\param[in]	clean		Le nettoyeur d'objet.
		 *\param[in]	merge		Le fusionneur de collection d'objets.
		 */
		inline Cache( Engine & engine
			, Producer && produce
			, Initialiser && initialise
			, Cleaner && clean
			, Merger && merge )
			: MyCacheType( engine
				, std::move( produce )
				, std::move( initialise )
				, std::move( clean )
				, std::move( merge ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~Cache()
		{
		}
		/**
		 *\~english
		 *\brief		Intialises the default material.
		 *\~french
		 *\brief		Initialise le matériau par défaut.
		 */
		C3D_API void initialise( MaterialType type );
		/**
		 *\~english
		 *\brief		sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Deletes the default material, flush the collection
		 *\~french
		 *\brief		Supprime le matériau par défaut, vide la collection
		 */
		C3D_API void clear();
		/**
		 *\~english
		 *\brief		Updates the pass buffer.
		 *\~french
		 *\brief		Met à jour le tampon de passes.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\param[in]	element	The element.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name	Le nom d'élément.
		 *\param[in]	element	L'élément.
		 */
		C3D_API MaterialSPtr add( Key const & name, MaterialSPtr element );
		/**
		 *\~english
		 *\brief		Creates an element.
		 *\param[in]	name			The element name.
		 *\param[in]	p_parameters	The other constructor parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un élément.
		 *\param[in]	name			Le nom d'élément.
		 *\param[in]	p_parameters	Les autres paramètres de construction.
		 *\return		L'élément créé.
		 */
		C3D_API MaterialSPtr add( Key const & name, MaterialType type );
		/**
		 *\~english
		 *\brief		Puts all the materials names in the given array
		 *\param[out]	names	The array of names to be filled
		 *\~french
		 *\brief		Remplit la liste des noms de tous les matériaux
		 *\param[out]	names	La liste de noms
		 */
		C3D_API void getNames( castor::StringArray & names );
		/**
		 *\~english
		 *\brief		Writes materials in a text file
		 *\param[out]	file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Ecrit les matériaux dans un fichier texte
		 *\param[out]	file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool write( castor::TextFile & file )const;
		/**
		 *\~english
		 *\brief		Reads materials from a text file
		 *\param[in]	file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Lit les matériaux à partir d'un fichier texte
		 *\param[in]	file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool read( castor::TextFile & file );
		/**
		 *\~english
		 *\brief		Retrieves the default material
		 *\~french
		 *\brief		Récupère le matériau par défaut
		 */
		inline MaterialSPtr getDefaultMaterial()const
		{
			return m_defaultMaterial;
		}
		/**
		 *\~english
		 *\return		The pass buffer.
		 *\~french
		 *\return		Le tampon de passes.
		 */
		inline PassBuffer const & getPassBuffer()const
		{
			REQUIRE( m_passBuffer );
			return *m_passBuffer;
		}

	protected:
		//!\~english	The default material.
		//!\~french		Le matériau par défaut
		MaterialSPtr m_defaultMaterial;
		//!\~english	The passes buffer.
		//!\~french		Le tampon de passes.
		std::shared_ptr< PassBuffer > m_passBuffer;
	};
	using MaterialCache = Cache< Material, castor::String >;
	DECLARE_SMART_PTR( MaterialCache );
}

#endif
