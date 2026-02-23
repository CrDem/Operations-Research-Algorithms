#include "Highs.h"
#include <iostream>
#include <string>

std::string projectRoot = PROJECT_ROOT;

#include "game_loader.h"

using namespace std;
int main() {
    
    GameLP game = loadGameFromCSV(projectRoot + "/data/A_100.csv");
    HighsModel& model = game.model;

    // Create a Highs instance
    Highs highs;
    HighsStatus return_status;
    
    // Pass the model to HiGHS
    return_status = highs.passModel(model);
    assert(return_status==HighsStatus::kOk);
    
    // Get a const reference to the LP data in HiGHS
    const HighsLp& lp = highs.getLp();
    
    // Solve the model
    return_status = highs.run();
    assert(return_status==HighsStatus::kOk);
    
    // Get the model status
    const HighsModelStatus& model_status = highs.getModelStatus();
    assert(model_status==HighsModelStatus::kOptimal);

    // output
    const HighsSolution& sol = highs.getSolution();
    double sum_x = 0.0;
    for (double x : sol.col_value)
        sum_x += x;

    double value = 1.0 / sum_x - game.shift;

    cout << "Game value: " << value << "\n\n";

    cout << "First player strategy (p):\n";
    for (size_t i = 0; i < sol.col_value.size(); ++i) {
        double p = sol.col_value[i] / sum_x;
        cout << "p[" << i << "] = " << p << "\n";
    }

    cout << "\nSecond player strategy (q):\n";
    double sum_y = 0.0;
    for (double y : sol.row_dual)
        sum_y += y;

    for (size_t j = 0; j < sol.row_dual.size(); ++j) {
        double q = sol.row_dual[j] / sum_y;
        cout << "q[" << j << "] = " << q << "\n";
    }

    return 0;
}