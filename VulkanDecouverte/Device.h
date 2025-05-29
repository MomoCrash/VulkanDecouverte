#pragma once

#include "framework.h"

class Device
{
public:

    Device();
    
    VkDevice m_device;

    VkQueue m_presentQueue;
    VkQueue m_graphicsQueue;

    VkDevice const& getDevice() const;

    VkQueue const& getPresentQueue() const;
    VkQueue const& getGraphicsQueue() const;

};
