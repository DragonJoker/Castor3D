/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
		class TextWriter
			: public LightCategory::TextWriter
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextWriter( Castor::String const & p_tabs, DirectionalLight const * p_category = nullptr );
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
			C3D_API bool WriteInto( Castor::TextFile & p_file )override;

		private:
			DirectionalLight const * m_category;
		};

	private:
		friend class Scene;

	private:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_viewport	The shadow map render viewport.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_viewport	Le viewport utilisé pour le rendu de la map d'ombres.
		 */
		C3D_API DirectionalLight( Viewport & p_viewport );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~DirectionalLight();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory.
		 *\param[in]	p_viewport	The shadow map render viewport.
		 *\return		A light source.
		 *\~french
		 *\brief		Fonction de création utilisée par Factory.
		 *\param[in]	p_viewport	Le viewport utilisé pour le rendu de la map d'ombres.
		 *\return		Une source lumineuse.
		 */
		C3D_API static LightCategorySPtr Create( Viewport & p_viewport );
		/**
		 *\copydoc		Castor::LightCategory::Update
		 */
		C3D_API void Update( Castor::Size const & p_size )override;
		/**
		 *\copydoc		Castor::LightCategory::CreateTextLoader
		 */
		C3D_API std::unique_ptr < LightCategory::TextWriter > CreateTextWriter( Castor::String const & p_tabs )override
		{
			return std::make_unique< TextWriter >( p_tabs, this );
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
