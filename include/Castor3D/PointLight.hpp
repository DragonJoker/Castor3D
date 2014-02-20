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
#ifndef ___C3D_PointLight___
#define ___C3D_PointLight___

#include "Light.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents a PointLight
	\remark		A point light is a light which enlights from a point in all directions.
	\~french
	\brief		Classe de représentation d'une PointLight
	\remark		Une point light est une source de lumière à un point donné qui illumine dans toutes les directions
	*/
	class C3D_API PointLight : public LightCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		PointLight loader
		\~french
		\brief		Loader de PointLight
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
			virtual bool operator ()( PointLight const & p_light, Castor::TextFile & p_file );
		};

	private:
		friend class Scene;
		//!\~english	The attenuation components : constant, linear and quadratic	\~french	Les composantes d'attenuation : constante, linéaire et quadratique
		Castor::Point3f m_attenuation;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Not to be used by the user, use Scene::CreateLight function instead
		 *\~french
		 *\brief		Constructeur
		 *\remark		A ne pas utiliser par l'utilisateur, utiliser Scene::CreateLight à la place
		 */
		PointLight();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~PointLight();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		A light
		 *\~french
		 *\brief		Fonction de création utilisée par Factory
		 *\return		Une lumière
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
		 *\brief		Sets attenuation components
		 *\param[in]	p_const		The constant attenuation
		 *\param[in]	p_linear	The linear attenuation
		 *\param[in]	p_quadratic	The quadratic attenuation
		 *\~french
		 *\brief		Définit les composantes d'atténuation
		 *\param[in]	p_const		Attenuation constante
		 *\param[in]	p_linear	Attenuation linéaire
		 *\param[in]	p_quadratic	Attenuation quadratique
		 */
		void SetAttenuation	( float p_const, float p_linear, float p_quadratic );
		/**
		 *\~english
		 *\brief		Sets attenuation components
		 *\param[in]	p_ptAttenuation	The attenuation components
		 *\~french
		 *\brief		Définit les composantes d'atténuation
		 *\param[in]	p_ptAttenuation	Les composantes d'attenuation
		 */
		void SetAttenuation	( Castor::Point3f const & p_ptAttenuation );
		/**
		 *\~english
		 *\brief		Sets attenuation components
		 *\param[in]	p_attenuation	The attenuation components
		 *\~french
		 *\brief		Définit les composantes d'atténuation
		 *\param[in]	p_ptAttenuation	Les composantes d'attenuation
		 */
		void SetAttenuation	( float const * p_pAttenuation );
		/**
		 *\~english
		 *\brief		Retrieves the attenuation components
		 *\return		The attenuation components
		 *\~french
		 *\brief		Récupère les composantes d'attenuation
		 *\return		Les composantes d'attenuation
		 */
		inline Castor::Point3f const & GetAttenuation ()const { return m_attenuation; }
		/**
		 *\~english
		 *\brief		Retrieves the attenuation components
		 *\return		The attenuation components
		 *\~french
		 *\brief		Récupère les composantes d'attenuation
		 *\return		Les composantes d'attenuation
		 */
		inline Castor::Point3f & GetAttenuation() { return m_attenuation; }
	};
}

#pragma warning( pop )

#endif
