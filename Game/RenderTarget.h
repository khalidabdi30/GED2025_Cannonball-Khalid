#pragma once
#include "MiniMapCamera.h"

//a camera that renders to a texture rather than the back buffer

class RenderTarget : public MiniMapCamera //uses minimap camera as a type for the render target.
{
public:

	RenderTarget();
	~RenderTarget();

	virtual void PreRender(DrawData* _DD) override;

	virtual void Tick(GameState* _GS) override;

	virtual void Load(ifstream& _file) override;
	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;

	ID3D11ShaderResourceView* GetShaderResourceView() { return m_textureSRV; }

protected:

	//clear render target
	void ClearRenderTarget(ID3D11DeviceContext* _pd3dImmediateContext, float red, float green, float blue, float alpha);

	int m_width, m_height; //size of the render target in pixels

	//the actual texture
	ID3D11Texture2D* m_renderTargetTexture = nullptr;

	//pointer to this texture as a shader resource view
	ID3D11ShaderResourceView* m_textureSRV = nullptr;

	//ditto for the depth buffer
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;

	//pointer to this object as a render target view
	ID3D11RenderTargetView* m_renderTargetView = nullptr;

	//locations and scale for drawing the related Image2D GameObject
	Vector2 m_posImg;
	Vector2 m_scaleImg;
};