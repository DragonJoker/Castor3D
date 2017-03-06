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
#ifndef ___C3D_LIGHT_CATEGORY_H___
#define ___C3D_LIGHT_CATEGORY_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/02/2010
	\~english
	\brief		Holds light components
	\remark		Derived into DirectionalLight, PointLight and SpotLight
	\~french
	\brief		Contient les composantes d'une lumière
	\remark		Dérivée en DirectionalLight, PointLight et SpotLight
	*/
	class LightCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		LightCategory loader
		\~french
		\brief		Loader de LightCategory
		*/
		class TextWriter
			: public Castor::TextWriter< LightCategory >
		{
		public:
			/**
			 *\~english
			 *\brief		Writes a LightCategory into a text file.
			 *\param[in]	p_file	The file.
			 *\~french
			 *\brief		Ecrit une LightCategory dans un fichier texte.
			 *\param[in]	p_file	Le fichier.
			 */
			C3D_API virtual bool WriteInto( Castor::TextFile & p_file ) = 0;

		protected:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a LightCategory into a text file.
			 *\param[in]	p_file	The file.
			 *\param[in]	p_light	The LightCategory to save.
			 *\~french
			 *\brief		Ecrit une LightCategory dans un fichier texte.
			 *\param[in]	p_file	Le fichier.
			 *\param[in]	p_light	La LightCategory.
			 */
			C3D_API bool operator()( LightCategory const & p_light, Castor::TextFile & p_file )override;
		};

	private:
		friend class Light;

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_lightType	The light category type.
		 *\param[in]	p_light		The parent Light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_lightType	Le type de catégorie de lumière.
		 *\param[in]	p_light		La Light parente.
		 */
		C3D_API explicit LightCategory( LightType p_lightType, Light & p_light );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~LightCategory();
		/**
		 *\~english
		 *\brief			Updates the light viewport.
		 *\param[in]		p_target	The target position, used by directional shadow map.
		 *\param[in,out]	p_viewport	The viewport to update.
		 *\param[in]		p_index		The light shadow map index, -1 if it doesn't have any.
		 *\~french
		 *\brief			Met le viewport de la source à jour.
		 *\param[in]		p_target	La position de la cible, utilisée pour la map d'ombres des source directionnelles.
		 *\param[in,out]	p_viewport	Le viewport à mettre à jour.
		 *\param[in]		p_index		L'indice de la shadow map de la lumière, -1 si elle n'en a pas.
		 */
		C3D_API virtual void Update( Castor::Point3r const & p_target
			, Viewport & p_viewport
			, int32_t p_index = -1 ) = 0;
		/**
		 *\~english
		 *\brief		Creates a LightCategroy specific TextLoader.
		 *\param[in]	p_tabs	The current indentation level.
		 *\return		The TextLoader.
		 *\~french
		 *\brief		Crée un TextLoader spécifique à la LightCategory.
		 *\param[in]	p_tabs	Le niveau d'intentation actuel.
		 *\return		Le TextLoader.
		 */
		C3D_API virtual std::unique_ptr< TextWriter > CreateTextWriter( Castor::String const & p_tabs ) = 0;
		/**
		 *\~english
		 *\brief		Puts the light into the given texture.
		 *\param[out]	p_texture	The texture that receives the light's data.
		 *\param[in]	p_index		The light index.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 *\param[out]	p_texture	La texture recevant les données de la source lumineuse.
		 *\param[in]	p_index		L'indice de la source lumineuse.
		 */
		C3D_API void Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Sets the light's colour.
		 *\param[in]	p_colour	The new value.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	p_colour	La nouvelle valeur.
		 */
		inline void SetColour( Castor::Point3f const & p_colour )
		{
			m_colour = p_colour;
		}
		/**
		 *\~english
		 *\brief		Sets the light's intensity values.
		 *\param[in]	p_intensity	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	p_intensity	Les nouvelles valeurs.
		 */
		inline void SetIntensity( Castor::Point3f const & p_intensity )
		{
			m_intensity = p_intensity;
		}
		/**
		 *\~english
		 *\brief		Sets the light's diffuse intensity
		 *\param[in]	p_intensity	The new value
		 *\~french
		 *\brief		Définit l'intensité diffuse
		 *\param[in]	p_intensity	La nouvelle valeur
		 */
		inline void SetAmbientIntensity( float p_intensity )
		{
			m_intensity[0] = p_intensity;
		}
		/**
		 *\~english
		 *\brief		Sets the light's diffuse intensity
		 *\param[in]	p_intensity	The new value
		 *\~french
		 *\brief		Définit l'intensité diffuse
		 *\param[in]	p_intensity	La nouvelle valeur
		 */
		inline void SetDiffuseIntensity( float p_intensity )
		{
			m_intensity[1] = p_intensity;
		}
		/**
		 *\~english
		 *\brief		Sets the light's specular intensity
		 *\param[in]	p_intensity	The new value
		 *\~french
		 *\brief		Définit l'intensité spéculaire
		 *\param[in]	p_intensity	La nouvelle valeur
		 */
		inline void SetSpecularIntensity( float p_intensity )
		{
			m_intensity[2] = p_intensity;
		}
		/**
		 *\~english
		 *\brief		Retrieves the light type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de lumière
		 *\return		La valeur
		 */
		inline LightType GetLightType()const
		{
			return m_lightType;
		}
		/**
		 *\~english
		 *\brief		Retrieves the ambient colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur ambiante
		 *\return		La valeur
		 */
		inline float GetAmbientIntensity()const
		{
			return m_intensity[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves the diffuse colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur diffuse
		 *\return		La valeur
		 */
		inline float GetDiffuseIntensity()const
		{
			return m_intensity[1];
		}
		/**
		 *\~english
		 *\brief		Retrieves the specular colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur spéculaire
		 *\return		La valeur
		 */
		inline float GetSpecularIntensity()const
		{
			return m_intensity[2];
		}
		/**
		 *\~english
		 *\return		The intensity values
		 *\~french
		 *\return		Les valeurs d'intensité
		 */
		inline Castor::Point3f const & GetIntensity()const
		{
			return m_intensity;
		}
		/**
		 *\~english
		 *\return		The intensity values
		 *\~french
		 *\return		Les valeurs d'intensité
		 */
		inline Castor::Point3f & GetIntensity()
		{
			return m_intensity;
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		inline Castor::Point3f const & GetColour()const
		{
			return m_colour;
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		inline Castor::Point3f & GetColour()
		{
			return m_colour;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent light
		 *\return		The value
		 *\~french
		 *\brief		Récupère la lumière parente
		 *\return		La valeur
		 */
		inline Light const & GetLight()const
		{
			return m_light;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent light
		 *\return		The value
		 *\~french
		 *\brief		Récupère la lumière parente
		 *\return		La valeur
		 */
		inline Light & GetLight()
		{
			return m_light;
		}

	protected:
		/**
		 *\~english
		 *\brief		Updates the light source's informations related to the scene node.
		 *\param[in]	p_node	The scene node.
		 *\~french
		 *\brief		Met à jour les informations de la source lumineuse, relatives au noeud de scène.
		 *\param[in]	p_node	Le noeud de scène.
		 */
		C3D_API virtual void UpdateNode( SceneNode const & p_node ) = 0;

	protected:
		void DoBindComponent( Castor::Point3f const & p_component, uint32_t p_index, uint32_t & p_offset, Castor::PxBufferBase & p_data )const;
		void DoBindComponent( Castor::Point4f const & p_component, uint32_t p_index, uint32_t & p_offset, Castor::PxBufferBase & p_data )const;
		void DoBindComponent( Castor::ConstCoords4f const & p_component, uint32_t p_index, uint32_t & p_offset, Castor::PxBufferBase & p_data )const;
		void DoBindComponent( Castor::Coords4f const & p_component, uint32_t p_index, uint32_t & p_offset, Castor::PxBufferBase & p_data )const;
		void DoBindComponent( Castor::Matrix4x4f const & p_component, uint32_t p_index, uint32_t & p_offset, Castor::PxBufferBase & p_data )const;
		void DoBindComponent( int32_t const & p_component, uint32_t p_index, uint32_t & p_offset, Castor::PxBufferBase & p_data )const;

	private:
		/**
		 *\~english
		 *\brief		Puts the light source's type specific data into the given texture.
		 *\param[out]	p_texture	The texture that receives the light's data.
		 *\param[in]	p_index		The light index.
		 *\param[in]	p_offset	The starting offset.
		 *\~french
		 *\brief		Met les informations spécifiques au type de source lumineuse dans la texture donnée.
		 *\param[out]	p_texture	La texture recevant les données de la source lumineuse.
		 *\param[in]	p_index		L'indice de la source lumineuse.
		 *\param[in]	p_offset	Le décalage de début.
		 */
		C3D_API virtual void DoBind( Castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const = 0;

	private:
		//!\~english	The light type.
		//!\~french		Le type de lumière.
		LightType m_lightType;
		//!\~english	The parent light.
		//!\~french		La lumière parente.
		Light & m_light;
		//!\~english	The colour.
		//!\~french		La couleur.
		Castor::Point3f m_colour{ 1.0, 1.0, 1.0 };
		//!\~english	The intensity values.
		//!\~french		Les valeurs d'intensité.
		Castor::Point3f m_intensity{ 0.0, 1.0, 1.0 };
	};
}

#endif
