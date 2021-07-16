/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassVisitor_HPP___
#define ___C3D_PassVisitor_HPP___

#include "PassModule.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	class PassVisitorBase
	{
	public:
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
		*	Values.
		*\~french
		*name
		*	Valeurs.
		**/
		/**@{*/
		C3D_API virtual void visit( castor::String const & name
			, bool & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int16_t & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint16_t & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int32_t & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint32_t & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int64_t & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint64_t & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, float & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, double & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, BlendMode & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, ParallaxOcclusionMode & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, PassFlags & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkCompareOp & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RgbColour & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RgbaColour & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::HdrRgbColour & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::HdrRgbaColour & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2f & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2i & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2ui & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3f & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3i & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3ui & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4f & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4i & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4ui & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, TextureFlag textureFlag
			, castor::Point2ui & value
			, uint32_t componentsCount
			, bool * control = nullptr ) = 0;

		template< typename TypeT >
		void visit( castor::String const & name
			, castor::ChangeTracked< TypeT > & value )
		{
			visit( name, *value, &value.control() );
		}

		template< typename TypeT >
		void visit( castor::String const & name
			, castor::GroupChangeTracked< TypeT > & value )
		{
			visit( name, *value, &value.control() );
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
		*	Global post effect configuration.
		*\~french
		*name
		*	Configuration globale de l'effet.
		**/
		/**@{*/
		void visit( castor::String const & name
			, bool & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, int16_t & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, uint16_t & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, int32_t & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, uint32_t & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, int64_t & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, uint64_t & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, float & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, double & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, BlendMode & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, ParallaxOcclusionMode & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, PassFlags & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkCompareOp & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::RgbColour & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::RgbaColour & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::HdrRgbColour & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::HdrRgbaColour & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2f & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2i & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2ui & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3f & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3i & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3ui & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4f & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4i & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4ui & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, TextureFlag textureFlag
			, castor::Point2ui & value
			, uint32_t componentsCount
			, bool * control )override
		{
		}
		/**@}*/

	public:
		using PassVisitorBase::visit;
	};
}

#endif
