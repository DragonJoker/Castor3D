/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CameraUbo_H___
#define ___C3D_CameraUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct CameraData
			: public sdw::StructInstanceHelperT< "C3D_CameraData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Vec4ArrayField< "frustumPlanes", 6u >
				, sdw::Mat4x4Field< "projection" >
				, sdw::Mat4x4Field< "invProjection" >
				, sdw::Mat4x4Field< "curView" >
				, sdw::Mat4x4Field< "invCurView" >
				, sdw::Mat4x4Field< "prvView" >
				, sdw::Mat4x4Field< "invPrvView" >
				, sdw::Mat4x4Field< "curViewProj" >
				, sdw::Mat4x4Field< "invCurViewProj" >
				, sdw::Mat4x4Field< "prvViewProj" >
				, sdw::Mat4x4Field< "invPrvViewProj" >
				, sdw::U32Vec2Field< "renderSize" >
				, sdw::Vec2Field< "jitter" >
				, sdw::Vec3Field< "position" >
				, sdw::FloatField< "gamma" >
				, sdw::UInt32Field< "debugIndex" >
				, sdw::FloatField< "pad" >
				, sdw::FloatField< "nearPlane" >
				, sdw::FloatField< "farPlane" > >
		{
			friend struct BillboardData;

			CameraData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
			{
			}

			C3D_API sdw::Vec4 projToView( sdw::Vec4 const & psPosition )const;
			C3D_API DerivVec4 projToView( DerivVec4 const & psPosition )const;
			C3D_API sdw::Vec4 viewToProj( sdw::Vec4 const & vsPosition )const;
			C3D_API sdw::Vec3 worldToCurView( sdw::Vec3 const & wsPosition )const;
			C3D_API sdw::Vec4 worldToCurView( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec4 worldToPrvView( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec4 curViewToWorld( sdw::Vec4 const & vsPosition )const;
			C3D_API DerivVec4 curViewToWorld( DerivVec4 const & vsPosition )const;
			C3D_API sdw::Vec4 prvViewToWorld( sdw::Vec4 const & vsPosition )const;
			C3D_API sdw::Vec4 worldToCurProj( sdw::Vec4 const & wsPosition )const;
			C3D_API sdw::Vec4 worldToPrvProj( sdw::Vec4 const & wsPosition )const;
			C3D_API DerivVec4 worldToPrvProj( DerivVec4 const & wsPosition )const;
			C3D_API sdw::Vec2 viewToScreenUV( Utils & utils
				, sdw::Vec4 vsPosition )const;
			C3D_API sdw::Vec2 worldToCurScreenUV( Utils & utils
				, sdw::Vec4 wsPosition )const;
			C3D_API sdw::Vec2 worldToPrvScreenUV( Utils & utils
				, sdw::Vec4 wsPosition )const;
			C3D_API sdw::Vec3 projToView( Utils & utils
				, sdw::Vec2 const & texCoord
				, sdw::Float const & depth )const;
			C3D_API sdw::Vec3 curViewToWorld( Utils & utils
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
			C3D_API sdw::Mat4 getInvViewProjMtx()const;
			C3D_API void jitter( sdw::Vec4 & csPosition )const;
			C3D_API void jitter( DerivVec4 & csPosition )const;
			C3D_API sdw::Vec3 transformCamera( sdw::Mat3 const & transform )const;
			C3D_API sdw::Vec3 getPosToCamera( sdw::Vec3 const & position )const;
			C3D_API sdw::Vec3 getCameraToPos( sdw::Vec3 const & position )const;
			C3D_API sdw::Vec2 depthPlanes()const;
			C3D_API sdw::Vec2 calcTexCoord( Utils & utils
				, sdw::Vec2 const & fragCoord )const;
			C3D_API sdw::Vec3 readNormal( sdw::Vec3 const & input )const;
			C3D_API sdw::Vec3 writeNormal( sdw::Vec3 const & input )const;

			auto renderSize()const { return getMember< "renderSize" >(); }
			auto nearPlane()const { return getMember< "nearPlane" >(); }
			auto farPlane()const { return getMember< "farPlane" >(); }
			auto position()const { return getMember< "position" >(); }
			auto gamma()const { return getMember< "gamma" >(); }
			auto debugIndex()const { return getMember< "debugIndex" >(); }

		private:
			auto projection()const { return getMember< "projection" >(); }
			auto invProjection()const { return getMember< "invProjection" >(); }
			auto curView()const { return getMember< "curView" >(); }
			auto invCurView()const { return getMember< "invCurView" >(); }
			auto prvView()const { return getMember< "prvView" >(); }
			auto invPrvView()const { return getMember< "invPrvView" >(); }
			auto curViewProj()const { return getMember< "curViewProj" >(); }
			auto invCurViewProj()const { return getMember< "invCurViewProj" >(); }
			auto prvViewProj()const { return getMember< "prvViewProj" >(); }
			auto invPrvViewProj()const { return getMember< "invPrvViewProj" >(); }
			auto jitter()const { return getMember< "jitter" >(); }
			auto frustumPlanes()const { return getMember< "frustumPlanes" >(); }

		public:
			sdw::Mat4x4 getInvProjMtx()const
			{
				return invProjection();
			}

			sdw::Mat4x4 getProjMtx()const
			{
				return projection();
			}

			sdw::Vec4 getFrustumPlane( sdw::UInt index )const
			{
				return frustumPlanes()[index];
			}
		};
	}

	class CameraUbo
	{
	public:
		using Configuration = CameraUboConfiguration;
		C3D_API CameraUbo( CameraUbo const & rhs ) = delete;
		C3D_API CameraUbo & operator=( CameraUbo const & rhs ) = delete;
		C3D_API CameraUbo( CameraUbo && rhs )noexcept = default;
		C3D_API CameraUbo & operator=( CameraUbo && rhs )noexcept = delete;
		C3D_API explicit CameraUbo( RenderDevice const & device );
		C3D_API ~CameraUbo()noexcept;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	size		The render size.
		 *\param[in]	camera		The camera holding the data.
		 *\param[in]	view		The new view matrix.
		 *\param[in]	projection	The new projection matrix.
		 *\param[in]	debugIndex	The currently selected debug data index.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	size		Les dimensions du rendu.
		 *\param[in]	camera		La caméra contenant les données.
		 *\param[in]	view		La nouvelle matrice de vue.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 *\param[in]	debugIndex	L'indice des données de debug actuellement sélectionnées.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API Configuration & cpuUpdate( castor::Size const & size
			, Camera const & camera
			, castor::Matrix4x4f const & view
			, castor::Matrix4x4f const & projection
			, uint32_t debugIndex
			, castor::Point2f const & jitter = castor::Point2f{} );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	camera		The camera holding the data.
		 *\param[in]	view		The new view matrix.
		 *\param[in]	projection	The new projection matrix.
		 *\param[in]	debugIndex	The currently selected debug data index.
		 *\param[in]	safeBanded	\p true to use safebanded size.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	camera		La caméra contenant les données.
		 *\param[in]	view		La nouvelle matrice de vue.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 *\param[in]	debugIndex	L'indice des données de debug actuellement sélectionnées.
		 *\param[in]	safeBanded	\p true pour utiliser la taille avec les safebands.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API Configuration & cpuUpdate( Camera const & camera
			, castor::Matrix4x4f const & view
			, castor::Matrix4x4f const & projection
			, uint32_t debugIndex
			, bool safeBanded
			, castor::Point2f const & jitter = castor::Point2f{} );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	camera		The camera holding the data.
		 *\param[in]	debugIndex	The currently selected debug data index.
		 *\param[in]	safeBanded	\p true to use safebanded size and projection matrix.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	camera		La caméra contenant les données.
		 *\param[in]	debugIndex	L'indice des données de debug actuellement sélectionnées.
		 *\param[in]	safeBanded	\p true pour utiliser la taille et la matrice de projection avec les safebands.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API Configuration & cpuUpdate( Camera const & camera
			, uint32_t debugIndex
			, bool safeBanded
			, castor::Point2f const & jitter = castor::Point2f{} );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	size		The render size.
		 *\param[in]	camera		The camera holding the data.
		 *\param[in]	debugIndex	The currently selected debug data index.
		 *\param[in]	safeBanded	\p true to use safebanded projection matrix.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	size		Les dimensions du rendu.
		 *\param[in]	camera		La caméra contenant les données.
		 *\param[in]	debugIndex	L'indice des données de debug actuellement sélectionnées.
		 *\param[in]	safeBanded	\p true pour utiliser la matrice de projection avec les safebands.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API Configuration & cpuUpdate( castor::Size const & size
			, Camera const & camera
			, uint32_t debugIndex
			, bool safeBanded
			, castor::Point2f const & jitter = castor::Point2f{} );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	size		The render size.
		 *\param[in]	view		The new view matrix.
		 *\param[in]	projection	The new projection matrix.
		 *\param[in]	debugIndex	The currently selected debug data index.
		 *\param[in]	frustum		The frustum, from which planes are copied.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	size		Les dimensions du rendu.
		 *\param[in]	view		La nouvelle matrice de vue.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 *\param[in]	debugIndex	L'indice des données de debug actuellement sélectionnées.
		 *\param[in]	frustum		Le frustum depuis lequel les plans sont copiés.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API Configuration & cpuUpdate( castor::Size const & size
			, castor::Matrix4x4f const & view
			, castor::Matrix4x4f const & projection
			, uint32_t debugIndex
			, Frustum const & frustum
			, castor::Point2f const & jitter = castor::Point2f{} );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\remarks		View matrix won't be updated.
		 *\param[in]	size		The render size.
		 *\param[in]	projection	The new projection matrix.
		 *\param[in]	debugIndex	The currently selected debug data index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\remarks		La matrice de vue ne sera pas mise à jour.
		 *\param[in]	size		Les dimensions du rendu.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 *\param[in]	debugIndex	L'indice des données de debug actuellement sélectionnées.
		 */
		C3D_API Configuration & cpuUpdate( castor::Size const & size
			, castor::Matrix4x4f const & projection
			, uint32_t debugIndex );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, "Camera", binding );
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

#define C3D_CameraEx( writer, binding, set, enabled )\
	sdw::UniformBuffer camera{ writer\
		, "C3D_Camera"\
		, "c3d_camera"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, sdw::type::MemoryLayout::eStd140\
		, enabled };\
	auto c3d_cameraData = camera.declMember< castor3d::shader::CameraData >( "c", enabled );\
	camera.end()

#define C3D_Camera( writer, binding, set )\
	C3D_CameraEx( writer, binding, set, true )

#endif
