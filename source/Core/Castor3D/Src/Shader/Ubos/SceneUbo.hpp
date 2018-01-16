/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneUbo_H___
#define ___C3D_SceneUbo_H___

#include "Shader/UniformBuffer.hpp"

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
		C3D_API explicit SceneUbo( Engine & engine );
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
		//!\~english	Name of the scene frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données de scène.
		C3D_API static castor::String const BufferScene;
		//!\~english	Name of the ambient light frame variable.
		//!\~french		Nom de la frame variable contenant la lumière ambiante.
		C3D_API static castor::String const AmbientLight;
		//!\~english	Name of the background colour frame variable.
		//!\~french		Nom de la frame variable contenant la couleur de fond.
		C3D_API static castor::String const BackgroundColour;
		//!\~english	Name of the lights count frame variable.
		//!\~french		Nom de la frame variable contenant le compte des lumières.
		C3D_API static castor::String const LightsCount;
		//!\~english	Name of the camera position frame variable.
		//!\~french		Nom de la frame variable contenant la position de la caméra.
		C3D_API static castor::String const CameraPos;
		//!\~english	Name of the camera near plane frame variable.
		//!\~french		Nom de la frame variable contenant la valeur du plan proche de la caméra.
		C3D_API static castor::String const CameraNearPlane;
		//!\~english	Name of the camera far plane frame variable.
		//!\~french		Nom de la frame variable contenant la valeur du plan éloigné de la caméra.
		C3D_API static castor::String const CameraFarPlane;
		//!\~english	Name of the fog type frame variable.
		//!\~french		Nom de la frame variable contenant le type de brouillard.
		C3D_API static castor::String const FogType;
		//!\~english	Name of the fog's density frame variable.
		//!\~french		Nom de la frame variable contenant la densité du brouillard.
		C3D_API static castor::String const FogDensity;

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

#define UBO_SCENE( writer, set )\
	glsl::Ubo scene{ writer\
		, castor3d::SceneUbo::BufferScene\
		, castor3d::SceneUbo::BindingPoint\
		, set };\
	auto c3d_ambientLight = scene.declMember< glsl::Vec4 >( castor3d::SceneUbo::AmbientLight );\
	auto c3d_backgroundColour = scene.declMember< glsl::Vec4 >( castor3d::SceneUbo::BackgroundColour );\
	auto c3d_lightsCount = scene.declMember< glsl::IVec4 >( castor3d::SceneUbo::LightsCount );\
	auto c3d_cameraPosition = scene.declMember< glsl::Vec3 >( castor3d::SceneUbo::CameraPos );\
	auto c3d_cameraNearPlane = scene.declMember< glsl::Float >( castor3d::SceneUbo::CameraNearPlane ); \
	auto c3d_cameraFarPlane = scene.declMember< glsl::Float >( castor3d::SceneUbo::CameraFarPlane );\
	auto c3d_fogType = scene.declMember< glsl::Int >( castor3d::SceneUbo::FogType );\
	auto c3d_fogDensity = scene.declMember< glsl::Float >( castor3d::SceneUbo::FogDensity );\
	scene.end()

#endif
