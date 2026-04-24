#include "pch.h"
#include "CMOGO.h"
#include "model.h"
#include <windows.h>
#include "Effects.h"
#include "camera.h"
#include "CommonStates.h"
#include "DrawData.h"
#include "Helper.h"
#include "stringHelp.h"

using namespace DirectX;

ID3D11RasterizerState* CMOGO::s_pRasterState = nullptr;
int CMOGO::m_count = 0;

using namespace DirectX;
CMOGO::CMOGO() :m_model(nullptr)
{
}

CMOGO::~CMOGO()
{
	//model shouldn't need deleting as it's linked to by a unique_ptr
	m_count--;

	//okay I've just deleted the last CMOGO let's get rid of this
	if (m_count == 0 && s_pRasterState)
	{
		s_pRasterState->Release();
		s_pRasterState = nullptr;
	}

	//TODO: This is start of system to remove the s_pRasterState when the last CMOGO is deleted
	//but no system to recreate it if a new CMOGO is created after all have been deleted
}

void CMOGO::Tick(GameState* _GS)
{
	if (!m_isActive) return;
	GameObject::Tick(_GS);
}

void CMOGO::Draw(DrawData* _DD)
{
	if (!m_isActive) return;

	//checkout here: https://github.com/microsoft/DirectXTK/wiki/Rendering-a-model
	// for more information about how this all works

	//a dirty hack as the CMO model drawer breaks the depth stencil state
	ID3D11DepthStencilState* DSS = nullptr;
	UINT ref;

	//pick up a copy of the current state...
	_DD->m_pd3dImmediateContext->OMGetDepthStencilState(&DSS, &ref);

	m_model->Draw(_DD->m_pd3dImmediateContext, *_DD->m_states, //graphics device and CommonStates required by model system
		m_worldMat, //world transform to position this model in the world
		_DD->m_cam->GetView(), _DD->m_cam->GetProj(), //view and projection matrix of the camera
		false,
		[&]() {_DD->m_pd3dImmediateContext->RSSetState(s_pRasterState); } //this VERY weird construction creates a function on the fly to set up the render states correctly else the model system overrides them BADLY
	);

	//...and put the depth stencil state back again
	_DD->m_pd3dImmediateContext->OMSetDepthStencilState(DSS, ref);

	//clear this copy away
	if (DSS)
	{
		DSS->Release();
	}
}

void CMOGO::Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game)
{

	GameObject::Init(_GD, _EF, _game);

	//if we've not created it yet do so now
	if (!s_pRasterState)
	{
		//Setup Raster State
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		// Create the rasterizer state from the description we just filled out.
		HRESULT hr = _GD->CreateRasterizerState(&rasterDesc, &s_pRasterState);
		assert(hr == S_OK);
	}

	string filePath = "../Assets/" + m_file + ".cmo";

	wchar_t* file = Helper::charToWChar(filePath.c_str());

	m_model = Model::CreateFromCMO(_GD, file, *_EF);
}

void CMOGO::OnCollision(GameObject* _other)
{
	GameObject::OnCollision(_other);
}


void CMOGO::Load(ifstream& _file)
{
	GameObject::Load(_file);
	StringHelp::String(_file, "FILE", m_file);
}