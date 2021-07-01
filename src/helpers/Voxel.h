#ifndef VOXEL_H
#define VOXEL_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

typedef unsigned char byte;

struct Voxel{
    int x;
    int y;
    int z;
};

struct BinvoxData{
    int version;
    int width, height, depth;
    int size;
    int voxel_count;
    int surface_voxel_count;
    float tx, ty, tz;
    float scale;
    bool*** voxels;
    std::vector<Voxel> surface_voxels;
};

/* Init 3 dimentional array */
bool*** init3DArray(int width, int height, int depth){
    bool*** voxels = new bool**[width];
    for (int w = 0; w < width; w++){
        voxels[w] = new bool*[height];
        for (int h = 0; h < height; h++){
            voxels[w][h] = new bool[depth];
            for (int d = 0; d < depth; d++){
                voxels[w][h][d] = false;
            }
        }
    }
    return voxels;
}

/* Load BinvoxData from file */
int read_binvox(std::string filespec, BinvoxData* data){
    std::ifstream *input = new std::ifstream(filespec.c_str(), std::ios::in | std::ios::binary);

    //
    // read header
    //
    std::string line;
    *input >> line;  // #binvox
    if (line.compare("#binvox") != 0) {
        std::cout << "Error: first line reads [" << line << "] instead of [#binvox]" << std::endl;
        delete input;
        return 0;
    }
    *input >> data->version;
    
    data->depth = -1;
    int done = 0;
    while(input->good() && !done) {
        *input >> line;
        if (line.compare("data") == 0) done = 1;
        else if (line.compare("dim") == 0) {
            *input >> data->depth >> data->height >> data->width;
            std::cout << "Size = " << data->depth << " " << data->height << " " << data->width << std::endl;
        }
        else if (line.compare("translate") == 0) {
            *input >> data->tx >> data->ty >> data->tz;
        }
        else if (line.compare("scale") == 0) {
            *input >> data->scale;
        }
        else {
            std::cout << "  unrecognized keyword [" << line << "], skipping" << std::endl;
            char c;
            do {  // skip until end of line
                c = input->get();
            } while(input->good() && (c != '\n'));

        }
    }
    if (!done) {
        std::cout << "  error reading header" << std::endl;
        return 0;
    }
    if (data->depth == -1) {
        std::cout << "  missing dimensions in header" << std::endl;
        return 0;
    }

    data->size = data->width * data->height * data->depth;
    data->voxels = init3DArray(data->width, data->height, data->depth);

    //
    // read voxel data
    //
    byte value;
    byte count;
    int index = 0;
    int end_index = 0;
    int nr_voxels = 0;

    input->unsetf(std::ios::skipws);  // need to read every byte now (!)
    *input >> value;  // read the linefeed char

    while((end_index < data->size) && input->good()) {
        *input >> value >> count;

        if (input->good()) {
            end_index = index + count;
            if (end_index > data->size) return 0;
            for(int i=index; i < end_index; i++) {
                int z = i / (data->width * data->height);
                int idx = i - (z * data->width * data->height);
                int y = idx / data->width;
                int x = idx % data->width;
                data->voxels[x][y][z] = value;
            }

            if (value) nr_voxels += count;
            index = end_index;
        }  // if file still ok

    }
    data->voxel_count = nr_voxels;
    input->close();
    std::cout << "Total = " << nr_voxels << " voxels" << std::endl;
    return 1;
}

#endif