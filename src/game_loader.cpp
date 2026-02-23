#include "game_loader.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <limits>

using Matrix = std::vector<std::vector<double>>;

static Matrix readCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filepath);

    Matrix matrix;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cell;
        std::vector<double> row;

        while (std::getline(ss, cell, ';')) {
            row.push_back(std::stod(cell));
        }

        if (!matrix.empty() && row.size() != matrix[0].size())
            throw std::runtime_error("Inconsistent row size in CSV");

        matrix.push_back(row);
    }

    if (matrix.empty())
        throw std::runtime_error("Empty matrix");

    return matrix;
}

GameLP loadGameFromCSV(const std::string& filepath) {
    Matrix A = readCSV(filepath);

    const int m = static_cast<int>(A.size());        // строки
    const int n = static_cast<int>(A[0].size());     // столбцы

    // shifting
    double min_val = std::numeric_limits<double>::max();
    for (const auto& row : A)
        for (double v : row)
            min_val = std::min(min_val, v);

    double shift = 0.0;
    if (min_val <= 0.0)
        shift = -min_val + 1.0;

    if (shift != 0.0) {
        for (auto& row : A)
            for (auto& v : row)
                v += shift;
    }

    // creating model
    HighsModel model;
    model.lp_.num_col_ = m;
    model.lp_.num_row_ = n;
    model.lp_.sense_ = ObjSense::kMinimize;
    model.lp_.offset_ = 0.0;

    model.lp_.col_cost_.assign(m, 1.0);

    model.lp_.col_lower_.assign(m, 0.0);
    model.lp_.col_upper_.assign(m, kHighsInf);

    model.lp_.row_lower_.assign(n, 1.0);
    model.lp_.row_upper_.assign(n, kHighsInf);

    model.lp_.a_matrix_.format_ = MatrixFormat::kColwise;

    model.lp_.a_matrix_.start_.resize(m + 1);
    std::vector<HighsInt>& index = model.lp_.a_matrix_.index_;
    std::vector<double>& value = model.lp_.a_matrix_.value_;

    HighsInt nnz = 0;
    for (int i = 0; i < m; ++i) {
        model.lp_.a_matrix_.start_[i] = nnz;

        for (int j = 0; j < n; ++j) {
            double a_ij = A[i][j];
            if (a_ij != 0.0) {
                index.push_back(j);   // строка
                value.push_back(a_ij);
                nnz++;
            }
        }
    }
    model.lp_.a_matrix_.start_[m] = nnz;

    GameLP result;
    result.model = std::move(model);
    result.shift = shift;
    return result;
}