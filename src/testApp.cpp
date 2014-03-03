#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    sideWind=0.01;
    gravity=0.15;
    
    
    //桜画像読み込み:imreadはアルファチャンネルに対応していないため後でアルファチャンネルを透過にする必要あり
    cv::Mat sakuraTmp = cv::imread("sakura2.png");
    cv::cvtColor(sakuraTmp, sakura, CV_BGR2BGRA);
    for (int y=0; y<sakura.rows; ++y) {
        for (int x=0; x<sakura.cols; ++x) {
            sakura.data[y * sakura.step + x * 4 + 3] = 0;
        }
    }
    
    //カメラ入力と各Matの初期化
    cap.open(0);
	cap >> src;
	cv::cvtColor(src, gray, CV_BGR2GRAY);
	gray.copyTo(prevGray);
	gray.copyTo(subGray);
    show = &img;
    cv::cvtColor(src, img, CV_BGR2BGRA);
    src.copyTo(canvasColor);
    gray.copyTo(canvasBW);
    
    //key入力初期化
	key = 'g';
	
    //ウィンドウ及びパラメータートラックバー初期化
    cv::namedWindow("video", 1);
	thr = 9;
	cv::createTrackbar("thr", "video", &thr, 255);
	sigma = 11;
	cv::createTrackbar("sigma", "video", &sigma, 255);
    
    
	//パーティクルを1000個生成
	for (int i = 0; i < 1000; i++){
		Particle myParticle;
        myParticle.setImage(img);
        myParticle.friction = 0.01;
        myParticle.radius = 10;
		myParticle.setup(Vec2f(-sideWind, gravity));
		particles.push_back(myParticle);
	}
	
	//ベクトル場の初期化 (画面いっぱいに、横102コ、縦76コのベクトル)
    VF.setImage(canvasColor);
	VF.setupField(102, 76, img.cols, img.rows);
    
    
    
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    //カメラ入力
    cap >> src;
    if (!src.data) {
        isEndProcess = true;
    }
    
    //グレースケール化
    cv::cvtColor(src, gray, CV_BGR2GRAY);
    
    //ガウシアン平滑化
    cv::setTrackbarPos("sigma", "video", sigma|1);
    cv::GaussianBlur(gray, gray, cv::Size(0, 0), sigma);
    
    //一つ前の画像との差分取得
    cv::absdiff(gray, prevGray, subGray);
    
    //二値化
    cv::setTrackbarPos("thr", "video", thr);
    cv::threshold(subGray, bin, thr, 63, CV_THRESH_BINARY);
    
    //エッジ抽出
    cv::Canny(bin, canny, 50, 200);
    
    //後で等高線画像として足し合わせるため元画像をコピー
    bin.copyTo(bins);
    canny.copyTo(cannys);
    
    //等高線処理１回目
    for(int i=0; i<10; ++i){
        cv::erode(bin, bin, cv::Mat());
    }
    cv::Canny(bin, canny, 50, 200);
    cv::threshold(bin, bin, 5, 127, CV_THRESH_BINARY);
    cv::bitwise_or(bin, bins, bins);
    cv::bitwise_or(canny, cannys, cannys);
    
    //等高線処理２回目
    for(int i=0; i<10; ++i){
        cv::erode(bin, bin, cv::Mat());
    }
    cv::Canny(bin, canny, 50, 200);
    cv::threshold(bin, bin, 5, 192, CV_THRESH_BINARY);
    cv::bitwise_or(bin, bins, bins);
    cv::bitwise_or(canny, cannys, cannys);
    cv::threshold(cannys, cannys, 5, 255, CV_THRESH_BINARY);
    
    //等高線画像作成：計３段階
    cv::bitwise_or(bins, cannys, canvasBW);
    
    //後でベクトルなどをカラーで書き込むために等高線画像をカラー化
    cv::cvtColor(canvasBW, canvasColor, CV_GRAY2BGR);
    //canvasColor.copyTo(img);
    
    //ウィンドウ表示用の画像imgにsrcをコピー
    src.copyTo(img);
    
    //等高線画像から方向ベクトルを抽出
    for(int y=0; y<cannys.rows; y+=VF.scaley){
        for (int x=0; x<cannys.cols; x+=VF.scalex) {
            for (int y1=y; y1<y+VF.scaley; ++y1) {
                for (int x1=x; x1<x+VF.scalex; ++x1) {
                    if(static_cast<int>(cannys.data[y1 * cannys.step + x1]) == 255){
                        
                        int tmp = 3;
                        //cv::rectangle(canvasColor, cv::Point(x1-tmp, y1-tmp), cv::Point(x1+tmp, y1+tmp), cv::Scalar(0, 255, 0));
                        
                        //矩形の辺上の点を保存するtmpPt
                        std::vector<cv::Point> tmpPt;
                        
                        for (int x2=x1-tmp; x2<=x1+tmp; ++x2) {
                            if(static_cast<int>(cannys.data[(y1-tmp) * cannys.step + x2]) == 255){
                                tmpPt.push_back(cv::Point(x2, y1-tmp));
                                //std::cout << "tmp1- : ";
                            }
                            
                            if(static_cast<int>(cannys.data[(y1+tmp) * cannys.step + x2]) == 255){
                                tmpPt.push_back(cv::Point(x2, y1+tmp));
                                //std::cout << "tmp1+ : ";
                            }
                        }
                        
                        for (int y2=y1-tmp; y2<=y1+tmp; ++y2) {
                            
                            if(static_cast<int>(cannys.data[y2 * cannys.step + (x1-tmp)]) == 255){
                                tmpPt.push_back(cv::Point(x1-tmp, y2));
                                //std::cout << "tmp2- : ";
                            }
                            
                            if(static_cast<int>(cannys.data[y2 * cannys.step + (x1+tmp)]) == 255){
                                tmpPt.push_back(cv::Point(x1+tmp, y2));
                                //std::cout << "tmp2+ : ";
                            }
                            
                        }
                        
                        //std::cout << tmp1.x << ", " << tmp1.y << ", " << tmp2.x << ", " << tmp2.y << std::endl;
                        
                        //std::cout << tmpPt.size() << std::endl;
                        
                        //取得した２点を基にベクトルを生成
                        if(tmpPt.size()>=2){
                            //cv::line(canvasColor, tmpPt[0], tmpPt[1], cv::Scalar(0, 0, 255));
                            
                            // 90度回転
                            float radian = PI * 90.0f / 180.0f;
                            int tmpPt0xDash;
                            int tmpPt0yDash;
                            tmpPt0xDash = (tmpPt[0].x-x1)*cos(radian) - (tmpPt[0].y-y1)*sin(radian);
                            tmpPt0yDash = (tmpPt[0].x-x1)*sin(radian) + (tmpPt[0].y-y1)*cos(radian);
                            tmpPt0xDash += x1;
                            tmpPt0yDash += y1;
                            
                            if(tmpPt0xDash < 0){
                                tmpPt0xDash = 0;
                            }
                            if(tmpPt0xDash > canvasColor.cols){
                                tmpPt0xDash = canvasColor.cols;
                            }
                            if(tmpPt0yDash < 0){
                                tmpPt0yDash = 0;
                            }
                            if(tmpPt0yDash > canvasColor.rows){
                                tmpPt0yDash = canvasColor.rows;
                            }
                            
                            int tmpPt1xDash;
                            int tmpPt1yDash;
                            tmpPt1xDash = (tmpPt[1].x-x1)*cos(radian) - (tmpPt[1].y-y1)*sin(radian);
                            tmpPt1yDash = (tmpPt[1].x-x1)*sin(radian) + (tmpPt[1].y-y1)*cos(radian);
                            tmpPt1xDash += x1;
                            tmpPt1yDash += y1;
                            
                            if(tmpPt1xDash < 0){
                                tmpPt1xDash = 0;
                            }
                            if(tmpPt1xDash > canvasColor.cols){
                                tmpPt1xDash = canvasColor.cols;
                            }
                            if(tmpPt1yDash < 0){
                                tmpPt1yDash = 0;
                            }
                            if(tmpPt1yDash > canvasColor.rows){
                                tmpPt1yDash = canvasColor.rows;
                            }
                            
                            cv::line(canvasColor, cv::Point(tmpPt0xDash, tmpPt0yDash), cv::Point(tmpPt1xDash, tmpPt1yDash), cv::Scalar(0, 0, 255));
                            
                            if(static_cast<int >(bins.data[tmpPt0yDash * bins.step + tmpPt0xDash]) < static_cast<int >(bins.data[tmpPt1yDash * bins.step + tmpPt1xDash])){
                                cv::circle(canvasColor, cv::Point(tmpPt0xDash, tmpPt0yDash), 2, cv::Scalar(0, 255, 0));
                                VF.addForceFromPos(x1, y1, Vec2f(tmpPt0xDash-tmpPt1xDash, tmpPt0yDash-tmpPt1yDash));
                            }else{
                                cv::circle(canvasColor, cv::Point(tmpPt1xDash, tmpPt1yDash), 2, cv::Scalar(0, 255, 0));
                                VF.addForceFromPos(x1, y1, Vec2f(tmpPt1xDash-tmpPt0xDash, tmpPt1yDash-tmpPt0yDash));
                            }
                        }
                        
                        y1=y+VF.scaley;
                        x1=x+VF.scalex;
                        
                    }
                }
            }
            
        }
    }
    
    
	for (int i = 0; i < particles.size(); i++){
		
		//particleの力をリセット
		particles[i].resetForce();
		
		//ベクトル場から、それぞれのparticleにかかる力を算出
        cv::Point force;
        
		force = VF.getForceFromPos(particles[i].position.x, particles[i].position.y);
		
		//Particleの状態を更新
		particles[i].addForce(force.x-sideWind, force.y + gravity);
		particles[i].updateForce();
		particles[i].update();
        particles[i].rebornParticle();
	}
	
	//ベクトル場の力の減衰
	VF.fadeField(0.95f);
    
    
    //    std::cout << "px : " << particles[0].position.x << ", py : " << particles[0].position.y
    //    << "vx : " << particles[0].velocity.x << ", vy : " << particles[0].velocity.y
    //    << "fx : " << particles[0].force.x << ", fy : " << particles[0].force.y << std::endl;
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
	VF.draw();
    for (int i = 0; i < particles.size(); i++){
		particles[i].draw();
        /*
         if(0 < particles[i].position.x && 0 < particles[i].position.y && particles[i].position.x+sakura.cols < img.cols && particles[i].position.y+sakura.rows < img.rows){
         //std::cout << i << std::endl;
         sakura.copyTo(img(cv::Rect(particles[i].position.x, particles[i].position.y, sakura.cols, sakura.rows)));
         }
         */
        
	}
    
    cv::imshow("video", *show);
    
    gray.copyTo(prevGray);
    
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    key = cv::waitKey(1);
    switch (key) {
        case 'q':
            isEndProcess = true;
            break;
        case 'g':
            show = &gray;
            break;
        case 's':
            show = &subGray;
            break;
        case 'b':
            show = &bins;
            break;
        case 'c':
            show = &cannys;
            break;
        case 'v':
            show = &canvasColor;
            break;
        case 'i':
            show = &img;
            break;
        case 'z':
            cv::imwrite("img.bmp", img);
            break;
        case 'p':
            cv::waitKey();
            break;
        default:
            break;
    }
    
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
