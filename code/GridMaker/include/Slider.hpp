//
//  Slider.hpp
//  GridMaker
//
//  Created by William Lindmeier on 10/17/13.
//
//

#pragma once

#include "cinder/Cinder.h"

struct Slider
{
    Slider(ci::Rectf rect) : mRect(rect), mValue(0), mIsActive(false){};
    Slider(){};
    ~Slider(){};
    
    ci::Rectf mRect;
    float mValue;
    bool mIsActive;
    
    void update(ci::Vec2f mousePosition)
    {
        ci::Vec2f relPos = mousePosition - mRect.getUpperLeft();
        float scalarVal = relPos.x / mRect.getWidth();
        mValue = std::min<float>(1.0f, std::max<float>(0.0f, scalarVal));
    }
    
    bool contains(ci::Vec2f position)
    {
        return mRect.contains(position);
    }
    
    float getValue()
    {
        return mValue;
    }
    
    void setValue(const float value)
    {
        mValue = value;
    }
    
    void setIsActive(bool isActive)
    {
        mIsActive = isActive;
    }
    
    bool getIsActive()
    {
        return mIsActive;
    }
    
    void render(bool isEnabled)
    {
        ci::gl::lineWidth(1.0f);
        ci::gl::color(ci::Color::white());
        ci::gl::drawLine(ci::Vec2f(mRect.x1, mRect.getCenter().y),
                         ci::Vec2f(mRect.x2, mRect.getCenter().y));
        ci::gl::lineWidth(6.0f);
        if (isEnabled)
        {
            ci::gl::color(ci::ColorAf(1.0f, 0, 0, 1.0f));
        }
        else
        {
            ci::gl::color(ci::ColorAf(0.5f, 0.5f, 0.5f, 1.0f));
        }
        float offsetX = mRect.getWidth() * mValue;
        ci::gl::drawLine(ci::Vec2f(mRect.x1 + offsetX,
                                   mRect.y1),
                         ci::Vec2f(mRect.x1 + offsetX,
                                   mRect.y2));
    }
};
