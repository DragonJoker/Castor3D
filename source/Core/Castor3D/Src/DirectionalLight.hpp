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
#ifndef ___C3D_DIRECTIONAL_LIGHT_H___
#define ___C3D_DIRECTIONAL_LIGHT_H___

#include "Light.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version 	0.1
	\date 		09/02/2010
	\~english
	\brief		Class which represents a Directional Light
	\remark		A directional light is a light which enlights from an infinite point in a given direction
	\~french
	\brief		Classe representant une lumière directionnelle
	\remark		Une lumière directionnelle illumine à partir d'un point infini dans une direction donnée
	*/
	class C3D_API DirectionalLight
		:	public LightCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		DirectionalLight loader
		\~french
		\brief		Loader de DirectionalLight
		*/
		class C3D_API TextLoader : public LightCategory::TextLoader
		{
		public:
			/**
			 *\~english
			 *\brief		Writes a light into a text file
			 *\param[in]	p_file	The file to save the cameras in
			 *\param[in]	p_light	The light to save
			 *\~french
			 *\brief		Ecrit une lumière dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_light	La lumière
			 */
			virtual bool operator()( DirectionalLight const & p_light, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		DirectionalLight loader
		\~english
		\brief		Loader de DirectionalLight
		*/
		class C3D_API BinaryParser : public LightCategory::BinaryParser
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
			virtual bool Fill( DirectionalLight const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( DirectionalLight & p_obj, BinaryChunk & p_chunk )const;
		};

	private:
		friend class Scene;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Not to be used by the user, use Scene::CreateLight function instead
		 *\~french
		 *\brief		Constructeur
		 *\remark		A ne pas utiliser par l'utilisateur, utiliser Scene::CreateLight à la place
		 */
		DirectionalLight();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~DirectionalLight();
		/**
		 *\~english
		 *\brief		Creation function, used by the Factory
		 *\return		The light
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		La lumière
		 */
		static LightCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Renders the light
		 *\param[in]	p_pRenderer		The renderer used to ... render this light
		 *\~french
		 *\brief		Fonction de rendu sans shader
		 *\param[in]	p_pRenderer		Le renderer
		 */
		virtual void Render( LightRendererSPtr p_pRenderer );
		/**
		 *\~english
		 *\brief		Renders the light
		 *\param[in]	p_pRenderer		The renderer used to ... render this light
		 *\param[in]	p_pProgram		The shader program that will receive the light's components
		 *\~french
		 *\brief		Fonction de rendu avec shader
		 *\param[in]	p_pRenderer		Le renderer
		 *\param[in]	p_pProgram		Le shader qui va recevoir les informations de la lumière
		 */
		virtual void Render( LightRendererSPtr p_pRenderer, ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Sets the light source direction
		 *\param[in]	p_position	The new value
		 *\~french
		 *\brief		Définit la direction de la source
		 *\param[in]	p_position	La nouvelle valeur
		 */
		virtual void SetDirection( Castor::Point3f const & p_direction );
		/**
		 *\~english
		 *\brief		Retrieves the light source direction
		 *\return		The value
		 *\~french
		 *\brief		Récupère la direction de la source
		 *\return		La valeur
		 */
		virtual Castor::Point3f GetDirection()const;
	};
}

#pragma warning( pop )

#endif
