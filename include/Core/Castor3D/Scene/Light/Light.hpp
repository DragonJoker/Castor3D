/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_H___
#define ___C3D_LIGHT_H___

#include "LightModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmConfig.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Scene/MovableObject.hpp"
#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#include <atomic>

namespace castor3d
{
	class Light
		: public MovableObject
	{
	public:
		/**
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
			 *\param[in]	tabs	the current indentation.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	tabs	L'indentation courante.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Saves a Light into a text file.
			 *\param[in]	file	The file.
			 *\param[in]	object	The Light to save.
			 *\return		\p true if everything is OK.
			 *\~french
			 *\brief		Sauvegarde la Light donné dans un fichier texte.
			 *\param[in]	file	Le fichier.
			 *\param[in]	object	La Light à enregistrer.
			 *\return		\p true si tout s'est bien passé.
			 */
			C3D_API bool operator()( Light const & object
				, castor::TextFile & file )override;
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
			, SceneNode & node
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
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Records the light data into given buffer.
		 *\param[out]	buffer	The buffer.
		 *\~french
		 *\brief		Enregistre les données de l'image dans le tampon donné.
		 *\param[out]	buffer	Le tampon.
		 */
		C3D_API void bind( castor::Point4f * buffer )const;
		/**
		 *\~english
		 *\brief		Records the light data into given buffer.
		 *\param[in]	index	The light index inside the lights buffer.
		 *\param[out]	buffer	The buffer.
		 *\~french
		 *\brief		Enregistre les données de l'image dans le tampon donné.
		 *\param[in]	index	L'indice de la source lumineuse dans le tampon de lumières.
		 *\param[out]	buffer	Le tampon.
		 */
		inline void bind( uint32_t index, castor::Point4f * buffer )
		{
			m_bufferIndex = index;
			bind( buffer );
		}
		/**
		 *\~english
		 *\brief		Attaches this light to a SceneNode.
		 *\param[in]	node	The new light's parent node.
		 *\~french
		 *\brief		Attache cette lumière au SceneNode donné.
		 *\param[in]	node	Le nouveau node parent de cette lumière.
		 */
		C3D_API void attachTo( SceneNode & node )override;
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

		inline bool hasChanged()const
		{
			return m_dirty;
		}

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
			return m_currentShadowCaster;
		}

		inline bool isExpectedShadowProducer()const
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

		inline uint32_t getShadowMapIndex()const
		{
			return m_shadowMapIndex;
		}

		inline uint32_t getBufferIndex()const
		{
			return m_bufferIndex;
		}

		inline bool needsRsmShadowMaps()const
		{
			return getGlobalIlluminationType() != GlobalIlluminationType::eNone;
		}

		inline GlobalIlluminationType getGlobalIlluminationType()const
		{
			return m_currentGlobalIllumination;
		}

		inline GlobalIlluminationType getExpectedGlobalIlluminationType()const
		{
			return m_globalIllumination;
		}

		inline RsmConfig const & getRsmConfig()const
		{
			return m_rsmConfig;
		}

		inline RsmConfig & getRsmConfig()
		{
			return m_rsmConfig;
		}

		inline LpvConfig const & getLpvConfig()const
		{
			return m_lpvConfig;
		}

		inline LpvConfig & getLpvConfig()
		{
			return m_lpvConfig;
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

		inline void setRawMinOffset( float value )
		{
			m_category->setRawMinOffset( value );
		}

		inline void setRawMaxSlopeOffset( float value )
		{
			m_category->setRawMaxSlopeOffset( value );
		}

		inline void setPcfMinOffset( float value )
		{
			m_category->setPcfMinOffset( value );
		}

		inline void setPcfMaxSlopeOffset( float value )
		{
			m_category->setPcfMaxSlopeOffset( value );
		}

		inline void setVsmMaxVariance( float value )
		{
			m_category->setVsmMaxVariance( value );
		}

		inline void setVsmVarianceBias( float value )
		{
			m_category->setVsmVarianceBias( value );
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
			onChanged( *this );
		}

		inline void setShadowType( ShadowType value )
		{
			m_shadowType = value;
		}

		inline void setShadowMap( ShadowMapRPtr value, uint32_t index = 0u )
		{
			m_shadowMap = value;
			m_shadowMapIndex = index;
		}

		inline void setGlobalIlluminationType( GlobalIlluminationType value )
		{
			m_globalIllumination = value;
			onChanged( *this );
		}
		/**@}*/

	public:
		OnLightChanged onChanged;

	protected:
		void onNodeChanged( SceneNode const & node );

	protected:
		bool m_enabled{ false };
		bool m_shadowCaster{ false };
		std::atomic_bool m_currentShadowCaster{ false };
		bool m_dirty{ true };
		ShadowType m_shadowType{ ShadowType::eNone };
		LightCategorySPtr m_category;
		ShadowMapRPtr m_shadowMap{ nullptr };
		uint32_t m_shadowMapIndex{ 0u };
		GlobalIlluminationType m_globalIllumination{ GlobalIlluminationType::eNone };
		std::atomic< GlobalIlluminationType > m_currentGlobalIllumination{ GlobalIlluminationType::eNone };
		uint32_t m_bufferIndex{ 0u };
		RsmConfig m_rsmConfig;
		LpvConfig m_lpvConfig;
	};
}

#endif
