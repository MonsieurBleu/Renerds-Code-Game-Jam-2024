#include "Mesh.hpp"
#include <FenceGenerator.hpp>

#define FENCE_SIZE 512

void generateFence(ModelRef fence, Scene scene) {
  int fenceSegmentSize;
  vec3 pos(0, 0, 0);
  vec3 increment(1, 0, 0);
  vec3 rot(0, 0, 0);

  for (int i = 0; i < 4; i++) {
    for (float j = 0; j < FENCE_SIZE; j += fenceSegmentSize) {
      ModelRef fenceSegment = fence->copyWithSharedMesh();
      fenceSegment->state
        .setPosition(pos)
        .setRotation(rot);
      pos += increment;
    }
    cross(increment, vec3(0, 1, 0));
    cross(increment, vec3(0, 1, 0));
  }

}
