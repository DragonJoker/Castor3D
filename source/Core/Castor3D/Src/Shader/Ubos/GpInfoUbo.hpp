/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpInfoUbo_H___
#define ___C3D_GpInfoUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace castor3d
{
	class GpInfoUbo
	{
	public:
		explicit GpInfoUbo( Engine & engine );
		~GpInfoUbo();
		void update( castor::Size const & p_size
			, Camera const & p_camera
			, castor::Matrix4x4r const & p_invViewProj
			, castor::Matrix4x4r const & p_invView
			, castor::Matrix4x4r const & p_invProj );

		inline UniformBuffer & getUbo()
		{
			return m_ubo;
		}

	public:
		static const castor::String GPInfo;
		static const castor::String InvViewProj;
		static const castor::String InvView;
		static const castor::String InvProj;
		static const castor::String View;
		static const castor::String Proj;
		static const castor::String RenderSize;
		static constexpr uint32_t BindingPoint = 4u;

	private:
		//!\~english	The uniform buffer containing Geometry pass informations.
		//!\~french		Le tampon d'uniformes contenant les informations de la geometry pass.
		UniformBuffer m_ubo;
		//!\~english	The uniform variable containing inverted projection-view matrix.
		//!\~french		La variable uniforme contenant la matrice projection-vue inversée.
		Uniform4x4fSPtr m_invViewProjUniform;
		//!\~english	The uniform variable containing inverted view matrix.
		//!\~french		La variable uniforme contenant la matrice vue inversée.
		Uniform4x4fSPtr m_invViewUniform;
		//!\~english	The uniform variable containing inverted projection matrix.
		//!\~french		La variable uniforme contenant la matrice projection inversés.
		Uniform4x4fSPtr m_invProjUniform;
		//!\~english	The uniform variable containing view matrix.
		//!\~french		La variable uniforme contenant la matrice vue.
		Uniform4x4fSPtr m_gViewUniform;
		//!\~english	The uniform variable containing projection matrix.
		//!\~french		La variable uniforme contenant la matrice projection.
		Uniform4x4fSPtr m_gProjUniform;
		//!\~english	The shader variable containing the render area size.
		//!\~french		La variable de shader contenant les dimensions de la zone de rendu.
		Uniform2fSPtr m_renderSize;
	};
}

#define UBO_GPINFO( p_writer )\
	glsl::Ubo gpInfo{ p_writer, castor3d::GpInfoUbo::GPInfo, castor3d::GpInfoUbo::BindingPoint };\
	auto c3d_mtxInvViewProj = gpInfo.declMember< glsl::Mat4 >( castor3d::GpInfoUbo::InvViewProj );\
	auto c3d_mtxInvView = gpInfo.declMember< glsl::Mat4 >( castor3d::GpInfoUbo::InvView );\
	auto c3d_mtxInvProj = gpInfo.declMember< glsl::Mat4 >( castor3d::GpInfoUbo::InvProj );\
	auto c3d_mtxGView = gpInfo.declMember< glsl::Mat4 >( castor3d::GpInfoUbo::View );\
	auto c3d_mtxGProj = gpInfo.declMember< glsl::Mat4 >( castor3d::GpInfoUbo::Proj );\
	auto c3d_renderSize = gpInfo.declMember< glsl::Vec2 >( castor3d::GpInfoUbo::RenderSize );\
	gpInfo.end()

#endif
