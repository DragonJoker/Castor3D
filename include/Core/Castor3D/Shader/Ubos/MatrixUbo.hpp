/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MatrixUbo_H___
#define ___C3D_MatrixUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct MatrixData
			: public sdw::StructInstance
		{
			friend struct BillboardData;

			C3D_API MatrixData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, MatrixData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec4 projToView( sdw::Vec4 const & psPosition )const;
			C3D_API sdw::Vec4 viewToProj( sdw::Vec4 const & vsPosition )const;
			C3D_API sdw::Vec4 worldToCurView( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec4 worldToPrvView( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec4 curViewToWorld( sdw::Vec4 const & vsPosition )const;
			C3D_API sdw::Vec4 prvViewToWorld( sdw::Vec4 const & vsPosition )const;
			C3D_API sdw::Vec4 worldToCurProj( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec4 worldToPrvProj( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec2 viewToScreenUV( Utils & utils
				, sdw::Vec4 vsPosition )const;
			C3D_API sdw::Vec2 worldToCurScreenUV( Utils & utils
				, sdw::Vec4 wsPosition )const;
			C3D_API sdw::Vec2 worldToPrvScreenUV( Utils & utils
				, sdw::Vec4 wsPosition )const;
			C3D_API sdw::Vec3 projToView( Utils & utils
				, sdw::Vec2 const & texCoord
				, sdw::Float const & depth )const;
			C3D_API sdw::Vec4 curProjToWorld( sdw::Vec4 const & position )const;
			C3D_API sdw::Vec3 curProjToWorld( Utils & utils
				, sdw::Vec2 const & texCoord
				, sdw::Float const & depth )const;
			C3D_API sdw::Vec3 prvProjToWorld( Utils & utils
				, sdw::Vec2 const & texCoord
				, sdw::Float const & depth )const;
			C3D_API sdw::Vec3 getCurViewRight()const;
			C3D_API sdw::Vec3 getPrvViewRight()const;
			C3D_API sdw::Vec3 getCurViewUp()const;
			C3D_API sdw::Vec3 getPrvViewUp()const;
			C3D_API sdw::Vec3 getCurViewCenter()const;
			C3D_API sdw::Vec3 getPrvViewCenter()const;
			C3D_API void jitter( sdw::Vec4 & csPosition )const;
			C3D_API sdw::Mat4 getInvViewProjMtx()const;

			sdw::Mat4 const & getInvProjMtx()const
			{
				return m_invProjection;
			}

			sdw::Mat4 const & getProjMtx()const
			{
				return m_projection;
			}

			sdw::Vec4 getFrustumPlane( sdw::UInt index )const
			{
				return m_frustumPlanes[index];
			}

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Mat4 m_projection;
			sdw::Mat4 m_invProjection;
			sdw::Mat4 m_curView;
			sdw::Mat4 m_prvView;
			sdw::Mat4 m_curViewProj;
			sdw::Mat4 m_prvViewProj;
			sdw::Vec4 m_jitter;
			sdw::Array< sdw::Vec4 > m_frustumPlanes;
		};
	}

	class MatrixUbo
	{
	public:
		using Configuration = MatrixUboConfiguration;
		C3D_API MatrixUbo( MatrixUbo const & rhs ) = delete;
		C3D_API MatrixUbo & operator=( MatrixUbo const & rhs ) = delete;
		C3D_API MatrixUbo( MatrixUbo && rhs ) = default;
		C3D_API MatrixUbo & operator=( MatrixUbo && rhs ) = delete;
		C3D_API explicit MatrixUbo( RenderDevice const & device );
		C3D_API ~MatrixUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	view		The new view matrix.
		 *\param[in]	projection	The new projection matrix.
		 *\param[in]	frustum		The frustum, from which planes are copied.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	view		La nouvelle matrice de vue.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 *\param[in]	frustum		Le frustum depuis lequel les plans sont copiés.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API void cpuUpdate( castor::Matrix4x4f const & view
			, castor::Matrix4x4f const & projection
			, Frustum const & frustum
			, castor::Point2f const & jitter = castor::Point2f{} );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\remarks		View matrix won't be updated.
		 *\param[in]	projection	The new projection matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\remarks		La matrice de vue ne sera pas mise à jour.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 */
		C3D_API void cpuUpdate( castor::Matrix4x4f const & projection );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, "Matrix", binding );
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

#define C3D_Matrix( writer, binding, set )\
	sdw::UniformBuffer matrices{ writer\
		, "C3D_Matrices"\
		, "c3d_matrices"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, sdw::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_matrixData = matrices.declMember< castor3d::shader::MatrixData >( "d" );\
	matrices.end()

#endif
