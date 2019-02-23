#ifndef GOOGLE_PIZZA_PIZZA_CUTTER_H
#define GOOGLE_PIZZA_PIZZA_CUTTER_H

#include <array>
#include <optional>
#include <ostream>
#include <tuple>
#include <vector>


struct pizza_cell
{
    char ingr;
    int owner = -1;
};

using pizza_row_t = std::vector<pizza_cell>;
using pizza_t = std::vector<pizza_row_t>;

struct pizza_slice
{
    int row1;
    int col1;
    int row2;
    int col2;
};

std::ostream& operator<<(std::ostream& os, pizza_slice ps) noexcept;

class pizza_cutter
{
public:
    pizza_cutter(int min_each, int max_total, const pizza_t& pizza) noexcept
            : pizza_{pizza}, min_each_{min_each}, max_total_{max_total}, slices_{} {}

    pizza_cutter(int min_each, int max_total, pizza_t&& pizza) noexcept
            : pizza_{std::move(pizza)}, min_each_{min_each}, max_total_{max_total}, slices_{} {}

    std::vector<pizza_slice> solve() noexcept;

private:
    enum class direction {right, down, left, up};
    constexpr static std::array<direction, 4> directions
            {direction::right, direction::down, direction::left, direction::up};
    int max_total_;
    int min_each_;
    pizza_t pizza_;
    std::vector<pizza_slice> slices_;

    void try_to_create_slices_for_each_point() noexcept;
    void try_to_extend_all_slices() noexcept;
    std::optional<pizza_slice> create_slice_for(int row, int col) const noexcept;
    void mark_slice(pizza_slice ps, int slice_id) noexcept;
    std::optional<pizza_slice> extend_slice(pizza_slice ps, direction dir) const noexcept;
    bool is_new_part_valid(pizza_slice new_part) const noexcept;
    std::tuple<int,int> count_ingredients(pizza_slice ps) const noexcept;

    bool is_minimum_ingredients(int i1, int i2) const noexcept
    {
        return i1 >= min_each_ && i2 >= min_each_;
    }

    bool is_too_large(int i1, int i2) const noexcept
    {
        return i1 + i2 > max_total_;
    }
};

#endif //GOOGLE_PIZZA_PIZZA_CUTTER_H
