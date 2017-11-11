/*
See LICENSE file in root folder
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
	\brief		Light source implementation.
	\~french
	\brief		Implémentation d'une source lumineuse.
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
		\brief		Light loader.
		\~french
		\brief		Light loader.
		*/
		class TextWriter
			: public castor::TextWriter< Light >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Saves a Light into a text file.
			 *\param[in]	file	the file.
			 *\param[in]	object	the Light to save.
			 *\return		\p true if everything is OK.
			 *\~french
			 *\brief		Sauvegarde la Light donné dans un fichier texte.
			 *\param[in]	file	Le fichier.
			 *\param[in]	object	La Light à enregistrer.
			 *\return		\p true si tout s'est bien passé.
			 */
			C3D_API bool operator()( Light const & object, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name		The light name.
		 *\param[in]	scene		Parent scene.
		 *\param[in]	node		The parent scene node.
		 *\param[in]	factory		Factory used to create the LightCategory.
		 *\param[in]	lightType	The light type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name		Le nom de la lumière.
		 *\param[in]	scene		La scène parente.
		 *\param[in]	node		Le scene node parent.
		 *\param[in]	factory		La fabrique de LightCategory.
		 *\param[in]	lightType	Le type de lumière.
		 */
		C3D_API Light( castor::String const & name
			, Scene & scene
			, SceneNodeSPtr node
			, LightFactory & factory
			, LightType lightType );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Light();
		/**
		 *\~english
		 *\brief		Updates the light.
		 *\~french
		 *\brief		Met la source à jour.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief			Updates the light viewport.
		 *\param[in]		target		The target position, used by directional shadow map.
		 *\param[in,out]	viewport	The viewport to update.
		 *\param[in]		index		The light shadow map index, -1 if it doesn't have any.
		 *\~french
		 *\brief			Met le viewport de la source à jour.
		 *\param[in]		target		La position de la cible, utilisée pour la map d'ombres des source directionnelles.
		 *\param[in,out]	viewport	Le viewport à mettre à jour.
		 *\param[in]		index		L'indice de la shadow map de la lumière, -1 si elle n'en a pas.
		 */
		C3D_API void updateShadow( castor::Point3r const & target
			, Viewport & viewport
			, int32_t index = -1 );
		/**
		 *\~english
		 *\brief		Puts the light into the given texture.
		 *\param[out]	texture	The texture that receives the light's data.
		 *\param[in]	index	The light index.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 *\param[out]	texture	La texture recevant les données de la source lumineuse.
		 *\param[in]	index	L'indice de la source lumineuse.
		 */
		C3D_API void bind( castor::PxBufferBase & texture, uint32_t index );
		/**
		 *\~english
		 *\brief		Attaches this light to a Material.
		 *\param[in]	node	The new light's parent node.
		 *\~french
		 *\brief		Attache cette lumière au node donné.
		 *\param[in]	node	Le nouveau node parent de cette lumière.
		 */
		C3D_API void attachTo( SceneNodeSPtr node )override;
		/**
		 *\~english
		 *\return		The DirectionalLight.
		 *\~french
		 *\return		La DirectionalLight.
		 */
		C3D_API DirectionalLightSPtr getDirectionalLight()const;
		/**
		 *\~english
		 *\return		The PointLight.
		 *\~french
		 *\return		La PointLight.
		 */
		C3D_API PointLightSPtr getPointLight()const;
		/**
		 *\~english
		 *\return		The SpotLight
		 *\~french
		 *\return		La SpotLight.
		 */
		C3D_API SpotLightSPtr getSpotLight()const;
		/**
		 *\~english
		 *\return		The light type.
		 *\~french
		 *\return		Le type de lumière.
		 */
		inline LightType getLightType()const
		{
			return m_category->getLightType();
		}
		/**
		 *\~english
		 *\return		The light enabled status.
		 *\~french
		 *\return		Le statut d'activation de la lumière.
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
		 *\brief		Sets the colour.
		 *\param[in]	values	The new value.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	values	La nouvelle valeur.
		 */
		inline void setColour( float * values )
		{
			m_category->setColour( castor::Point3f( values[0], values[1], values[2] ) );
		}
		/**
		 *\~english
		 *\brief		Sets the colour.
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
		 *\brief		Sets the colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setColour( castor::Point3f const & value )
		{
			m_category->setColour( value );
		}
		/**
		 *\~english
		 *\brief		Sets the colour.
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setColour( castor::RgbColour const & value )
		{
			m_category->setColour( toRGBFloat( value ) );
		}
		/**
		 *\~english
		 *\brief		Sets the light's intensity values.
		 *\param[in]	values	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	values	Les nouvelles valeurs.
		 */
		inline void setIntensity( float * values )
		{
			m_category->setIntensity( castor::Point2f( values[0], values[1] ) );
		}
		/**
		 *\~english
		 *\brief		Sets the light's intensity values.
		 *\param[in]	d, s	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	d, s	Les nouvelles valeurs.
		 */
		inline void setIntensity( float d, float s )
		{
			m_category->setIntensity( castor::Point2f( d, s ) );
		}
		/**
		 *\~english
		 *\brief		Sets the light's intensity values.
		 *\param[in]	value	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	value	Les nouvelles valeurs.
		 */
		inline void setIntensity( castor::Point2f const & value )
		{
			m_category->setIntensity( value );
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse intensity.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'intensité diffuse.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setDiffuseIntensity( float value )
		{
			m_category->setDiffuseIntensity( value );
		}
		/**
		 *\~english
		 *\brief		Sets the specular intensity.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'intensité spéculaire.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setSpecularIntensity( float value )
		{
			m_category->setSpecularIntensity( value );
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
		 *\brief		Sets the light enabled status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut d'activation de la lumère.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setEnabled( bool value )
		{
			m_enabled = value;
		}
		/**
		 *\~english
		 *\brief		Enables the light.
		 *\~french
		 *\brief		Active la lumière.
		 */
		inline void enable()
		{
			setEnabled( true );
		}
		/**
		 *\~english
		 *\brief		Disables the light.
		 *\~french
		 *\brief		Désactive la lumière.
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
		 *\brief		Sets the light shadow casting status.
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

	public:
		OnLightChanged onChanged;

	protected:
		void onNodeChanged( SceneNode const & node );

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
