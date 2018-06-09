/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneUbo_H___
#define ___C3D_SceneUbo_H___

#include "Castor3DPrerequisites.hpp"

#include <Buffer/UniformBuffer.hpp>

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
	private:
		struct Configuration
		{
			castor::Point4f ambientLight;
			castor::Point4f backgroundColour;
			castor::Point4i lightsCount;
			castor::Point4f cameraPos;
			castor::Point2i windowSize;
			float cameraNearPlane;
			float cameraFarPlane;
			int32_t fogType;
			float fogDensity;
		};

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
		 *\brief		Initialises the UBO.
		 *\~french
		 *\brief		Initialise l'UBO.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	camera	The current camera.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	camera	La camera actuelle.
		 */
		C3D_API void updateCameraPosition( Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	camera	The current camera.
		 *\param[in]	fog		The fog configuration.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	camera	La camera actuelle.
		 *\param[in]	fog		La configuration du brouillard.
		 */
		C3D_API void update( Camera const & camera
			, Fog const & fog )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	scene	The rendered scene.
		 *\param[in]	camera	The current camera.
		 *\param[in]	lights	The lights are updated too.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	scene	La scène dessinée.
		 *\param[in]	camera	La camera actuelle.
		 *\param[in]	lights	Les sources lumineuses sont mises à jour elles aussi.
		 */
		C3D_API void update( Scene const & scene
			, Camera const & camera
			, bool lights = true )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	window	The window dimensions.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	window	Les dimensions de la fenêtre.
		 */
		C3D_API void setWindowSize( castor::Size const & window )const;
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		inline renderer::UniformBuffer< Configuration > & getUbo()
		{
			return *m_ubo;
		}

		inline renderer::UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

	public:
		C3D_API static uint32_t const BindingPoint;
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
		//!\~english	Name of the window dimensions frame variable.
		//!\~french		Nom de la frame variable contenant les dimensions de la fenêtre.
		C3D_API static castor::String const WindowSize;
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
		Engine & m_engine;
		renderer::UniformBufferPtr< Configuration > m_ubo;
	};
}

#define UBO_SCENE( writer, binding, set )\
	glsl::Ubo scene{ writer\
		, castor3d::SceneUbo::BufferScene\
		, binding\
		, set };\
	auto c3d_ambientLight = scene.declMember< glsl::Vec4 >( castor3d::SceneUbo::AmbientLight );\
	auto c3d_backgroundColour = scene.declMember< glsl::Vec4 >( castor3d::SceneUbo::BackgroundColour );\
	auto c3d_lightsCount = scene.declMember< glsl::IVec4 >( castor3d::SceneUbo::LightsCount );\
	auto c3d_cameraPosition = scene.declMember< glsl::Vec4 >( castor3d::SceneUbo::CameraPos );\
	auto c3d_windowSize = scene.declMember< glsl::IVec2 >( castor3d::SceneUbo::WindowSize );\
	auto c3d_cameraNearPlane = scene.declMember< glsl::Float >( castor3d::SceneUbo::CameraNearPlane ); \
	auto c3d_cameraFarPlane = scene.declMember< glsl::Float >( castor3d::SceneUbo::CameraFarPlane );\
	auto c3d_fogType = scene.declMember< glsl::Int >( castor3d::SceneUbo::FogType );\
	auto c3d_fogDensity = scene.declMember< glsl::Float >( castor3d::SceneUbo::FogDensity );\
	scene.end()

#endif
