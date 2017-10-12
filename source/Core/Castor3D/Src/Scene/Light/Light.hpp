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
#ifndef ___C3D_LIGHT_H___
#define ___C3D_LIGHT_H___

#include "Scene/MovableObject.hpp"

#include "LightCategory.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Light source implementation
	\~french
	\brief		Implémentation d'une source lumineuse
	\remark
	*/
	class Light
		: public MovableObject
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		24/05/2016
		\~english
		\brief		Light loader
		\~french
		\brief		Light loader
		*/
		class TextWriter
			: public castor::TextWriter< Light >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Saves a Light into a text file
			 *\param[in]	p_file		the file
			 *\param[in]	p_object	the Light to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde la Light donné dans un fichier texte
			 *\param[in]	p_file		Le fichier
			 *\param[in]	p_object	La Light à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			C3D_API bool operator()( Light const & p_object, castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name		The light name
		 *\param[in]	p_scene		Parent scene
		 *\param[in]	p_factory	Factory used to create the LightCategory
		 *\param[in]	p_lightType	The light type
		 *\param[in]	p_node		The parent scene node
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name		Le nom de la lumière
		 *\param[in]	p_scene		La scène parente
		 *\param[in]	p_factory	La fabrique de LightCategory
		 *\param[in]	p_lightType	Le type de lumière
		 *\param[in]	p_node		Le scene node parent
		 */
		C3D_API Light( castor::String const & p_name, Scene & p_scene, SceneNodeSPtr p_node, LightFactory & p_factory, LightType p_lightType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Light();
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
		C3D_API void update( castor::Point3r const & p_target
			, Viewport & p_viewport
			, int32_t p_index = -1 );
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
		C3D_API void bind( castor::PxBufferBase & p_texture, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Attaches this light to a Material
		 *\param[in]	p_node	The new light's parent node
		 *\~french
		 *\brief		Attache cette lumière au node donné
		 *\param[in]	p_node	Le nouveau node parent de cette lumière
		 */
		C3D_API void attachTo( SceneNodeSPtr p_node )override;
		/**
		 *\~english
		 *\brief		Retrieves the DirectionalLight category
		 *\return		The value
		 *\~french
		 *\brief		Récupère la DirectionalLight
		 *\return		La valeur
		 */
		C3D_API DirectionalLightSPtr getDirectionalLight()const;
		/**
		 *\~english
		 *\brief		Retrieves the PointLight category
		 *\return		The value
		 *\~french
		 *\brief		Récupère la PointLight
		 *\return		La valeur
		 */
		C3D_API PointLightSPtr getPointLight()const;
		/**
		 *\~english
		 *\brief		Retrieves the SpotLight category
		 *\return		The value
		 *\~french
		 *\brief		Récupère la SpotLight
		 *\return		La valeur
		 */
		C3D_API SpotLightSPtr getSpotLight()const;
		/**
		 *\~english
		 *\brief		Retrieves the light type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de lumière
		 *\return		La valeur
		 */
		inline LightType getLightType()const
		{
			return m_category->getLightType();
		}
		/**
		 *\~english
		 *\brief		Retrieves the light enabled status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'activation de la lumière
		 *\return		La valeur
		 */
		inline bool isEnabled()const
		{
			return m_enabled;
		}
		/**
		 *\~english
		 *\return		The light colour.
		 *\~french
		 *\return		La couleur de la lumière.
		 */
		inline castor::Point3f const & getColour()const
		{
			return m_category->getColour();
		}
		/**
		 *\~english
		 *\return		The intensity values.
		 *\~french
		 *\return		Les valeurs d'intensité.
		 */
		inline castor::Point2f const & getIntensity()const
		{
			return m_category->getIntensity();
		}
		/**
		 *\~english
		 *\return		The depth of the far plane.
		 *\~french
		 *\return		La profondeur du plan éloigné.
		 */
		inline float getFarPlane()const
		{
			return m_category->getFarPlane();
		}
		/**
		 *\~english
		 *\return		The diffuse intensity.
		 *\~french
		 *\return		L'intensité diffuse.
		 */
		inline float getDiffuseIntensity()const
		{
			return m_category->getDiffuseIntensity();
		}
		/**
		 *\~english
		 *\return		The specular intensity.
		 *\~french
		 *\return		L'intensité spéculaire.
		 */
		inline float getSpecularIntensity()const
		{
			return m_category->getSpecularIntensity();
		}
		/**
		 *\~english
		 *\brief		sets the colour.
		 *\param[in]	p_values	The new value.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	p_values	La nouvelle valeur.
		 */
		inline void setColour( float * p_values )
		{
			m_category->setColour( castor::Point3f( p_values[0], p_values[1], p_values[2] ) );
		}
		/**
		 *\~english
		 *\brief		sets the colour.
		 *\param[in]	r, g, b	The RGB components.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	r, g, b	Les composantes RGB.
		 */
		inline void setColour( float r, float g, float b )
		{
			m_category->setColour( castor::Point3f( r, g, b ) );
		}
		/**
		 *\~english
		 *\brief		sets the colour.
		 *\param[in]	p_colour	The new value.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	p_colour	La nouvelle valeur.
		 */
		inline void setColour( castor::Point3f const & p_colour )
		{
			m_category->setColour( p_colour );
		}
		/**
		 *\~english
		 *\brief		sets the colour.
		 *\param[in]	p_colour	The new value
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	p_colour	La nouvelle valeur
		 */
		inline void setColour( castor::Colour const & p_colour )
		{
			m_category->setColour( toRGBFloat( p_colour ) );
		}
		/**
		 *\~english
		 *\brief		sets the light's intensity values.
		 *\param[in]	p_values	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	p_values	Les nouvelles valeurs.
		 */
		inline void setIntensity( float * p_values )
		{
			m_category->setIntensity( castor::Point2f( p_values[0], p_values[1], p_values[2] ) );
		}
		/**
		 *\~english
		 *\brief		sets the light's intensity values.
		 *\param[in]	a, d, s	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	a, d, s	Les nouvelles valeurs.
		 */
		inline void setIntensity( float d, float s )
		{
			m_category->setIntensity( castor::Point2f( d, s ) );
		}
		/**
		 *\~english
		 *\brief		sets the light's intensity values.
		 *\param[in]	p_intensity	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	p_intensity	Les nouvelles valeurs.
		 */
		inline void setIntensity( castor::Point2f const & p_intensity )
		{
			m_category->setIntensity( p_intensity );
		}
		/**
		 *\~english
		 *\brief		sets the diffuse intensity.
		 *\param[in]	p_intensity	The new value.
		 *\~french
		 *\brief		Définit l'intensité diffuse.
		 *\param[in]	p_intensity	La nouvelle valeur.
		 */
		inline void setDiffuseIntensity( float p_intensity )
		{
			m_category->setDiffuseIntensity( p_intensity );
		}
		/**
		 *\~english
		 *\brief		sets the specular intensity.
		 *\param[in]	p_intensity	The new value.
		 *\~french
		 *\brief		Définit l'intensité spéculaire.
		 *\param[in]	p_intensity	La nouvelle valeur.
		 */
		inline void setSpecularIntensity( float p_intensity )
		{
			m_category->setSpecularIntensity( p_intensity );
		}
		/**
		 *\~english
		 *\return		The light's cube box.
		 *\~french
		 *\return		La cube box de la lumière.
		 */
		inline castor::CubeBox const & getCubeBox()const
		{
			return m_category->getCubeBox();
		}
		/**
		 *\~english
		 *\brief		sets the light enabled status
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit le statut d'activation de la lumère
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setEnabled( bool value )
		{
			m_enabled = value;
		}
		/**
		 *\~english
		 *\brief		Enables the light
		 *\~french
		 *\brief		Active la lumière
		 */
		inline void enable()
		{
			setEnabled( true );
		}
		/**
		 *\~english
		 *\brief		Disables the light
		 *\~french
		 *\brief		Désactive la lumière
		 */
		inline void disable()
		{
			setEnabled( false );
		}
		/**
		 *\~english
		 *\return		The light category.
		 *\~french
		 *\return		La catégorie de source lumineuse.
		 */
		inline LightCategorySPtr getCategory()const
		{
			return m_category;
		}
		/**
		 *\~english
		 *\return		Tells if the light casts shadows.
		 *\~french
		 *\return		Dit si la lumière projette des ombres.
		 */
		inline bool isShadowProducer()const
		{
			return m_shadowCaster;
		}
		/**
		 *\~english
		 *\brief		sets the light shadow casting status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut de projection d'ombre de la lumère
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setShadowProducer( bool value )
		{
			m_shadowCaster = value;
		}
		/**
		 *\~english
		 *\return		Tells if the light casts shadows.
		 *\~french
		 *\return		Dit si la lumière projette des ombres.
		 */
		inline ShadowMapRPtr getShadowMap()const
		{
			return m_shadowMap;
		}
		/**
		 *\~english
		 *\brief		Sets the light's shadow map.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la texture d'ombres de la lumière.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setShadowMap( ShadowMapRPtr value )
		{
			m_shadowMap = value;
		}

	protected:
		void onNodeChanged( SceneNode const & p_node );

	protected:
		//!\~english	Tells the light is enabled.
		//!\~french		Dit si la lumière est active ou pas.
		bool m_enabled{ false };
		//!\~english	Tells if the light casts shadows.
		//!\~french		Dit si la lumière projette des ombres.
		bool m_shadowCaster{ false };
		//!\~english	The Light category that effectively holds light data.
		//!\~french		la light category contenant les données de la lumière.
		LightCategorySPtr m_category;
		//!\~english	The shadow map.
		//!\~french		La texture d'ombres.
		ShadowMapRPtr m_shadowMap{ nullptr };
	};
}

#endif
