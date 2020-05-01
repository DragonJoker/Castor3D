/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugUbo_H___
#define ___C3D_DebugUbo_H___

#include "DebugConfig.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	class DebugUbo
	{
	public:
		using Configuration = DebugConfig;
		/**
		*\~english
		*\name
		*	Copy/Move construction/assignment operation.
		*\~french
		*\name
		*	Constructeurs/Opérateurs d'affectation par copie/déplacement.
		*/
		/**@{*/
		C3D_API DebugUbo( DebugUbo const & ) = delete;
		C3D_API DebugUbo & operator=( DebugUbo const & ) = delete;
		C3D_API DebugUbo( DebugUbo && ) = default;
		C3D_API DebugUbo & operator=( DebugUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit DebugUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~DebugUbo();
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
		 *\brief		Updates the UBO configuration.
		 *\param[in]	config		The configuration.
		 *\~french
		 *\brief		Met à jour la configuration de l'UBO.
		 *\param[in]	config		La nouvelle configuration.
		 */
		C3D_API void update( DebugConfig const & config );
		/**
		 *\~english
		 *\name			Getters.
		 *\~french
		 *\name			Accesseurs.
		 */
		inline UniformBuffer< Configuration > & getUbo()
		{
			CU_Require( m_ubo );
			return *m_ubo;
		}

		inline UniformBuffer< Configuration > const & getUbo()const
		{
			CU_Require( m_ubo );
			return *m_ubo;
		}
		/**@}*/

	public:
		C3D_API static uint32_t const BindingPoint;
		C3D_API static castor::String const BufferDebug;
		C3D_API static castor::String const DebugLightEye;
		C3D_API static castor::String const DebugLightVSPosition;
		C3D_API static castor::String const DebugLightWSPosition;
		C3D_API static castor::String const DebugLightWSNormal;
		C3D_API static castor::String const DebugLightTexCoord;
		C3D_API static castor::String const DebugDeferredDiffuseLighting;
		C3D_API static castor::String const DebugDeferredSpecularLighting;
		C3D_API static castor::String const DebugDeferredOcclusion;
		C3D_API static castor::String const DebugDeferredSSSTransmittance;
		C3D_API static castor::String const DebugDeferredIBL;
		C3D_API static castor::String const DebugDeferredNormals;
		C3D_API static castor::String const DebugDeferredWorldPos;
		C3D_API static castor::String const DebugDeferredViewPos;
		C3D_API static castor::String const DebugDeferredDepth;
		C3D_API static castor::String const DebugDeferredData1;
		C3D_API static castor::String const DebugDeferredData2;
		C3D_API static castor::String const DebugDeferredData3;
		C3D_API static castor::String const DebugDeferredData4;
		C3D_API static castor::String const DebugDeferredData5;

	private:
		Engine & m_engine;
		UniformBufferUPtr< Configuration > m_ubo;
	};
}

#define UBO_DEBUG( writer, binding, set )\
	sdw::Ubo debug{ writer\
		, castor3d::DebugUbo::BufferDebug\
		, binding\
		, set };\
	auto c3d_debugLightEye = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugLightEye );\
	auto c3d_debugLightVSPosition = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugLightVSPosition );\
	auto c3d_debugLightWSPosition = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugLightWSPosition );\
	auto c3d_debugLightWSNormal = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugLightWSNormal );\
	auto c3d_debugLightTexCoord = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugLightTexCoord );\
	auto c3d_debugDeferredDiffuseLighting = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredDiffuseLighting );\
	auto c3d_debugDeferredSpecularLighting = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredSpecularLighting );\
	auto c3d_debugDeferredOcclusion = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredOcclusion );\
	auto c3d_debugDeferredSSSTransmittance = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredSSSTransmittance );\
	auto c3d_debugDeferredIBL = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredIBL );\
	auto c3d_debugDeferredNormals = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredNormals );\
	auto c3d_debugDeferredWorldPos = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredWorldPos );\
	auto c3d_debugDeferredViewPos = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredViewPos );\
	auto c3d_debugDeferredDepth = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredDepth );\
	auto c3d_debugDeferredData1 = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredData1 );\
	auto c3d_debugDeferredData2 = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredData2 );\
	auto c3d_debugDeferredData3 = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredData3 );\
	auto c3d_debugDeferredData4 = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredData4 );\
	auto c3d_debugDeferredData5 = debug.declMember< sdw::Int >( castor3d::DebugUbo::DebugDeferredData5 );\
	debug.end()

#endif
