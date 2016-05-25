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
#ifndef ___C3D_DIRECTIONAL_LIGHT_H___
#define ___C3D_DIRECTIONAL_LIGHT_H___

#include "Light.hpp"

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
	class DirectionalLight
		: public LightCategory
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
		class TextLoader
			: public LightCategory::TextLoader
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::String const & p_tabs, DirectionalLight const * p_category = nullptr );
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
			C3D_API bool operator()( DirectionalLight const & p_light, Castor::TextFile & p_file );
			/**
			 *\copydoc		Castor::LightCategory::WriteInto
			 */
			C3D_API virtual bool WriteInto( Castor::TextFile & p_file )override;

		private:
			DirectionalLight const * m_category;
		};

	private:
		friend class Scene;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remarks		Not to be used by the user, use Scene::CreateLight function instead
		 *\~french
		 *\brief		Constructeur
		 *\remarks		A ne pas utiliser par l'utilisateur, utiliser Scene::CreateLight à la place
		 */
		C3D_API DirectionalLight();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~DirectionalLight();
		/**
		 *\~english
		 *\brief		Creation function, used by the Factory
		 *\return		The light
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		La lumière
		 */
		C3D_API static LightCategorySPtr Create();
		/**
		 *\copydoc		Castor::LightCategory::CreateTextLoader
		 */
		C3D_API virtual std::unique_ptr < LightCategory::TextLoader > CreateTextLoader( Castor::String const & p_tabs )
		{
			return std::make_unique< TextLoader >( p_tabs, this );
		}
		/**
		 *\~english
		 *\brief		Puts the light into the given texture.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 */
		C3D_API virtual void Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Sets the light source direction
		 *\param[in]	p_direction	The new value
		 *\~french
		 *\brief		Définit la direction de la source
		 *\param[in]	p_direction	La nouvelle valeur
		 */
		C3D_API virtual void SetDirection( Castor::Point3f const & p_direction );
		/**
		 *\~english
		 *\brief		Retrieves the light source direction
		 *\return		The value
		 *\~french
		 *\brief		Récupère la direction de la source
		 *\return		La valeur
		 */
		C3D_API virtual Castor::Point3f GetDirection()const;
	};
}

#endif
