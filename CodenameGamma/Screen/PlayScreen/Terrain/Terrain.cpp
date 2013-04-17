#include "Terrain.h"

Terrain::Terrain(void)
{
}

Terrain::Terrain(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext)
{
	m_VertexBuffer	= 0;
	m_IndexBuffer	= 0;
	m_HeigthMap		= 0;
	m_BlendMap		= 0;

	m_DeviceContext		= DeviceContext;
	m_Device			= Device;

	m_Width			= 0;
	m_Height		= 0;
	m_QuadWidth		= 0;
	m_QuadHeight	= 0;

	XMStoreFloat4x4(&m_TexTransform, XMMatrixScaling(1, 1, 1));
}

Terrain::~Terrain(void)
{

}

void Terrain::LoadMap(std::string map)
{
	std::string config		= map + "\\map.cfg";
	std::string textruePath = map + "\\Textures\\";
	std::string normalPath	= map + "\\NormalMaps\\";


	std::ifstream file;
	file.open(config);

	std::string str;

	float width, height, resX, resZ;

	file >> str;
	file >> width;
	file >>str;
	file >> height;
	file >>str;
	file >> resX;
	file >>str;
	file >> resZ;

	CreateGrid(resX, resZ, width / (resX - 1), height / (resZ - 1));
	
	while(!file.eof())
	{
		str = "";
		file >> str;

		//Ladda texturer
		if		(str == "Texture1")		ParseTexture(file, textruePath, 0);
		else if	(str == "Texture2")		ParseTexture(file, textruePath, 1);
		else if	(str == "Texture3")		ParseTexture(file, textruePath, 2);
		else if	(str == "Texture4")		ParseTexture(file, textruePath, 3);

		//Ladda normalmaps
		else if	(str == "NormalMap1")	ParseNormal(file, normalPath, 0);
		else if	(str == "NormalMap2")	ParseNormal(file, normalPath, 1);
		else if	(str == "NormalMap3")	ParseNormal(file, normalPath, 2);
		else if	(str == "NormalMap4")	ParseNormal(file, normalPath, 3);

		else if(str == "BlendMap")		ParseBlendMap(file, map);
		else if(str == "HeightMap")		ParseHeightMap(file, map);

		else if(str == "TextureDimensions")	ParseTextureDim(file);
	}
}

void Terrain::ParseTexture(std::ifstream& file, std::string texturePath, int index)
{
	std::string filename;
	file >> filename;
	LoadTexture(&m_GroundTextures[index], texturePath + filename);
}

void Terrain::ParseNormal(std::ifstream& file, std::string normalPath, int index)
{
	std::string filename;
	file >> filename;
	LoadTexture(&m_NormalTextures[index], normalPath + filename);
}

void Terrain::ParseBlendMap(std::ifstream& file, std::string path)
{
	path += "\\";
	std::string filename;
	float resX;	
	float resY;
	file >> filename;
	file >> resX;
	file >> resY;

	LoadBlendMap(path + filename, resX, resY);
}

void Terrain::ParseHeightMap(std::ifstream& file, std::string path)
{
	path += "\\";
	std::string filename;
	float resX;	
	float resY;
	file >> filename;
	file >> resX;
	file >> resY;

	LoadHeightMap(path + filename, resX, resY);
}

void Terrain::ParseTextureDim(std::ifstream& file)
{
	float scaleX, scaleY;
	file >> scaleX;
	file >> scaleY;

	scaleX = (float) (m_Width * m_QuadWidth) / scaleX;
	scaleY = (float) (m_Height * m_QuadHeight) / scaleY;

	XMStoreFloat4x4(&m_TexTransform, XMMatrixScaling(scaleX, scaleY, 1));
}

void Terrain::LoadTexture(ID3D11ShaderResourceView** SRV, std::string path)
{
	if(D3DX11CreateShaderResourceViewFromFile(m_Device, path.c_str(), 0, 0, SRV, 0 ))
		::MessageBox(0, "Failed to create ShaderResourceView (Terrain)", "Error", MB_OK);
}

void Terrain::CreateGrid(int Width, int Height, float QuadWidth, float QuadHeight)
{
	m_Width			= Width;
	m_Height		= Height;
	m_QuadWidth		= QuadWidth;
	m_QuadHeight	= QuadHeight;

	//Vertices.clear();

	float W = (float)Width;
	float H = (float)Height;

	for (int z = 0; z < Height; ++z)
	{
		float Z = z * QuadHeight;
		for (int x = 0; x < Width; ++x)
		{
			float X = x * QuadWidth;

			float TexX = (float) x / (float) Width;
			float TexZ = (float) z / (float) Height;

			Vertex::Terrain vertex;

			vertex.Pos		= XMFLOAT3(X, 0, Z);
			vertex.Normal	= XMFLOAT3(0, 1, 0);
			vertex.Tex		= XMFLOAT2(TexX, TexZ);

			//Calculate Tangent
			vertex.BlendData= XMFLOAT4(1,1,1,1);
			vertex.TangentU = XMFLOAT4(0,0,0,0);

			Vertices.push_back(vertex);
		}
	}

	//Indices.clear();
	UINT indexCount = (m_Width - 1) * (m_Height - 1) * 6;
	Indices.resize(indexCount);

	//Lägger till triangel för triangel i indexfältet.
	int k = 0;
	for (int z = 0; z < m_Height - 1; z++)
		for (int x = 0; x < m_Width - 1; x++)
		{
			//Triangel A
			Indices[k]		= z * m_Width + x;
			Indices[k+1]	= z * m_Width + x + 1;
			Indices[k+2]	= (z+1) * m_Width + x;

			//Triangel B
			Indices[k+3]	= z * m_Width + x + 1;
			Indices[k+4]	= (z+1) * m_Width + x;
			Indices[k+5]	= (z+1) * m_Width + x + 1;
			k += 6;
		}	

	SetVertices();
	SetIndices();
}

void Terrain::SetVertices()
{
	ReleaseCOM(m_VertexBuffer);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Terrain) * Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &Vertices[0];

	m_Device->CreateBuffer(&vbd, &vinitData, &m_VertexBuffer);
}

void Terrain::SetIndices()
{
	ReleaseCOM(m_IndexBuffer);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * Indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &Indices[0];

	m_Device->CreateBuffer(&ibd, &iinitData, &m_IndexBuffer);
}

void Terrain::LoadHeightMap(std::string path, int Width, int Height)
{
	m_HeigthMap = new HeigthMap();
	m_HeigthMap->Load(path, Width, Height);

	for (UINT i = 0; i < Vertices.size(); ++i)
	{
		float x = Vertices[i].Pos.x / (m_Width * m_QuadWidth);
		float z = Vertices[i].Pos.z / (m_Height * m_QuadHeight);
		Vertices[i].Pos.y = m_HeigthMap->GetHeight(x, z);
	}
	
	//Normaliserar vertexpunktens normal.
	for (UINT i = 0; i < Vertices.size(); ++i)
		Vertices[i].Normal = XMFLOAT3(0, 0, 0);

	//Ändra Normalerna:
	int numTriangles = 2 * (m_Width - 1) * (m_Height - 1);
	for (int i = 0; i < numTriangles; i++)
	{
		//Index för vertex per triangel.
		UINT i0 = Indices[i*3+0];
		UINT i1 = Indices[i*3+1];
		UINT i2 = Indices[i*3+2];

		//De tre vertexpunkterna för triangeln.
		Vertex::Terrain v0 = Vertices[i0];
		Vertex::Terrain v1 = Vertices[i1];
		Vertex::Terrain v2 = Vertices[i2];

		//Beräknar nomalen på triangelns yta.
		XMVECTOR v0Pos = XMLoadFloat3(&v0.Pos);
		XMVECTOR v1Pos = XMLoadFloat3(&v1.Pos);
		XMVECTOR v2Pos = XMLoadFloat3(&v2.Pos);

		XMVECTOR e0 = v1Pos - v0Pos;
		XMVECTOR e1 = v2Pos - v0Pos;

		XMVECTOR facenormal = XMVector3Cross(e0, e1);
		facenormal = XMVector3Normalize(facenormal);
		
		//Ser till så att normalen pekar uppåt.
		float dot = 0;
		XMStoreFloat(&dot, XMVector3Dot(facenormal, XMVectorSet(0, 1, 0, 0)));

		if (dot < 0)
			facenormal *= -1;
			
		//Lägger adderar ytnomalen till varje vertex som tillhör triangeln.
		XMVECTOR i0Normal = XMLoadFloat3(&Vertices[i0].Normal);
		XMVECTOR i1Normal = XMLoadFloat3(&Vertices[i1].Normal);
		XMVECTOR i2Normal = XMLoadFloat3(&Vertices[i2].Normal);

		i0Normal += facenormal;
		i1Normal += facenormal;
		i2Normal += facenormal;
		
		XMStoreFloat3(&Vertices[i0].Normal, i0Normal);
		XMStoreFloat3(&Vertices[i1].Normal, i1Normal);
		XMStoreFloat3(&Vertices[i2].Normal, i2Normal);
	}
	
	//Normaliserar vertexpunktens normal.
	for (UINT i = 0; i < Vertices.size(); ++i)
	{
		XMVECTOR Normal = XMLoadFloat3(&Vertices[i].Normal);
		Normal = XMVector3Normalize(Normal);
		XMStoreFloat3(&Vertices[i].Normal, Normal);
	}
	
	SetVertices();
}

void Terrain::LoadBlendMap(std::string path, int Width, int Height)
{
	m_BlendMap = new BlendMap();
	m_BlendMap->Load(path, Width, Height);

	for (UINT i = 0; i < Vertices.size(); ++i)
	{
		float x = Vertices[i].Pos.x / (m_Width * m_QuadWidth);
		float z = Vertices[i].Pos.z / (m_Height * m_QuadHeight);
		Vertices[i].BlendData = m_BlendMap->GetBlendData(x, z);
	}

	SetVertices();
}

void Terrain::Draw(ID3D11DeviceContext* dc)
{
	UINT vertexStride	= sizeof(Vertex::Terrain);
	UINT offset			= 0;

	dc->IASetVertexBuffers(0, 1, &m_VertexBuffer, &vertexStride, &offset);
	dc->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	int indexCount = Indices.size();
	dc->DrawIndexed(indexCount, 0, 0);
}