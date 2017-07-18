/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_GpInfoUbo_H___
#define ___C3D_GpInfoUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace Castor3D
{
	class GpInfoUbo
	{
	public:
		GpInfoUbo( Engine & p_engine );
		~GpInfoUbo();
		void Update( Castor::Size const & p_size
			, Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj );

		inline UniformBuffer & GetUbo()
		{
			return m_gpInfoUbo;
		}

	public:
		static const Castor::String GPInfo;
		static const Castor::String InvViewProj;
		static const Castor::String InvView;
		static const Castor::String InvProj;
		static const Castor::String View;
		static const Castor::String Proj;
		static const Castor::String RenderSize;
		static constexpr uint32_t BindingPoint = 4u;

	private:
		//!\~english	The uniform buffer containing Geometry pass informations.
		//!\~french		Le tampon d'uniformes contenant les informations de la geometry pass.
		UniformBuffer m_gpInfoUbo;
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
	GLSL::Ubo l_gpInfo{ p_writer, Castor3D::GpInfoUbo::GPInfo, Castor3D::GpInfoUbo::BindingPoint };\
	auto c3d_mtxInvViewProj = l_gpInfo.DeclMember< GLSL::Mat4 >( Castor3D::GpInfoUbo::InvViewProj );\
	auto c3d_mtxInvView = l_gpInfo.DeclMember< GLSL::Mat4 >( Castor3D::GpInfoUbo::InvView );\
	auto c3d_mtxInvProj = l_gpInfo.DeclMember< GLSL::Mat4 >( Castor3D::GpInfoUbo::InvProj );\
	auto c3d_mtxGView = l_gpInfo.DeclMember< GLSL::Mat4 >( Castor3D::GpInfoUbo::View );\
	auto c3d_mtxGProj = l_gpInfo.DeclMember< GLSL::Mat4 >( Castor3D::GpInfoUbo::Proj );\
	auto c3d_renderSize = l_gpInfo.DeclMember< GLSL::Vec2 >( Castor3D::GpInfoUbo::RenderSize );\
	l_gpInfo.End()

#endif
