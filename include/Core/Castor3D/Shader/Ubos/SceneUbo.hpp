/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneUbo_H___
#define ___C3D_SceneUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"

namespace castor3d
{
	class SceneUbo
	{
	private:
		using Configuration = SceneUboConfiguration;

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
		C3D_API SceneUbo & operator=( SceneUbo && ) = delete;
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
		C3D_API void gpuUpdateCameraPosition( Camera const & camera )const;
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
		C3D_API void gpuUpdate( Camera const * camera
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
		C3D_API void gpuUpdate( Scene const & scene
			, Camera const * camera
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
		inline UniformBufferT< Configuration > & getUbo()
		{
			return *m_ubo;
		}

		inline UniformBufferT< Configuration > const & getUbo()const
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
		//!\~english	Name of the clip informations (window size, near and far plane) frame variable.
		//!\~french		Nom de la frame variable contenant les informations de clipping (dimensions de la fenêtre, plans proche et lointain).
		C3D_API static castor::String const ClipInfo;
		//!\~english	Name of the fog informations (type and density) frame variable.
		//!\~french		Nom de la frame variable contenant les informations de brouillard (type et densité).
		C3D_API static castor::String const FogInfo;

	private:
		Engine & m_engine;
		UniformBufferUPtrT< Configuration > m_ubo;
	};
}

#define UBO_SCENE( writer, binding, set )\
	sdw::Ubo scene{ writer\
		, castor3d::SceneUbo::BufferScene\
		, binding\
		, set };\
	auto c3d_ambientLight = scene.declMember< sdw::Vec4 >( castor3d::SceneUbo::AmbientLight );\
	auto c3d_backgroundColour = scene.declMember< sdw::Vec4 >( castor3d::SceneUbo::BackgroundColour );\
	auto c3d_lightsCount = scene.declMember< sdw::Vec4 >( castor3d::SceneUbo::LightsCount );\
	auto c3d_cameraPosition = scene.declMember< sdw::Vec4 >( castor3d::SceneUbo::CameraPos );\
	auto c3d_clipInfo = scene.declMember< sdw::Vec4 >( castor3d::SceneUbo::ClipInfo );\
	auto c3d_fogInfo = scene.declMember< sdw::Vec4 >( castor3d::SceneUbo::FogInfo );\
	scene.end()

#endif
