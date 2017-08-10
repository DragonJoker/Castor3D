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
#ifndef ___C3D_FireworksParticle_H___
#define ___C3D_FireworksParticle_H___

#include <Scene/ParticleSystem/CpuParticleSystem.hpp>

namespace Fireworks
{
	class ParticleSystem
		: public castor3d::CpuParticleSystem
	{
	public:
		ParticleSystem( castor3d::ParticleSystem & p_parent );
		virtual ~ParticleSystem();
		static castor3d::CpuParticleSystemUPtr create( castor3d::ParticleSystem & p_parent );
		void emitParticle( float p_type, castor::Point3f const & p_position, castor::Point3f const & p_velocity, float p_age );
		/**
		 *\copydoc		castor3d::CpuParticleSystem::Update
		 */
		uint32_t update( castor::Milliseconds const & p_time
			, castor::Milliseconds const & p_totalTime )override;

	private:
		/**
		 *\copydoc		castor3d::CpuParticleSystem::doInitialise
		 */
		bool doInitialise()override;
		/**
		 *\copydoc		castor3d::CpuParticleSystem::doCleanup
		 */
		void doCleanup()override;

	public:
		static castor::String const Type;
		static castor::String const Name;

	private:
		uint32_t m_firstUnused{ 1u };
	};
}

#endif
