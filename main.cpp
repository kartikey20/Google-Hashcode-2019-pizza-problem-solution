#include "pizza_cutter.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


pizza_cutter parse_input_file(const std::string& file_name);
void print_results_to_file(const std::vector<pizza_slice>& res, const std::string& file_name);

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: google_pizza <input_file_name> <output_file_name>\n";
    }

    try {
        pizza_cutter pc = parse_input_file(argv[1]);
        auto results = pc.solve();
        print_results_to_file(results, argv[2]);
    } catch(const std::ios_base::failure& ios_exc) {
        std::cout << ios_exc.what() << std::endl;
        return 1;
    }

    return 0;
}

pizza_cutter parse_input_file(const std::string& file_name) {
    std::ifstream ifs(file_name);

    if (!ifs.is_open()) {
        throw std::ios_base::failure("Cannot open the input file.");
    }

    int rows, cols, min_each, max_total;
    ifs >> rows >> cols >> min_each >> max_total;
    ifs.get();

    pizza_t pizza;
    for (int i = 0; i < rows; ++i) {
        pizza_row_t row;
        row.reserve(cols);

        for (int j = 0; j < cols; ++j) {
            char val = ifs.get();
            row.push_back(pizza_cell{val});
        }

        pizza.push_back(std::move(row));
        ifs.get();
    }

    return pizza_cutter(min_each, max_total, std::move(pizza));
}

void print_results_to_file(const std::vector<pizza_slice>& res, const std::string& file_name) {
    std::ofstream ofs(file_name);

    if (!ofs.is_open()) {
        throw std::ios_base::failure("Cannot open the output file.");
    }

    ofs << res.size() << std::endl;

    for (auto& slice : res) {
        ofs << slice << std::endl;
    }
}
