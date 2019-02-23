#include "pizza_cutter.h"

#include <iostream>



std::ostream& operator<<(std::ostream& os, pizza_slice ps) noexcept
{
    os << ps.row1 << ' ' << ps.col1 << ' ' << ps.row2 << ' ' << ps.col2;
    return os;
}

std::vector<pizza_slice> pizza_cutter::solve() noexcept
{
    try_to_create_slices_for_each_point();
    for (int i = 0; i < 4; ++i)
        try_to_extend_all_slices();

    return std::move(slices_);
}

void pizza_cutter::try_to_create_slices_for_each_point() noexcept
{
    for (int row = 0; row < pizza_.size(); row++) {
        for (int col = 0; col < pizza_[0].size(); col++) {
            auto slice = create_slice_for(row, col);
            if (!slice) continue;
            int next_slice_id = slices_.size();
            mark_slice(slice.value(), next_slice_id);
            slices_.push_back(slice.value());
        }
    }
}

std::optional<pizza_slice> pizza_cutter::extend_slice(pizza_slice ps, direction dir) const noexcept
{
    switch(dir) {
        case direction::right:
            if (is_new_part_valid(pizza_slice{ps.row1, ps.col2 + 1, ps.row2, ps.col2 + 1}))
                return pizza_slice{ps.row1, ps.col1, ps.row2, ps.col2 + 1};
            break;
        case direction::down:
            if (is_new_part_valid(pizza_slice{ps.row2 + 1, ps.col1, ps.row2 + 1, ps.col2}))
                return pizza_slice{ps.row1, ps.col1, ps.row2 + 1, ps.col2};
            break;
        case direction::left:
            if (is_new_part_valid(pizza_slice{ps.row1, ps.col1 - 1, ps.row2, ps.col1 - 1}))
                return pizza_slice{ps.row1, ps.col1 - 1, ps.row2, ps.col2};
            break;
        case direction::up:
            if (is_new_part_valid(pizza_slice{ps.row1 - 1, ps.col1, ps.row1 - 1, ps.col2}))
                return pizza_slice{ps.row1 - 1, ps.col1, ps.row2, ps.col2};
            break;
    }
    return std::nullopt;
}

bool pizza_cutter::is_new_part_valid(pizza_slice new_part) const noexcept
{
    if (new_part.col1 < 0 || new_part.col2 >= pizza_[0].size() ||
        new_part.row1 < 0 || new_part.row2 >= pizza_.size()) {
        return false;
    }

    for (int row = new_part.row1; row <= new_part.row2; ++row) {
        for (int col = new_part.col1; col <= new_part.col2; ++col) {
            if (pizza_[row][col].owner != -1)
                return false;
        }
    }

    return true;
}

std::optional<pizza_slice> pizza_cutter::create_slice_for(int row, int col) const noexcept
{
    if (pizza_[row][col].owner != -1) return std::nullopt;

    pizza_slice slice{row, col, row, col};

    bool look_for_next = true;
    while (look_for_next) {
        bool could_be_extended = false;
        bool all_is_too_large = true;

        pizza_slice candidate;
        for (auto d : directions) {
            auto new_slice = extend_slice(slice, d);

            if (new_slice) {
                could_be_extended = true;
                auto [mushrooms, tomatoes] = count_ingredients(new_slice.value());

                if (is_too_large(mushrooms, tomatoes))
                    continue;

                all_is_too_large = false;

                if (is_minimum_ingredients(mushrooms, tomatoes))
                    return new_slice;

                candidate = new_slice.value();
            }
        }
        slice = candidate;
        look_for_next = could_be_extended && !all_is_too_large;
    }

    return std::nullopt;
}

void pizza_cutter::mark_slice(pizza_slice ps, int slice_id) noexcept
{
    for (int row = ps.row1; row <= ps.row2; ++row) {
        for (int col = ps.col1; col <= ps.col2; ++col) {
            pizza_[row][col].owner = slice_id;
        }
    }
}

std::tuple<int,int> pizza_cutter::count_ingredients(pizza_slice ps) const noexcept
{
    int mushrooms = 0;
    int tomatoes = 0;

    for (int row = ps.row1; row <= ps.row2; ++row) {
        for (int col = ps.col1; col <= ps.col2; ++col) {
            if (pizza_[row][col].ingr == 'M') {
                ++mushrooms;
            } else {
                ++tomatoes;
            }
        }
    }

    return std::tie(mushrooms, tomatoes);
}

void pizza_cutter::try_to_extend_all_slices() noexcept
{
    for (int i = 0; i < slices_.size(); ++i) {
        for (auto d : directions) {
            auto new_slice = extend_slice(slices_[i], d);
            if (!new_slice) continue;

            auto [mushrooms, tomatoes] = count_ingredients(new_slice.value());

            if (is_too_large(mushrooms, tomatoes))
                continue;
            mark_slice(new_slice.value(), i);
            slices_[i] = new_slice.value();
        }
    }
}
