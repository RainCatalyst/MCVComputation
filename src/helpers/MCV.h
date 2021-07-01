#ifndef MCV_H
#define MCV_H

#include "Voxel.h"

/* Check if two voxels are reachable along the straight line */
bool isReachable(int x1, int y1, int z1, const int x2, const int y2, const int z2, BinvoxData* data){
    int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
    int point[3];

    point[0] = x1;
    point[1] = y1;
    point[2] = z1;
    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    x_inc = (dx < 0) ? -1 : 1;
    l = abs(dx);
    y_inc = (dy < 0) ? -1 : 1;
    m = abs(dy);
    z_inc = (dz < 0) ? -1 : 1;
    n = abs(dz);
    dx2 = l << 1;
    dy2 = m << 1;
    dz2 = n << 1;

    if ((l >= m) && (l >= n)) {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for (i = 0; i < l; i++) {
            if (!data->voxels[point[0]][point[1]][point[2]])
                return false;
            if (err_1 > 0) {
                point[1] += y_inc;
                err_1 -= dx2;
            }
            if (err_2 > 0) {
                point[2] += z_inc;
                err_2 -= dx2;
            }
            err_1 += dy2;
            err_2 += dz2;
            point[0] += x_inc;
        }
    } else if ((m >= l) && (m >= n)) {
        err_1 = dx2 - m;
        err_2 = dz2 - m;
        for (i = 0; i < m; i++) {
            if (!data->voxels[point[0]][point[1]][point[2]])
                return false;
            if (err_1 > 0) {
                point[0] += x_inc;
                err_1 -= dy2;
            }
            if (err_2 > 0) {
                point[2] += z_inc;
                err_2 -= dy2;
            }
            err_1 += dx2;
            err_2 += dz2;
            point[1] += y_inc;
        }
    } else {
        err_1 = dy2 - n;
        err_2 = dx2 - n;
        for (i = 0; i < n; i++) {
            if (!data->voxels[point[0]][point[1]][point[2]])
                return false;
            if (err_1 > 0) {
                point[1] += y_inc;
                err_1 -= dz2;
            }
            if (err_2 > 0) {
                point[0] += x_inc;
                err_2 -= dz2;
            }
            err_1 += dy2;
            err_2 += dx2;
            point[2] += z_inc;
        }
    }
    return true;
}

/* Count the number of reachable voxels on the surface */
int countReachable(int ax, int ay, int az, BinvoxData* data, int start=0){
    int count = 0;
    for (int i = start; i<data->surface_voxel_count; i++){
        Voxel voxel = data->surface_voxels[i];
        if (isReachable(ax, ay, az, voxel.x, voxel.y, voxel.z, data))
            count++;
    }
    return count;
}

/* Check if voxel is empty or outside the grid */
bool isFree(int x, int y, int z, BinvoxData* data){
    return x < 0 || y < 0 || z < 0 || x >= data->width || y >= data->height || z >= data->depth || data->voxels[x][y][z] == 0;
}

/* Check if voxel lies on the surface */
bool isSurfaceVoxel(int x, int y, int z, BinvoxData* data){
    return data->voxels[x][y][z] != 0 && (isFree(x - 1, y, z, data) || isFree(x + 1, y, z, data)
        || isFree(x, y - 1, z, data) || isFree(x, y + 1, z, data)
        || isFree(x, y, z - 1, data) || isFree(x, y, z + 1, data));
}

/* Find and store the surface voxels */
void findSurfaceVoxels(BinvoxData* data){
    int count = 0;
    for (int x = 0; x < data->width; x++) {
        for (int y = 0; y < data->height; y++) {
            for (int z = 0; z < data->depth; z++) {
                if (isSurfaceVoxel(x, y, z, data)) {
                    data->surface_voxels.push_back({x, y, z});
                    count++;
                }
            }
        }
    }
    data->surface_voxel_count = count;
    std::cout << "Surface voxels = " << count << std::endl;
}

/* Compute the MCV value */
double computeMCV(BinvoxData* data){
    double totalValue = 0;
    for (int i = 0; i<data->surface_voxel_count; i++){
        Voxel voxel = data->surface_voxels[i];
        int reached = countReachable(voxel.x, voxel.y, voxel.z, data, i);
        // Integer overflow?
        totalValue += reached * 2;
    }
    return (totalValue) / (double)(data->surface_voxel_count * data->surface_voxel_count);
}

#endif