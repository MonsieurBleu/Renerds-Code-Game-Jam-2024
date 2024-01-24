#pragma once
#include <../Engine/include/ObjectGroup.hpp>
#include "Audio.hpp"


class ModelSound : public ObjectGroup
{
    public :
        AudioSource soundSource;
        virtual void update(bool forceUpdate = false);
};

void ModelSound::update(bool forceUpdate)
{
    soundSource.setPosition(state.position);
    ObjectGroup::update();
}