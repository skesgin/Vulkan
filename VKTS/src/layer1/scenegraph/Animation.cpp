/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Animation.hpp"

namespace vkts
{

Animation::Animation() :
    IAnimation(), name(""), start(0.0f), stop(0.0f), currentSection(-1), allMarkers(), allChannels()
{
}

Animation::Animation(const Animation& other) :
    IAnimation(), name(other.name + "_clone"), start(other.start), stop(other.stop), currentSection(other.currentSection), allChannels()
{
    for (size_t i = 0; i < other.allMarkers.size(); i++)
    {
        allMarkers.append(other.allMarkers[i]->clone());
    }

    for (size_t i = 0; i < other.allChannels.size(); i++)
    {
        allChannels.append(other.allChannels[i]->clone());
    }
}

Animation::~Animation()
{
    destroy();
}

//
// IAnimation
//

const std::string& Animation::getName() const
{
    return name;
}

void Animation::setName(const std::string& name)
{
    this->name = name;
}

float Animation::getStart() const
{
	if (currentSection == -1 || currentSection == 0)
	{
		return start;
	}

	return allMarkers[currentSection - 1]->getTime();
}

void Animation::setStart(const float start)
{
    this->start = start;
}

float Animation::getStop() const
{
	if (currentSection == -1 || currentSection == (int32_t)allMarkers.size())
	{
		return stop;
	}

	return allMarkers[currentSection]->getTime();
}

void Animation::setStop(const float stop)
{
    this->stop = stop;
}

void Animation::setCurrentSection(const int32_t currentSection)
{
	if (currentSection < -1 || allMarkers.size() == 0)
	{
		this->currentSection = -1;
	}
	else if (currentSection > (int32_t)allMarkers.size())
	{
		this->currentSection = (int32_t)allMarkers.size();
	}
	else
	{
		this->currentSection = currentSection;
	}
}

int32_t Animation::getCurrentSection() const
{
	return currentSection;
}

void Animation::addMarker(const IMarkerSP& marker)
{
	if (allMarkers.size() == 0)
	{
		currentSection = 0;
	}

	allMarkers.append(marker);
}

VkBool32 Animation::removeMarker(const IMarkerSP& marker)
{
	auto result = allMarkers.remove(marker);

	if (allMarkers.size() == 0)
	{
		currentSection = -1;
	}

    return result;
}

size_t Animation::getNumberMarkers() const
{
    return allMarkers.size();
}

const SmartPointerVector<IMarkerSP>& Animation::getMarkers() const
{
    return allMarkers;
}

void Animation::addChannel(const IChannelSP& channel)
{
    allChannels.append(channel);
}

VkBool32 Animation::removeChannel(const IChannelSP& channel)
{
    return allChannels.remove(channel);
}

size_t Animation::getNumberChannels() const
{
    return allChannels.size();
}

const SmartPointerVector<IChannelSP>& Animation::getChannels() const
{
    return allChannels;
}

//
// ICloneable
//

IAnimationSP Animation::clone() const
{
	auto result = IAnimationSP(new Animation(*this));

	if (result.get() && result->getNumberChannels() != getNumberChannels())
	{
		return IAnimationSP();
	}

	if (result.get() && result->getNumberMarkers() != getNumberMarkers())
	{
		return IAnimationSP();
	}

    return result;
}

//
// IDestroyable
//

void Animation::destroy()
{
    for (size_t i = 0; i < allChannels.size(); i++)
    {
        allChannels[i]->destroy();
    }
    allChannels.clear();
}

} /* namespace vkts */
