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
#ifndef ___C3D_HdrConfigUbo_H___
#define ___C3D_HdrConfigUbo_H___

#include "UniformBuffer.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		HDR configuration Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour la configuration HDR.
	*/
	class HdrConfigUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API HdrConfigUbo( HdrConfigUbo const & ) = delete;
		C3D_API HdrConfigUbo & operator=( HdrConfigUbo const & ) = delete;
		C3D_API HdrConfigUbo( HdrConfigUbo && ) = default;
		C3D_API HdrConfigUbo & operator=( HdrConfigUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API HdrConfigUbo( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~HdrConfigUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_config	The HDR configuration.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_config	La configuration HDR.
		 */
		C3D_API void Update( HdrConfig const & p_config )const;
		/**
		 *\~english
		 *\name			Getters.
		 *\~french
		 *\name			Getters.
		 */
		/**@{*/
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
		static constexpr uint32_t BindingPoint = 3u;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The shadow receiver matrix variable.
		//!\~french		La variable de la réception d'ombres.
		Uniform1f & m_exposure;
		//!\~english	The material index matrix variable.
		//!\~french		La variable de l'indice du matériau.
		Uniform1f & m_gamma;
	};
}

#define UBO_HDR_CONFIG( Writer )\
	Ubo l_hdrConfig{ l_writer, ShaderProgram::BufferHdrConfig, HdrConfigUbo::BindingPoint };\
	auto c3d_exposure = l_hdrConfig.DeclMember< Float >( ShaderProgram::Exposure );\
	auto c3d_gamma = l_hdrConfig.DeclMember< Float >( ShaderProgram::Gamma );\
	l_hdrConfig.End()

#endif
