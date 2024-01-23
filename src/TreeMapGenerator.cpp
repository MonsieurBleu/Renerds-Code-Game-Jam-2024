#include "Mesh.hpp"
#include "ObjectGroup.hpp"
#include <TreeMapGenerator.hpp>
#include <stb/stb_image.h>
#include <string>

#define THRESHOLD 127
#define GRID_SIZE 256
#define GRID_SQUARE_SIZE 10

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

void generateTreeAtSpot(float x, float y, int val, ModelRef trunk,
                        ModelRef leaves, ObjectGroupRef forest) {
  if (val > THRESHOLD) {
    ModelRef trunkToPlace = trunk->copyWithSharedMesh();
    ModelRef leavesToPlace = leaves->copyWithSharedMesh();
    trunkToPlace->state.setPosition(vec3(x, 0, y)).scaleScalar(5.0);
    leavesToPlace->state.setPosition(vec3(x, 0, y)).scaleScalar(5.0);
    leavesToPlace->noBackFaceCulling = true;
    forest->add(trunkToPlace);
    forest->add(leavesToPlace);
  }
}

void generateTreesFromHeatMap(Scene &scene, std::string path, ModelRef trunk,
                              ModelRef leaves) {
  int mapWidth, mapHeight, n;
  stbi_uc *tex = stbi_load(path.c_str(), &mapWidth, &mapHeight, &n, 1);
  ObjectGroupRef forest = newObjectGroup();

  for (float x = 0; x < GRID_SIZE; x += GRID_SQUARE_SIZE) {
    for (float y = 0; y < GRID_SIZE; y += GRID_SQUARE_SIZE) {
      float xOnMap = (x / GRID_SIZE);
      float yOnMap = (y / GRID_SIZE);
      vec2 uv(xOnMap, yOnMap);
      generateTreeAtSpot(x, y, getPixel(tex, mapWidth, mapHeight, uv), trunk,
                         leaves, forest);
    }
  }
  scene.add(forest);
  stbi_image_free(tex);
}
