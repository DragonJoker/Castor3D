/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_H___
#define ___C3D_LIGHT_H___

#include "LightModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"

#include "Castor3D/Scene/MovableObject.hpp"
#include "Castor3D/Scene/Shadow.hpp"
#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#pragma warning( pop )

namespace castor3d
{
	class Light
		: public MovableObject
	{
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
		 *\brief			CPU Update.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Mise à jour CPU.
		 *\param[in, out]	updater	Les données d'update.
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
		void bind( uint32_t index, castor::Point4f * buffer )
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

		bool hasChanged()const
		{
			return m_dirty;
		}

		LightType getLightType()const
		{
			return m_category->getLightType();
		}

		bool isEnabled()const
		{
			return m_enabled;
		}

		castor::Point3f const & getColour()const
		{
			return m_category->getColour();
		}

		castor::Point2f const & getIntensity()const
		{
			return m_category->getIntensity();
		}

		float getFarPlane()const
		{
			return m_category->getFarPlane();
		}

		float getDiffuseIntensity()const
		{
			return m_category->getDiffuseIntensity();
		}

		float getSpecularIntensity()const
		{
			return m_category->getSpecularIntensity();
		}

		castor::BoundingBox const & getBoundingBox()const
		{
			return m_category->getBoundingBox();
		}

		LightCategorySPtr getCategory()const
		{
			return m_category;
		}

		bool isShadowProducer()const
		{
			return m_currentShadowCaster;
		}

		bool isExpectedShadowProducer()const
		{
			return m_shadowCaster;
		}

		ShadowType getShadowType()const
		{
			return m_shadows.filterType;
		}

		ShadowMapRPtr getShadowMap()const
		{
			return m_shadowMap;
		}

		uint32_t getShadowMapIndex()const
		{
			return m_shadowMapIndex;
		}

		uint32_t getBufferIndex()const
		{
			return m_bufferIndex;
		}

		bool needsRsmShadowMaps()const
		{
			return getGlobalIlluminationType() != GlobalIlluminationType::eNone;
		}

		GlobalIlluminationType getGlobalIlluminationType()const
		{
			return m_currentGlobalIllumination;
		}

		GlobalIlluminationType getExpectedGlobalIlluminationType()const
		{
			return m_shadows.globalIllumination;
		}

		RsmConfig const & getRsmConfig()const
		{
			return m_shadows.rsmConfig;
		}

		RsmConfig & getRsmConfig()
		{
			return m_shadows.rsmConfig;
		}

		LpvConfig const & getLpvConfig()const
		{
			return m_shadows.lpvConfig;
		}

		LpvConfig & getLpvConfig()
		{
			return m_shadows.lpvConfig;
		}

		uint32_t getVolumetricSteps()const
		{
			return m_shadows.volumetricSteps;
		}

		float getVolumetricScatteringFactor()const
		{
			return m_shadows.volumetricScattering;
		}

		castor::Point2f const & getShadowRawOffsets()const
		{
			return m_shadows.rawOffsets;
		}

		castor::Point2f const & getShadowPcfOffsets()const
		{
			return m_shadows.pcfOffsets;
		}

		castor::Point2f const & getShadowVariance()const
		{
			return m_shadows.variance;
		}

		ShadowConfig const & getShadowConfig()const
		{
			return m_shadows;
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
		void setColour( float * values )
		{
			m_category->setColour( castor::Point3f( values[0], values[1], values[2] ) );
		}

		void setColour( float r, float g, float b )
		{
			m_category->setColour( castor::Point3f( r, g, b ) );
		}

		void setColour( castor::Point3f const & value )
		{
			m_category->setColour( value );
		}

		void setColour( castor::RgbColour const & value )
		{
			m_category->setColour( toRGBFloat( value ) );
		}

		void setIntensity( float * values )
		{
			m_category->setIntensity( castor::Point2f( values[0], values[1] ) );
		}

		void setIntensity( float d, float s )
		{
			m_category->setIntensity( castor::Point2f( d, s ) );
		}

		void setIntensity( castor::Point2f const & value )
		{
			m_category->setIntensity( value );
		}

		void setDiffuseIntensity( float value )
		{
			m_category->setDiffuseIntensity( value );
		}

		void setSpecularIntensity( float value )
		{
			m_category->setSpecularIntensity( value );
		}

		void setEnabled( bool value )
		{
			m_enabled = value;
		}

		void enable()
		{
			setEnabled( true );
		}

		void disable()
		{
			setEnabled( false );
		}

		void setShadowProducer( bool value )
		{
			m_shadowCaster = value;
			onChanged( *this );
		}

		void setShadowMap( ShadowMapRPtr value, uint32_t index = 0u )
		{
			m_shadowMap = value;
			m_shadowMapIndex = index;
		}

		void setGlobalIlluminationType( GlobalIlluminationType value )
		{
			m_shadows.globalIllumination = value;
			onChanged( *this );
		}

		void setShadowType( ShadowType value )
		{
			m_shadows.filterType = value;
		}

		void setVolumetricSteps( uint32_t value )
		{
			m_shadows.volumetricSteps = value;
		}

		void setVolumetricScatteringFactor( float value )
		{
			m_shadows.volumetricScattering = value;
		}

		void setRawMinOffset( float value )
		{
			m_shadows.rawOffsets[0] = value;
		}

		void setRawMaxSlopeOffset( float value )
		{
			m_shadows.rawOffsets[1] = value;
		}

		void setPcfMinOffset( float value )
		{
			m_shadows.pcfOffsets[0] = value;
		}

		void setPcfMaxSlopeOffset( float value )
		{
			m_shadows.pcfOffsets[1] = value;
		}

		void setVsmMaxVariance( float value )
		{
			m_shadows.variance[0] = value;
		}

		void setVsmVarianceBias( float value )
		{
			m_shadows.variance[1] = value;
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
		ShadowConfig m_shadows;
		LightCategorySPtr m_category;
		ShadowMapRPtr m_shadowMap{ nullptr };
		uint32_t m_shadowMapIndex{ 0u };
		std::atomic< GlobalIlluminationType > m_currentGlobalIllumination{ GlobalIlluminationType::eNone };
		uint32_t m_bufferIndex{ 0u };
	};
}

#endif
