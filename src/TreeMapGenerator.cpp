#include "Mesh.hpp"
#include "ObjectGroup.hpp"
#include <GameGlobals.hpp>
#include <TreeMapGenerator.hpp>
#include <stb/stb_image.h>
#include <string>

#define GRID_SIZE 256
#define GRID_SQUARE_SIZE 16


unsigned char getValue(unsigned char *tex, int texH, int texW, int x, int y) {
  x = clamp(x, 0, texH - 1);
  y = clamp(y, 0, texW - 1);
  int c = (texW * x + y);

  return tex[c];
}

int getPixel(unsigned char *tex, int texH, int texW, vec2 uv) {
  if (!tex)
    return 0;

  uv = vec2(uv.y, uv.x);

  float x = texH * uv.x;
  float y = texW * uv.y;

  int c1 = getValue(tex, texH, texW, floor(x), floor(y));
  int c2 = getValue(tex, texH, texW, ceil(x), floor(y));
  int c3 = getValue(tex, texH, texW, floor(x), ceil(y));
  int c4 = getValue(tex, texH, texW, ceil(x), ceil(y));

  int r = (c1 + c2 + c3 + c4) / 4;

  return r;
}

void addTree(float x, float y, treeSizes trunkModels, treeSizes leavesModels, int size, ObjectGroupRef forest) {
  ModelRef trunk;
  ModelRef leaves;
  switch (size) {
    case 1:
    trunk = trunkModels.t0 -> copyWithSharedMesh();
    leaves = leavesModels.t0 -> copyWithSharedMesh();
    break;
    case 2:
    trunk = trunkModels.t1 -> copyWithSharedMesh();
    leaves = leavesModels.t1 -> copyWithSharedMesh();
    break;
    default:
    trunk = trunkModels.t2 -> copyWithSharedMesh();
    leaves = leavesModels.t2 -> copyWithSharedMesh();
  }
  trunk->state.setPosition(vec3(x, 0, y));
  leaves->state.setPosition(vec3(x, 1.4, y));
  forest->add(trunk);
  forest->add(leaves);
}

void generateTreeAtSpot(float x, float y, int val, treeSizes &trunkModels, treeSizes &leavesModels, ObjectGroupRef forest) {
  /* if val == 0 : 100% no tree
   * if val <= 0x55 : 50% small tree, 50% no tree
   * if 0x55 < val <= 0x80 : 60% small tree, 40% medium tree
   * if 0x80 < val <= 0xAA: 33% small, 33% medium, 33% large
   * if 0xAA < val <= 0XFE: 50% medium, 50% large
   * if val == 0x255: 20% medium, 80% large
   * */
   ModelRef trunk;
   ModelRef leaves;
   int size;
   float rand;

   if (val == 0 ) return;
   rand = GameGlobals::randomFloat01();
   if (val < 0x55) {
     if (rand > 0.5) addTree(x, y, trunkModels, leavesModels, 0, forest);
   }
   else if (val < 0x80) {
     if (rand < 0.6) {
       addTree(x, y, trunkModels, leavesModels, 0, forest);
     } else {
       addTree(x, y, trunkModels, leavesModels, 1, forest);
     }
   }
   else if (val < 0xAA) {
     if (rand < 0.33) {
       addTree(x, y, trunkModels, leavesModels, 0, forest);
     } else if (rand < 0.66) {
       addTree(x, y, trunkModels, leavesModels, 1, forest);
     } else {
       addTree(x, y, trunkModels, leavesModels, 2, forest);
     }
   } else if (val < 0xFE) {
     if (rand < 0.5) {
       addTree(x, y, trunkModels, leavesModels, 1, forest);
     } else {
       addTree(x, y, trunkModels, leavesModels, 2, forest);
     }
   } else {
     addTree(x, y, trunkModels, leavesModels, 2, forest);
   }
}

void generateTreesFromHeatMap(Scene &scene, std::string path, treeSizes trunks,
                              treeSizes leaves) {
  int mapWidth, mapHeight, n;
  stbi_uc *tex = stbi_load(path.c_str(), &mapWidth, &mapHeight, &n, 1);
  ObjectGroupRef forest = newObjectGroup();

  for (float x = 0; x < GRID_SIZE; x += GRID_SQUARE_SIZE) {
    for (float y = 0; y < GRID_SIZE; y += GRID_SQUARE_SIZE) {
      float xOnMap = (x / GRID_SIZE);
      float yOnMap = (y / GRID_SIZE);
      vec2 uv(xOnMap, yOnMap);  
      generateTreeAtSpot(x, y, getPixel(tex, mapWidth, mapHeight, uv), trunks, leaves, forest);
    }
  }
  scene.add(forest);
  stbi_image_free(tex);
}
