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
		: public Castor3D::CpuParticleSystem
	{
	public:
		ParticleSystem( Castor3D::ParticleSystem & p_parent );
		virtual ~ParticleSystem();
		static Castor3D::CpuParticleSystemUPtr Create( Castor3D::ParticleSystem & p_parent );
		void EmitParticle( float p_type, Castor::Point3f const & p_position, Castor::Point3f const & p_velocity, float p_age );
		/**
		 *\copydoc		Castor3D::CpuParticleSystem::Update
		 */
		uint32_t Update( float p_time, float p_totalTime )override;

	private:
		/**
		 *\copydoc		Castor3D::CpuParticleSystem::DoInitialise
		 */
		bool DoInitialise()override;
		/**
		 *\copydoc		Castor3D::CpuParticleSystem::DoCleanup
		 */
		void DoCleanup()override;

	public:
		static Castor::String const Type;
		static Castor::String const Name;

	private:
		uint32_t m_firstUnused{ 1u };
	};
}

#endif
