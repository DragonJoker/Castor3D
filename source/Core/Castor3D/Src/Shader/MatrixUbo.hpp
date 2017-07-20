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
#ifndef ___C3D_MatrixUbo_H___
#define ___C3D_MatrixUbo_H___

#include "UniformBuffer.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Matrices Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour les matrices.
	*/
	class MatrixUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API MatrixUbo( MatrixUbo const & ) = delete;
		C3D_API MatrixUbo & operator=( MatrixUbo const & ) = delete;
		C3D_API MatrixUbo( MatrixUbo && ) = default;
		C3D_API MatrixUbo & operator=( MatrixUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API MatrixUbo( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MatrixUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_view			The new view matrix.
		 *\param[in]	p_projection	The new projection matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_view			La nouvelle matrice de vue.
		 *\param[in]	p_projection	La nouvelle matrice de projection.
		 */
		C3D_API void Update( Castor::Matrix4x4r const & p_view
			, Castor::Matrix4x4r const & p_projection )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\remarks		View matrix won't be updated.
		 *\param[in]	p_projection	The new projection matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\remarks		La matrice de vue ne sera pas mise à jour.
		 *\param[in]	p_projection	La nouvelle matrice de projection.
		 */
		C3D_API void Update( Castor::Matrix4x4r const & p_projection )const;
		/**
		 *\~english
		 *\name			Getters.
		 *\~french
		 *\name			Getters.
		 */
		inline UniformBuffer & GetUbo()
		{
			return m_ubo;
		}

		inline UniformBuffer const & GetUbo()const
		{
			return m_ubo;
		}
		/**@}*/

	public:
		static constexpr uint32_t BindingPoint = 1u;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The view matrix variable.
		//!\~french		La variable de la matrice vue.
		Uniform4x4f & m_view;
		//!\~english	The projection matrix variable.
		//!\~french		La variable de la matrice projection.
		Uniform4x4f & m_projection;
		//!\~english	The inverse projection matrix variable.
		//!\~french		La variable de la matrice projection inverse.
		Uniform4x4f & m_invProjection;
	};
}

#define UBO_MATRIX( Writer )\
	GLSL::Ubo matrices{ writer, Castor3D::ShaderProgram::BufferMatrix, Castor3D::MatrixUbo::BindingPoint };\
	auto c3d_mtxProjection = matrices.DeclMember< GLSL::Mat4 >( Castor3D::RenderPipeline::MtxProjection );\
	auto c3d_mtxView = matrices.DeclMember< GLSL::Mat4 >( Castor3D::RenderPipeline::MtxView );\
	auto c3d_mtxInvProjection = matrices.DeclMember< GLSL::Mat4 >( Castor3D::RenderPipeline::MtxInvProjection );\
	matrices.End()

#endif
