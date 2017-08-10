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
#ifndef ___C3D_SceneUbo_H___
#define ___C3D_SceneUbo_H___

#include "UniformBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Scene Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour la scène.
	*/
	class SceneUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API SceneUbo( SceneUbo const & ) = delete;
		C3D_API SceneUbo & operator=( SceneUbo const & ) = delete;
		C3D_API SceneUbo( SceneUbo && ) = default;
		C3D_API SceneUbo & operator=( SceneUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API SceneUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SceneUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_camera	The current camera.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_camera	La camera actuelle.
		 */
		C3D_API void updateCameraPosition( Camera const & p_camera )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_camera	The current camera.
		 *\param[in]	p_fog		The fog configuration.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_camera	La camera actuelle.
		 *\param[in]	p_fog		La configuration du brouillard.
		 */
		C3D_API void update( Camera const & p_camera
			, Fog const & p_fog )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_camera	The current camera.
		 *\param[in]	p_lights	The lights are updated too.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_scene		La scène dessinée.
		 *\param[in]	p_camera	La camera actuelle.
		 *\param[in]	p_lights	Les sources lumineuses sont mises à jour elles aussi.
		 */
		C3D_API void update( Scene const & p_scene
			, Camera const & p_camera
			, bool p_lights = true )const;
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
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
		static constexpr uint32_t BindingPoint = 3u;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The ambient light.
		//!\~french		La luminosité ambiante.
		Uniform4f & m_ambientLight;
		//!\~english	The background colour.
		//!\~french		La couleur de fond.
		Uniform4f & m_backgroundColour;
		//!\~english	The lights counts.
		//!\~french		Les comptes des lumières.
		Uniform4i & m_lightsCount;
		//!\~english	The camera position.
		//!\~french		La position de la caméra.
		Uniform3f & m_cameraPos;
		//!\~english	The camera far plane value.
		//!\~french		La valeur du plan éloigné de la caméra.
		Uniform1f & m_cameraNearPlane;
		//!\~english	The camera far plane value.
		//!\~french		La valeur du plan éloigné de la caméra.
		Uniform1f & m_cameraFarPlane;
		//!\~english	The fog type.
		//!\~french		Le type de brouillard.
		Uniform1i & m_fogType;
		//!\~english	The fog density.
		//!\~french		La densité du brouillard.
		Uniform1f & m_fogDensity;
	};
}

#define UBO_SCENE( Writer )\
	GLSL::Ubo scene{ writer, castor3d::ShaderProgram::BufferScene, castor3d::SceneUbo::BindingPoint };\
	auto c3d_v4AmbientLight = scene.declMember< GLSL::Vec4 >( castor3d::ShaderProgram::AmbientLight );\
	auto c3d_v4BackgroundColour = scene.declMember< GLSL::Vec4 >( castor3d::ShaderProgram::BackgroundColour );\
	auto c3d_lightsCount = scene.declMember< GLSL::IVec4 >( castor3d::ShaderProgram::LightsCount );\
	auto c3d_v3CameraPosition = scene.declMember< GLSL::Vec3 >( castor3d::ShaderProgram::CameraPos );\
	auto c3d_v3CameraNearPlane = scene.declMember< GLSL::Float >( castor3d::ShaderProgram::CameraNearPlane ); \
	auto c3d_v3CameraFarPlane = scene.declMember< GLSL::Float >( castor3d::ShaderProgram::CameraFarPlane );\
	auto c3d_iFogType = scene.declMember< GLSL::Int >( castor3d::ShaderProgram::FogType );\
	auto c3d_fFogDensity = scene.declMember< GLSL::Float >( castor3d::ShaderProgram::FogDensity );\
	scene.end()

#endif
