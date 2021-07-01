#include <string>
#include <fstream>
#include <vector>

#include "helpers/Timer.h"
#include "helpers/MCV.h"

/* Benchmark the MCV computation using the path to the model */
void benchmark(std::string path){
    Timer clock;
    BinvoxData* data = new BinvoxData;
    if (read_binvox(path, data) != 1)
        return;
    clock.tock();
    std::cout << "------ Loading file = " << clock.duration().count() << " ms\n";
    clock.tick();
    findSurfaceVoxels(data);
    clock.tock();
    std::cout << "------ Finding surface voxels = " << clock.duration().count() << " ms\n";
    clock.tick();
    std::cout << "MCV: " << computeMCV(data) << std::endl;
    clock.tock();
    std::cout << "------ Computing MCV = " << clock.duration().count() << " ms\n";
    delete data;
}

int main() {
    std::string model_path;
    std::cout << "Model path:" << std::endl;
    std::getline(std::cin, model_path);
    benchmark(model_path);
    std::cin.get();
    return 0;
}