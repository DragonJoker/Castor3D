/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PipelineVisitorBase_HPP___
#define ___C3D_PipelineVisitorBase_HPP___

#include "MiscellaneousModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
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
		C3D_API virtual void visit( DebugConfig const & ubo ) = 0;
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
		*	Global post effect configuration.
		*\~french
		*name
		*	Configuration globale de l'effet.
		**/
		/**@{*/
		C3D_API virtual void visit( castor::String const & name
			, float & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, int32_t & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, uint32_t & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2f & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2i & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point2ui & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3f & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3i & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point3ui & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4f & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4i & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Point4ui & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::Matrix4x4f & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< float > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value ) = 0;
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
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, float & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, int32_t & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, uint32_t & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2f & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2i & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2ui & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3f & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3i & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3ui & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4f & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4i & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4ui & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Matrix4x4f & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< float > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< int32_t > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< uint32_t > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< float > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< int32_t > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< uint32_t > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2f > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2i > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2ui > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3f > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3i > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3ui > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4f > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4i > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4ui > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Matrix4x4f > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< float > > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< int32_t > > & value ) = 0;
		C3D_API virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< uint32_t > > & value ) = 0;
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

		void visit( DebugConfig const & ubo )override
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
			, float & value )override
		{
		}

		void visit( castor::String const & name
			, int32_t & value )override
		{
		}

		void visit( castor::String const & name
			, uint32_t & value )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2f & value )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2i & value )override
		{
		}

		void visit( castor::String const & name
			, castor::Point2ui & value )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3f & value )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3i & value )override
		{
		}

		void visit( castor::String const & name
			, castor::Point3ui & value )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4f & value )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4i & value )override
		{
		}

		void visit( castor::String const & name
			, castor::Point4ui & value )override
		{
		}

		void visit( castor::String const & name
			, castor::Matrix4x4f & value )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< float > & value )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value )override
		{
		}

		void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value )override
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
			, castor::String const & ubo
			, castor::String const & uniform
			, float & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, int32_t & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, uint32_t & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2f & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2i & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2ui & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3f & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3i & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3ui & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4f & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4i & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4ui & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Matrix4x4f & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< float > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< int32_t > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< uint32_t > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< float > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< int32_t > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< uint32_t > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2f > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2i > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2ui > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3f > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3i > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3ui > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4f > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4i > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4ui > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Matrix4x4f > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< float > > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< int32_t > > & value )override
		{
		}

		void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< uint32_t > > & value )override
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
