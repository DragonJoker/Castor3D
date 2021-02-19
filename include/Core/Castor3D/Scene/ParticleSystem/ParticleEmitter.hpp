/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ParticleEmitter_H___
#define ___C3D_ParticleEmitter_H___

#include "ParticleModule.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	class ParticleEmitter
	{
	public:
		C3D_API explicit ParticleEmitter( castor3d::ParticleDeclaration const & decl );
		C3D_API virtual ~ParticleEmitter() = default;
		/**
		 *\~english
		 *\brief		Emits a particle with given values.
		 *\param[in]	value	The particle values.
		 *\~french
		 *\brief		Emet une particle ayant les valeurs données.
		 *\param[in]	value	Les valeurs de la particule.
		 */
		C3D_API castor3d::Particle emit( ParticleValues const & value );

		using OnEmitFunction = std::function< void( Particle const & particle ) >;
		using OnEmitSignal = castor::Signal< OnEmitFunction >;
		using OnEmitConnection = OnEmitSignal::connection;

		OnEmitSignal onEmit;

	protected:
		castor3d::ParticleDeclaration const & m_decl;
	};
}

#endif
