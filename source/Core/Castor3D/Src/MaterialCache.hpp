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
#ifndef ___C3D_MATERIAL_CACHE_H___
#define ___C3D_MATERIAL_CACHE_H___

#include "Cache/Cache.hpp"

#include "Material/Material.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct CachedObjectNamer< Material >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/07/2016
	\version	0.9.0
	\~english
	\brief		Helper structure to create an element.
	\~french
	\brief		Structure permettant de créer un élément.
	*/
	template<>
	struct ElementProducer< Material, Castor::String, Engine >
	{
		using ElemPtr = std::shared_ptr< Material >;

		inline ElemPtr operator()( Castor::String const & p_key, Engine & p_engine )
		{
			return std::make_shared< Material >( p_key, p_engine );
		}
	};
	using MaterialProducer = ElementProducer< Material, Castor::String, Engine >;
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Material collection, with additional functions
	\~french
	\brief		Collection de matériaux, avec des fonctions additionnelles
	*/
	class MaterialCache
		: public Cache< Material, Castor::String, MaterialProducer >
	{
	public:
		using Elem = Material;
		using Key = Castor::String;
		using Collection = Castor::Collection< Elem, Key >;
		using ElemPtr = std::shared_ptr< Elem >;
		using Producer = MaterialProducer;
		using Initialiser = ElementInitialiser< Elem >;
		using Cleaner = ElementCleaner< Elem >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_owner	The owner.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_owner	Le propriétaire.
		 */
		inline MaterialCache( EngineGetter && p_get
							  , Producer && p_produce
							  , Initialiser && p_initialise = Initialiser{}
							  , Cleaner && p_clean = Cleaner{} )
			: Cache< Material, Castor::String, MaterialProducer >( std::move( p_get ), std::move( p_produce ), std::move( p_initialise ), std::move( p_clean ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~MaterialCache ()
		{
		}
		/**
		 *\~english
		 *\brief		Materials initialisation function.
		 *\remarks		Intialises the default material, a renderer must have been loaded.
		 */
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Deletes the default material, flush the collection
		 *\~french
		 *\brief		Supprime le matériau par défaut, vide la collection
		 */
		C3D_API void Clear();
		/**
		 *\~english
		 *\brief		Puts all the materials names in the given array
		 *\param[out]	p_names	The array of names to be filled
		 *\~french
		 *\brief		Remplit la liste des noms de tous les matériaux
		 *\param[out]	p_names	La liste de noms
		 */
		C3D_API void GetNames( Castor::StringArray & p_names );
		/**
		 *\~english
		 *\brief		Writes materials in a text file
		 *\param[out]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Ecrit les matériaux dans un fichier texte
		 *\param[out]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Write( Castor::TextFile & p_file )const;
		/**
		 *\~english
		 *\brief		Reads materials from a text file
		 *\param[in]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Lit les matériaux à partir d'un fichier texte
		 *\param[in]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Read( Castor::TextFile & p_file );
		/**
		 *\~english
		 *\brief		Retrieves the default material
		 *\~french
		 *\brief		Récupère le matériau par défaut
		 */
		inline MaterialSPtr	GetDefaultMaterial()const
		{
			return m_defaultMaterial;
		}

	protected:
		//!\~english	The default material.
		//!\~french		Le matériau par défaut
		MaterialSPtr m_defaultMaterial;
	};
	/**
	 *\~english
	 *\brief		Creates a Material cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache de Material.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	inline std::unique_ptr< MaterialCache >
	MakeCache( EngineGetter && p_get, MaterialProducer && p_produce )
	{
		return std::make_unique< MaterialCache >( std::move( p_get ), std::move( p_produce ) );
	}
}

#endif
