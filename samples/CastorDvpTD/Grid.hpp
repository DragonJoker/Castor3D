#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	struct PathNode
	{
		uint32_t m_x;
		uint32_t m_y;
	};

	using Path = std::vector< PathNode >;

	struct Cell
	{
		enum class State
			: uint8_t
		{
			Empty,
			Start,
			Tower,
			Path,
			Target,
			Invalid,
		};

		inline Cell( uint32_t p_x = 0u, uint32_t p_y = 0u, State p_state = State::Empty )
			: m_x{ p_x }
			, m_y{ p_y }
			, m_state{ p_state }
		{
		}

		uint32_t m_x{ 0u };
		uint32_t m_y{ 0u };
		State m_state{ State::Empty };
	};

	class Grid
	{
	public:
		Grid();
		~Grid();

		inline auto begin()const
		{
			return m_cells.begin();
		}

		inline auto begin()
		{
			return m_cells.begin();
		}

		inline auto end()const
		{
			return m_cells.end();
		}

		inline auto end()
		{
			return m_cells.end();
		}

		inline auto getWidth()const
		{
			return m_width;
		}

		inline auto getHeight()const
		{
			return m_height;
		}

		inline auto & operator()( uint32_t x, uint32_t y )const
		{
			return m_cells[x * m_height + y];
		}

		inline auto & operator()( uint32_t x, uint32_t y )
		{
			return m_cells[x * m_height + y];
		}

	private:
		constexpr static uint32_t m_width{ 32u };
		constexpr static uint32_t m_height{ 32u };
		using CellArray = std::array< Cell, m_width * m_height >;
		CellArray m_cells;
	};
}
