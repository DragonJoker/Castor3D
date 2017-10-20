/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_SAMPLER_H___
#define ___C3DGLES3_SAMPLER_H___

#include "Common/GlES3Object.hpp"

#include <Texture/Sampler.hpp>

namespace GlES3Render
{
	class GlES3Sampler
		: public Castor3D::Sampler
		, private Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >;

	private:
		typedef std::function< void() > PUnbindFunction;
		typedef std::function< void( uint32_t p_index ) > PBindFunction;

	public:
		GlES3Sampler( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, Castor::String const & p_name );
		virtual ~GlES3Sampler();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual void Bind( uint32_t p_index )const;
		virtual void Unbind( uint32_t p_index )const;

	private:
		void DoAdjustMinMipModes( GlES3InterpolationMode & p_min, GlES3InterpolationMode & p_mip );

	private:
		GlES3TexDim m_glDimension;
	};
}

#endif
