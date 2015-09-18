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
#ifndef ___C3D_MATERIAL_MANAGER_H___
#define ___C3D_MATERIAL_MANAGER_H___

#include "Castor3DPrerequisites.hpp"
#include "BinaryParser.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Material collection, with additional functions
	\~french
	\brief		Collection de matériaux, avec des fonctions additionnelles
	*/
	class C3D_API MaterialManager
		: public MaterialCollection
	{
	private:
		DECLARE_VECTOR(	MaterialWPtr, MaterialWPtr );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MaterialManager( Engine * p_engine );
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
		virtual bool Write( Castor::TextFile & p_file )const;
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
		virtual bool Read( Castor::TextFile & p_file );
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
		virtual bool Save( Castor::BinaryFile & p_file )const;
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
		virtual bool Load( Castor::BinaryFile & p_file );
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

	private:
		//!\~english The default material	\~french Le matériau par défaut
		MaterialSPtr m_defaultMaterial;
		//!\~english The engine	\~french Le moteur
		Engine * m_engine;
	};
}

#pragma warning( pop )

#endif
