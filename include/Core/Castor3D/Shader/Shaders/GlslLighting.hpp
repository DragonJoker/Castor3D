/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLighting_H___
#define ___C3D_GlslLighting_H___

#include "SdwModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/CompositeTypes/StorageBuffer.hpp>
#include <ShaderWriter/CompositeTypes/Struct.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

#include <unordered_set>

namespace castor3d::shader
{
	static const castor::String TOO_MANY_OBJECTS = cuT( "Too many lighting models declared" );
	static const castor::String DUPLICATE_OBJECT = cuT( "Lighting model already declared" );

	class LightingModelFactory
	{
	public:
		using Obj = LightingModel;
		using Key = castor::String;
		using PtrType = LightingModelUPtr;
		using Creator = LightingModelCreator;
		using ObjPtr = PtrType;
		struct Entry
		{
			Key name;
			Creator create;
			LightingModelID lightingModelId;
			BackgroundModelID backgroundModelId;
		};
		using ObjCont = std::vector< Entry >;

	public:
		/**
		 *\~english
		 *\brief		Registers an object type
		 *\param[in]	key		The object type
		 *\param[in]	creator	The object creation function
		 *\~french
		 *\brief		Enregistre un type d'objet
		 *\param[in]	key		Le type d'objet
		 *\param[in]	creator	La fonction de création d'objet
		 */
		LightingModelID registerType( Key const & key
			, BackgroundModelID backgroundModelId
			, Creator create )
		{
			LightingModelID lightingModelId{};
			auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [&key]( Entry const & lookup )
				{
					return key == lookup.name;
				} );

			if ( it == m_registered.end() )
			{
				if ( m_currentId > MaxLightingModels )
				{
					CU_Exception( TOO_MANY_OBJECTS );
				}

				lightingModelId = ++m_currentId;
			}
			else
			{
				lightingModelId = it->lightingModelId;
			}

			auto & entry = m_registered.emplace_back();
			entry.name = key;
			entry.create = create;
			entry.lightingModelId = lightingModelId;
			entry.backgroundModelId = backgroundModelId;
			return lightingModelId;
		}
		/**
		 *\~english
		 *\brief		Unregisters an object type.
		 *\param[in]	key					The object type.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\~french
		 *\brief		Désenregistre un type d'objet.
		 *\param[in]	key					Le type d'objet.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 */
		void unregisterType( Key const & key
			, BackgroundModelID backgroundModelId )
		{
			auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [&key, backgroundModelId]( Entry const & lookup )
				{
					return key == lookup.name
						&& backgroundModelId == lookup.backgroundModelId;
				} );

			if ( it != m_registered.end() )
			{
				m_registered.erase( it );
			}
		}
		/**
		 *\~english
		 *\brief		Unregisters an object type.
		 *\param[in]	lightingModelId		The lighting model ID.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\~french
		 *\brief		Désenregistre un type d'objet.
		 *\param[in]	lightingModelId		L'ID du modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 */
		void unregisterType( LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId )
		{
			auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [lightingModelId, backgroundModelId]( Entry const & lookup )
				{
					return lightingModelId == lookup.lightingModelId
					&& backgroundModelId == lookup.backgroundModelId;
				} );

			if ( it != m_registered.end() )
			{
				m_registered.erase( it );
			}
		}
		/**
		 *\~english
		 *\return		The unique lighting models IDs.
		 *\~french
		 *\return		Les ID uniques des modèles d'éclairage.
		 */
		std::vector< LightingModelID > getLightingModelsID()const
		{
			std::unordered_set< LightingModelID > result;

			for ( auto & entry : m_registered )
			{
				result.emplace( entry.lightingModelId );
			}

			return { result.begin(), result.end() };
		}
		/**
		 *\~english
		 *\param[in]	backgroundModelId	The background model ID.
		 *\return		The lighting models for given background model.
		 *\~french
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 *\return		Les modèles d'éclairage du mod_le de fond donné.
		 */
		std::vector< LightingModelID > getLightingModelsID( BackgroundModelID backgroundModelId )const
		{
			std::vector< LightingModelID > result;

			for ( auto & entry : m_registered )
			{
				if ( entry.backgroundModelId == backgroundModelId )
				{
					result.push_back( entry.lightingModelId );
				}
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Creates an object from an ID.
		 *\param[in]	id		The object ID.
		 *\param[in]	params	The creation parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet à partir d'un ID.
		 *\param[in]	id		L'ID d'objet.
		 *\param[in]	params	Les paramètres de création.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		ObjPtr create( LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId
			, Parameters && ... params )const
		{
			if ( lightingModelId == 0u )
			{
				return nullptr;
			}

			auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [lightingModelId, backgroundModelId]( Entry const & lookup )
				{
					return lightingModelId == lookup.lightingModelId
						&& backgroundModelId == lookup.backgroundModelId;
				} );

			if ( it == m_registered.end() )
			{
				CU_Exception( castor::ERROR_UNKNOWN_OBJECT );
			}

			return it->create( lightingModelId
				, std::forward< Parameters >( params )... );
		}

	private:
		LightingModelID m_currentId{};
		ObjCont m_registered;
	};

	class LightingModel
	{
	public:
		C3D_API LightingModel( LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, Shadow & shadowModel
			, Lights & lights
			, bool hasIblSupport
			, bool enableVolumetric
			, std::string prefix );
		C3D_API virtual ~LightingModel() = default;

		inline LightingModelID getLightingModelId()const
		{
			return m_lightingModelId;
		}

		inline bool hasIblSupport()const
		{
			return m_hasIblSupport;
		}

		C3D_API sdw::Vec3 combine( BlendComponents const & components
			, sdw::Vec3 const & incident
			, sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & directScattering
			, sdw::Vec3 const & directCoatingSpecular
			, sdw::Vec2 const & directSheen
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & directAmbient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 reflectedDiffuse
			, sdw::Vec3 reflectedSpecular
			, sdw::Vec3 refracted
			, sdw::Vec3 coatReflected
			, sdw::Vec3 sheenReflected );
		C3D_API sdw::Vec3 combine( BlendComponents const & components
			, sdw::Vec3 const & incident
			, sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & directScattering
			, sdw::Vec3 const & directCoatingSpecular
			, sdw::Vec2 const & directSheen
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & directAmbient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Vec3 reflectedDiffuse
			, sdw::Vec3 reflectedSpecular
			, sdw::Vec3 refracted
			, sdw::Vec3 coatReflected
			, sdw::Vec3 sheenReflected );

		C3D_API virtual sdw::Float getFinalTransmission( BlendComponents const & components
			, sdw::Vec3 const & incident ) = 0;
		C3D_API virtual sdw::Vec3 adjustDirectAmbient( BlendComponents const & components
			, sdw::Vec3 const & directAmbient )const = 0;
		C3D_API virtual sdw::Vec3 adjustDirectSpecular( BlendComponents const & components
			, sdw::Vec3 const & directSpecular )const = 0;
		/**
		*\name
		*	Deferred renderring
		*/
		//\{
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		C3D_API void compute( DirectionalLight const & light
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void compute( PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void compute( SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		C3D_API sdw::Vec3 computeDiffuse( DirectionalLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		C3D_API sdw::Vec3 computeDiffuse( PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		C3D_API sdw::Vec3 computeDiffuse( SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		//\}
		//\}

	protected:
		C3D_API void doComputeAttenuation( sdw::Float const attenuation
			, OutputComponents & output );
		C3D_API void doApplyShadows( DirectionalLight const & light
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void doApplyShadows( PointLight const & light
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void doApplyShadows( SpotLight const & light
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void doApplyShadowsDiffuse( DirectionalLight const & light
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, sdw::Vec3 & output );
		C3D_API void doApplyShadowsDiffuse( PointLight const & light
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, sdw::Vec3 & output );
		C3D_API void doApplyShadowsDiffuse( SpotLight const & light
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, sdw::Vec3 & output );

		C3D_API virtual void doInitialiseBackground( BackgroundModel & background );
		C3D_API virtual sdw::Vec3 doComputeRadiance( Light const & light
			, sdw::Vec3 const & lightDirection )const;
		C3D_API virtual void doComputeSheenTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec2 & output );
		C3D_API virtual void doComputeScatteringTerm( sdw::Vec3 const & radiance
			, Light const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 & output );
		C3D_API virtual sdw::Float doGetNdotL( LightSurface const & lightSurface
			, BlendComponents const & components );
		C3D_API virtual sdw::Float doGetNdotH( LightSurface const & lightSurface
			, BlendComponents const & components );

		C3D_API virtual sdw::Vec3 doComputeDiffuseTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float & isLit
			, sdw::Vec3 & output ) = 0;
		C3D_API virtual void doComputeSpecularTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec3 & output ) = 0;
		C3D_API virtual void doComputeCoatingTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec3 & output ) = 0;

	private:
		C3D_API virtual sdw::Vec3 doComputeLight( Light light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 & radiance
			, OutputComponents & output );
		C3D_API virtual sdw::Vec3 doComputeLightDiffuse( Light light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 & radiance );

		C3D_API virtual sdw::Vec3 doCombine( BlendComponents const & components
			, sdw::Vec3 const & incident
			, sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & directScattering
			, sdw::Vec3 const & directCoatingSpecular
			, sdw::Vec2 const & directSheen
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & directAmbient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 const & reflectedDiffuse
			, sdw::Vec3 const & reflectedSpecular
			, sdw::Vec3 const & refracted
			, sdw::Vec3 const & coatReflected
			, sdw::Vec3 const & sheenReflected ) = 0;

	protected:
		LightingModelID m_lightingModelId;
		sdw::ShaderWriter & m_writer;
		Materials const & m_materials;
		Utils & m_utils;
		Shadow & m_shadowModel;
		Lights & m_lights;
		bool m_hasIblSupport;
		bool m_enableVolumetric;
		std::string m_prefix;
		std::unique_ptr< sdw::Struct > m_type;
		std::unique_ptr< sdw::UInt > m_directionalCascadeIndex;
		std::unique_ptr< sdw::UInt > m_directionalCascadeCount;
		std::unique_ptr< sdw::Mat4 > m_directionalTransform;
		sdw::Function< sdw::Void
			, PDirectionalLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, OutputComponents & > m_computeDirectional;
		sdw::Function< sdw::Void
			, PPointLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, OutputComponents & > m_computePoint;
		sdw::Function< sdw::Void
			, PSpotLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, OutputComponents & > m_computeSpot;
		sdw::Function< sdw::Vec3
			, InOutDirectionalLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt > m_computeDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, InOutPointLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt > m_computePointDiffuse;
		sdw::Function< sdw::Vec3
			, InOutSpotLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt > m_computeSpotDiffuse;
	};
}

#endif
