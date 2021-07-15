/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PipelineVisitorBase_HPP___
#define ___C3D_PipelineVisitorBase_HPP___

#include "MiscellaneousModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>

#include <RenderGraph/ImageViewData.hpp>

namespace castor3d
{
	class PipelineVisitorBase
	{
	public:
		struct Config
		{
			bool forceSubPassesVisit;
			bool forceMiplevelsVisit;
		};

	protected:
		explicit PipelineVisitorBase( Config config )
			: config{ std::move( config ) }
		{
		}

	public:
		C3D_API virtual ~PipelineVisitorBase() = default;
		/**
		*\~english
		*name
		*	Shader source.
		*\~french
		*name
		*	Source de shader.
		**/
		/**@{*/
		C3D_API virtual void visit( ShaderModule const & shader ) = 0;
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
			if ( doFilter( viewId ) )
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
		*	Global pipeline configuration.
		*\~french
		*name
		*	Configuration globale du pipeline.
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
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, int32_t & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, uint32_t & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2f & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2i & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2ui & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3f & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3i & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3ui & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4f & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4i & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4ui & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Matrix4x4f & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< float > & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< int32_t > & value
			, bool * control = nullptr ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< uint32_t > & value
			, bool * control = nullptr ) = 0;

		template< typename TypeT >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::ChangeTracked< TypeT > & value )
		{
			visit( name, shaders, bufferName, varName, *value, &value.control() );
		}

		template< typename TypeT >
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::GroupChangeTracked< TypeT > & value )
		{
			visit( name, shaders, bufferName, varName, *value, &value.control() );
		}
		/**@}*/

	protected:
		C3D_API virtual bool doFilter( crg::ImageViewId const & viewId )const = 0;

	private:
		C3D_API virtual void doVisit( castor::String const & name
			, crg::ImageViewId viewId
			, VkImageLayout layout
			, TextureFactors const & factors ) = 0;

	public:
		Config const config;
	};

	class PipelineVisitor
		: public PipelineVisitorBase
	{
	protected:
		explicit PipelineVisitor( Config config )
			: PipelineVisitorBase{ std::move( config ) }
		{
		}

	public:
		/**
		*\~english
		*name
		*	Shader source.
		*\~french
		*name
		*	Source de shader.
		**/
		/**@{*/
		void visit( ShaderModule const & shader )override
		{
		}

		void visit( DebugConfig const & config )override
		{
		}
		/**@}*/
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
		/**@}*/
		/**
		*\~english
		*name
		*	UBO configuration.
		*\~french
		*name
		*	Configuration d'UBO.
		**/
		/**@{*/
		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, float & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, int32_t & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, uint32_t & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2f & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2i & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point2ui & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3f & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3i & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point3ui & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4f & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4i & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Point4ui & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::Matrix4x4f & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< float > & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< int32_t > & value
			, bool * control )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & bufferName
			, castor::String const & varName
			, castor::RangedValue< uint32_t > & value
			, bool * control )override
		{
		}
		/**@}*/

	public:
		using PipelineVisitorBase::visit;

	private:
		bool doFilter( crg::ImageViewId const & viewId )const override
		{
			return true;
		}

		void doVisit( castor::String const & name
			, crg::ImageViewId viewId
			, VkImageLayout layout
			, TextureFactors const & factors = TextureFactors{} )override
		{
		}
	};
}

#endif
