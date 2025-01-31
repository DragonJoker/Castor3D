/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_CATEGORY_H___
#define ___C3D_LIGHT_CATEGORY_H___

#include "LightModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMapsModule.hpp"
#include "Castor3D/Scene/Shadow.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Graphics/BoundingBox.hpp>

namespace castor3d
{
	class LightCategory
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	lightType		The light category type.
		 *\param[in]	dirty			Used to tell the owner some changes have occured.
		 *\param[in]	changedCallback	Callback to call when changes have occured.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	lightType		Le type de catégorie de lumière.
		 *\param[in]	dirty			Utilisé pour dire au parent que des changements ont eu lieu.
		 *\param[in]	changedCallback	Callback à appeler lorsque des changements ont eu lieu.
		 */
		C3D_API explicit LightCategory( LightType lightType
			, bool & dirty
			, castor::Function< void() > changedCallback );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~LightCategory()noexcept = default;
		/**
		 *\~english
		 *\brief		Creates an instance of this light category.
		 *\param[in]	node	The parent node.
		 *\~french
		 *\brief		Crée une instance de cette catégorie de lumière.
		 *\param[in]	node	Le scene node parent.
		 */
		C3D_API virtual LightInstanceUPtr instantiate( SceneNode & node
			, castor::Function< void() > onGpuChanged ) = 0;
		/**
		 *\~english
		 *\brief		Updates the light.
		 *\~french
		 *\brief		Met la source à jour.
		 */
		C3D_API void update();
		/**
		*\~english
		*\brief
		*	ConfigurationVisitorBase acceptance function.
		*\param vis
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de ConfigurationVisitorBase.
		*\param vis
		*	Le ... visiteur.
		*/
		C3D_API void accept( ConfigurationVisitorBase & vis );
		/**
		 *\~english
		 *\brief			Clones this object into the given one.
		 *\param[in,out]	output	Receives this object's data.
		 *\~french
		 *\brief			Clone cet objet dans celui donné.
		 *\param[in,out]	output	Reçoit les données de cet objet.
		 */
		C3D_API void cloneInto( LightCategory & output )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		bool isShadowProducer()const
		{
			return m_currentShadowCaster;
		}

		bool isExpectedShadowProducer()const
		{
			return m_shadows.enabled;
		}

		ShadowType getShadowType()const
		{
			return m_shadows.filterType;
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

		float getVsmMinVariance()const
		{
			return m_shadows.vsmMinVariance;
		}

		float getVsmLightBleedingReduction()const
		{
			return m_shadows.vsmLightBleedingReduction;
		}

		castor::RangedValue< uint32_t > getShadowPcfFilterSize()const
		{
			return m_shadows.pcfFilterSize;
		}

		castor::RangedValue< uint32_t > getShadowPcfSampleCount()const
		{
			return m_shadows.pcfSampleCount;
		}

		ShadowConfig const & getShadowConfig()const
		{
			return m_shadows;
		}

		ShadowConfig & getShadowConfig()
		{
			return m_shadows;
		}

		LightType getLightType()const
		{
			return m_lightType;
		}

		float getFarPlane()const
		{
			return m_farPlane;
		}

		castor::Point3f const & getColour()const
		{
			return m_colour;
		}

		castor::BoundingBox const & getBoundingBox()const
		{
			return m_cubeBox;
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
		void setColour( castor::Point3f const & value )
		{
			m_colour = value;
		}

		void setShadowConfig( ShadowConfig config )
		{
			m_shadows = castor::move( config );
		}

		void setGlobalIlluminationType( GlobalIlluminationType value )
		{
			m_shadows.globalIllumination = value;
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

		void setPcfFilterSize( uint32_t value )
		{
			m_shadows.pcfFilterSize = value;
		}

		void setPcfSampleCount( uint32_t value )
		{
			m_shadows.pcfSampleCount = value;
		}

		void setVsmMinVariance( float value )
		{
			m_shadows.vsmMinVariance = value;
		}

		void setVsmLightBleedingReduction( float value )
		{
			m_shadows.vsmLightBleedingReduction = value;
		}
		/**@}*/

	protected:
		bool & m_dirty;
		castor::Function< void() > m_changedCallback;

	private:
		LightType m_lightType;
		std::atomic_bool m_currentShadowCaster{};
		std::atomic< GlobalIlluminationType > m_currentGlobalIllumination{};
		castor::GroupChangeTracked< castor::Point3f > m_colour;
		ShadowConfig m_shadows;
		/**
		 *\~english
		 *\brief		Updates the light.
		 *\~french
		 *\brief		Met la source à jour.
		 */
		virtual void doUpdate() = 0;
		/**
		*\~english
		*\brief
		*	ConfigurationVisitorBase acceptance function.
		*\param vis
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de ConfigurationVisitorBase.
		*\param vis
		*	Le ... visiteur.
		*/
		virtual void doAccept( ConfigurationVisitorBase & vis ) = 0;
		/**
		 *\~english
		 *\brief			Clones this object into the given one.
		 *\param[in,out]	output	Receives this object's data.
		 *\~french
		 *\brief			Clone cet objet dans celui donné.
		 *\param[in,out]	output	Reçoit les données de cet objet.
		 */
		virtual void doCloneInto( LightCategory & output )const = 0;

	protected:
		//!\~english	The cube box for the light volume of effect.
		//!\~french		La cube box pour le volume d'effet de la lumière.
		castor::BoundingBox m_cubeBox;
		//!\~english	The far plane's depth.
		//!\~french		La profondeur du plan éloigné.
		float m_farPlane{ 1.0f };
	};

	class LightInstance
	{
	public:
		struct LightData
			: ShaderBufferTypes
		{
			Float3 colour;
			Float1 range;
			Float1 intensity;
			Float1 shadowMapIndex;
			Float1 cascadeCount;
			Float1 pad;
			Float3 posDir;
			Float1 enabled;
		};

		static uint32_t constexpr LightMbrAlign = 4u * sizeof( float );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~LightInstance()noexcept = default;
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
		 *\brief			Updates the shadow informations.
		 *\param[in,out]	lightCamera	The camera that receives the light spot data.
		 *\param[in]		index		The shadow map index.
		 *\~french
		 *\brief			Met à jour les information d'ombre.
		 *\param[in,out]	lightCamera	La caméra qui reçoit les données de spot de la lumière.
		 *\param[in]		index		L'indice de la shadow map.
		 */
		C3D_API bool updateShadow( Camera const & viewCamera
			, Camera * lightCamera
			, int32_t index );
		/**
		 *\~english
		 *\brief			Clones this object into the given one.
		 *\param[in,out]	output	Receives this object's data.
		 *\~french
		 *\brief			Clone cet objet dans celui donné.
		 *\param[in,out]	output	Reçoit les données de cet objet.
		 */
		C3D_API void cloneInto( LightInstance & output )const;
		/**
		 *\~english
		 *\brief		Puts the light into the given buffer.
		 *\param[in]	index	The light index in the buffer.
		 *\param[in]	offset	The light data offset in the buffer.
		 *\param[out]	data	Receives the light's data.
		 *\~french
		 *\brief		Met la lumière dans le buffer donné.
		 *\param[in]	index	L'index de la source lumineuse dans le buffer.
		 *\param[in]	offset	L'offset des données de la source lumineuse dans le buffer.
		 *\param[out]	data	Reçoit les données de la source lumineuse.
		 */
		C3D_API void fillLightBuffer( bool enabled
			, uint32_t index
			, VkDeviceSize offset
			, castor::Point4f * data );
		/**
		 *\~english
		 *\brief		Puts the shadow data into the given buffer.
		 *\param[out]	data	Receives the light's shadow data.
		 *\~french
		 *\brief		Met les données d'ombre dans le buffer donné.
		 *\param[out]	data	Reçoit les données d'ombres de la source lumineuse.
		 */
		C3D_API virtual void fillShadowBuffer( AllShadowData & data )const = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API castor::String const & getName()const noexcept;
		C3D_API Scene * getScene()const noexcept;

		SceneNode & getNode()const
		{
			return *m_node;
		}

		LightCategory const & getCategory()const
		{
			return m_category;
		}

		LpvConfig const & getLpvConfig()const
		{
			return m_category.getLpvConfig();
		}

		RsmConfig const & getRsmConfig()const
		{
			return m_category.getRsmConfig();
		}

		LightType getLightType()const
		{
			return m_category.getLightType();
		}

		bool isShadowProducer()const
		{
			return m_category.isShadowProducer();
		}

		GlobalIlluminationType getGlobalIlluminationType()const
		{
			return m_category.getGlobalIlluminationType();
		}

		GlobalIlluminationType getExpectedGlobalIlluminationType()const
		{
			return m_category.getExpectedGlobalIlluminationType();
		}

		ShadowType getShadowType()const
		{
			return m_category.getShadowType();
		}

		bool needsRsmShadowMaps()const
		{
			return m_category.needsRsmShadowMaps();
		}

		float getFarPlane()const
		{
			return m_category.getFarPlane();
		}

		ShadowMapRPtr getShadowMap()const
		{
			return m_shadowMap;
		}

		int32_t getShadowMapIndex()const
		{
			return m_shadowMapIndex;
		}

		uint32_t getBufferIndex()const
		{
			return m_bufferIndex;
		}

		VkDeviceSize getBufferOffset()const
		{
			return m_bufferOffset;
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
		void setShadowMapIndex( int32_t index )
		{
			if ( m_shadowMapIndex != index )
			{
				m_shadowMapIndex = index;
				m_onGpuChanged();
			}
		}

		void setShadowMap( ShadowMapRPtr value
			, int32_t index = -1 )
		{
			if ( m_shadowMap != value
				|| m_shadowMapIndex != index )
			{
				m_shadowMap = value;
				m_shadowMapIndex = index;
				m_onGpuChanged();
			}
		}

		void setNode( SceneNode & node )
		{
			m_node = &node;
			m_onGpuChanged();
		}
		/**@}*/

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	node			The parent node.
		 *\param[in]	dirty			Used to tell the owner some changes have occured.
		 *\param[in]	onGpuChanged	Callback when GPU data has changed.
		 *\param[in]	category		The light category.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	node			Le scene node parent.
		 *\param[in]	dirty			Utilisé pour dire au parent que des changements ont eu lieu.
		 *\param[in]	onGpuChanged	Callback appelé quand les données GPU ont changé.
		 *\param[in]	category		La catégorie de lumière.
		 */
		C3D_API explicit LightInstance( SceneNode & node
			, bool & dirty
			, castor::Function< void() > onGpuChanged
			, LightCategory & category );
		/**
		 *\~english
		 *\brief		Puts the shadow data into the given buffer.
		 *\param[out]	data	Receives the light's shadow data.
		 *\~french
		 *\brief		Met les données d'ombre dans le buffer donné.
		 *\param[out]	data	Reçoit les données d'ombres de la source lumineuse.
		 */
		C3D_API void doFillBaseShadowData( BaseShadowData & data )const;

	private:
		/**
		 *\~english
		 *\brief			Updates the CPU data.
		 *\~french
		 *\brief			Met à jour les données CPU.
		 */
		virtual void doUpdate() = 0;
		/**
		 *\~english
		 *\brief			Updates the shadow informations.
		 *\param[in,out]	lightCamera	The camera that receives the light spot data.
		 *\param[in]		index		The shadow map index.
		 *\~french
		 *\brief			Met à jour les information d'ombre.
		 *\param[in,out]	lightCamera	La caméra qui reçoit les données de spot de la lumière.
		 *\param[in]		index		L'indice de la shadow map.
		 */
		virtual bool doUpdateShadow( Camera const & viewCamera
			, Camera * lightCamera
			, int32_t index ) = 0;
		/**
		 *\~english
		 *\brief		Puts the light into the given buffer.
		 *\param[out]	data	Receives the light's data.
		 *\~french
		 *\brief		Met la lumière dans le buffer donné.
		 *\param[out]	data	Reçoit les données de la source lumineuse.
		 */
		virtual void doFillLightBuffer( castor::Point4f * data )const = 0;
		/**
		 *\~english
		 *\brief			Clones this object into the given one.
		 *\param[in,out]	output	Receives this object's data.
		 *\~french
		 *\brief			Clone cet objet dans celui donné.
		 *\param[in,out]	output	Reçoit les données de cet objet.
		 */
		virtual void doCloneInto( LightInstance & output )const = 0;

	protected:
		bool & m_dirty;
		SceneNode * m_node;
		LightCategory & m_category;
		castor::Function< void() > m_onGpuChanged;
		ShadowMapRPtr m_shadowMap{};
		int32_t m_shadowMapIndex{ -1 };
		uint32_t m_bufferIndex{ InvalidIndex };
		VkDeviceSize m_bufferOffset{};
	};
}

#endif
