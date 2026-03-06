/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslVolumeShaders_H___
#define ___C3D_GlslVolumeShaders_H___

#include "Castor3D/Render/Volumetric/VolumetricModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace c3d
{
	class VolumeComponentRegister;
}

namespace c3d::shader
{
	struct Volume
		: public sdw::StructInstanceHelperT < "Volume"
			, sdw::type::MemoryLayout::eC
			, sdw::UInt32Field< "id" >
			, sdw::UInt32Field< "type" >
			, sdw::FloatField< "begin" >
			, sdw::FloatField< "end" >
			, sdw::FloatField< "stepSize" >
			, sdw::FloatField< "sampleCount" > >
	{
		SDW_DeclStructInstance( C3D_INL_API, Volume );

		Volume( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, id{ getMember< "id" >() }
			, type{ getMember< "type" >() }
			, begin{ getMember< "begin" >() }
			, end{ getMember< "end" >() }
			, stepSize{ getMember< "stepSize" >() }
			, sampleCount{ getMember< "sampleCount" >() }
		{
		}

		C3D_API Volume( sdw::UInt32 const & id
			, sdw::UInt32 const & type
			, sdw::Float const & begin
			, sdw::Float const & end
			, sdw::Float const & stepSize
			, sdw::Float const & samples );

		sdw::UInt32 id;
		sdw::UInt32 type;
		sdw::Float begin;
		sdw::Float end;
		sdw::Float stepSize;
		sdw::Float sampleCount;
	};

	struct VolumesTraversalResult
		: public sdw::StructInstance
	{
		SDW_DeclStructInstance( C3D_INL_API, VolumesTraversalResult );

		C3D_API VolumesTraversalResult( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		C3D_API explicit VolumesTraversalResult( sdw::ShaderWriter & writer
			, VolumeShaders const & volumeShaders
			, sdw::Vec2 const & size );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, VolumeShaders const & volumeShaders );

		// Scattered light
		sdw::Vec3 inscatter;
		// Transmittance in [0,1] (unitless)
		sdw::Vec3 transmittance;
		sdw::Vec2 renderSize;
		sdw::Boolean transmittanceAboveThreshold;

	protected:
		static sdw::expr::ExprPtr makeInit( sdw::ShaderWriter const & writer
			, VolumeShaders const & volumeShaders
			, sdw::Vec2 const & size );
	};

	class Volumes
	{
	public:
		C3D_API Volumes( sdw::ShaderWriter & writer
			, VolumeShaders const & volumeShaders );

		C3D_API uint32_t registerVolumeType( VolumeTraversalFunc const & traversal, VolumeStepFunc const & step );
		C3D_API sdw::Float addVolume( uint32_t type, sdw::Float const & begin, sdw::Float const & end, sdw::Float const & samples );
		C3D_API sdw::RetVoid step( Volume const & volume, Ray const & ray, sdw::Float const & sample, sdw::Float const & t, sdw::Float const & dt, VolumesTraversalResult const & traversal );
		C3D_API sdw::RetVoid traverse( Volume const & volume, Ray const & ray, sdw::Float const & sample, sdw::Float const & t, sdw::Float const & dt, VolumesTraversalResult & result );

		sdw::UInt const & size()const
		{
			return m_volumeCount;
		}

		RetVolume operator[]( sdw::UInt const & index )
		{
			return m_volumes[index];
		}

		sdw::Array< Volume > const & getVolumes()const noexcept
		{
			return m_volumes;
		}

		sdw::UInt const & getCount()const noexcept
		{
			return m_volumeCount;
		}

	private:

	private:
		sdw::ShaderWriter & m_writer;
		VolumeShaders const & m_volumeShaders;
		sdw::Array< Volume > m_volumes;
		sdw::UInt m_volumeCount;
		c3d::Vector< VolumeTraversalFunc const * > m_traversalFuncs;
		c3d::Vector< VolumeStepFunc const * > m_stepFuncs;
		sdw::Function< sdw::Void
			, InVolume, InRay, sdw::InFloat, sdw::InFloat, sdw::InFloat
			, InOutVolumesTraversalResult > m_traverseVolume;
		VolumeStepFunc m_stepVolume;
	};

	class VolumeShaders
	{
	public:
		C3D_API VolumeShaders( sdw::ShaderWriter & writer
			, VolumeComponentRegister const & volumeRegister
			, c3d::Extent2D const & targetExtent
			, bool hasDepth
			, uint32_t enabledPlugins
			, uint32_t & binding );
		C3D_API ~VolumeShaders()noexcept;
		C3D_API void fillType( sdw::type::BaseStruct & type )const;
		C3D_API void fillInit( sdw::type::BaseStruct & type
			, sdw::expr::ExprList & inits )const;
		C3D_API void registerVolumeTypes( Volumes & volumes )const;
		C3D_API void initialise( sdw::Vec2 const & pixelCoord, Ray & ray
			, Volumes & volumes, VolumesTraversalResult & result )const;
		C3D_API void finalise( Ray const & ray, VolumesTraversalResult & result )const;

		C3D_API Engine & getEngine()const noexcept;

	private:
		VolumeComponentRegister const & m_volumeRegister;
		Vector< VolumeComponentShaderPtr > m_volumesShaders;
	};
}

#endif
