/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MatrixUbo_H___
#define ___C3D_MatrixUbo_H___

#include "UbosModule.hpp"

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
			C3D_API MatrixData & operator=( MatrixData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec4 viewToProj( sdw::Vec4 const & vsPosition )const;
			C3D_API sdw::Vec4 worldToCurView( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec4 worldToPrvView( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec4 worldToCurProj( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec4 worldToPrvProj( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec3 getCurViewRight()const;
			C3D_API sdw::Vec3 getPrvViewRight()const;
			C3D_API sdw::Vec3 getCurViewUp()const;
			C3D_API sdw::Vec3 getPrvViewUp()const;
			C3D_API sdw::Vec3 getCurViewCenter()const;
			C3D_API sdw::Vec3 getPrvViewCenter()const;
			C3D_API void jitter( sdw::Vec4 & csPosition )const;

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
		};
	}

	class MatrixUbo
	{
	public:
		using Configuration = MatrixUboConfiguration;
		C3D_API explicit MatrixUbo( RenderDevice const & device );
		C3D_API ~MatrixUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	view		The new view matrix.
		 *\param[in]	projection	The new projection matrix.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	view		La nouvelle matrice de vue.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API void cpuUpdate( castor::Matrix4x4f const & view
			, castor::Matrix4x4f const & projection
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
			return m_ubo.createPassBinding( pass, binding );
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

	public:
		C3D_API static castor::String const BufferMatrix;
		C3D_API static castor::String const MatrixData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_MATRIX( writer, binding, set )\
	sdw::Ubo matrices{ writer\
		, castor3d::MatrixUbo::BufferMatrix\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_matrixData = matrices.declStructMember< shader::MatrixData >( castor3d::MatrixUbo::MatrixData );\
	matrices.end()

#endif
