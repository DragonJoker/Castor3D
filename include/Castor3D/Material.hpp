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
#ifndef ___C3D_Material___
#define ___C3D_Material___

#include "Prerequisites.hpp"

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
	class C3D_API Material : public Castor::Resource<Material>
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Material loader
		\~french
		\brief		Loader de Material
		*/
		class C3D_API BinaryLoader : public Castor::Loader< Material, Castor::eFILE_TYPE_BINARY, Castor::BinaryFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			BinaryLoader();
			/**
			 *\~english
			 *\brief			Loads a material from a binary file
			 *\param[in,out]	p_material	The material to load
			 *\param[in,out]	p_file		The file where to load the material
			 *\~french
			 *\brief			Charge un matériau depuis un fichier binaire
			 *\param[in,out]	p_material	Le matériau
			 *\param[in,out]	p_file		Le fichier
			 */
			virtual bool operator ()( Material & p_material, Castor::BinaryFile & p_file);
			/**
			 *\~english
			 *\brief			Saves a material into a binary file
			 *\param[in]		p_material	The material to save
			 *\param[in,out]	p_file		The file where to save the material
			 *\~french
			 *\brief			Enregistre un matériau dans un fichier binaire
			 *\param[in]		p_material	Le matériau
			 *\param[in,out]	p_file		Le fichier
			 */
			virtual bool operator ()( Material const & p_material, Castor::BinaryFile & p_file);
		};
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
		class C3D_API TextLoader : public Castor::Loader< Material, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode=Castor::File::eENCODING_MODE_ASCII );
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
			virtual bool operator ()( Material const & p_material, Castor::TextFile & p_file);
		};

	private:
		PassPtrArray m_passes;
		Engine * m_pEngine;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine			The core engine
		 *\param[in]	p_strName			The material name
		 *\param[in]	p_iNbInitialPasses	The initial passes count
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine			Le moteur
		 *\param[in]	p_strName			Le nom du matériau
		 *\param[in]	p_iNbInitialPasses	Le nombre initial de passes
		 */
		Material( Engine * p_pEngine, Castor::String const & p_strName = Castor::cuEmptyString, int p_iNbInitialPasses=1);
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
		const PassSPtr GetPass( uint32_t p_index)const;
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
		PassSPtr GetPass( uint32_t p_index);
		/**
		 *\~english
		 *\brief		Destroys the pass at the given index
		 *\param[in]	p_index	The pass index
		 *\~french
		 *\brief		Destroys the pass at the given index
		 *\param[in]	p_index	L'index de la passe
		 */
		void DestroyPass( uint32_t p_index);
		/**
		 *\~english
		 *\brief		Retrieves the passes count
		 *\return		The count
		 *\~french
		 *\brief		Récupère le nombre de passes
		 *\return		Le nombre
		 */
		inline uint32_t GetPassCount()const { return uint32_t( m_passes.size() ); }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator on the beginning of the passes array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le début du tableau de passes
		 *\return		L'itérateur
		 */
		inline PassPtrArrayConstIt Begin()const { return m_passes.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the beginning of the passes array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le début du tableau de passes
		 *\return		L'itérateur
		 */
		inline PassPtrArrayIt Begin() { return m_passes.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator on the end of the passes array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin du tableau de passes
		 *\return		L'itérateur
		 */
		inline PassPtrArrayConstIt End()const { return m_passes.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the end of the passes array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau de passes
		 *\return		L'itérateur
		 */
		inline PassPtrArrayIt End() { return m_passes.end(); }
		/**
		 *\~english
		 *\return		\p true if all passes needs alpha blending
		 *\~french
		 *\return		\p true si toutes les passes ont besoin d'alpha blending
		 */
		bool HasAlphaBlending()const;
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
	class C3D_API MaterialManager : public MaterialCollection
	{
	private:
		DECLARE_VECTOR(	MaterialWPtr, MaterialWPtr );
		MaterialSPtr		m_defaultMaterial;		//!< The default material
		Engine *			m_pEngine;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MaterialManager( Engine * p_pEngine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~MaterialManager();
		/**
		 *\~english
		 *\brief		Materials initialisation function
		 *\remark		Intialises the default material, a renderer must have been loaded
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Sets all the materials to be cleaned up
		 *\~french
		 *\brief		Met tous les matériaux à nettoyer
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Deletes the default material, flush the collection
		 *\~french
		 *\brief		Supprime le matériau par défaut, vide la collection
		 */
		void DeleteAll();
		/**
		 *\~english
		 *\brief		Puts all the materials names in the given array
		 *\param[out]	p_names	The array of names to be filled
		 *\~french
		 *\brief		Remplit la liste des noms de tous les matériaux
		 *\param[out]	p_names	La liste de noms
		 */
		void GetNames( Castor::StringArray & p_names );
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
		virtual bool Write( Castor::TextFile & p_file)const;
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
		virtual bool Read( Castor::TextFile & p_file);
		/**
		 *\~english
		 *\brief		Writes materials in a binary file
		 *\param[out]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Ecrit les matériaux dans un fichier binaire
		 *\param[out]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Save( Castor::BinaryFile & p_file)const;
		/**
		 *\~english
		 *\brief		Reads materials from a binary file
		 *\param[in]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Lit les matériaux à partir d'un fichier binaire
		 *\param[in]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Load( Castor::BinaryFile & p_file);
		/**
		 *\~english
		 *\brief		Retrieves the default material
		 *\~french
		 *\brief		Récupère le matériau par défaut
		 */
		inline MaterialSPtr	GetDefaultMaterial	()const	{ return m_defaultMaterial; }
	};
}

#pragma warning( pop )

#endif
