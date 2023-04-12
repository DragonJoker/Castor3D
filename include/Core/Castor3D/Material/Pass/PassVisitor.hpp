/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassVisitor_HPP___
#define ___C3D_PassVisitor_HPP___

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

namespace castor3d
{
	class PassVisitorBase
	{
	public:
		template< typename ControlT >
		using ControlsListT = std::vector< ControlT * >;
		using ControlsList = ControlsListT< bool >;
		using AtomicControlsList = ControlsListT< std::atomic_bool >;

		template< typename ControlT >
		static ControlsListT< ControlT > makeControlsList( ControlT * control )
		{
			return ( control
				? ControlsListT< ControlT >{ control }
				: ControlsListT< ControlT >{} );
		}

		struct Config
		{
			bool forceSubPassesVisit;
			bool forceMiplevelsVisit;
		};

	protected:
		explicit PassVisitorBase( Config config )
			: config{ std::move( config ) }
		{
		}

	public:
		C3D_API virtual ~PassVisitorBase() = default;
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
			, PassComponentTextureFlag textureFlag
			, TextureFlagConfiguration & value
			, uint32_t componentsCount
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
			, PassComponentTextureFlag textureFlag
			, TextureFlagConfiguration & value
			, uint32_t componentsCount
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
			, PassComponentTextureFlag textureFlag
			, TextureFlagConfiguration & value
			, uint32_t componentsCount
			, ControlT * control = nullptr )
		{
			visit( name, textureFlag, value, componentsCount, makeControlsList( control ) );
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

	public:
		Config const config;
	};

	class PassVisitor
		: public PassVisitorBase
	{
	protected:
		explicit PassVisitor( Config config )
			: PassVisitorBase{ std::move( config ) }
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
			, PassComponentTextureFlag textureFlag
			, TextureFlagConfiguration & value
			, uint32_t componentsCount
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
			, PassComponentTextureFlag textureFlag
			, TextureFlagConfiguration & value
			, uint32_t componentsCount
			, AtomicControlsList controls )override
		{
		}
		/**@}*/

	public:
		using PassVisitorBase::visit;
	};
}

#pragma GCC diagnostic pop

#endif
