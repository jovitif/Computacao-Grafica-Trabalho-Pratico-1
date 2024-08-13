#include "DXUT.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

/*
                                            JOÃO VITOR FERNANDES DE SALES - TRABALHO PRÁTICO 01
*/

struct Vertex
{
    XMFLOAT3 Pos; // Coordenadas (x, y, z)
    XMFLOAT4 Color; // RGBA (Red, Green, Blue, Alpha)
};

class Curves : public App
{
private:
    ID3D12RootSignature* rootSignature;
    ID3D12PipelineState* pipelineState;

    // Gráficos
    Mesh* geometry;
    Mesh* lineSuporteGeometry;
    Mesh* linesSuporteGeometry;
    Mesh* curveGeometry;
    Mesh* numPontosControle;

    // Dados dos Pontos
    Vertex* curvas;

    // Constantes
    static const uint MaxVertex = 6;
    static const uint curvesPoints = 20;
    static const uint lineCountSuporte = 2;
    static const uint quantCountSuporte = 4;

    // Vértices
    Vertex vertices[MaxVertex];
    Vertex line[lineCountSuporte];
    Vertex lines[quantCountSuporte];
    Vertex pontos[quantCountSuporte][MaxVertex];

    // Localização e Contadores
    uint count = 300;
    uint numPontos = 0;
    uint contadorCurvas = 0;
    XMFLOAT2 pontosLocalizacao[4]{};
    XMFLOAT4 pontosVertice[MaxVertex] = { // Mostrar o ponto clicado 
        XMFLOAT4(-0.02f, 0.02f, 0.0f, 1.0f),
        XMFLOAT4(0.02f, 0.02f, 0.0f, 1.0f),
        XMFLOAT4(0.02f, -0.02f, 0.0f, 1.0f),
        XMFLOAT4(0.02f, -0.02f, 0.0f, 1.0f),
        XMFLOAT4(-0.02f, -0.02f, 0.0f, 1.0f),
        XMFLOAT4(-0.02f, 0.02f, 0.0f, 1.0f)
    };

public:
    // Métodos padrões do DirectX
    void Init();
    void Update();
    void Display();
    void Finalize();

    void BuildRootSignature();
    void BuildPipelineState();

    void CreateCurves(float x, float y); // Criação das curvas
    float BuildBezierCurve(float, float, float, float, float); // Realizar calculo da curva de Bezier
    void AddControlPoint(float x, float y); // Adicionar pontos de controle e linha de suporte

    void SaveCurves(); // Salvar curva em arquivo .txt
    void LoadCurves(); // Carregar uma curva salva em arquivo .txt
    void DeleteCurves(); // Eliminar uma curva
};

void Curves::Init()
{
    // Reseta os comandos anteriores
    graphics->ResetCommands();

    // Aloca memória para a curva
    curvas = new Vertex[count];

    // Inicializa os buffers de vértices
    const uint vbSize = MaxVertex * sizeof(Vertex);

    // Inicializa o buffer de geometria
    geometry = new Mesh(vertices, vbSize, sizeof(Vertex));

    // Inicializa os buffers para as curvas e pontos
    curveGeometry = new Mesh(curvas, count * sizeof(Vertex), sizeof(Vertex));
    numPontosControle = new Mesh(pontos, quantCountSuporte * MaxVertex * sizeof(Vertex), sizeof(Vertex));
    linesSuporteGeometry = new Mesh(lines, quantCountSuporte * sizeof(Vertex), sizeof(Vertex));
    lineSuporteGeometry = new Mesh(line, lineCountSuporte * sizeof(Vertex), sizeof(Vertex));

    // Configura o Root Signature e o Pipeline State
    BuildRootSignature();
    BuildPipelineState();

    // Submete os comandos para o GPU
    graphics->SubmitCommands();
}



void Curves::DeleteCurves() {
    numPontos = 0;
    contadorCurvas = 0;

    if (curvas) {
        delete[] curvas;
        curvas = nullptr; 
    }

    curvas = new Vertex[count];

    if (curveGeometry) {
        graphics->ResetCommands();
        graphics->Copy(curvas, curveGeometry->vertexBufferSize, curveGeometry->vertexBufferUpload, curveGeometry->vertexBufferGPU);
        graphics->SubmitCommands();
    }

    if (linesSuporteGeometry) {
        graphics->ResetCommands();
        graphics->Copy(lines, linesSuporteGeometry->vertexBufferSize, linesSuporteGeometry->vertexBufferUpload, linesSuporteGeometry->vertexBufferGPU);
        graphics->SubmitCommands();
    }

    if (numPontosControle) {
        graphics->ResetCommands();
        graphics->Copy(pontos, numPontosControle->vertexBufferSize, numPontosControle->vertexBufferUpload, numPontosControle->vertexBufferGPU);
        graphics->SubmitCommands();
    }
}


void Curves::SaveCurves() {
    ofstream arquivo("curves.txt");

    if (arquivo.is_open()) {
        arquivo << count << '\n';
        arquivo << contadorCurvas << '\n';

        for (int i{}; i < quantCountSuporte; i++) {
            arquivo << lines[i].Pos.x << ' ' << lines[i].Pos.y << ' ' << lines[i].Pos.z << ' ';
            arquivo << lines[i].Color.x << ' ' << lines[i].Color.y << ' ' << lines[i].Color.z << ' ' << lines[i].Color.w << '\n';
        }

        arquivo << numPontos << '\n';

        for (int i{}; i < quantCountSuporte; i++) {
            for (int j{}; j < MaxVertex; j++) {
                arquivo << pontos[i][j].Pos.x << ' ' << pontos[i][j].Pos.y << ' ' << pontos[i][j].Pos.z << ' ';
                arquivo << pontos[i][j].Color.x << ' ' << pontos[i][j].Color.y << ' ' << pontos[i][j].Color.z << ' ' << pontos[i][j].Color.w << '\n';
            }
        }

        for (int i{}; i < quantCountSuporte; i++) 
            arquivo << pontosLocalizacao[i].x << ' ' << pontosLocalizacao[i].y << '\n';

        for (int i{}; i < 2; i++) {
            arquivo << line[i].Pos.x << ' ' << line[i].Pos.y << ' ' << line[i].Pos.z << ' ';
            arquivo << line[i].Color.x << ' ' << line[i].Color.y << ' ' << line[i].Color.z << ' ' << line[i].Color.w << '\n';
        }

        for (int i{}; i < contadorCurvas; i++) {
            arquivo << curvas[i].Pos.x << ' ' << curvas[i].Pos.y << ' ' << curvas[i].Pos.z << ' ';
            arquivo << curvas[i].Color.x << ' ' << curvas[i].Color.y << ' ' << curvas[i].Color.z << ' ' << curvas[i].Color.w << '\n';
        }

        arquivo.close();
    }
}

void Curves::LoadCurves() {
    ifstream arquivo("curves.txt");

    if (arquivo.is_open()) {
        string linha;
        int i = 0;

        getline(arquivo, linha);
        count = stoi(linha);
        curvas = new Vertex[count];

        getline(arquivo, linha);
        contadorCurvas = stoi(linha);

        for (i = 0; i < quantCountSuporte; i++) {
            getline(arquivo, linha);
            istringstream iss(linha);
            iss >> lines[i].Pos.x >> lines[i].Pos.y >> lines[i].Pos.z;
            iss >> lines[i].Color.x >> lines[i].Color.y >> lines[i].Color.z >> lines[i].Color.w;
        }

        getline(arquivo, linha);
        numPontos = stoi(linha);

        for (i = 0; i < quantCountSuporte; i++) {
            for (int j = 0; j < MaxVertex; j++) {
                getline(arquivo, linha);
                istringstream iss(linha);
                iss >> pontos[i][j].Pos.x >> pontos[i][j].Pos.y >> pontos[i][j].Pos.z;
                iss >> pontos[i][j].Color.x >> pontos[i][j].Color.y >> pontos[i][j].Color.z >> pontos[i][j].Color.w;
            }
        }

        for (i = 0; i < quantCountSuporte; i++) {
            getline(arquivo, linha);
            istringstream iss(linha);
            iss >> pontosLocalizacao[i].x >> pontosLocalizacao[i].y;
        }

        for (i = 0; i < 2; i++) {
            getline(arquivo, linha);
            istringstream iss(linha);
            iss >> line[i].Pos.x >> line[i].Pos.y >> line[i].Pos.z;
            iss >> line[i].Color.x >> line[i].Color.y >> line[i].Color.z >> line[i].Color.w;
        }

        for (i = 0; i < contadorCurvas; i++) {
            getline(arquivo, linha);
            istringstream iss(linha);
            iss >> curvas[i].Pos.x >> curvas[i].Pos.y >> curvas[i].Pos.z;
            iss >> curvas[i].Color.x >> curvas[i].Color.y >> curvas[i].Color.z >> curvas[i].Color.w;
        }

        curveGeometry = new Mesh(&curvas, (count * sizeof(Vertex)), sizeof(Vertex));

        arquivo.close();

        // Atualiza na tela os dados da curva 
        graphics->ResetCommands();
        graphics->Copy(curvas, curveGeometry->vertexBufferSize, curveGeometry->vertexBufferUpload, curveGeometry->vertexBufferGPU);
        graphics->SubmitCommands();

    }
}

// Recebe todos os pontos e gera a curva de Bezier
float Curves::BuildBezierCurve(float t, float p1, float p2, float p3, float p4) {
    float u = 1 - t;
    float u2 = u * u;
    float t2 = t * t;

    return u * u2 * p1 + 3 * u2 * t * p2 + 3 * u * t2 * p3 + t * t2 * p4; // Equação para formar a curva de Bezier
} 


void Curves::CreateCurves(float x, float y) {
    // Verifica se há 4 pontos de controle
    if (numPontos == 4) {
        // Ajusta as posições dos pontos para criar continuidade na curva
        for (int i = 0; i < MaxVertex; i++) {
            pontos[1][i].Pos.x = pontos[3][i].Pos.x + (pontos[3][i].Pos.x - pontos[2][i].Pos.x);
            pontos[1][i].Pos.y = pontos[3][i].Pos.y + (pontos[3][i].Pos.y - pontos[2][i].Pos.y);
            pontos[0][i].Pos.x = pontos[3][i].Pos.x;
            pontos[0][i].Pos.y = pontos[3][i].Pos.y;
        }

        // Atualiza as linhas de suporte para os novos pontos
        lines[0] = lines[3];
        lines[1].Pos.x = lines[3].Pos.x + (lines[3].Pos.x - lines[2].Pos.x);
        lines[1].Pos.y = lines[3].Pos.y + (lines[3].Pos.y - lines[2].Pos.y);
        numPontos = 2;

        // Atualiza as localizações dos pontos
        pontosLocalizacao[0] = pontosLocalizacao[3];
        pontosLocalizacao[1].x = pontosLocalizacao[3].x + (pontosLocalizacao[3].x - pontosLocalizacao[2].x);
        pontosLocalizacao[1].y = pontosLocalizacao[3].y + (pontosLocalizacao[3].y - pontosLocalizacao[2].y);

        // Atualiza a linha de suporte
        line[0] = { XMFLOAT3(pontosLocalizacao[1].x, pontosLocalizacao[1].y, 0.0f), XMFLOAT4(Colors::Yellow) };

        // Atualiza o buffer de vértices para os pontos de controle e linhas de suporte
        graphics->ResetCommands();
        graphics->Copy(pontos, numPontosControle->vertexBufferSize, numPontosControle->vertexBufferUpload, numPontosControle->vertexBufferGPU);
        graphics->SubmitCommands();

        graphics->ResetCommands();
        graphics->Copy(lines, linesSuporteGeometry->vertexBufferSize, linesSuporteGeometry->vertexBufferUpload, linesSuporteGeometry->vertexBufferGPU);
        graphics->SubmitCommands();

        contadorCurvas += 20;

        // Verifica se é necessário aumentar a capacidade do buffer de curvas
        if (contadorCurvas >= count) {
            if (count * 2 <= 1000) {
                int newCount = count * 2;
                Vertex* aux = new Vertex[newCount];

                // Copia as curvas existentes para o novo buffer
                for (int i = 0; i < contadorCurvas; i++) {
                    aux[i] = curvas[i];
                }

                delete[] curvas;
                count = newCount;
                curvas = aux;

                // Cria uma nova geometria para a curva
                delete curveGeometry;
                curveGeometry = new Mesh(&curvas, (count * sizeof(Vertex)), sizeof(Vertex));

                // Atualiza o buffer de vértices para as curvas
                graphics->ResetCommands();
                graphics->Copy(curvas, curveGeometry->vertexBufferSize, curveGeometry->vertexBufferUpload, curveGeometry->vertexBufferGPU);
                graphics->SubmitCommands();
            }
            else {
                contadorCurvas = 1000;
            }
        }
    }

    // Verifica se há mais de um ponto de controle
    if (numPontos > 1) {
        if (numPontos == 2) {
            pontosLocalizacao[2] = pontosLocalizacao[3] = { x, y };
        }
        else if (numPontos == 3) {
            pontosLocalizacao[3] = { x, y };
        }
    }

    // Gera os pontos da curva de Bezier usando os pontos de controle
    if (numPontos > 1) {
        for (int i = 0; i < curvesPoints; i++) {
            float t = i / 20.0f;
            curvas[i + contadorCurvas] = {
                XMFLOAT3(BuildBezierCurve(t, pontosLocalizacao[0].x, pontosLocalizacao[1].x, pontosLocalizacao[2].x, pontosLocalizacao[3].x),
                         BuildBezierCurve(t, pontosLocalizacao[0].y, pontosLocalizacao[1].y, pontosLocalizacao[2].y, pontosLocalizacao[3].y), 0.0f),
                XMFLOAT4(Colors::White)
            };
        }

        // Atualiza o buffer de vértices para a curva
        graphics->ResetCommands();
        graphics->Copy(curvas, curveGeometry->vertexBufferSize, curveGeometry->vertexBufferUpload, curveGeometry->vertexBufferGPU);
        graphics->SubmitCommands();
    }

    // Atualiza as posições dos vértices baseados na nova posição
    for (int i = 0; i < MaxVertex; i++) {
        vertices[i].Pos.x = x + pontosVertice[i].x;
        vertices[i].Pos.y = y + pontosVertice[i].y;
    }

    // Atualiza o buffer de vértices para a geometria
    graphics->ResetCommands();
    graphics->Copy(vertices, geometry->vertexBufferSize, geometry->vertexBufferUpload, geometry->vertexBufferGPU);
    graphics->SubmitCommands();

    // Se há entre 1 e 3 pontos de controle, atualiza a linha de suporte
    if (numPontos >= 1 && numPontos <= 3) {
        line[1] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::Yellow) };

        graphics->ResetCommands();
        graphics->Copy(line, lineSuporteGeometry->vertexBufferSize, lineSuporteGeometry->vertexBufferUpload, lineSuporteGeometry->vertexBufferGPU);
        graphics->SubmitCommands();
    }
}


void Curves::AddControlPoint(float x, float y)
{
    // Verifica se o número de pontos de controle está dentro do limite (0 a 3)
    if (numPontos >= 0 && numPontos <= 3) {
        // Para cada vértice, define a posição e a cor
        for (int i = 0; i < MaxVertex; i++) {
            pontos[numPontos][i].Pos.x = x + pontosVertice[i].x; // Ajusta a posição x
            pontos[numPontos][i].Pos.y = y + pontosVertice[i].y; // Ajusta a posição y
            pontos[numPontos][i].Color = { XMFLOAT4(Colors::OrangeRed) }; // Define a cor do vértice
        }

        // Armazena a localização do ponto de controle
        pontosLocalizacao[numPontos] = { x, y };

        // Atualiza o buffer de vértices para os pontos de controle
        graphics->ResetCommands();
        graphics->Copy(pontos, numPontosControle->vertexBufferSize, numPontosControle->vertexBufferUpload, numPontosControle->vertexBufferGPU);
        graphics->SubmitCommands();

        // Define e atualiza as linhas de suporte para os pontos de controle
        lines[numPontos] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::Yellow) };
        line[0] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::Yellow) };

        // Atualiza o buffer de vértices para as linhas de suporte
        graphics->ResetCommands();
        graphics->Copy(lines, linesSuporteGeometry->vertexBufferSize, linesSuporteGeometry->vertexBufferUpload, linesSuporteGeometry->vertexBufferGPU);
        graphics->SubmitCommands();

        // Incrementa o número de pontos de controle
        numPontos++;
    }
}



void Curves::Update()
{

    // Capturar posição do mouse
    float cx = float(window->CenterX());
    float cy = float(window->CenterY());
    float mx = float(input->MouseX());
    float my = float(input->MouseY());

    // Calculos
    float x = (mx - cx) / cx;
    float y = (cy - my) / cy;

    // Lógica de criação da curva
    CreateCurves(x, y);

    // Pegar tecla precionada
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    if (input->KeyPress(VK_DELETE)) 
        DeleteCurves();

    if (input->KeyPress(VK_LBUTTON))
        AddControlPoint(x, y);
       
    if (input->KeyPress('S'))
        SaveCurves();

    if (input->KeyPress('L'))
        LoadCurves();
   
    Display(); // No final sempre chama o display para mostrar na tela
}

void Curves::Display()
{
    // Limpeza do Pipeline
    graphics->Clear(pipelineState);
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);

    // Geometria básica
    graphics->CommandList()->IASetVertexBuffers(0, 1, geometry->VertexBufferView());
    graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    graphics->CommandList()->DrawInstanced(6, 1, 0, 0);

    // Pontos de controle
    graphics->CommandList()->IASetVertexBuffers(0, 1, numPontosControle->VertexBufferView());
    graphics->CommandList()->DrawInstanced(6 * numPontos, 1, 0, 0);

    // Linha de controle
    graphics->CommandList()->IASetVertexBuffers(0, 1, linesSuporteGeometry->VertexBufferView());
    graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
    graphics->CommandList()->DrawInstanced(numPontos, 1, 0, 0);

    // Linha iterativa
    graphics->CommandList()->IASetVertexBuffers(0, 1, lineSuporteGeometry->VertexBufferView());
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    if(numPontos >= 1) graphics->CommandList()->DrawInstanced(lineCountSuporte, 1, 0, 0);

    // Curva Bézier
    graphics->CommandList()->IASetVertexBuffers(0, 1, curveGeometry->VertexBufferView());
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    graphics->CommandList()->DrawInstanced(count, 1, 0, 0);

    graphics->Present(); // Mostrar na tela
}



void Curves::Finalize()
{
    rootSignature->Release();
    pipelineState->Release();
    // Deletando cada ponteiro
    delete curvas;
    delete geometry;
    delete lineSuporteGeometry;
    delete linesSuporteGeometry;
    delete curveGeometry;
    delete numPontosControle;

}

void Curves::BuildRootSignature()
{
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 0;
    rootSigDesc.pParameters = nullptr;
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.pStaticSamplers = nullptr;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* serializedRootSig = nullptr;
    ID3DBlob* error = nullptr;

    ThrowIfFailed(D3D12SerializeRootSignature(
        &rootSigDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig,
        &error));

    ThrowIfFailed(graphics->Device()->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));
}

void Curves::BuildPipelineState()
{

    D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    ID3DBlob* vertexShader;
    ID3DBlob* pixelShader;

    D3DReadFileToBlob(L"Shaders/Vertex.cso", &vertexShader);
    D3DReadFileToBlob(L"Shaders/Pixel.cso", &pixelShader);

    D3D12_RASTERIZER_DESC rasterizer = {};
    rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterizer.CullMode = D3D12_CULL_MODE_NONE;
    rasterizer.FrontCounterClockwise = FALSE;
    rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer.DepthClipEnable = TRUE;
    rasterizer.MultisampleEnable = FALSE;
    rasterizer.AntialiasedLineEnable = FALSE;
    rasterizer.ForcedSampleCount = 0;
    rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    D3D12_BLEND_DESC blender = {};
    blender.AlphaToCoverageEnable = FALSE;
    blender.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        blender.RenderTarget[i] = defaultRenderTargetBlendDesc;

    D3D12_DEPTH_STENCIL_DESC depthStencil = {};
    depthStencil.DepthEnable = TRUE;
    depthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencil.StencilEnable = FALSE;
    depthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    depthStencil.FrontFace = defaultStencilOp;
    depthStencil.BackFace = defaultStencilOp;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.pRootSignature = rootSignature;
    pso.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    pso.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    pso.BlendState = blender;
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState = rasterizer;
    pso.DepthStencilState = depthStencil;
    pso.InputLayout = { inputLayout, 2 };
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pso.SampleDesc.Count = graphics->Antialiasing();
    pso.SampleDesc.Quality = graphics->Quality();
    graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState));

    vertexShader->Release();
    pixelShader->Release();
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    try
    {
        Engine* engine = new Engine();
        engine->window->Mode(WINDOWED);
        engine->window->Size(1024, 600);
        engine->window->ResizeMode(ASPECTRATIO);
        engine->window->Color(0, 0, 0);
        engine->window->Title("Trabalho Prático 01");
        engine->window->Icon(IDI_ICON);
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);
        engine->Start(new Curves());

        delete engine;
    }
    catch (Error& e)
    {
        MessageBox(nullptr, e.ToString().data(), "Curves", MB_OK);
    }

    return 0;
}