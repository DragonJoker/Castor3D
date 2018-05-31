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
		C3D_API void bind( castor::Point4f * buffer );
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
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API DirectionalLightSPtr getDirectionalLight()const;
		C3D_API PointLightSPtr getPointLight()const;
		C3D_API SpotLightSPtr getSpotLight()const;

		inline LightType getLightType()const
		{
			return m_category->getLightType();
		}

		inline bool isEnabled()const
		{
			return m_enabled;
		}

		inline castor::Point3f const & getColour()const
		{
			return m_category->getColour();
		}

		inline castor::Point2f const & getIntensity()const
		{
			return m_category->getIntensity();
		}

		inline float getFarPlane()const
		{
			return m_category->getFarPlane();
		}

		inline float getDiffuseIntensity()const
		{
			return m_category->getDiffuseIntensity();
		}

		inline float getSpecularIntensity()const
		{
			return m_category->getSpecularIntensity();
		}

		inline castor::BoundingBox const & getBoundingBox()const
		{
			return m_category->getBoundingBox();
		}

		inline uint32_t getVolumetricSteps()const
		{
			return m_category->getVolumetricSteps();
		}

		inline float getVolumetricScatteringFactor()const
		{
			return m_category->getVolumetricScatteringFactor();
		}

		inline LightCategorySPtr getCategory()const
		{
			return m_category;
		}

		inline bool isShadowProducer()const
		{
			return m_shadowCaster;
		}

		inline ShadowType getShadowType()const
		{
			return m_shadowType;
		}

		inline ShadowMapRPtr getShadowMap()const
		{
			return m_shadowMap;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		inline void setColour( float * values )
		{
			m_category->setColour( castor::Point3f( values[0], values[1], values[2] ) );
		}

		inline void setColour( float r, float g, float b )
		{
			m_category->setColour( castor::Point3f( r, g, b ) );
		}

		inline void setColour( castor::Point3f const & value )
		{
			m_category->setColour( value );
		}

		inline void setColour( castor::RgbColour const & value )
		{
			m_category->setColour( toRGBFloat( value ) );
		}

		inline void setIntensity( float * values )
		{
			m_category->setIntensity( castor::Point2f( values[0], values[1] ) );
		}

		inline void setIntensity( float d, float s )
		{
			m_category->setIntensity( castor::Point2f( d, s ) );
		}

		inline void setIntensity( castor::Point2f const & value )
		{
			m_category->setIntensity( value );
		}

		inline void setDiffuseIntensity( float value )
		{
			m_category->setDiffuseIntensity( value );
		}

		inline void setSpecularIntensity( float value )
		{
			m_category->setSpecularIntensity( value );
		}

		inline void setVolumetricSteps( uint32_t value )
		{
			m_category->setVolumetricSteps( value );
		}

		inline void setVolumetricScatteringFactor( float value )
		{
			m_category->setVolumetricScatteringFactor( value );
		}

		inline void setEnabled( bool value )
		{
			m_enabled = value;
		}

		inline void enable()
		{
			setEnabled( true );
		}

		inline void disable()
		{
			setEnabled( false );
		}

		inline void setShadowProducer( bool value )
		{
			m_shadowCaster = value;
		}

		inline void setShadowType( ShadowType value )
		{
			m_shadowType = value;
		}

		inline void setShadowMap( ShadowMapRPtr value )
		{
			m_shadowMap = value;
		}
		/**@}*/

	public:
		OnLightChanged onChanged;

	protected:
		void onNodeChanged( SceneNode const & node );

	protected:
		bool m_enabled{ false };
		bool m_shadowCaster{ false };
		ShadowType m_shadowType{ ShadowType::eNone };
		LightCategorySPtr m_category;
		ShadowMapRPtr m_shadowMap{ nullptr };
	};
}

#endif
