/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapUbo_H___
#define ___C3D_ShadowMapUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct ShadowMapData
			: public sdw::StructInstanceHelperT< "C3D_ShadowMapData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Mat4x4Field< "lightProjection" >
				, sdw::Mat4x4Field< "lightView" >
				, sdw::Vec4Field< "lightPosFarPlane" >
				, sdw::UIntField< "lightOffset" > >
		{
			C3D_API ShadowMapData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{  writer, castor::move( expr ), enabled }
			{
			}

			C3D_API sdw::Vec4 worldToView( sdw::Vec4 const & pos )const;
			C3D_API sdw::Vec4 viewToProj( sdw::Vec4 const & pos )const;
			C3D_API sdw::Float getNormalisedDepth( sdw::Vec3 const & pos )const;
			C3D_API DirectionalLight getDirectionalLight( Lights & lights )const;
			C3D_API PointLight getPointLight( Lights & lights )const;
			C3D_API SpotLight getSpotLight( Lights & lights )const;

		private:
			auto lightProjection()const { return getMember< "lightProjection" >(); }
			auto lightView()const { return getMember< "lightView" >(); }
			auto lightPosFarPlane()const { return getMember< "lightPosFarPlane" >(); }
			auto lightOffset()const { return getMember< "lightOffset" >(); }
		};
	}

	class ShadowMapUbo
	{
	public:
		using Configuration = ShadowMapUboConfiguration;

	public:
		C3D_API explicit ShadowMapUbo( RenderDevice const & device );
		C3D_API ShadowMapUbo( ShadowMapUbo const & rhs ) = delete;
		C3D_API ShadowMapUbo & operator=( ShadowMapUbo const & rhs ) = delete;
		C3D_API ShadowMapUbo( ShadowMapUbo && rhs )noexcept = default;
		C3D_API ShadowMapUbo & operator=( ShadowMapUbo && rhs )noexcept = delete;
		C3D_API ~ShadowMapUbo()noexcept;
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

		ashes::WriteDescriptorSet getDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return m_ubo.getDescriptorWrite( dstBinding, dstArrayElement );
		}

		void addDescriptorWrite( ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			descriptorWrites.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
			++dstBinding;
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_ShadowMap( writer, binding, set )\
	sdw::UniformBuffer shadowMapCfg{ writer\
		, "C3D_ShadowMapConfig"\
		, "c3d_shadowMapConfig"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_shadowMapData = shadowMapCfg.declMember< castor3d::shader::ShadowMapData >( "d" );\
	shadowMapCfg.end()

#endif
