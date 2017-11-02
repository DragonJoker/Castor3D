/*
See LICENSE file in root folder
*/
#ifndef ___LightStreaks_KawaseUbo_H___
#define ___LightStreaks_KawaseUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace light_streaks
{
	class KawaseUbo
	{
	public:
		explicit KawaseUbo( castor3d::Engine & engine );
		~KawaseUbo();
		void update( castor::Size const & size
			, castor::Point2f const & direction
			, uint32_t pass );

		inline castor3d::UniformBuffer & getUbo()
		{
			return m_ubo;
		}

	public:
		static castor::String const Name;
		static castor::String const PixelSize;
		static castor::String const Direction;
		static castor::String const Samples;
		static castor::String const Attenuation;
		static castor::String const Pass;
		static constexpr uint32_t BindingPoint = 2u;

	private:
		castor3d::UniformBuffer m_ubo;
		castor3d::Uniform2fSPtr m_pixelSize;
		castor3d::Uniform2fSPtr m_direction;
		castor3d::Uniform1iSPtr m_samples;
		castor3d::Uniform1fSPtr m_attenuation;
		castor3d::Uniform1iSPtr m_pass;
	};
}

#define UBO_KAWASE( p_writer )\
	glsl::Ubo kawase{ p_writer, KawaseUbo::Name, KawaseUbo::BindingPoint };\
	auto c3d_pixelSize = kawase.declMember< Vec2 >( KawaseUbo::PixelSize );\
	auto c3d_direction = kawase.declMember< Vec2 >( KawaseUbo::Direction );\
	auto c3d_samples = kawase.declMember< Int >( KawaseUbo::Samples );\
	auto c3d_attenuation = kawase.declMember< Float >( KawaseUbo::Attenuation );\
	auto c3d_pass = kawase.declMember< Int >( KawaseUbo::Pass );\
	kawase.end()

#endif
