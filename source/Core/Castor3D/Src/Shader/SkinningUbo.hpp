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
#ifndef ___C3D_SkinningUbo_H___
#define ___C3D_SkinningUbo_H___

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
	class SkinningUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API SkinningUbo( SkinningUbo const & ) = delete;
		C3D_API SkinningUbo & operator=( SkinningUbo const & ) = delete;
		C3D_API SkinningUbo( SkinningUbo && ) = default;
		C3D_API SkinningUbo & operator=( SkinningUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API SkinningUbo( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkinningUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_materialIndex	The overlay's material index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_materialIndex	L'index du matériau de l'incrustation.
		 */
		C3D_API void Update( AnimatedSkeleton const & p_skeleton )const;
		/**
		 *\~english
		 *\brief		Declares the GLSL variables needed to compute skinning in vertex shader.
		 *\param[in]	p_writer	The GLSL writer.
		 *\param[in]	p_flags		The program flags.
		 *\~french
		 *\brief		Déclare les variables nécessaires au calcul du skinning dans le vertex shader.
		 *\param[in]	p_writer	Le GLSL writer.
		 *\param[in]	p_flags		Les indicateurs du programme.
		 */
		C3D_API static void Declare( GLSL::GlslWriter & p_writer
			, ProgramFlags const & p_flags );
		/**
		 *\~english
		 *\brief		Computes skinning transformation in vertex shader.
		 *\param[in]	p_writer	The GLSL writer.
		 *\param[in]	p_flags		The program flags.
		 *\return		The resulting matrix.
		 *\~french
		 *\brief		Effectue le calcul de la transformation du skinning dans le vertex shader.
		 *\param[in]	p_writer	Le GLSL writer.
		 *\param[in]	p_flags		Les indicateurs du programme.
		 *\return		La matrice résultat.
		 */
		C3D_API static GLSL::Mat4 ComputeTransform( GLSL::GlslWriter & p_writer
			, ProgramFlags const & p_flags );
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
		static constexpr uint32_t BindingPoint = 5u;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The bones matrices uniform variable.
		//!\~french		Le variable uniforme contenant les matrices des os.
		Uniform4x4f & m_bonesMatrix;
	};
}

#endif
