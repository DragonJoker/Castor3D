#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	castor::String const ModelIndices::BufferName = cuT( "ModelsIndices" );
	castor::String const ModelIndices::DataName = cuT( "c3d_modelsData" );

	sdw::Mat4 ModelIndices::getPrvModelMtx( PipelineFlags const & flags
		, sdw::Mat4 const & curModelMatrix )const
	{
		if ( flags.enableInstantiation() )
		{
			return curModelMatrix;
		}

		return prvMtxModel();
	}

	sdw::Mat3 ModelIndices::getNormalMtx( bool hasSkin
		, sdw::Mat4 const & curModelMatrix )const
	{
		if ( hasSkin )
		{
			return transpose( inverse( mat3( curModelMatrix ) ) );
		}

		return mat3( mtxNormal() );
	}

	sdw::Mat3 ModelIndices::getNormalMtx( PipelineFlags const & flags
		, sdw::Mat4 const & curModelMatrix )const
	{
		if ( flags.enableInstantiation()
			|| ( flags.hasSkinData() && !flags.hasWorldPosInputs() ) )
		{
			return transpose( inverse( mat3( curModelMatrix ) ) );
		}

		return mat3( mtxNormal() );
	}

	sdw::Vec4 ModelIndices::worldToModel( sdw::Vec4 const & pos )const
	{
		return inverse( getModelMtx() ) * pos;
	}

	sdw::Vec4 ModelIndices::modelToWorld( sdw::Vec4 const & pos )const
	{
		return getModelMtx() * pos;
	}

	sdw::Vec4 ModelIndices::modelToCurWorld( sdw::Vec4 const & pos )const
	{
		return getModelMtx() * pos;
	}

	sdw::Vec4 ModelIndices::modelToPrvWorld( sdw::Vec4 const & pos )const
	{
		return prvMtxModel() * pos;
	}

	sdw::Mat4 ModelIndices::getCurModelMtx( PipelineFlags const & flags
		, sdw::Mat4 const & transform )const
	{
		if ( flags.enableInstantiation() )
		{
			return transform;
		}

		return getModelMtx();
	}

	sdw::Mat4 ModelIndices::getCurModelMtx( SkinningData const & skinning
		, sdw::UInt const & skinningId
		, sdw::UVec4 const & boneIds0
		, sdw::UVec4 const & boneIds1
		, sdw::Vec4 const & boneWeights0
		, sdw::Vec4 const & boneWeights1 )const
	{
		if ( skinning.transforms )
		{
			return SkinningUbo::computeTransform( skinning
				, getModelMtx()
				, *getWriter()
				, skinningId
				, boneIds0
				, boneIds1
				, boneWeights0
				, boneWeights1 );
		}

		return getModelMtx();
	}
}
