#include "Mesh.hpp"
#include "ObjectGroup.hpp"
#include <FenceGenerator.hpp>

#define FENCE_SIZE ((int)(720 / 1.5))

void generateFence(ModelRef fenceSample, Scene &scene, PhysicsEngine &physicsEngine)
{
    float fenceSegmentSize = fenceSample->getVao()->getAABBMax().x - fenceSample->getVao()->getAABBMin().x;
    float fenceHeight = fenceSample->getVao()->getAABBMax().y - fenceSample->getVao()->getAABBMin().y;
    ObjectGroupRef fence = newObjectGroup();
    vec3 pos((-FENCE_SIZE) / 2.0, -4, (-FENCE_SIZE) / 2.0);
    const vec3 startPos = pos;
    vec3 increment(fenceSegmentSize, 0, 0);

    for (int i = 0; i < 4; i++)
    {
        for (float j = (-FENCE_SIZE) / 2.0; j < FENCE_SIZE / 2.0; j += fenceSegmentSize)
        {
            ModelRef fenceSegment = fenceSample->copyWithSharedMesh();
            fenceSegment->state
                .setPosition(pos)
                .setRotation(vec3(0, radians(90.0 * i), 0));
            fence->add(fenceSegment);

            pos += increment;
        }
        ModelRef fenceSegment = fenceSample->copyWithSharedMesh();
        fenceSegment->state
            .setPosition(pos)
            .setRotation(vec3(0, radians(90.0 * i), 0));
        fence->add(fenceSegment);
        increment = cross(increment, vec3(0, 1, 0));
    }
    scene.add(fence);

    const int offset = 0; //-15
    fence->state.setPosition(vec3(offset, 0, offset));

    // add walls colliders
    const float wallThickness = 0.1f;
    vec3 v1 = vec3(FENCE_SIZE + 10, fenceHeight, wallThickness);
    vec3 v2 = vec3(wallThickness, fenceHeight, FENCE_SIZE + 10);
    AABBCollider *wall1Collider = new AABBCollider(-v1, vec3(startPos.x, 0.0, startPos.z));
    AABBCollider *wall2Collider = new AABBCollider(-v2, vec3(0));
    AABBCollider *wall3Collider = new AABBCollider(-v1, vec3(0));
    AABBCollider *wall4Collider = new AABBCollider(-v2, vec3(0));

    RigidBodyRef wall1 = newRigidBody(
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 0.0, 0.0),
        quat(0.0, 0.0, 0.0, 1.0),
        vec3(0.0, 0.0, 0.0),
        wall1Collider,
        PhysicsMaterial(),
        0.0,
        false);

    RigidBodyRef wall2 = newRigidBody(
        vec3(offset, 0.0, offset),
        vec3(0.0, 0.0, 0.0),
        quat(0.0, 0.0, 0.0, 1.0),
        vec3(0.0, 0.0, 0.0),
        wall2Collider,
        PhysicsMaterial(),
        0.0,
        false);

    RigidBodyRef wall3 = newRigidBody(
        vec3(offset, 0.0, FENCE_SIZE + offset / 2.0f - wallThickness / 2.0f),
        vec3(0.0, 0.0, 0.0),
        quat(0.0, 0.0, 0.0, 1.0),
        vec3(0.0, 0.0, 0.0),
        wall3Collider,
        PhysicsMaterial(),
        0.0,
        false);

    RigidBodyRef wall4 = newRigidBody(
        vec3(FENCE_SIZE + offset / 2.0f, 0.0, offset),
        vec3(0.0, 0.0, 0.0),
        quat(0.0, 0.0, 0.0, 1.0),
        vec3(0.0, 0.0, 0.0),
        wall4Collider,
        PhysicsMaterial(),
        0.0,
        false);

    physicsEngine.addObject(wall1);
    // physicsEngine.addObject(wall2);
    // physicsEngine.addObject(wall3);
    // physicsEngine.addObject(wall4);
}
