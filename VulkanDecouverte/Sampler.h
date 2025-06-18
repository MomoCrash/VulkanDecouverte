#pragma once

#include "framework.h"

class Sampler
{
public:
    Sampler();
    ~Sampler();

    VkSampler& getSampler();

private:
    VkSampler m_textureSampler;
};
