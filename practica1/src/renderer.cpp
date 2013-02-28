#include "renderer.hpp"
#include "ofMain.h"
#include <algorithm>    // std::sort
#include <vector>       // std::vector
using namespace std;

int k = 400;
//--------------------------------------------------------------
void Renderer::perspective( const bool activate ){
    perspective_ = activate;
}

//--------------------------------------------------------------
Vertex Renderer::applyPerspective(const Vertex& vertex){
        Vertex res = vertex;
        if(perspective_){
            res.setX(vertex.getX() /( 1 - vertex.getZ() / k));
            res.setY(vertex.getY() /( 1 - vertex.getZ() / k));
        }
        return res;
}

//--------------------------------------------------------------
void Renderer::rLine(const Vertex& vertex0, const Vertex& vertex1){
    float x0 = vertex0.getX();
    float y0 = vertex0.getY();
    float z0 = vertex0.getZ();
    float x1 = vertex1.getX();
    float y1 = vertex1.getY();
    float z1 = vertex1.getZ();

    if(perspective_){
        x0 = x0 /( 1 - z0 / k);
        y0 = y0 /( 1 - z0 / k);
        x1 = x1 /( 1 - z1 / k);
        y1 = y1 /( 1 - z1 / k);
    }
    x0 = x0 + center.getX();
    y0 = y0 + center.getY();
    x1 = x1 + center.getX();
    y1 = y1 + center.getY();

    ofLine(x0,y0,x1,y1);
}

//--------------------------------------------------------------
void Renderer::rLine(float x0, float y0, float z0, float x1, float y1, float z1){
    if(perspective_){
        x0 = x0 /( 1 - z0 / k);
        y0 = y0 /( 1 - z0 / k);
        x1 = x1 /( 1 - z1 / k);
        y1 = y1 /( 1 - z1 / k);
    }
    x0 = x0 + center.getX();
    y0 = y0 + center.getY();
    x1 = x1 + center.getX();
    y1 = y1 + center.getY();

    ofLine(x0,y0,x1+1,y1);
}

//--------------------------------------------------------------
void Renderer::rTriangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2){
    rLine(vertex0, vertex1);
    rLine(vertex0, vertex2);
    rLine(vertex1, vertex2);
}

/*
   v0
   /\
  /__\
v1    v2
*/
//--------------------------------------------------------------
void Renderer::triangleFillBotFlat(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2){
    if( vertex0.getY() == vertex1.getY() || vertex1.getX() == vertex2.getX() ){
        return;
    }
    float inv_m01, inv_m02, x_i, x_f;

    inv_m01 = vertex1.getY() - vertex0.getY();
    if(inv_m01){
        inv_m01 = (vertex1.getX() - vertex0.getX())/inv_m01;
    }

    inv_m02 = vertex2.getY() - vertex0.getY();
    if(inv_m02){
        inv_m02 = (vertex2.getX() - vertex0.getX())/inv_m02;
    }

    x_i = vertex0.getX();
    x_f = vertex0.getX();
    for(int j = vertex0.getY(); j <= vertex1.getY(); j++){
        for(int i = x_i; i <= x_f; i++){
            rLine(i, j, 0, i, j, 0);
        }
        x_i += inv_m01;
        x_f += inv_m02;
    }
}

// v0___v1
//   \ /
//    \/
//     v2
//--------------------------------------------------------------
void Renderer::triangleFillTopFlat(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2){
    if( vertex0.getY() == vertex2.getY() || vertex0.getX() == vertex1.getX() ){
        return;
    }
    float inv_m20, inv_m21, x_i, x_f;

    inv_m20 = vertex0.getY() - vertex2.getY();
    if(inv_m20){
        inv_m20 = (vertex0.getX() - vertex2.getX())/inv_m20;
    }

    inv_m21 = vertex1.getY() - vertex2.getY();
    if(inv_m21){
        inv_m21 = (vertex1.getX() - vertex2.getX())/inv_m21;
    }

    x_i = vertex2.getX();
    x_f = vertex2.getX();
    for(int j = vertex2.getY(); j >= vertex0.getY(); j--){
        for(int i = x_i; i <= x_f; i++){
            rLine(i, j, 0, i, j, 0);
        }
        x_i -= inv_m20;
        x_f -= inv_m21;
    }
}

//--------------------------------------------------------------
void Renderer::rTriangleFill(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2){
    vector<Vertex> vertices;
    vertices.reserve(3);
    vertices.push_back(applyPerspective(vertex0));
    vertices.push_back(applyPerspective(vertex1));
    vertices.push_back(applyPerspective(vertex2));
    Vertex v3 = vertices[0];
    sort (vertices.begin(), vertices.end(), Vertex::compareYX);
    // El corte de v2 con la linea v1,v3
    v3 = vertices[1];
    float x0 =  vertices[0].getY();
    float x1 =  vertices[1].getY();
    float x2 =  vertices[2].getY();
    float y0 =  vertices[0].getY();
    float y2 =  vertices[2].getY();
    //Interpolate where vertex1.x cuts the line vertex0-vertex2
    v3.setY( y2*((x0-x1)/(x0-x2)) + y0*(1-(x0-x1)/(x0-x2)) );
    if(v3 == vertices[1]){
        triangleFillBotFlat(vertices[0], vertices[1], vertices[2]);
        triangleFillTopFlat(vertices[0], vertices[1], vertices[2]);
    }else{
        if(v3.getX() > vertices[1].getX()){
            triangleFillBotFlat(vertices[0], vertices[1], v3);
            triangleFillTopFlat(vertices[1], v3, vertices[2]);
        }else{
            triangleFillBotFlat(vertices[0], v3, vertices[1]);
            triangleFillTopFlat(v3, vertices[1], vertices[2]);
        }
    }
}

//--------------------------------------------------------------
void Renderer::rDrawBitmapString( string tex, const Vertex& vertex){
    ofDrawBitmapString(tex, vertex.getX() + center.getX(), vertex.getY() + center.getY());
}

//--------------------------------------------------------------
void Renderer::rRect( const Vertex& vertex, const float w, const float h){
    ofRect(vertex.getX() + center.getX(), vertex.getY() + center.getY(), w, h);
}
