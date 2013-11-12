//
//  TankContent.cpp
//  GridMaker
//
//  Created by William Lindmeier on 10/8/13.
//
//

#include "TankContent.h"
#include "cinder/ObjLoader.h"
#include "cinder/app/App.h"
#include "cinder/Utilities.h"
#include "cinder/Camera.h"
#include "Utilities.hpp"
#include "ContentProvider.h"

using namespace ci;
using namespace ci::app;
using namespace bigscreens;

namespace bigscreens
{
    
    TankContent::TankContent() :
    mGroundContent(10000.0)
    , mTankPosition(0,0,0)
    , mIsGroundVisible(true)
    , mTank( ContentProviderNew::ActorContent::getAdvancedTank() )
    // , mGroundPlane( ContentProviderNew::ActorContent::getFloorPlane() )
    {
    }
    
    void TankContent::setTankPosition(const ci::Vec3f tankPosition)
    {
        mTankPosition = tankPosition;
    }
    
    ci::Vec3f TankContent::getTankPosition()
    {
        return mTankPosition;
    }
    
    void TankContent::setGroundIsVisible(bool isVisible)
    {
        mIsGroundVisible = isVisible;
    }
    
    void TankContent::load(const std::string & objFilename)
    {
        loadShaders();
        
        loadScreen();

        loadGround();
        
        // Cam
        mCam.lookAt( Vec3f( 0, 200, 1000 ), Vec3f( 0, 100, 0 ) );
        
        mGroundOffset = Vec2f::zero();
    }
    
    void TankContent::loadShaders()
    {
        gl::GlslProg::Format screenShaderFormat;
        screenShaderFormat.vertex( ci::app::loadResource( "offset_texture.vert" ) )
        .fragment( ci::app::loadResource( "offset_texture.frag" ) );
        mTextureShader = ci::gl::GlslProg::create( screenShaderFormat );
        mTextureShader->uniform("uColor", Color::white());
        
        gl::GlslProg::Format groundShaderFormat;
        groundShaderFormat.vertex( ci::app::loadResource( "ground_texture.vert" ) )
        .fragment( ci::app::loadResource( "ground_texture.frag" ) );
        mGroundShader = ci::gl::GlslProg::create( groundShaderFormat );
        mGroundShader->uniform("uColor", Color::white());
    }
    
    void TankContent::loadScreen()
    {
        mScreenTexture = gl::TextureRef(new gl::Texture(loadImage(app::loadResource("screen.png"))));
        
        GLfloat data[8+8]; // verts, texCoords
        GLfloat *verts = data, *texCoords = data + 8;

        verts[0*2+0] = 1.0f;
        verts[0*2+1] = 0.0f;
        texCoords[0*2+0] = mScreenTexture->getRight();
        texCoords[0*2+1] = mScreenTexture->getTop();
        
        verts[1*2+0] = 0.0f;
        verts[1*2+1] = 0.0f;
        texCoords[1*2+0] = mScreenTexture->getLeft();
        texCoords[1*2+1] = mScreenTexture->getTop();
        
        verts[2*2+0] = 1.0f;
        verts[2*2+1] = 1.0f;
        texCoords[2*2+0] = mScreenTexture->getRight();
        texCoords[2*2+1] = mScreenTexture->getBottom();
        
        verts[3*2+0] = 0.0f;
        verts[3*2+1] = 1.0f;
        texCoords[3*2+0] = mScreenTexture->getLeft();
        texCoords[3*2+1] = mScreenTexture->getBottom();
        
        mScreenVao = gl::Vao::create();
        mScreenVao->bind();
        mScreenVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW );
        mScreenVbo->bind();
        
        int posLoc = mTextureShader->getAttribSemanticLocation( geom::Attrib::POSITION );
        gl::enableVertexAttribArray( posLoc );
        gl::vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        
        int texLoc = mTextureShader->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
        gl::enableVertexAttribArray( texLoc );
        gl::vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*8) );

        mScreenVao->unbind();
        mScreenVbo->unbind();
    }
    
    void TankContent::loadGround()
    {
        gl::Texture::Format texFormat;
        texFormat.setWrap(GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T);
        texFormat.mipMap(true);
        texFormat.maxAnisotropy(gl::Texture::getMaxMaxAnisotropy() );
        mGridTexture = gl::TextureRef(new gl::Texture(loadImage(app::loadResource("grid.png")), texFormat));
        
        mGroundContent.load(mGroundShader);
    }
    
    void TankContent::drawGround()
    {
        if (!mIsGroundVisible) return;
        
        gl::pushMatrices();

        gl::setMatrices( mCam );

        mGroundShader->bind();
        mGridTexture->bind();
        
        // Probably not necessary
        gl::enableAlphaBlending();
        
        mGroundShader->uniform("uTexCoordOffset", mGroundOffset);
        mGroundShader->uniform("uColor", ColorAf(0.75,0.75,0.75,1.0f));
        mGroundShader->uniform("uTexScale", 50.f);
        
        // Get the current plot
        float groundScale = mGroundContent.getScale();
        Vec3f groundOffset((-0.5f * groundScale),
                           0,
                           (-0.5f * groundScale));

        mGroundContent.render(GL_TRIANGLE_STRIP, groundOffset);
        
        mGridTexture->unbind();
        mGroundShader->unbind();

        // Removing this for now. Integrating it into the app has proven very awkward.
//		mGroundPlane->draw();
		
        gl::popMatrices();
    }
    
    void TankContent::reset()
    {
        mGroundOffset = Vec2f::zero();
        resetPositions();
    }
    
    void TankContent::resetPositions()
    {
        mCam.setPerspective( 45.0f, getWindowAspectRatio(), .01, 40000 );
        mTankPosition = Vec3f::zero();
    }
    
    void TankContent::setGroundOffset(const Vec2f offset)
    {
        mGroundOffset = offset;
    }

    // Lets the app take control of the cam
    void TankContent::update(std::function<void (ci::CameraPersp & cam)> update_func)
    {
        update_func(mCam);
        mTank->update(mNumFramesRendered);
    }
    
    void TankContent::drawScreen(const ci::Rectf & contentRect)
    {
        gl::pushMatrices();
        
        gl::setMatricesWindow(contentRect.getWidth(), contentRect.getHeight());
        gl::scale(contentRect.getWidth(), contentRect.getHeight());
        
        mTextureShader->bind();
        mScreenTexture->bind();

        gl::setDefaultShaderVars();
        
        // No offset
        mTextureShader->uniform("uTexCoordOffset", Vec2f(0,0));
        mTextureShader->uniform("uColor", ColorAf(1,1,1,1));

        mScreenVao->bind();
        mScreenVbo->bind();
        
        gl::drawArrays( GL_TRIANGLE_STRIP, 0, 4 );
        
        mScreenVao->unbind();
        mScreenVbo->unbind();
        
        mScreenTexture->unbind();
        mTextureShader->unbind();
        
        gl::popMatrices();
    }
    
    void TankContent::drawTank()
    {
        gl::pushMatrices();
        gl::setMatrices( mCam );
        gl::translate(mTankPosition);
        mTank->render( mCam );
        gl::popMatrices();
    }
    
    void TankContent::render(const ci::Vec2i & screenOffset, const ci::Rectf & contentRect)
    {
        // clear out both of the attachments of the FBO with black
        gl::clear( ColorAf( 0.0f, 0.0f, 0.0f, 0.0f ) );
        
        drawScreen(contentRect);
        
        drawGround();

        drawTank();
    }
}
