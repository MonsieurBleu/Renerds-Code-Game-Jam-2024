#include "Mesh.hpp"
#include "ObjectGroup.hpp"
#include <FenceGenerator.hpp>

#define FENCE_SIZE 256

void generateFence(ModelRef fenceSample, Scene &scene) {
  float fenceSegmentSize = fenceSample->getVao()->getAABBMax().x - fenceSample->getVao()->getAABBMin().x;
  ObjectGroupRef fence = newObjectGroup();
  vec3 pos(0, 0, 0);
  vec3 increment(fenceSegmentSize, 0, 0);

  for (int i = 0; i < 4; i++) {
    for (float j = 0; j < FENCE_SIZE; j += fenceSegmentSize) {
      ModelRef fenceSegment = fenceSample->copyWithSharedMesh();
      fenceSegment->state
        .setPosition(pos)
        .setRotation(vec3(0, radians(90.0*i), 0));
      fence->add(fenceSegment);

      pos += increment;
    }
    ModelRef fenceSegment = fenceSample->copyWithSharedMesh();
    fenceSegment->state
        .setPosition(pos)
        .setRotation(vec3(0, radians(90.0*i), 0));
      fence->add(fenceSegment);
    increment = cross(increment, vec3(0, 1, 0));
  }
  scene.add(fence);
}