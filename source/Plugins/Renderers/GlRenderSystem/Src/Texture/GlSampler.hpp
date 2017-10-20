/*
See LICENSE file in root folder
*/
#ifndef ___GL_SAMPLER_H___
#define ___GL_SAMPLER_H___

#include "Common/GlObject.hpp"

#include <Texture/Sampler.hpp>

namespace GlRender
{
	class GlSampler
		: public castor3d::Sampler
		, private Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >;

	private:
		typedef std::function< void() > PUnbindFunction;
		typedef std::function< void( uint32_t p_index ) > PBindFunction;

	public:
		GlSampler( OpenGl & p_gl, GlRenderSystem * renderSystem, castor::String const & p_name );
		virtual ~GlSampler();

		virtual bool initialise();
		virtual void cleanup();
		virtual void bind( uint32_t p_index )const;
		virtual void unbind( uint32_t p_index )const;

	private:
		void doAdjustMinMipModes( GlInterpolationMode & p_min, GlInterpolationMode & p_mip );

	private:
		GlTexDim m_glDimension;
	};
}

#endif
