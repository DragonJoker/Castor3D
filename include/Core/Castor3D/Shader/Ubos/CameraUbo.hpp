/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CameraUbo_H___
#define ___C3D_CameraUbo_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Shader/Ubos/Ubo.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace c3d
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
				, sdw::Vec3Field< "position" >
				, sdw::FloatField< "nearPlane" >
				, sdw::FloatField< "farPlane" >
				, sdw::FloatField< "pad0" >
				, sdw::FloatField< "pad1" >
				, sdw::FloatField< "pad2" > >
		{
			friend struct BillboardData;

			SDW_DeclStructInstance( C3D_API, CameraData );

			CameraData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
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
			C3D_API sdw::Vec2 viewToScreenUV( Utils & utils
				, sdw::Vec4 const & vsPosition )const;
			C3D_API sdw::Vec2 worldToCurScreenUV( Utils & utils
				, sdw::Vec4 const & wsPosition )const;
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
			C3D_API sdw::Vec3 transformCamera( sdw::Mat3 const & transform )const;
			C3D_API sdw::Vec3 getPosToCamera( sdw::Vec3 const & position )const;
			C3D_API sdw::Vec3 getCameraToPos( sdw::Vec3 const & position )const;
			C3D_API sdw::Vec2 depthPlanes()const;
			C3D_API sdw::Vec2 calcTexCoord( Utils & utils
				, sdw::Vec2 const & fragCoord )const;
			C3D_API sdw::Vec3 readNormal( sdw::Vec3 const & input )const;
			C3D_API sdw::Vec3 writeNormal( sdw::Vec3 const & input )const;

			auto nearPlane()const { return getMember< "nearPlane" >(); }
			auto farPlane()const { return getMember< "farPlane" >(); }
			auto position()const { return getMember< "position" >(); }

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

			sdw::Vec4 getFrustumPlane( sdw::UInt const & index )const
			{
				return frustumPlanes()[index];
			}
		};
	}

	class CameraUbo
		: public UboT< CameraUboConfiguration >
	{
	public:
		using Configuration = CameraUboConfiguration;

	public:
		C3D_API explicit CameraUbo( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	camera		The camera holding the data.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	camera		La caméra contenant les données.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API Configuration & cpuUpdate( Camera const & camera
			, Point2f const & jitter = Point2f{} );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	camera		The camera holding the data.
		 *\param[in]	view		The new view matrix.
		 *\param[in]	projection	The new projection matrix.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	camera		La caméra contenant les données.
		 *\param[in]	view		La nouvelle matrice de vue.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API Configuration & cpuUpdate( Camera const & camera
			, Matrix4x4f const & view
			, Matrix4x4f const & projection
			, Point2f const & jitter = Point2f{} );
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
		C3D_API Configuration & cpuUpdate( Matrix4x4f const & view
			, Matrix4x4f const & projection
			, Frustum const & frustum
			, Point2f const & jitter = Point2f{} );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\remarks		View matrix won't be updated.
		 *\param[in]	projection	The new projection matrix.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\remarks		La matrice de vue ne sera pas mise à jour.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API Configuration & cpuUpdate( Matrix4x4f const & projection
			, Point2f const & jitter = Point2f{} );
	};
}

#define C3D_CameraNamedEx( writer, suffix, binding, set, enabled )\
	sdw::UniformBuffer camera##suffix{ writer\
		, "C3D_Camera"#suffix\
		, "c3d_camera"#suffix\
		, uint32_t( binding )\
		, uint32_t( set )\
		, sdw::type::MemoryLayout::eStd140\
		, enabled };\
	auto c3d_cameraData##suffix = camera##suffix.declMember< c3d::shader::CameraData >( "c", enabled );\
	camera##suffix.end()

#define C3D_CameraEx( writer, binding, set, enabled )\
	C3D_CameraNamedEx( writer, , binding, set, enabled )

#define C3D_Camera( writer, binding, set )\
	C3D_CameraEx( writer, binding, set, true )

#define C3D_CameraNamed( writer, suffix, binding, set )\
	C3D_CameraNamedEx( writer, suffix, binding, set, true )

#endif
