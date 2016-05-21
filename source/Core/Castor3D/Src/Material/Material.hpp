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
#ifndef ___C3D_MATERIAL_H___
#define ___C3D_MATERIAL_H___

#include "Castor3DPrerequisites.hpp"
#include "Binary/BinaryParser.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Definition of a material
	\remark		A material is composed of one or more passes.
	\~french
	\brief		Définition d'un matériau
	\remark		Un matériau est composé d'une ou plusieurs passes
	*/
	class Material
		: public Castor::Resource< Material >
		, public std::enable_shared_from_this< Material >
		, public Castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief Material loader
		\remark
		\~french
		\brief Loader de Material
		*/
		class TextLoader
			: public Castor::Loader< Material, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief			Writes a material into a text file
			 *\param[in]		p_material	The material to save
			 *\param[in,out]	p_file		The file where to save the material
			 *\~french
			 *\brief			Ecrit un matériau dans un fichier texte
			 *\param[in]		p_material	Le matériau
			 *\param[in,out]	p_file		Le fichier
			 */
			C3D_API virtual bool operator()( Material const & p_material, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		15/04/2013
		\~english
		\brief		Viewport loader
		\~french
		\brief		Loader de Viewport
		*/
		class BinaryParser
			: public Castor3D::BinaryParser< Material >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path		The current folder path
			 *\param[in]	p_engine	The engine
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path		Le chemin d'accès au dossier courant
			 *\param[in]	p_engine	Le moteur
			 */
			C3D_API BinaryParser( Castor::Path const & p_path, Engine * p_engine );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Fill( Material const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
			 *\param[out]	p_obj	L'objet à lire
			 *\param[in]	p_chunk	Le chunk contenant les données
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Parse( Material & p_obj, BinaryChunk & p_chunk )const;

		private:
			Engine * m_engine;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name		The material name
		 *\param[in]	p_engine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name		Le nom du matériau
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API Material( Castor::String const & p_name, Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Material();
		/**
		 *\~english
		 *\brief		Initialises the material and all it's passes
		 *\~french
		 *\brief		Initialise le matériau et toutes ses passes
		 */
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Flushes passes
		 *\~french
		 *\brief		Supprime les passes
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Creates a pass
		 *\return		The created pass
		 *\~french
		 *\brief		Crée une passe
		 *\return		La passe créée
		 */
		C3D_API PassSPtr CreatePass();
		/**
		 *\~english
		 *\brief		Retrieves a pass and returns it
		 *\param[in]	p_index	The index of the wanted pass
		 *\return		The retrieved pass or nullptr if not found
		 *\~french
		 *\brief		Récupère une passe
		 *\param[in]	p_index	L'index de la passe voulue
		 *\return		La passe récupére ou nullptr si non trouvés
		 */
		C3D_API const PassSPtr GetPass( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Retrieves a pass and returns it
		 *\param[in]	p_index	The index of the wanted pass
		 *\return		The retrieved pass or nullptr if not found
		 *\~french
		 *\brief		Récupère une passe
		 *\param[in]	p_index	L'index de la passe voulue
		 *\return		La passe récupére ou nullptr si non trouvés
		 */
		C3D_API PassSPtr GetPass( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Destroys the pass at the given index
		 *\param[in]	p_index	The pass index
		 *\~french
		 *\brief		Destroys the pass at the given index
		 *\param[in]	p_index	L'index de la passe
		 */
		C3D_API void DestroyPass( uint32_t p_index );
		/**
		*\~english
		*\return		\p true if all passes needs alpha blending
		*\~french
		*\return		\p true si toutes les passes ont besoin d'alpha blending
		*/
		C3D_API bool HasAlphaBlending()const;
		/**
		 *\~english
		 *\brief		Retrieves the passes count
		 *\return		The count
		 *\~french
		 *\brief		Récupère le nombre de passes
		 *\return		Le nombre
		 */
		inline uint32_t GetPassCount()const
		{
			return uint32_t( m_passes.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator on the beginning of the passes array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le début du tableau de passes
		 *\return		L'itérateur
		 */
		inline PassPtrArrayConstIt begin()const
		{
			return m_passes.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the beginning of the passes array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le début du tableau de passes
		 *\return		L'itérateur
		 */
		inline PassPtrArrayIt begin()
		{
			return m_passes.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator on the end of the passes array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin du tableau de passes
		 *\return		L'itérateur
		 */
		inline PassPtrArrayConstIt end()const
		{
			return m_passes.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the end of the passes array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau de passes
		 *\return		L'itérateur
		 */
		inline PassPtrArrayIt end()
		{
			return m_passes.end();
		}

	public:
		//!\~english The default material name	\~french Le nom du matériau par défaut
		static const Castor::String DefaultMaterialName;

	private:
		PassPtrArray m_passes;
	};
}

#endif
