#include "Particle.h"

Particle::Particle(){
    radius = 10;
    friction = 0.01;
}

Particle::~Particle(){
    
}


void Particle::setup(ofVec2f _velocity){
    // 位置を設定
    position.x = 100 + (rand() % ofGetWidth() );
    position.y = -((rand() % ofGetHeight() ) );
    // 初期速度を設定
    velocity = _velocity;
}

void Particle::setup(float positionX, float positionY, float velocityX, float velocityY){
    // 位置を設定
    position = ofVec2f(positionX, positionY);
    // 初期速度を設定
    velocity = ofVec2f(velocityX, velocityY);
}

// 力をリセット
void Particle::resetForce(){
    force = ofVec2f(0, 0);
}

// 力を加える
void Particle::addForce(ofVec2f _force){
    force.x += _force.x;
    force.y += _force.y;
}
void Particle::addForce(float forceX, float forceY){
    force.x += forceX;
    force.y += forceY;
}

// 摩擦力の更新
void Particle::updateForce(){
    force.x -= velocity.x * friction;
    force.y -= velocity.y * friction;
}

// 位置の更新
void Particle::updatePos(){
    velocity.x += force.x;
    velocity.y += force.y;
    position.x += velocity.x;
    position.y += velocity.y;
}

// 力の更新と座標の更新をupdateとしてまとめる
void Particle::update(){
    updateForce();
    updatePos();
}

//画面外に出た再度出現
void Particle::rebornParticle(){
    if (position.x < 0 || position.x > ofGetWidth()+100 || position.y > ofGetHeight()) {
        position.x = 100 + (rand() % ofGetWidth());
        position.y = -((rand() % ofGetHeight()) );
        
        velocity.set(0,0);
        force.set(0,0);
    }
}

// 描画
void Particle::draw(){
    ofSetColor(242, 216, 233);
    ofCircle(position, radius);
}

