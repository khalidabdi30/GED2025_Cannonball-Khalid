#pragma once
#include "GameObject2D.h"

//an image Game Object 2D
//loads and displays a texture from a .dds file
//generate these files in the Assets Textures project

class ImageGO2D :public GameObject2D
{
public:
	ImageGO2D();
	virtual ~ImageGO2D();

	virtual void Tick(GameState* _GS) override;
	virtual void Draw(DrawData2D* _DD2D) override;

	virtual void Load(ifstream& _file) override;
	virtual void Init(ID3D11Device* _GD, Game* _game) override;

protected:

	ID3D11ShaderResourceView* m_pTextureRV;
};