#include "vectorField.h"

//コンストラクタ
vectorField::vectorField(){
}



//デストラクタ
vectorField::~vectorField(){
    
}



//ベクトル場の初期化
void vectorField::setupField(int innerW, int innerH, int outerW, int outerH){
	
	fieldWidth			= innerW;
	fieldHeight			= innerH;
	externalWidth		= outerW;
	externalHeight		= outerH;

    scalex = (float)externalWidth / (float)fieldWidth;
    scaley = (float)externalHeight / (float)fieldHeight;
    
	field.clear();
	
	fieldSize = fieldWidth * fieldHeight;
	for (int i = 0; i < fieldSize; i++){
		ofVec2f pt;
		pt = ofVec2f(0,0);
		field.push_back(pt);
	}
}

//消去
void vectorField::clear(){
    for (int i = 0; i < fieldSize; i++){
        field[i] = ofVec2f(0,0);
    }
}

//徐々に力が弱まるようにする
void vectorField::fadeField(float fadeAmount){
	for (int i = 0; i < fieldSize; i++){
        field[i] = ofVec2f(field[i].x*fadeAmount,field[i].y*fadeAmount);
    }
}

//ベクトル場の様子を描画
void vectorField::draw(){
		
    for (int i = 0; i < fieldWidth; i++){
        for (int j = 0; j < fieldHeight; j++){
            int pos = j * fieldWidth + i;

            float px = 	i * scalex;
            float py = 	j * scaley;
            float px2 = px + field[pos].x * 5;
            float py2 = py + field[pos].y * 5;
			
            ofSetColor(255, 100, 100);
            ofLine(ofVec2f(px, py), ofVec2f(px2, py2));
			
			ofVec2f line;
            line.set(px2-px, py2-py);
            float length = line.length();
            line.normalize();
            // 90度回転
            line.rotate(90);
            ofSetColor(255, 0, 0);
            ofLine(px - line.x*length*0.2, py - line.y*length*0.2, px + line.x*length*0.2, py + line.y*length*0.2);
        }
    }
}

//座標を指定して、その地点でのベクトル場から受ける力を算出
ofVec2f vectorField::getForceFromPos(float xpos, float ypos){
	
	ofVec2f frc;
	frc.set(0,0);
	
	float xPct = xpos / (float)externalWidth;
	float yPct = ypos / (float)externalHeight;
	
	if ((xPct < 0 || xPct > 1) || (yPct < 0 || yPct > 1)){
		return frc;	
	} 
	
    int fieldPosX = (int)(xPct * fieldWidth);
    int fieldPosY = (int)(yPct * fieldHeight);

    fieldPosX = MAX(0, MIN(fieldPosX, fieldWidth-1));
    fieldPosY = MAX(0, MIN(fieldPosY, fieldHeight-1));

    int pos = fieldPosY * fieldWidth + fieldPosX;
	
	frc.set(field[pos].x * 0.1, field[pos].y * 0.1 );
	return frc;

}


//座標を指定して、その地点から力をベクトル場に加える
void vectorField::addForceFromPos(float xpos, float ypos, ofVec2f vec){
		
	float xPct = xpos / (float)externalWidth;
	float yPct = ypos / (float)externalHeight;
	
	if ((xPct < 0 || xPct > 1) || (yPct < 0 || yPct > 1)){
		return;
	}
	
    int fieldPosX = (int)(xPct * fieldWidth);
    int fieldPosY = (int)(yPct * fieldHeight);
    
    fieldPosX = MAX(0, MIN(fieldPosX, fieldWidth-1));
    fieldPosY = MAX(0, MIN(fieldPosY, fieldHeight-1));
    
    int pos = fieldPosY * fieldWidth + fieldPosX;
	
	field[pos].x += vec.x*2;
    field[pos].y += vec.y*2;

}

