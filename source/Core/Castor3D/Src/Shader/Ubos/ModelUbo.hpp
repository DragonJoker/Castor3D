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
#ifndef ___C3D_ModelUbo_H___
#define ___C3D_ModelUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Model Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour le modèle.
	*/
	class ModelUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API ModelUbo( ModelUbo const & ) = delete;
		C3D_API ModelUbo & operator=( ModelUbo const & ) = delete;
		C3D_API ModelUbo( ModelUbo && ) = default;
		C3D_API ModelUbo & operator=( ModelUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API ModelUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ModelUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_shadowReceiver	Tells if the model receives shadows.
		 *\param[in]	p_materialIndex		The material index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_shadowReceiver	Dit si le modèle reçoit les ombres.
		 *\param[in]	p_materialIndex		L'indice du matériau.
		 */
		C3D_API void update( bool p_shadowReceiver
			, uint32_t p_materialIndex )const;
		/**
		 *\~english
		 *\brief		sets the environment map index value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur de l'indice de la texture d'environnement.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		C3D_API void setEnvMapIndex( uint32_t p_value );
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		/**@{*/
		inline UniformBuffer & getUbo()
		{
			return m_ubo;
		}

		inline UniformBuffer const & getUbo()const
		{
			return m_ubo;
		}
		/**@}*/

	public:
		static constexpr uint32_t BindingPoint = 4u;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The shadow receiver matrix variable.
		//!\~french		La variable de la réception d'ombres.
		Uniform1i & m_shadowReceiver;
		//!\~english	The material index matrix variable.
		//!\~french		La variable de l'indice du matériau.
		Uniform1i & m_materialIndex;
		//!\~english	The environment map index matrix variable.
		//!\~french		La variable de l'indice de la texture d'environnement.
		Uniform1i & m_environmentIndex;
	};
}

#define UBO_MODEL( Writer )\
	glsl::Ubo model{ writer, castor3d::ShaderProgram::BufferModel, castor3d::ModelUbo::BindingPoint };\
	auto c3d_shadowReceiver = model.declMember< glsl::Int >( castor3d::ShaderProgram::ShadowReceiver );\
	auto c3d_materialIndex = model.declMember< glsl::Int >( castor3d::ShaderProgram::MaterialIndex );\
	auto c3d_envMapIndex = model.declMember< glsl::Int >( castor3d::ShaderProgram::EnvironmentIndex );\
	model.end()

#endif
