/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapUbo_H___
#define ___C3D_ShadowMapUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct ShadowMapData
			: public sdw::StructInstance
		{
			C3D_API ShadowMapData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API ShadowMapData & operator=( ShadowMapData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec4 worldToView( sdw::Vec4 const & pos )const;
			C3D_API sdw::Vec4 viewToProj( sdw::Vec4 const & pos )const;
			C3D_API sdw::Float getLinearisedDepth( sdw::Vec3 const & pos )const;
			C3D_API DirectionalLight getDirectionalLight( LightingModel const & lighting )const;
			C3D_API PointLight getPointLight( LightingModel const & lighting )const;
			C3D_API SpotLight getSpotLight( LightingModel const & lighting )const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Mat4 m_lightProjection;
			sdw::Mat4 m_lightView;
			sdw::Vec4 m_lightPosFarPlane;
			sdw::UInt m_lightIndex;
		};
	}

	class ShadowMapUbo
	{
	public:
		using Configuration = ShadowMapUboConfiguration;

	public:
		C3D_API explicit ShadowMapUbo( RenderDevice const & device );
		C3D_API ~ShadowMapUbo();
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
		C3D_API static castor::String const BufferShadowMap;
		C3D_API static castor::String const ShadowMapData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_SHADOWMAP( writer, binding, set )\
	sdw::Ubo shadowMapCfg{ writer\
		, castor3d::ShadowMapUbo::BufferShadowMap\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_shadowMapData = shadowMapCfg.declStructMember< castor3d::shader::ShadowMapData >( castor3d::ShadowMapUbo::ShadowMapData );\
	shadowMapCfg.end()

#endif
