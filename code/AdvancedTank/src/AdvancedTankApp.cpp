#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ObjLoader.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/TriMesh.h"
#include "cinder/Rand.h"
#include "AdvancedTank.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bigscreens;

class OBJTestApp : public AppNative
{
  public:
    
	void setup();
	void mouseDown( MouseEvent event );
    void keyUp(KeyEvent event);
	void update();
	void draw();
    
    AdvancedTankRef mTank;

    CameraPersp         mCam;
    float               mCameraRotation;
    
    bool                mRotate;
};

void OBJTestApp::setup()
{
    mTank = AdvancedTankRef(new AdvancedTank());
    
    // Cam
    mCam.setPerspective( 15.0f, (float)getWindowWidth() / getWindowHeight(), .01, 40000 );
    mCam.lookAt( Vec3f( 0, 0, 0 ), Vec3f( 0, 0, 0 ) );
    
    mRotate = true;
    mCameraRotation = 0.0f;
}

void OBJTestApp::mouseDown( MouseEvent event )
{
    mTank->setFrameContentID(1);
    mTank->fire();
}

void OBJTestApp::keyUp(KeyEvent event)
{
    if (event.getChar() == ' ')
    {
        mRotate = !mRotate;
    }
}

void OBJTestApp::update()
{
    if (mRotate)
    {
        mCameraRotation -= 0.005f;
    }
    
    float camX = cosf(mCameraRotation) * 100;
    float camZ = sinf(mCameraRotation) * 100;
    float camY = 10;
    
    Vec3f lookAt( 0, 0, 0 );
    // NOTE: Only for "tank.obj"
    camY *= 120;
    camX *= 60;
    camZ *= 60;
    lookAt.y = 100;

    mCam.lookAt( Vec3f( camX, camY, camZ ), lookAt );
}

void OBJTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::pushMatrices();
    gl::setMatrices( mCam );

    Vec3f target = Vec3f(cos(getElapsedFrames() * 0.01) * 2000,
                            800,
                            sin(getElapsedFrames() * 0.01) * 2000);
    mTank->setTargetPosition(target);
    mTank->setFrameContentID(1);
    mTank->update(getElapsedFrames());
    mTank->render(mCam, 1);
    
    gl::popMatrices();
}

CINDER_APP_NATIVE( OBJTestApp, RendererGl )
