/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapUbo_H___
#define ___C3D_ShadowMapUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

namespace castor3d
{
	class ShadowMapUbo
	{
	public:
		using Configuration = ShadowMapUboConfiguration;

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API ShadowMapUbo( ShadowMapUbo const & ) = delete;
		C3D_API ShadowMapUbo & operator=( ShadowMapUbo const & ) = delete;
		C3D_API ShadowMapUbo( ShadowMapUbo && ) = default;
		C3D_API ShadowMapUbo & operator=( ShadowMapUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit ShadowMapUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ShadowMapUbo();
		/**
		 *\~english
		 *\brief		Initialises the UBO.
		 *\~french
		 *\brief		Initialise l'UBO.
		 */
		C3D_API void initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	light	The light source from which the shadow map is generated.
		 *\param[in]	index	The shadow pass index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	light	La source lumineuse depuis laquelle la shadow map est générée.
		 *\param[in]	index	L'index de la passe d'ombres.
		 */
		C3D_API void update( Light const & light
			, uint32_t index );

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		C3D_API static uint32_t const BindingPoint;
		C3D_API static castor::String const BufferShadowMap;
		C3D_API static castor::String const LightProjection;
		C3D_API static castor::String const LightView;
		C3D_API static castor::String const LightPosFarPlane;
		C3D_API static castor::String const LightIndex;

	private:
		Engine & m_engine;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_SHADOWMAP( writer, binding, set )\
	sdw::Ubo shadowMapCfg{ writer\
		, castor3d::ShadowMapUbo::BufferShadowMap\
		, binding\
		, set };\
	auto c3d_lightProjection = shadowMapCfg.declMember< sdw::Mat4 >( castor3d::ShadowMapUbo::LightProjection );\
	auto c3d_lightView = shadowMapCfg.declMember< sdw::Mat4 >( castor3d::ShadowMapUbo::LightView ); \
	auto c3d_lightPosFarPlane = shadowMapCfg.declMember< sdw::Vec4 >( castor3d::ShadowMapUbo::LightPosFarPlane );\
	auto c3d_lightIndex = shadowMapCfg.declMember< sdw::UInt >( castor3d::ShadowMapUbo::LightIndex );\
	shadowMapCfg.end()

#endif
