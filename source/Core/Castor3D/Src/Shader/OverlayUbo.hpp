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
#ifndef ___C3D_OverlayUbo_H___
#define ___C3D_OverlayUbo_H___

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
	class OverlayUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API OverlayUbo( OverlayUbo const & ) = delete;
		C3D_API OverlayUbo & operator=( OverlayUbo const & ) = delete;
		C3D_API OverlayUbo( OverlayUbo && ) = default;
		C3D_API OverlayUbo & operator=( OverlayUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API OverlayUbo( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~OverlayUbo();
		/**
		 *\~english
		 *\brief		Sets the overlay position value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur de la position de l'incrustation.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		C3D_API void SetPosition( Castor::Position const & p_position );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_materialIndex	The overlay's material index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_materialIndex	L'index du matériau de l'incrustation.
		 */
		C3D_API void Update( int p_materialIndex )const;
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

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The uniform variable containing overlay position.
		//!\~french		La variable uniforme contenant la position de l'incrustation.
		Uniform2i & m_position;
		//!\~english	The uniform variable containing overlay's material index.
		//!\~french		La variable uniforme contenant l'indice du matériau de l'incrustation.
		Uniform1i & m_material;
	};
}

#define UBO_OVERLAY( Writer )\
	GLSL::Ubo l_overlay{ l_writer, ShaderProgram::BufferOverlay, 2u };\
	auto c3d_position = l_overlay.DeclMember< GLSL::IVec2 >( ShaderProgram::OvPosition );\
	auto c3d_materialIndex = l_overlay.DeclMember< GLSL::Int >( ShaderProgram::MaterialIndex );\
	l_overlay.End()

#endif
