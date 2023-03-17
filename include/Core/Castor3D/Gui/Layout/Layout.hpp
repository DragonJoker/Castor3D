/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Layout_H___
#define ___C3D_Layout_H___

#include "Castor3D/Gui/Layout/LayoutItemFlags.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class Spacer
	{
	public:
		//! Set the spacer size to this value to create a dynamic spacer.
		static uint32_t constexpr Dynamic = ~( 0u );

	public:
		/** Constructor
		*\param[in] size
		*	The spacer size.
		*/
		explicit Spacer( uint32_t size = Dynamic )noexcept
			: m_size{ size }
		{
		}

		/**
		*\return	\p true if spacer size varies
		*/
		bool isDynamic()const noexcept
		{
			return m_size == Dynamic;
		}

		/**
		*\return	The spacer size.
		*/
		uint32_t getSize()const noexcept
		{
			return m_size;
		}

	private:
		uint32_t m_size;
	};

	class Layout
		: public castor::Named
	{
	public:
		struct Item
		{
			enum Type
			{
				eControl,
				eSpacer,
			};

			union Value
			{
				ControlRPtr control;
				SpacerRPtr spacer;
			};

			/** Creates a control item.
			*\param[in] c
			*	The item control.
			*\param[in] f
			*	The item flags.
			*/
			explicit Item( Control & c
				, LayoutItemFlags f
				, OnControlChangedConnection o )noexcept
				: m_type{ eControl }
				, m_flags{ f }
				, m_connection{ std::move( o ) }
			{
				m_value.control = &c;
			}

			/** Creates a spacer item.
			*\param[in] s
			*	The item spacer.
			*/
			explicit Item( Spacer & s )noexcept
				: m_type{ eSpacer }
				, m_flags{}
			{
				m_value.spacer = &s;
			}

			/** \return	\p true if this item is a control, \p nullptr if not.
			*/
			auto isControl()const noexcept
			{
				return m_type == eControl;
			}

			/** \return	The control if this item is a control, \p nullptr if not.
			*/
			auto control()const noexcept
			{
				return isControl()
					? m_value.control
					: nullptr;
			}

			/** \return	The item flags.
			*/
			auto flags()const noexcept
			{
				return m_flags;
			}

			/** \return	\p true if this item is a spacer, \p nullptr if not.
			*/
			auto isSpacer()const noexcept
			{
				return m_type == eSpacer;
			}

			/** \return	The spacer if this item is a spacer, \p nullptr if not.
			*/
			auto spacer()const noexcept
			{
				return isSpacer()
					? m_value.spacer
					: nullptr;
			}

			/** \return	The size including padding.
			*/
			C3D_API castor::Size getPaddedSize()const noexcept;

			/** \return	The item horizontal alignment.
			*/
			auto hAlign()const noexcept
			{
				return flags().hAlign();
			}

			/** \return	The item vertical alignment.
			*/
			auto vAlign()const noexcept
			{
				return flags().vAlign();
			}

			/** \return	The item expand status.
			*/
			auto expand()const noexcept
			{
				return flags().expand();
			}

			/** \return	The item reserve space if hidden status.
			*/
			auto reserveSpaceIfHidden()const noexcept
			{
				return flags().reserveSpaceIfHidden();
			}

			/** \return	The padding value at given index.
			*/
			uint32_t padding( uint32_t i )const noexcept
			{
				return flags().padding( i );
			}

			/** \return	The padding size at given index.
			*/
			uint32_t paddingSize( uint32_t i )const noexcept
			{
				return flags().paddingSize( i );
			}

		private:
			Type m_type;
			Value m_value;
			LayoutItemFlags m_flags;
			OnControlChangedConnection m_connection;
		};

	public:
		/**@name General */
		//@{

		C3D_API virtual ~Layout()noexcept = default;
		C3D_API Layout( Layout const & )noexcept = delete;
		C3D_API Layout & operator=( Layout const & )noexcept = delete;
		C3D_API Layout( Layout && )noexcept = delete;
		C3D_API Layout & operator=( Layout && )noexcept = delete;

		/** Constructor
		*\param[in] typeName
		*	The layout type name.
		*/
		C3D_API explicit Layout( castor::String const & typeName
			, LayoutControl & container );
		/** Marks the layout as dirty.
		*/
		C3D_API void markDirty();

		//@}
		/**@name Controls management */
		//@{

		/** Adds a control.
		*\param[in] control
		*	The control.
		*\param[in] flags
		*	The layout flags for the control.
		*/
		C3D_API void addControl( Control & control
			, LayoutItemFlags flags = {} );

		/** Removes a control.
		*\param[in] control
		*	The control.
		*/
		C3D_API void removeControl( Control & control );

		/** Adds a space.
		*\param[in] size
		*	The space size
		*/
		C3D_API void addSpacer( uint32_t size = Spacer::Dynamic );

		//@}

		LayoutControl & getContainer()const noexcept
		{
			return m_container;
		}

	protected:
		/** Updates the layout and its controls.
		*/
		C3D_API void update();

	private:
		/** Updates the dependant data.
		*/
		virtual void doUpdate() = 0;

	protected:
		LayoutControl & m_container;
		std::vector< Item > m_items;
		std::vector< SpacerUPtr > m_spacers;
		CpuFrameEvent * m_event{};
		std::atomic_bool m_updating{ false };
	};
}

#endif
