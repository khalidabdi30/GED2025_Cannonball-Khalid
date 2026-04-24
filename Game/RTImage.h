#pragma once
#include "ImageGO2D.h"

class RenderTarget;

//displays a render target as a 2D image

class RTImage : public ImageGO2D
{
public:
	RTImage();
	virtual ~RTImage();

	virtual void Init(ID3D11Device* _GD, Game* _game) override;
	virtual void Draw(DrawData2D* _DD2D) override;

protected:
	RenderTarget* m_rt = nullptr;
};

