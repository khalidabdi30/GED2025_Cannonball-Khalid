#pragma once
#include "VBGO.h"
#include "vertex.h"

//=================================================================
//procedurally generate a VBGO Cube
//each side be divided in to _size * _size squares (2 triangles per square)
//=================================================================

class VBCube : public VBGO
{
public:
	VBCube() {};
	virtual ~VBCube() {};

	//initialize the Vertex and Index buffers for the cube
	void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;

	void Load(ifstream& _file) override;

protected:
	//this is to allow custom versions of this which create the basic cube and then distort it
	//see VBSpiral and VBPillow
	virtual void Transform(WORD* _indices = nullptr) { _indices; };

	int m_size = 0;
	myVertex* m_vertices = nullptr;
};