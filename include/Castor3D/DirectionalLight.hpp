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
#ifndef ___C3D_DirectionalLight___
#define ___C3D_DirectionalLight___

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
	class C3D_API DirectionalLight : public LightCategory
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
			virtual bool operator ()( DirectionalLight const & p_light, Castor::TextFile & p_file );
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
		 *\brief		Sets the light's type and position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit le type de la lumière et sa position
		 *\param[in]	La nouvelle valeur
		 */
		virtual void SetPositionType( Castor::Point4f const & p_ptPosition) { LightCategory::SetPositionType( p_ptPosition );DoNormalisePosition(); }
		/**
		 *\~english
		 *\brief		Sets the light's type and position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit le type de la lumière et sa position
		 *\param[in]	La nouvelle valeur
		 */
		virtual void SetPositionType( Castor::Point4d const & p_ptPosition) { LightCategory::SetPositionType( p_ptPosition );DoNormalisePosition(); }
		/**
		 *\~english
		 *\brief		Sets the light position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position de la lumière
		 *\param[in]	La nouvelle valeur
		 */
		virtual void SetPosition( Castor::Point3f const & p_ptPosition) { LightCategory::SetPosition( p_ptPosition );DoNormalisePosition(); }
		/**
		 *\~english
		 *\brief		Sets the light position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position de la lumière
		 *\param[in]	La nouvelle valeur
		 */
		virtual void SetPosition( Castor::Point3d const & p_ptPosition) { LightCategory::SetPosition( p_ptPosition );DoNormalisePosition(); }

	private:
		/**
		 *\~english
		 *\brief		Normalises the position as a directional light has just a direction
		 *\~french
		 *\brief		Normalise la position car elle correspond à une direction pour les umières directionnelles
		 */
		void DoNormalisePosition();
	};
}

#pragma warning( pop )

#endif
