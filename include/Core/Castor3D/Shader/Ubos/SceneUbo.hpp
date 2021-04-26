/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneUbo_H___
#define ___C3D_SceneUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

namespace castor3d
{
	namespace shader
	{
		struct SceneData
			: public sdw::StructInstance
		{
			friend struct BillboardData;
			friend class Fog;

			C3D_API SceneData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API SceneData & operator=( SceneData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec3 transformCamera( sdw::Mat3 const & transform )const;
			C3D_API sdw::Vec3 getPosToCamera( sdw::Vec3 const & position )const;
			C3D_API sdw::Vec3 getCameraToPos( sdw::Vec3 const & position )const;
			C3D_API sdw::Vec3 getAmbientLight()const;
			C3D_API sdw::Vec3 getCameraPosition()const;
			C3D_API sdw::Vec4 getBackgroundColour( Utils const & utils
				, sdw::Float const & gamma )const;
			C3D_API sdw::Vec4 getBackgroundColour( HdrConfigData const & hdrConfigData )const;
			C3D_API sdw::Int getDirectionalLightCount()const;
			C3D_API sdw::Int getPointLightCount()const;
			C3D_API sdw::Int getSpotLightCount()const;
			C3D_API sdw::Vec4 computeAccumulation( Utils const & utils
				, sdw::Float const & depth
				, sdw::Vec3 const & colour
				, sdw::Float const & alpha
				, sdw::Float const & accumulationOperator )const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Vec4 m_ambientLight;
			sdw::Vec4 m_backgroundColour;
			sdw::Vec4 m_lightsCount;
			sdw::Vec4 m_cameraPosition;
			sdw::Vec4 m_clipInfo;
			sdw::Vec4 m_fogInfo;
		};
	}

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
		 *\brief		Constructor.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API explicit SceneUbo( RenderDevice const & device );
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
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise l'UBO.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void initialise( RenderDevice const & device );
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
		C3D_API void cpuUpdateCameraPosition( Camera const & camera );
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
		C3D_API void cpuUpdate( Camera const * camera
			, Fog const & fog );
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
		C3D_API void cpuUpdate( Scene const & scene
			, Camera const * camera
			, bool lights = true );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	window	The window dimensions.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	window	Les dimensions de la fenêtre.
		 */
		C3D_API void setWindowSize( castor::Size const & window );

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		C3D_API static castor::String const BufferScene;
		C3D_API static castor::String const SceneData;

	private:
		Engine & m_engine;
		RenderDevice const * m_device{};
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_SCENE( writer, binding, set )\
	sdw::Ubo scene{ writer\
		, castor3d::SceneUbo::BufferScene\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_sceneData = scene.declStructMember< castor3d::shader::SceneData >( castor3d::SceneUbo::SceneData );\
	scene.end()

#endif
