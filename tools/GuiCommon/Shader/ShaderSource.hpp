/*
See LICENSE file in root folder
*/
#ifndef ___GC_ShaderSource_HPP___
#define ___GC_ShaderSource_HPP___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/propgrid/propgrid.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace GuiCommon
{
	enum class UniformType
	{
		eFloat,
		eInt,
		eUInt,
		eVec2f,
		eVec3f,
		eVec4f,
		eVec2i,
		eVec3i,
		eVec4i,
		eVec2ui,
		eVec3ui,
		eVec4ui,
		eMat4f,
	};
	inline wxString const & getName( UniformType value )
	{
		static c3d::Vector< wxString > const Names
		{
			wxT( "Float" ),
			wxT( "Int" ),
			wxT( "UInt" ),
			wxT( "Vec2f" ),
			wxT( "Vec3f" ),
			wxT( "Vec4f" ),
			wxT( "Vec2i" ),
			wxT( "Vec3i" ),
			wxT( "Vec4i" ),
			wxT( "Vec2ui" ),
			wxT( "Vec3ui" ),
			wxT( "Vec4ui" ),
			wxT( "Mat4f" ),
		};
		return Names[size_t( value )];
	}

	template< typename T >
	struct UniformTyper;
	template<>
	struct UniformTyper< float >
	{
		static UniformType constexpr value = UniformType::eFloat;
	};
	template<>
	struct UniformTyper< int32_t >
	{
		static UniformType constexpr value = UniformType::eInt;
	};
	template<>
	struct UniformTyper< uint32_t >
	{
		static UniformType constexpr value = UniformType::eUInt;
	};
	template<>
	struct UniformTyper< c3d::Point2f >
	{
		static UniformType constexpr value = UniformType::eVec2f;
	};
	template<>
	struct UniformTyper< c3d::Point3f >
	{
		static UniformType constexpr value = UniformType::eVec3f;
	};
	template<>
	struct UniformTyper< c3d::Point4f >
	{
		static UniformType constexpr value = UniformType::eVec4f;
	};
	template<>
	struct UniformTyper< c3d::Point2i >
	{
		static UniformType constexpr value = UniformType::eVec2i;
	};
	template<>
	struct UniformTyper< c3d::Point3i >
	{
		static UniformType constexpr value = UniformType::eVec3i;
	};
	template<>
	struct UniformTyper< c3d::Point4i >
	{
		static UniformType constexpr value = UniformType::eVec4i;
	};
	template<>
	struct UniformTyper< c3d::Point2ui >
	{
		static UniformType constexpr value = UniformType::eVec2ui;
	};
	template<>
	struct UniformTyper< c3d::Point3ui >
	{
		static UniformType constexpr value = UniformType::eVec3ui;
	};
	template<>
	struct UniformTyper< c3d::Point4ui >
	{
		static UniformType constexpr value = UniformType::eVec4ui;
	};
	template<>
	struct UniformTyper< c3d::Matrix4x4f >
	{
		static UniformType constexpr value = UniformType::eMat4f;
	};

	class UniformValueBase
	{
	protected:
		UniformValueBase( wxString const & name
			, UniformType type
			, bool isTracked
			, bool isRanged )
			: m_name{ name }
			, m_type{ type }
			, m_isTracked{ isTracked }
			, m_isRanged{ isRanged }
		{
		}

	public:
		virtual ~UniformValueBase() = default;

		inline wxString const & getName()const
		{
			return m_name;
		}

		inline UniformType getType()const
		{
			return m_type;
		}

		inline bool isTracked()const
		{
			return m_isTracked;
		}

		inline bool isRanged()const
		{
			return m_isRanged;
		}

	private:
		wxString m_name;
		UniformType m_type;
		bool m_isTracked;
		bool m_isRanged;
	};

	template< typename T >
	class UniformValue
		: public UniformValueBase
	{
	public:
		using ValueType = T;
		using RefType = ValueType &;
		using ConstRefType = ValueType const &;
		static bool constexpr IsTracked = false;
		static bool constexpr IsRanged = false;

	public:
		UniformValue( wxString const & name
			, RefType value )
			: UniformValueBase{ name, UniformTyper< ValueType >::value, IsTracked, IsRanged }
			, m_value{ value }
		{
		}

		inline ConstRefType getValue()const
		{
			return m_value;
		}

		inline RefType getValue()
		{
			return m_value;
		}

	private:
		RefType m_value;
	};

	template< typename T >
	class UniformValue< c3d::ChangeTracked< T > >
		: public UniformValueBase
	{
	public:
		using ValueType = T;
		using RefType = c3d::ChangeTracked< ValueType > & ;
		using ConstRefType = ValueType const &;
		static bool constexpr IsTracked = true;
		static bool constexpr IsRanged = false;

	public:
		UniformValue( wxString const & name
			, RefType & value
			, [[maybe_unused]] bool isTracked )
			: UniformValueBase{ name, UniformTyper< ValueType >::value, IsTracked, IsRanged }
			, m_value{ value }
		{
		}

		inline ConstRefType getValue()const
		{
			return m_value.value();
		}

		inline RefType getValue()
		{
			return m_value;
		}

	private:
		RefType m_value;
	};

	template< typename T >
	class UniformValue< c3d::RangedValue< T > >
		: public UniformValueBase
	{
	public:
		using ValueType = T;
		using RefType = c3d::RangedValue< ValueType > &;
		using ConstRefType = ValueType const &;
		static bool constexpr IsTracked = true;
		static bool constexpr IsRanged = false;

	public:
		UniformValue( wxString const & name
			, RefType value )
			: UniformValueBase{ name, UniformTyper< ValueType >::value, IsTracked, IsRanged }
			, m_value{ value }
		{
		}

		inline ConstRefType getValue()const
		{
			return m_value.value();
		}

		inline RefType getValue()
		{
			return m_value;
		}

	private:
		RefType m_value;
	};

	template< typename T >
	class UniformValue< c3d::ChangeTracked< c3d::RangedValue< T > > >
		: public UniformValueBase
	{
	public:
		using ValueType = T;
		using RefType = c3d::ChangeTracked< c3d::RangedValue< ValueType > > &;
		using ConstRefType = ValueType const &;
		static bool constexpr IsTracked = true;
		static bool constexpr IsRanged = false;

	public:
		UniformValue( wxString const & name
			, RefType value )
			: UniformValueBase{ name, UniformTyper< ValueType >::value, IsTracked, IsRanged }
			, m_value{ value }
		{
		}

		inline ConstRefType getValue()const
		{
			return m_value.value().value();
		}

		inline RefType getValue()
		{
			return m_value;
		}

	private:
		RefType m_value;
	};
	
	template< typename T >
	class TrackedUniformValue
		: public UniformValueBase
	{
	public:
		using UniformValueType = UniformValue< T >;
		using ValueType = typename UniformValueType::ValueType;
		using RefType = typename UniformValueType::RefType;
		using ConstRefType = typename UniformValueType::ConstRefType;
		static bool constexpr IsTracked = true;
		static bool constexpr IsRanged = UniformValueType::IsRanged;

	public:
		TrackedUniformValue( wxString const & name
			, RefType value
			, bool * control )
			: UniformValueBase{ name, UniformTyper< ValueType >::value, IsTracked, IsRanged }
			, m_value{ value }
			, m_control{ control }
		{
		}

		inline ConstRefType getValue()const
		{
			return m_value;
		}

		inline RefType getValue()
		{
			return m_value;
		}

	private:
		RefType m_value;
		bool * m_control;
	};

	template< typename ValueT >
	c3d::RawUniquePtr< UniformValueBase > makeUniformValue( wxString const & name
		, ValueT & value )
	{
		return c3d::makeRawUnique< UniformValue< ValueT > >( name, value );
	}
	
	template< typename ValueT, typename ControlT >
	c3d::RawUniquePtr< UniformValueBase > makeUniformValue( wxString const & name
		, ValueT & value
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > const & )
	{
		return c3d::makeRawUnique< UniformValue< ValueT > >( name, value );
	}

	struct UniformBufferValues
	{
		explicit UniformBufferValues( c3d::String const & pname
			, VkShaderStageFlags pstages = {}
			, c3d::Vector< c3d::RawUniquePtr< UniformValueBase > > puniforms = {} )
			: name{ pname }
			, stages{ pstages }
			, uniforms{ c3d::move( puniforms ) }
		{
		}

		UniformBufferValues( UniformBufferValues const & ) = delete;
		UniformBufferValues( UniformBufferValues && ) = default;
		UniformBufferValues & operator=( UniformBufferValues const & ) = delete;
		UniformBufferValues & operator=( UniformBufferValues && ) = default;

		wxString name;
		VkShaderStageFlags stages;
		c3d::Vector< c3d::RawUniquePtr< UniformValueBase > > uniforms;
	};

	struct ShaderEntryPoint
	{
		ShaderEntryPoint( ast::Shader * shader
			, c3d::SpirVShader source
			, ast::EntryPoint entryPoint )
			: shader{ shader }
			, source{ c3d::move( source ) }
			, entryPoint{ entryPoint }
		{
		}

		ast::Shader * shader;
		c3d::SpirVShader source;
		ast::EntryPoint entryPoint;
	};

	struct ShaderSource
	{
		explicit ShaderSource( c3d::String pname
			, c3d::Vector< ShaderEntryPoint > psources = {}
			, c3d::Vector< UniformBufferValues > pubos = {} )
			: name{ c3d::move( pname ) }
			, sources{ c3d::move( psources ) }
			, ubos{ c3d::move( pubos ) }
		{
		}

		ShaderSource( ShaderSource const & ) = delete;
		ShaderSource( ShaderSource && ) = default;
		ShaderSource & operator=( ShaderSource const & ) = delete;
		ShaderSource & operator=( ShaderSource && ) = default;

		c3d::String name;
		c3d::Vector< ShaderEntryPoint > sources;
		c3d::Vector< UniformBufferValues > ubos;
	};

	using ShaderSources = c3d::Vector< ShaderSource >;
}

#endif
