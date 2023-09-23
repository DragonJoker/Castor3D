/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ConfigurationVisitor_HPP___
#define ___C3D_ConfigurationVisitor_HPP___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

namespace castor3d
{
	class ConfigurationVisitorBase
	{
	public:
		struct Config
		{
			bool allowProgramsVisit{};
		};

		template< typename ControlT >
		using ControlsListT = std::vector< ControlT * >;
		template< typename EnumT >
		using OnEnumValueChangeT = std::function< void( EnumT oldV, EnumT newV ) >;

		using ControlsList = ControlsListT< bool >;
		using AtomicControlsList = ControlsListT< std::atomic_bool >;

		using OnSEnumValueChange = OnEnumValueChangeT< int32_t >;
		using OnUEnumValueChange = OnEnumValueChangeT< uint32_t >;

		template< typename ControlT >
		static ControlsListT< ControlT > makeControlsList( ControlT * control )
		{
			return ( control
				? ControlsListT< ControlT >{ control }
				: ControlsListT< ControlT >{} );
		}

	protected:
		explicit ConfigurationVisitorBase( Config config = { false } )
			: config{ std::move( config ) }
		{
		}

	public:
		C3D_API virtual ~ConfigurationVisitorBase() = default;
		/**
		*\~english
		*name
		*	Names.
		*\~french
		*name
		*	Noms.
		**/
		/**@{*/
		C3D_API virtual void visit( castor::String const & name
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, AtomicControlsList controls ) = 0;

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, ControlT * control = nullptr )
		{
			visit( name, makeControlsList( control ) );
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Shader source.
		*\~french
		*name
		*	Source de shader.
		**/
		/**@{*/
		C3D_API virtual void visit( ShaderModule const & shader
			, bool forceProgramsVisit ) = 0;

		void visit( ShaderModule const & shader )
		{
			if ( !config.allowProgramsVisit )
			{
				return;
			}

			visit( shader, config.allowProgramsVisit );
		}

		C3D_API virtual void visit( DebugConfig const & config ) = 0;
		/**@}*/
		/**
		*\~english
		*name
		*	Intermediate images.
		*\~french
		*name
		*	Images intermédiaires.
		**/
		/**@{*/
		void visit( castor::String const & name
			, crg::ImageViewId const & viewId
			, VkImageLayout layout
			, TextureFactors const & factors = {} )
		{
			if ( doFilter( viewId, factors ) )
			{
				doVisit( name
					, viewId
					, layout
					, factors );
			}
		}

		void visit( castor::String const & name
			, Texture const & texture
			, VkImageLayout layout
			, TextureFactors const & factors = {} )
		{
			visit( name
				, texture.sampledViewId
				, layout
				, factors );
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Values.
		*\~french
		*name
		*	Valeurs.
		**/
		/**@{*/
		C3D_API virtual void visit( castor::String const & name
			, bool & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int16_t & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint16_t & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int32_t & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint32_t & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int64_t & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint64_t & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, float & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, double & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, BlendMode & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, ParallaxOcclusionMode & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkCompareOp & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RgbColour & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RgbaColour & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::HdrRgbColour & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::HdrRgbaColour & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2f & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2i & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2ui & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3f & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3i & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3ui & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4f & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4i & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4ui & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, float & value
			, castor::Range< float > const & range
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int32_t & value
			, castor::Range< int32_t > const & range
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint32_t & value
			, castor::Range< uint32_t > const & range
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, PassComponentTextureFlag textureFlag
			, TextureFlagConfiguration & value
			, uint32_t componentsCount
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int32_t & enumValue
			, castor::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint32_t & enumValue
			, castor::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, ControlsList controls ) = 0;

		C3D_API virtual void visit( castor::String const & name
			, bool & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int16_t & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint16_t & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int32_t & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint32_t & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int64_t & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint64_t & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, float & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, double & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, BlendMode & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, ParallaxOcclusionMode & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkCompareOp & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RgbColour & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RgbaColour & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::HdrRgbColour & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::HdrRgbaColour & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2f & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2i & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2ui & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3f & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3i & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3ui & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4f & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4i & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4ui & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, float & value
			, castor::Range< float > const & range
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int32_t & value
			, castor::Range< int32_t > const & range
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint32_t & value
			, castor::Range< uint32_t > const & range
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, PassComponentTextureFlag textureFlag
			, TextureFlagConfiguration & value
			, uint32_t componentsCount
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int32_t & enumValue
			, castor::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint32_t & enumValue
			, castor::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, AtomicControlsList controls ) = 0;

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, bool & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, int16_t & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, uint16_t & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, int32_t & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, uint32_t & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, int64_t & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, uint64_t & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, float & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, double & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, BlendMode & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, ParallaxOcclusionMode & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkCompareOp & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::RgbaColour & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::HdrRgbColour & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::HdrRgbaColour & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::Point2f & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::Point2i & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::Point2ui & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::Point3f & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::Point3i & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::Point3ui & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::Point4f & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::Point4i & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::Point4ui & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, ControlT * control = nullptr )
		{
			visit( name, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, float & value
			, castor::Range< float > const & range
			, ControlT * control = nullptr )
		{
			visit( name, value, range, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, int32_t & value
			, castor::Range< int32_t > const & range
			, ControlT * control = nullptr )
		{
			visit( name, value, range, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, uint32_t & value
			, castor::Range< uint32_t > const & range
			, ControlT * control = nullptr )
		{
			visit( name, value, range, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, PassComponentTextureFlag textureFlag
			, TextureFlagConfiguration & value
			, uint32_t componentsCount
			, ControlT * control = nullptr )
		{
			visit( name, textureFlag, value, componentsCount, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, int32_t & enumValue
			, castor::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, ControlT * control = nullptr )
		{
			visit( name, enumValue, enumNames, onChange, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, uint32_t & enumValue
			, castor::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, ControlT * control = nullptr )
		{
			visit( name, enumValue, enumNames, onChange, makeControlsList( control ) );
		}

		template< typename TypeT, typename ControlT >
		void visit( castor::String const & name
			, castor::ChangeTrackedT< TypeT, ControlT > & value
			, ControlsListT< ControlT > controls = ControlsListT< ControlT >{} )
		{
			controls.push_back( &value.control() );
			visit( name, value.naked(), std::move( controls ) );
		}

		template< typename TypeT, typename ControlT >
		void visit( castor::String const & name
			, castor::GroupChangeTrackedT< TypeT, ControlT > & value
			, ControlsListT< ControlT > controls = ControlsListT< ControlT >{} )
		{
			controls.push_back( &value.control() );
			visit( name, value.naked(), std::move( controls ) );
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Shader buffer configuration.
		*\~french
		*name
		*	Configuration de shader buffer.
		**/
		/**@{*/
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, float & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, int32_t & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, uint32_t & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2f & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2i & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2ui & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3f & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3i & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3ui & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4f & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4i & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4ui & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Matrix4x4f & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< float > & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< int32_t > & value
			, ControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< uint32_t > & value
			, ControlsList controls ) = 0;

		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, float & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, int32_t & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, uint32_t & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2f & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2i & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2ui & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3f & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3i & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3ui & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4f & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4i & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4ui & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Matrix4x4f & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< float > & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< int32_t > & value
			, AtomicControlsList controls ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< uint32_t > & value
			, AtomicControlsList controls ) = 0;

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, float & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, int32_t & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, uint32_t & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2f & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2i & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2ui & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3f & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3i & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3ui & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4f & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4i & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4ui & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Matrix4x4f & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< float > & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< int32_t > & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< uint32_t > & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename TypeT, typename ControlT >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::ChangeTrackedT< TypeT, ControlT > & value
			, ControlsListT< ControlT > controls = ControlsListT< ControlT >{} )
		{
			controls.push_back( &value.control() );
			visit( name, shaders, bufferName, varName, *value, std::move( controls ) );
		}

		template< typename TypeT, typename ControlT >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::GroupChangeTrackedT< TypeT, ControlT > & value
			, ControlsListT< ControlT > controls = ControlsListT< ControlT >{} )
		{
			controls.push_back( &value.control() );
			visit( name, shaders, bufferName, varName, *value, std::move( controls ) );
		}

		template< typename TypeT, typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::ChangeTracked< TypeT > & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}

		template< typename TypeT, typename ControlT = bool >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::GroupChangeTracked< TypeT > & value
			, ControlT * control = nullptr )
		{
			visit( name, shaders, bufferName, varName, value, makeControlsList( control ) );
		}
		/**@}*/

	protected:
		C3D_API virtual bool doFilter( crg::ImageViewId const & viewId
			, TextureFactors const & factors )const = 0;

	private:
		C3D_API virtual void doVisit( castor::String const & name
			, crg::ImageViewId viewId
			, VkImageLayout layout
			, TextureFactors const & factors ) = 0;

	public:
		Config const config;
	};

	class ConfigurationVisitor
		: public ConfigurationVisitorBase
	{
	protected:
		explicit ConfigurationVisitor( Config config = {} )
			: ConfigurationVisitorBase{ std::move( config ) }
		{
		}

	public:
		/**
		*\~english
		*name
		*	Names.
		*\~french
		*name
		*	Noms.
		**/
		/**@{*/
		void visit( castor::String const & name
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, AtomicControlsList controls )override
		{
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Shader source.
		*\~french
		*name
		*	Source de shader.
		**/
		/**@{*/
		void visit( ShaderModule const & value
			, bool forceProgramsVisit )override
		{
		}

		void visit( DebugConfig const & value )override
		{
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Values.
		*\~french
		*name
		*	Valeurs.
		**/
		/**@{*/
		void visit( castor::String const & name
			, bool & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, int16_t & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, uint16_t & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, int32_t & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, uint32_t & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, int64_t & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, uint64_t & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, float & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, double & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, BlendMode & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, ParallaxOcclusionMode & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkCompareOp & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::RgbColour & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::RgbaColour & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::HdrRgbColour & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::HdrRgbaColour & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2f & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2i & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2ui & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3f & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3i & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3ui & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4f & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4i & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4ui & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, float & value
			, castor::Range< float > const & range
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, int32_t & value
			, castor::Range< int32_t > const & range
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, uint32_t & value
			, castor::Range< uint32_t > const & range
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, PassComponentTextureFlag textureFlag
			, TextureFlagConfiguration & value
			, uint32_t componentsCount
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, int32_t & enumValue
			, castor::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, uint32_t & enumValue
			, castor::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, bool & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, int16_t & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, uint16_t & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, int32_t & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, uint32_t & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, int64_t & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, uint64_t & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, float & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, double & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, BlendMode & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, ParallaxOcclusionMode & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkCompareOp & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::RgbColour & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::RgbaColour & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::HdrRgbColour & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::HdrRgbaColour & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2f & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2i & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2ui & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3f & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3i & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3ui & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4f & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4i & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4ui & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, float & value
			, castor::Range< float > const & range
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, int32_t & value
			, castor::Range< int32_t > const & range
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, uint32_t & value
			, castor::Range< uint32_t > const & range
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, PassComponentTextureFlag textureFlag
			, TextureFlagConfiguration & value
			, uint32_t componentsCount
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, int32_t & enumValue
			, castor::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, uint32_t & enumValue
			, castor::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, AtomicControlsList controls )override
		{
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Shader buffer configuration.
		*\~french
		*name
		*	Configuration de buffer de shader.
		**/
		/**@{*/
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, float & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, int32_t & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, uint32_t & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2f & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2i & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2ui & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3f & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3i & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3ui & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4f & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4i & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4ui & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Matrix4x4f & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< float > & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< int32_t > & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< uint32_t > & value
			, ControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, float & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, int32_t & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, uint32_t & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2f & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2i & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2ui & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3f & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3i & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3ui & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4f & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4i & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4ui & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Matrix4x4f & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< float > & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< int32_t > & value
			, AtomicControlsList controls )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< uint32_t > & value
			, AtomicControlsList controls )override
		{
		}
		/**@}*/

	public:
		using ConfigurationVisitorBase::visit;

	protected:
		bool doFilter( crg::ImageViewId const & viewId
			, TextureFactors const & factors )const override
		{
			return true;
		}

	private:
		void doVisit( castor::String const & name
			, crg::ImageViewId viewId
			, VkImageLayout layout
			, TextureFactors const & factors = TextureFactors{} )override
		{
		}
	};
}

#pragma GCC diagnostic pop

#endif
