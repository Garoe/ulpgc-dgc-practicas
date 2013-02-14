#ifndef DRAWABLE_OBJECT_H
#define DRAWABLE_OBJECT_H

#include "vertex.hpp"
#include "renderer.hpp"
#include <cmath>
using namespace std;

enum Axis { X, Y, Z };
enum Subtype { CUBE, REVOLUTION };

class DrawableObject {

    protected:
        Vertex* vertices;
        Vertex* transVertices;
        int totalVertices;
        Subtype subtype;
        double transMatrix[4][4];
        double auxMatrix[4][4];
        void multiplyMatrix( double matrix0[4][4], double matrix1[4][4], int firstSave = 1 );


    public:
        DrawableObject( int totalVertices_ );
        DrawableObject( const DrawableObject& otherDrawableObject );
        virtual ~DrawableObject();
        virtual void draw(){};
        virtual int getTotalVertices() { return totalVertices; };
        virtual void rotate( Axis axis, double amount, int permanent);
        virtual void translate( double tX, double tY, int permanent);
        virtual void resetTransMatrix();
        virtual void resetAuxMatrix();
        virtual Subtype getSubtype() { return subtype; };
        static int nObjects;
        static Renderer* renderer;
};

#endif
// DRAWABLE_OBJECT_H
