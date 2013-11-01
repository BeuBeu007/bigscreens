//
//  TankConvergenceContent.cpp
//  BigScreensComposite
//
//  Created by William Lindmeier on 10/31/13.
//
//

#include "TankConvergenceContent.h"
#include "cinder/CinderMath.h"

using namespace ci;
using namespace ci::app;
using namespace bigscreens;


TankConvergenceContent::TankConvergenceContent() : TankContent()
{
    mGroundContent = GroundContent(20000.0);
};

// NOTE: This draws a collection of tanks
void TankConvergenceContent::drawTank()
{
    // Make the tanks ease into position
    mTankShader->bind();
    mTankVao->bind();
    mTankElementVbo->bind();

    gl::pushMatrices();
    gl::setMatrices( mCam );
    
    mTankShader->uniform("uColor", ColorAf(1,1,1,1));
    
    float scalarProgress = 1.0 - std::min<float>(1.0, (float)mNumFramesRendered / 1000.0f);
    float weightedProgress = scalarProgress * scalarProgress;

    static const int kNumTanks = 20;
    for (int i = 0; i < kNumTanks; ++i)
    {
        float scalarOffset = (float)i / (float)kNumTanks;
        float rads = M_PI * 2 * scalarOffset;

        // A simple distance based on sin that looks somewhat staggered
        float iDist = fabs(sin(i)) * 5000;
        
        float tankDist = 5000 + (iDist * weightedProgress);
        float x = cos(rads) * tankDist;
        float y = 0;
        float z = sin(rads) * tankDist;
        drawSingleTankAtPosition(Vec3f(x, y, z), rads);
    }
    
    gl::popMatrices();
    mTankElementVbo->unbind();
    mTankVao->unbind();
    mTankShader->unbind();
}

void TankConvergenceContent::drawSingleTankAtPosition(const Vec3f & position, const float radianRotation)
{
    gl::pushMatrices();
    gl::translate(position);
    gl::rotate(270.0 - ci::toDegrees(radianRotation), 0, 1, 0);
    
    gl::setDefaultShaderVars();

    gl::drawElements( GL_LINES, mTankMesh->getNumIndices(), GL_UNSIGNED_INT, 0 );
    gl::popMatrices();
}