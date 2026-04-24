#include "pch.h"
#include "RenderTarget.h"
#include "DrawData.h"
#include <assert.h>
#include <iostream>

#include "GameState.h"
#include "stringHelp.h"
#include "GO2DFactory.h"
#include "RTImage.h"
#include "Game.h"

RenderTarget::RenderTarget()
{
	m_type = "RENDERTARGET";
	m_canBeMainCamera = false;
}

RenderTarget::~RenderTarget()
{
	//TODO: Here I'm actually releasing resources, really ought to have this EVRYWHERE!
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_textureSRV)
	{
		m_textureSRV->Release();
		m_textureSRV = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = nullptr;
	}

}

void RenderTarget::PreRender(DrawData* _DD)
{
	//clear render target to black
	ClearRenderTarget(_DD->m_pd3dImmediateContext, 0.0f, 0.0f, 0.0f, 1.0f);

	// Clear the depth buffer.
	_DD->m_pd3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//set rendering system to aim to this texture
	_DD->m_pd3dImmediateContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	_DD->m_pd3dImmediateContext->RSSetViewports(1, &m_viewport);
}

void RenderTarget::Tick(GameState* _GS)
{
	MiniMapCamera::Tick(_GS);
}

void RenderTarget::Load(ifstream& _file)
{
	Camera::Load(_file);
	StringHelp::Int(_file, "WIDTH", m_width);
	StringHelp::Int(_file, "HEIGHT", m_height);
	StringHelp::Float2(_file, "POS", m_posImg.x, m_posImg.y);
	StringHelp::Float2(_file, "SCALE", m_scaleImg.x, m_scaleImg.y);
}

void RenderTarget::Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game)
{
	//create texture
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = m_width;
	textureDesc.Height = m_height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = _GD->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = _GD->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = _GD->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_textureSRV);

	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = m_width;
	depthBufferDesc.Height = m_height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = _GD->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);

	// Initailze the depth stencil view description.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = _GD->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);

	//add related Image 2D GameObject to the game
	RTImage* img = (RTImage*)GO2DFactory::makeNewGO2D("RENDERTARGET");
	img->SetPos(m_posImg);
	img->SetScale(m_scaleImg);
	img->SetName(m_name);
	_game->AddGameObject2D(img);

	//set up viewport
	m_viewport = { 0.0f, 0.0f, (float)m_width, (float)m_height, 0.f, 1.f };
	m_aspectRatio = (float)m_width / (float)m_height;
}

void RenderTarget::ClearRenderTarget(ID3D11DeviceContext* _pd3dImmediateContext, float _red, float _green, float _blue, float _alpha)
{
	float colour[4];

	// Setup the colour to clear the buffer to.
	colour[0] = _red;
	colour[1] = _green;
	colour[2] = _blue;
	colour[3] = _alpha;

	// Clear the texture.
	if (m_renderTargetView)
	{
		_pd3dImmediateContext->ClearRenderTargetView(m_renderTargetView, colour);
	}
	return;
}
