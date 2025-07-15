/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ParticleEmitter_H___
#define ___C3D_ParticleEmitter_H___

#include "ParticleModule.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace c3d
{
	class ParticleEmitter
	{
	public:
		C3D_API explicit ParticleEmitter( ParticleDeclaration const & decl );
		C3D_API virtual ~ParticleEmitter()noexcept = default;
		/**
		 *\~english
		 *\brief		Emits a particle with given values.
		 *\param[in]	value	The particle values.
		 *\~french
		 *\brief		Emet une particle ayant les valeurs données.
		 *\param[in]	value	Les valeurs de la particule.
		 */
		C3D_API Particle emit( ParticleValues const & value )const;

		using OnEmitFunction = Function< void( Particle const & particle ) >;
		using OnEmitSignal = SignalT< OnEmitFunction >;
		using OnEmitConnection = OnEmitSignal::connection;

		OnEmitSignal onEmit;

	protected:
		ParticleDeclaration const & m_decl;
	};
}

#endif
