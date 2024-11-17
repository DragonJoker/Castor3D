/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PbrPass_H___
#define ___C3D_PbrPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

namespace castor3d
{
	class PbrPass
	{
	public:
		C3D_API static PassUPtr create( LightingModelID lightingModelId
			, Material & parent );

		C3D_API static castor::StringView const Type;
		C3D_API static castor::StringView const LightingModel;

		C3D_API static shader::DiffuseBrdfDesc const DefaultDiffuseBrdf;
		C3D_API static shader::DiffuseBrdfArray const DiffuseBrdfs;

		C3D_API static shader::SpecularBrdfDesc const DefaultSpecularBrdf;
		C3D_API static shader::SpecularBrdfArray const SpecularBrdfs;

		C3D_API static shader::SheenBrdfDesc const DefaultSheenBrdf;
		C3D_API static shader::SheenBrdfArray const SheenBrdfs;

		C3D_API static shader::ClearcoatBrdfDesc const DefaultClearcoatBrdf;
		C3D_API static shader::ClearcoatBrdfArray const ClearcoatBrdfs;

		C3D_API static shader::ScatteringModelDesc const DefaultScatteringModel;
		C3D_API static shader::ScatteringModelArray const ScatteringModels;
	};
}

#endif
