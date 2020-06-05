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

namespace castor3d
{
	class PipelineVisitorBase
	{
	protected:
		inline explicit PipelineVisitorBase( bool forceSubPassesVisit )
			: forceSubPassesVisit{ forceSubPassesVisit }
		{
		}

	public:
		virtual inline ~PipelineVisitorBase()
		{
		}
		/**
		*\~english
		*name
		*	Shader source.
		*\~french
		*name
		*	Source de shader.
		**/
		/**@{*/
		virtual void visit( ShaderModule const & shader ) = 0;
		virtual void visit( DebugConfig const & ubo ) = 0;
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
		virtual void visit( castor::String const & name
			, ashes::ImageView const & view
			, TextureFactors const & factors = {} ) = 0;
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
		virtual void visit( castor::String const & name
			, float & value ) = 0;
		virtual void visit( castor::String const & name
			, int32_t & value ) = 0;
		virtual void visit( castor::String const & name
			, uint32_t & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point2f & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point2i & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point2ui & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point3f & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point3i & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point3ui & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point4f & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point4i & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point4ui & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Matrix4x4f & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::RangedValue< float > & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value ) = 0;
		virtual void visit( castor::String const & name
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
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, float & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, int32_t & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, uint32_t & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2f & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2i & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2ui & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3f & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3i & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3ui & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4f & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4i & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4ui & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Matrix4x4f & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< float > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< int32_t > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< uint32_t > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< float > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< int32_t > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< uint32_t > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2f > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2i > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2ui > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3f > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3i > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3ui > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4f > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4i > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4ui > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Matrix4x4f > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< float > > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< int32_t > > & value ) = 0;
		virtual void visit( castor::String const & name
			, VkShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< uint32_t > > & value ) = 0;
		/**@}*/

	public:
		bool const forceSubPassesVisit;
	};

	class PipelineVisitor
		: public PipelineVisitorBase
	{
	protected:
		inline PipelineVisitor( bool forceSubPassesVisit )
			: PipelineVisitorBase{ forceSubPassesVisit }
		{
		}

	public:
		virtual inline ~PipelineVisitor()
		{
		}
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
		*	Intermediate images.
		*\~french
		*name
		*	Images intermédiaires.
		**/
		/**@{*/
		void visit( castor::String const & name
			, ashes::ImageView const & view
			, TextureFactors const & factors = TextureFactors{} )override
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
	};
}

#endif
