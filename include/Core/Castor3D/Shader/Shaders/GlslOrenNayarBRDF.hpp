/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslOrenNayarBRDF_H___
#define ___C3D_GlslOrenNayarBRDF_H___

#include "Castor3D/Shader/Shaders/GlslDiffuseBRDF.hpp"

namespace castor3d::shader
{
	class QualitativeOrenNayarBRDF
		: public DiffuseBRDF
	{
	public:
		C3D_API QualitativeOrenNayarBRDF( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );

		C3D_API static DiffuseBRDFPtr create( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API static castor::StringView constexpr Name{ cuT( "qualitative_oren_nayar" ) };

	private:
		void doGenerate( BlendComponents const & components
			, LightSurface const & lightSurface )override;
	};

	class FujiiOrenNayarBRDF
		: public DiffuseBRDF
	{
	public:
		C3D_API FujiiOrenNayarBRDF( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );

		C3D_API static DiffuseBRDFPtr create( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API static castor::StringView constexpr Name{ cuT( "fujii_oren_nayar" ) };

	private:
		void doGenerate( BlendComponents const & components
			, LightSurface const & lightSurface )override;
	};

	class EnergyConservativeOrenNayarBRDF
		: public DiffuseBRDF
	{
	public:
		C3D_API EnergyConservativeOrenNayarBRDF( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );

		C3D_API static DiffuseBRDFPtr create( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API static castor::StringView constexpr Name{ cuT( "energy_conservative_oren_nayar" ) };

	private:
		void doGenerate( BlendComponents const & components
			, LightSurface const & lightSurface )override;

	private:
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_fujiiOrenNayarAlbedo;
	};
}

#endif
