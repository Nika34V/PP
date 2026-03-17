#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <string>
#include <windows.h>

using namespace std;
using namespace chrono;

vector<vector<double>> loadMatrixData(const string& path, int& dimension) {
    ifstream input(path);
    if (!input.is_open()) {
        cerr << "Ошибка: невозможно открыть файл " << path << endl;
        exit(1);
    }
    
    input >> dimension;
    vector<vector<double>> mat(dimension, vector<double>(dimension));
    
    for (int row = 0; row < dimension; ++row) {
        for (int col = 0; col < dimension; ++col) {
            input >> mat[row][col];
        }
    }
    
    input.close();
    return mat;
}

void saveMatrixToFile(const string& path, const vector<vector<double>>& mat) {
    ofstream output(path);
    if (!output.is_open()) {
        cerr << "Ошибка: невозможно создать файл " << path << endl;
        exit(1);
    }
    
    size_t dimension = mat.size();  // используем size_t
    output << dimension << endl;
    
    for (size_t row = 0; row < dimension; ++row) {  // size_t для индексов
        for (size_t col = 0; col < dimension; ++col) {
            output << fixed << setprecision(6) << mat[row][col] << " ";
        }
        output << endl;
    }
    
    output.close();
}

void savePerformanceReport(const string& path, int dim, long long microseconds) {
    ofstream output(path);
    if (!output.is_open()) {
        cerr << "Ошибка: невозможно создать отчет " << path << endl;
        exit(1);
    }
    
    double milliseconds = microseconds / 1000.0;
    long long totalOps = 2LL * dim * dim * dim;
    double gflops = totalOps / (microseconds / 1e6) / 1e6;
    
    output << "\nОТЧЕТ ОБ УМНОЖЕНИИ МАТРИЦ\n";
    output << "Размерность: " << dim << " x " << dim << "\n";
    output << "\nХАРАКТЕРИСТИКИ ЗАДАЧИ:\n";
    output << "  Количество элементов в матрице: " << dim * dim << "\n";
    output << "  Суммарное число элементов (A+B+C): " << 3 * dim * dim << "\n";
    output << "  Занимаемая память: " << fixed << setprecision(2) 
         << (3.0 * dim * dim * sizeof(double)) / 1024.0 << " КБ\n";
    output << "  Количество операций: " << totalOps << " (сложения + умножения)\n";
    output << "\nВРЕМЕННЫЕ ХАРАКТЕРИСТИКИ:\n";
    output << "  " << microseconds << " микросекунд\n";
    output << "  " << fixed << setprecision(3) << milliseconds << " миллисекунд\n";
    output << "  " << fixed << setprecision(6) << milliseconds / 1000.0 << " секунд\n";
    output << "\nПРОИЗВОДИТЕЛЬНОСТЬ:\n";
    output << "  " << fixed << setprecision(2) << gflops << " MFLOPS\n";
    
    output.close();
}

vector<vector<double>> computeMatrixProduct(const vector<vector<double>>& X, 
                                           const vector<vector<double>>& Y) {
    size_t dim = X.size();  // используем size_t
    vector<vector<double>> Z(dim, vector<double>(dim, 0.0));
    
    for (size_t row = 0; row < dim; ++row) {  // size_t для индексов
        for (size_t col = 0; col < dim; ++col) {
            for (size_t idx = 0; idx < dim; ++idx) {
                Z[row][col] += X[row][idx] * Y[idx][col];
            }
        }
    }
    
    return Z;
}

vector<vector<double>> createRandomSquareMatrix(int dim) {
    vector<vector<double>> mat(dim, vector<double>(dim));
    for (int row = 0; row < dim; ++row) {
        for (int col = 0; col < dim; ++col) {
            mat[row][col] = rand() % 10;
        }
    }
    return mat;
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    string pathA, pathB, pathC, pathReport;
    int dimension = 0;
    bool randomMode = false;
    
    if (argc == 1) {
        randomMode = true;
        dimension = 5;
        pathReport = "results.txt";
        cout << "Режим генерации: случайные матрицы размером " << dimension << "x" << dimension << endl;
        
    } else if (argc == 5) {
        pathA = argv[1];
        pathB = argv[2];
        pathC = argv[3];
        pathReport = argv[4];
        cout << "Режим загрузки: чтение матриц из файлов" << endl;
        
    } else if (argc == 4 && string(argv[1]) == "-r") {
        randomMode = true;
        dimension = stoi(argv[2]);
        pathReport = argv[3];
        cout << "Режим генерации: случайные матрицы размером " << dimension << "x" << dimension << endl;
        
    } else {
        cerr << "\nФОРМАТ ВЫЗОВА:\n";
        cerr << "  " << argv[0] << "                     # генерация матриц 5x5\n";
        cerr << "  " << argv[0] << " -r N report.txt     # генерация NxN, отчет в report.txt\n";
        cerr << "  " << argv[0] << " A.txt B.txt C.txt R.txt  # умножение из файлов\n";
        return 1;
    }
    
    vector<vector<double>> X, Y;
    
    if (randomMode) {
        srand(static_cast<unsigned int>(time(nullptr)));  // явное приведение типа
        X = createRandomSquareMatrix(dimension);
        Y = createRandomSquareMatrix(dimension);
        
        saveMatrixToFile("matrix_A.txt", X);
        saveMatrixToFile("matrix_B.txt", Y);
        cout << "Созданы файлы с матрицами:" << endl;
        cout << "  matrix_A.txt" << endl;
        cout << "  matrix_B.txt" << endl;
    } else {
        int dimA, dimB;
        X = loadMatrixData(pathA, dimA);
        Y = loadMatrixData(pathB, dimB);
        
        if (dimA != dimB) {
            cerr << "Ошибка: размерности матриц не совпадают!" << endl;
            return 1;
        }
        dimension = dimA;
    }
    
    auto startTime = high_resolution_clock::now();
    vector<vector<double>> Z = computeMatrixProduct(X, Y);
    auto endTime = high_resolution_clock::now();
    
    auto elapsed = duration_cast<microseconds>(endTime - startTime);
    
    if (randomMode) {
        saveMatrixToFile("matrix_C.txt", Z);
        cout << "Результат сохранен в matrix_C.txt" << endl;
    } else {
        saveMatrixToFile(pathC, Z);
        cout << "Результат: " << pathC << endl;
    }
    
    savePerformanceReport(pathReport, dimension, elapsed.count());
    cout << "Отчет сохранен в " << pathReport << endl;
    
    cout << "\nСВОДКА РЕЗУЛЬТАТОВ" << endl;
    cout << "  Размерность: " << dimension << "x" << dimension << endl;
    cout << "  Время счета: " << elapsed.count() << " мкс (" 
         << fixed << setprecision(3) << elapsed.count() / 1000.0 << " мс)" << endl;
    cout << "  Детали в файле: " << pathReport << endl;
    
    return 0;
}