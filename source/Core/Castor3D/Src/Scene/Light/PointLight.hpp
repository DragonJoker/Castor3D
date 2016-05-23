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
#ifndef ___C3D_POINT_LIGHT_H___
#define ___C3D_POINT_LIGHT_H___

#include "Light.hpp"

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
	class PointLight
		: public LightCategory
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
		class TextLoader
			: public LightCategory::TextLoader
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
			C3D_API virtual bool operator()( PointLight const & p_light, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remarks		Not to be used by the user, use Scene::CreateLight function instead
		 *\~french
		 *\brief		Constructeur
		 *\remarks		A ne pas utiliser par l'utilisateur, utiliser Scene::CreateLight à la place
		 */
		C3D_API PointLight();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PointLight();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		A light
		 *\~french
		 *\brief		Fonction de création utilisée par Factory
		 *\return		Une lumière
		 */
		C3D_API static LightCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Puts the light into the given texture.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 */
		C3D_API virtual void Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Sets the light source position
		 *\param[in]	p_position	The new value
		 *\~french
		 *\brief		Définit la position de la source
		 *\param[in]	p_position	La nouvelle valeur
		 */
		C3D_API virtual void SetPosition( Castor::Point3r const & p_position );
		/**
		 *\~english
		 *\brief		Retrieves the light source position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de la source
		 *\return		La valeur
		 */
		C3D_API virtual Castor::Point3f GetPosition()const;
		/**
		 *\~english
		 *\brief		Sets attenuation components
		 *\param[in]	p_ptAttenuation	The attenuation components
		 *\~french
		 *\brief		Définit les composantes d'atténuation
		 *\param[in]	p_ptAttenuation	Les composantes d'attenuation
		 */
		C3D_API void SetAttenuation( Castor::Point3f const & p_ptAttenuation );
		/**
		 *\~english
		 *\brief		Retrieves the attenuation components
		 *\return		The attenuation components
		 *\~french
		 *\brief		Récupère les composantes d'attenuation
		 *\return		Les composantes d'attenuation
		 */
		inline Castor::Point3f const & GetAttenuation()const
		{
			return m_attenuation;
		}
		/**
		 *\~english
		 *\brief		Retrieves the attenuation components
		 *\return		The attenuation components
		 *\~french
		 *\brief		Récupère les composantes d'attenuation
		 *\return		Les composantes d'attenuation
		 */
		inline Castor::Point3f & GetAttenuation()
		{
			return m_attenuation;
		}

	private:
		friend class Scene;
		//!\~english The attenuation components : constant, linear and quadratic	\~french Les composantes d'attenuation : constante, linéaire et quadratique
		Castor::Point3f m_attenuation;
	};
}

#endif
