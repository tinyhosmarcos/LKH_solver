#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <chrono>

using namespace std;
namespace fs = filesystem;

void read_CSV(string argv, vector<string>* vectorX, vector<string>* vectorY){
    ifstream file(argv);
    string line;
    char delimitator = ',';
    getline(file, line);

    while(getline(file, line)){
        stringstream stream(line);
        string index, x, y;
        getline(stream, index, delimitator);
        getline(stream, x, delimitator);
        getline(stream, y, delimitator);

        //cout << "Index:" << index << endl;
        //cout << "x: " << x << endl;
        //cout << "y: " << y << endl;

        (*vectorX).push_back(x);
        (*vectorY).push_back(y);
    }
}

void write_TSP(vector<string> vectorX, vector<string> vectorY){
    string filename("LKH-2.0.9/cities_tsp/cities.tsp");
    fstream outfile;

    outfile.open(filename, std::ios_base::out);
    if (!outfile.is_open()) {
        cout << "failed to open " << filename << '\n';
    } else {
        outfile << "NAME : traveling-santa-2018-prime-paths" << endl;
        outfile << "COMMENT : traveling-santa-2018-prime-paths" << endl;
        outfile << "TYPE : TSP" << endl;
        outfile << "DIMENSION : " << vectorX.size() << endl;
        outfile << "EDGE_WEIGHT_TYPE : EUC_2D" << endl;
        outfile << "NODE_COORD_SECTION" << endl;
        for(unsigned int i = 0; i < vectorX.size(); i++){
            outfile << i+1 << " " << vectorX[i] << " " << vectorY[i] << endl;
        }
        outfile << "EOF" << endl;
        //cout << "Done Writing!" << endl;
    }
}

void write_parameters(){
    string filename("LKH-2.0.9/params_par/params.par");
    fstream outfile;

    outfile.open(filename, std::ios_base::out);
    if (!outfile.is_open()) {
        cout << "failed to open " << filename << '\n';
    } else {
        outfile << "PROBLEM_FILE = cities_tsp/cities.tsp" << endl;
        outfile << "OUTPUT_TOUR_FILE = solution_csv/tsp_solution.csv" << endl;
        outfile << "SEED = 2018" << endl;
        outfile << "CANDIDATE_SET_TYPE = POPMUSIC" << endl;
        outfile << "INITIAL_PERIOD = 10000" << endl;
        outfile << "MAX_TRIALS = 1000" << endl;
        //cout << "Done Writing!" << endl;
    }
}

double score_tour(string filename){
    double fiscore;
    ifstream file(filename);
    string line;
    string delimitator = "Length = ";
    getline(file, line);
    getline(file, line);
    string score_value = line.substr(19);
    //cout << score << endl;
    return stod(score_value);
}

double serial_solver(vector<string> paths){
    double best_score = INT_MAX;

    for (unsigned int i = 0; i < paths.size(); i++){
        vector<string> x;
        vector<string> y;
        read_CSV(paths[i], &x, &y);
        write_TSP(x,y);
        write_parameters();
        system("cd .\\LKH-2.0.9 & .\\LKH params_par/params.par");
        double score = score_tour("LKH-2.0.9/solution_csv/tsp_solution.csv");
        //cout << "\nScore [" << i << "]: " << score << " ------------------here!"<< endl;

        // Determine if the score calculated is the best score
        if(score < best_score) best_score = score;
    }

    return best_score;
}

int main(){
    string path = "dataset"; // Folder containing the cvs. files
    vector<string> paths; // Store all cvs. files paths
    
    // Collect the paths of all csv files within dataset
    for (const auto & entry : fs::directory_iterator(path)){
        //cout << entry.path() << endl;
        string path_string = entry.path().string();
        paths.push_back(path_string);
    }


    double best_score;

    chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = chrono::high_resolution_clock::now();
    best_score = serial_solver(paths);
	end = chrono::high_resolution_clock::now();
    int64_t duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    cout << endl << setw(10) << "Duration: " + to_string(duration) + " s\n";
    
    cout << "Best score " << best_score << endl;

    // Write the final score
    string filename = "final_score.txt";
    fstream outfile;
    outfile.open(filename, std::ios_base::out);
    if (!outfile.is_open()) {
        cout << "failed to open " << filename << '\n';
    } else {
        outfile << best_score;
        //cout << "Done Writing!" << endl;
    }
    
    return 0;
}