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
#ifndef ___C3D_MOVABLE_OBJECT_H___
#define ___C3D_MOVABLE_OBJECT_H___

#include "Castor3DPrerequisites.hpp"
#include "Animable.hpp"
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
	\brief		Movable object class
	\~french
	\brief		Classe d'objet déplaçable
	*/
	class C3D_API MovableObject
		:	public Animable
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		MovableObject loader
		\~english
		\brief		Loader de MovableObject
		*/
		class C3D_API TextLoader
			:	public Castor::Loader< MovableObject, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
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
			 *\brief		Writes a movable object into a text file
			 *\param[in]	p_file	the file to save the movable object in
			 *\param[in]	p_object	the movable object to save
			 *\~french
			 *\brief		Ecrit un MovableObject dans un fichier texte
			 *\param[in]	p_scene	Le MovableObject
			 *\param[in]	p_file	Le fichier
			 */
			virtual bool operator()( MovableObject const & p_object, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		MovableObject loader
		\~english
		\brief		Loader de MovableObject
		*/
		class C3D_API BinaryParser
			:	public Castor3D::BinaryParser< MovableObject >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			BinaryParser( Castor::Path const & p_path );
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
			virtual bool Fill( MovableObject const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( MovableObject & p_obj, BinaryChunk & p_chunk )const;
		};

	public :
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MovableObject( Scene * p_pScene, eMOVABLE_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_sn	Parent node
		 *\param[in]	p_name	The name
		 *\param[in]	p_eType	MovableObject type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_sn	Noeud parent
		 *\param[in]	p_name	Le nom
		 *\param[in]	p_eType	Le type de MovableObject
		 */
		MovableObject( Scene * p_pScene, SceneNode * p_sn, Castor::String const & p_name, eMOVABLE_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		MovableObject( MovableObject const & p_object );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		MovableObject( MovableObject && p_object );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_object	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_object	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		MovableObject & operator =( MovableObject const & p_object );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_object	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		MovableObject & operator =( MovableObject && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~MovableObject();
		/**
		 *\~english
		 *\brief		Cleans the pointers the object has created and that are not necessary (currently none)
		 *\~french
		 *\brief		Nettoie l'instance
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Detaches the movable object from it's parent
		 *\~french
		 *\brief		Détache l'objet de son parent
		 */
		void Detach();
		/**
		 *\~english
		 *\brief		Attaches the movable object to a node
		 *\~french
		 *\brief		Attache l'object à un noeud
		 */
		void AttachTo( SceneNode * p_node );
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		La valeur
		 */
		inline Castor::String const & GetName()const
		{
			return m_strName;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent node
		 *\return		The value
		 *\~french
		 *\brief		Récupère le noeud parent
		 *\return		La valeur
		 */
		inline SceneNode * GetParent()const
		{
			return m_pSceneNode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent scene
		 *\return		The value
		 *\~french
		 *\brief		Récupère la scène parente
		 *\return		La valeur
		 */
		inline Scene * GetScene()const
		{
			return m_pScene;
		}
		/**
		 *\~english
		 *\brief		Retrieves the object type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type d'objet
		 *\return		La valeur
		 */
		inline eMOVABLE_TYPE GetType()const
		{
			return m_eType;
		}
		/**
		 *\~english
		 *\brief		Sets the object name
		 *\param[in]	p_strName	The new value
		 *\~french
		 *\brief		Définit le nom de l'objet
		 *\param[in]	p_strName	La nouvelle valeur
		 */
		inline void SetName( Castor::String const & p_strName )
		{
			m_strName = p_strName;
		}

	protected:
		//!\~english Movable object type	\~french Le type d'objet déplaçable
		eMOVABLE_TYPE m_eType;
		//!\~english The object name	\~french Le nom de l'objet
		Castor::String m_strName;
		//!\~english The parent node name	\~french Le nom du noeud parent
		Castor::String m_strNodeName;
		//!\~english The parent scene node	\~french Le noeud parent
		SceneNode * m_pSceneNode;
		//!\~english The parent scene	\~french La scène parente
		Scene * m_pScene;
	};
}

#pragma warning( pop )

#endif
