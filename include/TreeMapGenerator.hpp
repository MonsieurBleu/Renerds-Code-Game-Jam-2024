#include <string>
#include <Scene.hpp>

typedef struct {
  ModelRef t0;
  ModelRef t1;
  ModelRef t2;
} treeSizes;

void generateTreesFromHeatMap(Scene &scene, std::string path, treeSizes trunk, treeSizes leaves);
