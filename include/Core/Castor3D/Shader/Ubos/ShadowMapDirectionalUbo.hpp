/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapDirectionalUbo_H___
#define ___C3D_ShadowMapDirectionalUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/CompositeTypes/Builtins.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct ShadowMapDirectionalData
			: public sdw::StructInstance
		{
			C3D_API ShadowMapDirectionalData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, ShadowMapDirectionalData );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::UInt getTileIndex( ModelInstancesData const & modelInstances
				, sdw::InVertex const & in )const;
			C3D_API sdw::Vec2 getTileMin( sdw::UInt const & tileIndex )const;
			C3D_API sdw::Vec2 getTileMax( sdw::Vec2 const & tileMin )const;
			C3D_API sdw::Vec4 worldToView( sdw::UInt const & tileIndex
				, sdw::Vec4 const & pos )const;
			C3D_API sdw::Vec4 viewToProj( sdw::UInt const & tileIndex
				, sdw::Vec4 const & pos )const;
			C3D_API TiledDirectionalLight getDirectionalLight( LightingModel const & lighting )const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Array< sdw::Mat4 > m_lightProjections;
			sdw::Array< sdw::Mat4 > m_lightViews;
			sdw::Vec4 m_lightTiles;
			sdw::UInt m_lightIndex;
		};
	}

	class ShadowMapDirectionalUbo
	{
	public:
		using Configuration = ShadowMapDirectionalUboConfiguration;

	public:
		C3D_API explicit ShadowMapDirectionalUbo( RenderDevice const & device );
		C3D_API ShadowMapDirectionalUbo( ShadowMapDirectionalUbo const & rhs ) = delete;
		C3D_API ShadowMapDirectionalUbo & operator=( ShadowMapDirectionalUbo const & rhs ) = delete;
		C3D_API ShadowMapDirectionalUbo( ShadowMapDirectionalUbo && rhs ) = default;
		C3D_API ShadowMapDirectionalUbo & operator=( ShadowMapDirectionalUbo && rhs ) = delete;
		C3D_API ~ShadowMapDirectionalUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	light	The light source from which the shadow map is generated.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	light	La source lumineuse depuis laquelle la shadow map est générée.
		 */
		C3D_API void update( DirectionalLight const & light );

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		ashes::WriteDescriptorSet getDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return m_ubo.getDescriptorWrite( dstBinding, dstArrayElement );
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		C3D_API static castor::String const BufferShadowMapDirectional;
		C3D_API static castor::String const ShadowMapDirectionalData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_SHADOWMAP_DIRECTIONAL( writer, binding, set )\
	sdw::Ubo shadowMapDirectionalCfg{ writer\
		, castor3d::ShadowMapDirectionalUbo::BufferShadowMapDirectional\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_shadowMapDirectionalData = shadowMapDirectionalCfg.declStructMember< castor3d::shader::ShadowMapDirectionalData >( castor3d::ShadowMapDirectionalUbo::ShadowMapDirectionalData );\
	shadowMapDirectionalCfg.end()

#endif
