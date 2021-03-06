//
//  Utilities.hpp
//  GridMaker
//
//  Created by William Lindmeier on 10/10/13.
//
//

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Rect.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "SharedTypes.hpp"
#include <time.h>
#include <sys/timeb.h>

#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)

#ifdef RENDER_FRAMES
#include "fftData.h"
#endif

namespace bigscreens
{
    template<typename T>
    static bool rectCompare(ci::RectT<T> rectA, ci::RectT<T> rectB)
    {
        return  (int)rectA.x1 == (int)rectB.x1 &&
        (int)rectA.x2 == (int)rectB.x2 &&
        (int)rectA.y1 == (int)rectB.y1 &&
        (int)rectA.y2 == (int)rectB.y2;
    }
    
    static long getMilliCount()
    {
#ifdef RENDER_FRAMES
        const static float kSimFPS = 60.0f;
        float secondsElapsed = ci::app::getElapsedFrames() / kSimFPS;
        return (long)(secondsElapsed * 1000);
#else
        timeb tb;
        ftime(&tb);
        long nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
        return nCount;
#endif
    }
    
    static long getMilliSpan(int nTimeStart)
    {
        int nSpan = getMilliCount() - nTimeStart;
        if(nSpan < 0)
            nSpan += 0x100000 * 1000;
        return nSpan;
    }
    
    static ci::Rectf rectfFromRectInt(ci::RectT<int> rectA)
    {
        return ci::Rectf((float)rectA.x1, (float)rectA.y1, (float)rectA.x2, (float)rectA.y2);
    }
    
    static bool rectsOverlap(ci::Rectf rectA, ci::Rectf rectB)
    {
        return (rectA.x1 < rectB.x2 && rectA.x2 > rectB.x1 &&
                rectA.y1 < rectB.y2 && rectA.y2 > rectB.y1);
    };
    
    static bool rectIsValid(const ci::Rectf & rect,
                     const std::vector<ScreenRegion> & regions,
                     const bool ignoreInactive = true)
    {
        bool didOverlap = false;
        for (int i = 0; i < regions.size(); ++i)
        {
            ScreenRegion reg = regions[i];
            if ((reg.isActive || !ignoreInactive) && rectsOverlap(reg.rect, rect))
            {
                didOverlap = true;
                break;
            }
        }
        return !didOverlap;
    }
    
    static ci::Rectf rectFromTwoPos(const ci::Vec2f & posA, const ci::Vec2f & posB)
    {
        int x1 = std::min(posA.x, posB.x);
        int x2 = std::max(posA.x, posB.x);
        int y1 = std::min(posA.y, posB.y);
        int y2 = std::max(posA.y, posB.y);
        return ci::Rectf(x1, y1, x2, y2);
    }
    
    static ci::Vec2i mousePositionSnappedToSize(const ci::Vec2i & pos, const int snapSize)
    {
        return pos - ci::Vec2i(pos.x % snapSize, pos.y % snapSize);
    }

    static ci::fs::path SharedAssetPath(bool isLocalApp)
    {
        if (isLocalApp)
        {
            // NOTE: SRC_PATH is a preprocessor alias to $SRCROOT.
            // Is there a Cinder way to do this?
            return ci::fs::path(EXPAND_AND_QUOTE(SRC_PATH)) / ".." / "BigScreensShared" / "Assets";
        }
        else
        {
            return cinder::app::getAppPath() / ".." / "Assets";
        }
    }
    
    static ci::fs::path SharedShaderAssetPath(const std::string & shaderName, bool isLocalApp = false)
    {
        // NOTE:
        // Define USE_DYNAMIC_ASSETS in your project settings (or elsewhere) to dynamically load shaders.
        // Easier than comment tag.
#ifdef USE_DYNAMIC_ASSETS
        return shaderName;
#else
        return SharedAssetPath(isLocalApp) / "shaders" / shaderName;
#endif
    }

    static inline ci::DataSourceRef LoadShader(const std::string & assetName)
    {
        //return ci::app::loadAsset(SharedShaderAssetPath(assetName, !IS_IAC));
        return ci::app::loadResource(assetName);
    }
    
    static ci::fs::path SharedGridAssetPath(bool isLocalApp)
    {
        return SharedAssetPath(isLocalApp) / "grid";
    }
    
    // Doesn't barf when the uniform doesnt exist
    #define TryAddingUniform(glsl, name, data) try { glsl->uniform( name, data ); } catch (cinder::gl::GlslUnknownUniform){}

    template <typename T>
    bool VectorFind(const std::vector<T> & vec, T item)
    {
        return std::find(vec.begin(), vec.end(), item) != vec.end();
    }
    
    template <typename T>
    void VectorErase(std::vector<T> & vec, T item)
    {
        vec.erase(std::remove(vec.begin(), vec.end(), item), vec.end());
    }
    
    static OriginAndDimension OriginAndDimensionFromRectf(const ci::Rectf & rect,
                                                          const float maxHeight)
    {
        return OriginAndDimension(ci::Vec2i( rect.x1, maxHeight - rect.y2 ),
                                  ci::Vec2i( rect.getWidth(), rect.getHeight() ) );
    }
    
}