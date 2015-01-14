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
#ifndef ___C3D_MATERIAL_H___
#define ___C3D_MATERIAL_H___

#include "Castor3DPrerequisites.hpp"
#include "BinaryParser.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API Material
		:	public Castor::Resource< Material >
		,	public std::enable_shared_from_this< Material >
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
		class C3D_API TextLoader
			:	public Castor::Loader< Material, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
			,	public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode = Castor::File::eENCODING_MODE_ASCII );
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
			virtual bool operator()( Material const & p_material, Castor::TextFile & p_file );
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
		class C3D_API BinaryParser
			:	public Castor3D::BinaryParser< Material >
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
			BinaryParser( Castor::Path const & p_path, Engine * p_engine );
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
			virtual bool Fill( Material const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( Material & p_obj, BinaryChunk & p_chunk )const;

		private:
			//!\~english The engine	\~french Le moteur
			Engine * m_engine;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine			The core engine
		 *\param[in]	p_strName			The material name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine			Le moteur
		 *\param[in]	p_strName			Le nom du matériau
		 */
		Material( Engine * p_pEngine, Castor::String const & p_strName = Castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Material();
		/**
		 *\~english
		 *\brief		Activates the material
		 *\~french
		 *\brief		Active le matériau
		 */
		void Render();
		/**
		 *\~english
		 *\brief		Activates the material in 2D mode
		 *\~french
		 *\brief		Active le matériau en mode 2D
		 */
		void Render2D();
		/**
		 *\~english
		 *\brief		Deactivates the material (to avoid it from interfering with other materials)
		 *\~french
		 *\brief		Désactive le matériau (pour qu'il n'interfère pas avec les autres)
		 */
		void EndRender();
		/**
		 *\~english
		 *\brief		Initialises the material and all it's passes
		 *\~french
		 *\brief		Initialise le matériau et toutes ses passes
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Flushes passes
		 *\~french
		 *\brief		Supprime les passes
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Creates a pass
		 *\return		The created pass
		 *\~french
		 *\brief		Crée une passe
		 *\return		La passe créée
		 */
		PassSPtr CreatePass();
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
		const PassSPtr GetPass( uint32_t p_index )const;
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
		PassSPtr GetPass( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Destroys the pass at the given index
		 *\param[in]	p_index	The pass index
		 *\~french
		 *\brief		Destroys the pass at the given index
		 *\param[in]	p_index	L'index de la passe
		 */
		void DestroyPass( uint32_t p_index );
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
		inline PassPtrArrayConstIt Begin()const
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
		inline PassPtrArrayIt Begin()
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
		inline PassPtrArrayConstIt End()const
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
		inline PassPtrArrayIt End()
		{
			return m_passes.end();
		}
		/**
		 *\~english
		 *\return		\p true if all passes needs alpha blending
		 *\~french
		 *\return		\p true si toutes les passes ont besoin d'alpha blending
		 */
		bool HasAlphaBlending()const;

	public:
		//!\~english The default material name	\~french Le nom du matériau par défaut
		static const Castor::String DefaultMaterialName;

	private:
		PassPtrArray m_passes;
		Engine * m_pEngine;
	};
}

#pragma warning( pop )

#endif
