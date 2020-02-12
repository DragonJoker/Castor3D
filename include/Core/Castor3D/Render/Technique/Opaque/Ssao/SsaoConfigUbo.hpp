/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoConfigUbo_H___
#define ___C3D_SsaoConfigUbo_H___

#include "SsaoModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"

namespace castor3d
{
	class SsaoConfigUbo
	{
	private:
		struct Configuration
		{
			//   vec4(-2.0f / (width*P[0][0]),
			//		  -2.0f / (height*P[1][1]),
			//		  ( 1.0f - P[0][2]) / P[0][0],
			//		  ( 1.0f + P[1][2]) / P[1][1])
			//
			//	where P is the projection matrix that maps camera space points
			//	to [-1, 1] x [-1, 1].  That is, SsaoConfigUbo::getProjectUnitMatrix().
			castor::Point4f projInfo;
			castor::Matrix4x4f viewMatrix;
			// Integer number of samples to take at each pixel.
			int32_t numSamples;
			// This is the number of turns around the circle that the spiral pattern makes.
			// This should be prime to prevent taps from lining up.
			int32_t numSpiralTurns;
			// The height in pixels of a 1m object if viewed from 1m away.
			// You can compute it from your projection matrix.  The actual value is just
			// a scale factor on radius; you can simply hardcode this to a constant (~500)
			// and make your radius value unitless (...but resolution dependent.)
			float projScale;
			// World-space AO radius in scene units (r).  e.g., 1.0m.
			float radius;
			// 1 / radius.
			float invRadius;
			// Squared radius.
			float radius2;
			// 1 / (squared radius).
			float invRadius2;
			// Bias to avoid AO in smooth corners, e.g., 0.01m.
			float bias;
			// intensity.
			float intensity;
			// intensity / radius ^ 6.
			float intensityDivR6;
			// Used for preventing AO computation on the sky (at infinite depth) and defining the CS Z to bilateral depth key scaling.
			// This need not match the real far plane.
			float farPlaneZ;
			// Increase to make depth edges crisper. Decrease to reduce flicker.
			float edgeSharpness;
			// Step in 2-pixel intervals since we already blurred against neighbors in the
			// first AO pass.  This constant can be increased while R decreases to improve
			// performance at the expense of some dithering artifacts.
			// 
			// Morgan found that a scale of 3 left a 1-pixel checkerboard grid that was
			// unobjectionable after shading was applied but eliminated most temporal incoherence
			// from using small numbers of sample taps.
			int32_t blurStepSize;
			// VkFilter radius in pixels. This will be multiplied by blurStepSize.
			int32_t blurRadius;
		};

	public:
		/**
		*\~english
		*\name			Copy/Move construction/assignment operation.
		*\~french
		*\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		*/
		/**@{*/
		C3D_API SsaoConfigUbo( SsaoConfigUbo const & ) = delete;
		C3D_API SsaoConfigUbo & operator=( SsaoConfigUbo const & ) = delete;
		C3D_API SsaoConfigUbo( SsaoConfigUbo && ) = default;
		C3D_API SsaoConfigUbo & operator=( SsaoConfigUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit SsaoConfigUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SsaoConfigUbo();
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
		 *\brief		Updates the UBO content.
		 *\param[in]	config	The SSAO configuration.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Met à jour le contenu de l'UBO.
		 *\param[in]	config	La configuratio du SSAO.
		 *\param[in]	camera	La caméra de rendu.
		 */
		C3D_API void update( SsaoConfig const & config
			, Camera const & camera );
		/**
		 *\~english
		 *\name			Getters.
		 *\~french
		 *\name			Accesseurs.
		 */
		inline UniformBuffer< Configuration > & getUbo()
		{
			return *m_ubo;
		}

		inline UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

	public:
		static uint32_t constexpr BindingPoint = 8u;
		C3D_API static castor::String const BufferSsaoConfig;
		C3D_API static castor::String const NumSamples;
		C3D_API static castor::String const NumSpiralTurns;
		C3D_API static castor::String const ProjScale;
		C3D_API static castor::String const Radius;
		C3D_API static castor::String const InvRadius;
		C3D_API static castor::String const Radius2;
		C3D_API static castor::String const InvRadius2;
		C3D_API static castor::String const Bias;
		C3D_API static castor::String const Intensity;
		C3D_API static castor::String const IntensityDivR6;
		C3D_API static castor::String const FarPlaneZ;
		C3D_API static castor::String const EdgeSharpness;
		C3D_API static castor::String const BlurStepSize;
		C3D_API static castor::String const BlurRadius;
		C3D_API static castor::String const ProjInfo;
		C3D_API static castor::String const ViewMatrix;

	private:
		Engine & m_engine;
		UniformBufferUPtr< Configuration > m_ubo;
	};
}

#define UBO_SSAO_CONFIG( Writer, Binding, Set )\
	sdw::Ubo ssaoConfig{ Writer, castor3d::SsaoConfigUbo::BufferSsaoConfig, Binding, Set };\
	auto c3d_projInfo = ssaoConfig.declMember< sdw::Vec4 >( castor3d::SsaoConfigUbo::ProjInfo );\
	auto c3d_viewMatrix = ssaoConfig.declMember< sdw::Mat4 >( castor3d::SsaoConfigUbo::ViewMatrix );\
	auto c3d_numSamples = ssaoConfig.declMember< sdw::Int >( castor3d::SsaoConfigUbo::NumSamples );\
	auto c3d_numSpiralTurns = ssaoConfig.declMember< sdw::Int >( castor3d::SsaoConfigUbo::NumSpiralTurns );\
	auto c3d_projScale = ssaoConfig.declMember< sdw::Float >( castor3d::SsaoConfigUbo::ProjScale );\
	auto c3d_radius = ssaoConfig.declMember< sdw::Float >( castor3d::SsaoConfigUbo::Radius );\
	auto c3d_invRadius = ssaoConfig.declMember< sdw::Float >( castor3d::SsaoConfigUbo::InvRadius );\
	auto c3d_radius2 = ssaoConfig.declMember< sdw::Float >( castor3d::SsaoConfigUbo::Radius2 );\
	auto c3d_invRadius2 = ssaoConfig.declMember< sdw::Float >( castor3d::SsaoConfigUbo::InvRadius2 );\
	auto c3d_bias = ssaoConfig.declMember< sdw::Float >( castor3d::SsaoConfigUbo::Bias );\
	auto c3d_intensity = ssaoConfig.declMember< sdw::Float >( castor3d::SsaoConfigUbo::Intensity );\
	auto c3d_intensityDivR6 = ssaoConfig.declMember< sdw::Float >( castor3d::SsaoConfigUbo::IntensityDivR6 );\
	auto c3d_farPlaneZ = ssaoConfig.declMember< sdw::Float >( castor3d::SsaoConfigUbo::FarPlaneZ );\
	auto c3d_edgeSharpness = ssaoConfig.declMember< sdw::Float >( castor3d::SsaoConfigUbo::EdgeSharpness );\
	auto c3d_blurStepSize = ssaoConfig.declMember< sdw::Int >( castor3d::SsaoConfigUbo::BlurStepSize );\
	auto c3d_blurRadius = ssaoConfig.declMember< sdw::Int >( castor3d::SsaoConfigUbo::BlurRadius );\
	ssaoConfig.end()

#endif
