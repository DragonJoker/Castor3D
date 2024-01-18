/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneUbo_H___
#define ___C3D_SceneUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct SceneData
			: public sdw::StructInstanceHelperT< "C3D_SceneData"
				, ast::type::MemoryLayout::eStd140
				, sdw::Vec3Field< "ambientLight" >
				, sdw::UInt32Field< "fogType" >
				, sdw::Vec3Field< "backgroundColour" >
				, sdw::FloatField< "fogDensity" > >
		{
			friend struct BillboardData;
			friend class Fog;
			friend class CommonFog;

			SceneData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			C3D_API sdw::Vec4 getBackgroundColour( Utils & utils
				, sdw::Float const gamma )const;
			C3D_API sdw::Vec4 getBackgroundColour( HdrConfigData const & hdrConfigData )const;
			C3D_API sdw::Vec4 computeAccumulation( Utils & utils
				, CameraData const & camera
				, sdw::Float const & depth
				, sdw::Vec3 const & colour
				, sdw::Float const & alpha
				, sdw::UInt const & accumulationOperator )const;

			auto ambientLight()const { return getMember< "ambientLight" >(); }
			auto backgroundColour()const { return getMember< "backgroundColour" >(); }
			auto fogType()const { return getMember< "fogType" >(); }
			auto fogDensity()const { return getMember< "fogDensity" >(); }
		};
	}

	class SceneUbo
	{
	private:
		using Configuration = SceneUboConfiguration;

	public:
		C3D_API SceneUbo( SceneUbo const & rhs ) = delete;
		C3D_API SceneUbo & operator=( SceneUbo const & rhs ) = delete;
		C3D_API SceneUbo( SceneUbo && rhs )noexcept = default;
		C3D_API SceneUbo & operator=( SceneUbo && rhs )noexcept = delete;
		C3D_API explicit SceneUbo( RenderDevice const & device );
		C3D_API ~SceneUbo()noexcept;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	fog		The fog configuration.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	fog		La configuration du brouillard.
		 */
		C3D_API Configuration & cpuUpdate( Fog const & fog );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	scene	The rendered scene.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	scene	La scène dessinée.
		 */
		C3D_API Configuration & cpuUpdate( Scene const & scene );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, "Scene", binding );
		}

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

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_Scene( writer, binding, set )\
	sdw::UniformBuffer sceneBuffer{ writer\
		, "C3D_Scene"\
		, "c3d_scene"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_sceneData = sceneBuffer.declMember< castor3d::shader::SceneData >( "d" );\
	sceneBuffer.end()

#endif
