#include "BlendMap.h"


BlendMap::BlendMap(void)
{
}

BlendMap::~BlendMap(void)
{
}

//Laddar BlendMap från RAW-fil
void BlendMap::Load(std::string filename, int Width, int Height)
{
	m_Width		= Width;
	m_Height	= Height;

	m_Map = new XMFLOAT4*[Height];
	for (int z = 0; z < Height; z++)
		m_Map[z] = new XMFLOAT4[Width];

	std::vector<unsigned char> vertexHeights(4 * Width * Height);

	//Läser in data från fil.
	std::ifstream fin;
	fin.open(filename, std::ios_base::binary);

	if (!fin) HRESULT(E_FAIL);

	fin.read((char *)&vertexHeights[0], (std::streamsize)vertexHeights.size());
	fin.close();

	int k = 0;
	for (int z = 0; z < Height; z++)
	{
		//Inverterar data i Z axeln.
		int Z = Height - 1 - z;
		for (int x = 0; x < Width; x++)
		{
			float r			= (float) vertexHeights[k];		//Rött värde för pixeln.
			float g			= (float) vertexHeights[k + 1];	//Grönt värde för pixeln.
			float b			= (float) vertexHeights[k + 2];	//Blått värde för pixeln.
			float a			= (float) vertexHeights[k + 3];	//Alpha värde för pixeln.
			float sum		= r + g + b + a;				//Summan av färgkomponenterna för pixeln.

			r				= r / sum;						//Andel rött i pixeln.
			g				= g / sum;						//Andel grönt i pixeln.
			b				= b / sum;						//Andel blått i pixeln.
			a				= a / sum;						//Andel alpha i pixeln.

			m_Map[Z][x] = XMFLOAT4(r,g,b, a);			//Sparar värderna.
			k += 4;											//Räknar fram.
		}
	}
}

//
XMFLOAT4 BlendMap::GetBlendData(float x, float z)
{
	if (x < 0 || z < 0 ) 
		return XMFLOAT4(0,0,0,0);

	int maxWidth  = (m_Width  - 1);
	int maxHeight = (m_Height - 1);

	float X = x * maxWidth;
	float Z = z * maxHeight;

	if ( X > maxWidth || Z > maxHeight )
		return XMFLOAT4(0,0,0,0);

	if ( X == maxWidth || Z == maxHeight )
		return m_Map[(int)Z][(int)X];

	//hämtar blenddata för hörnen i den rektangel man står i
	XMVECTOR a = XMLoadFloat4(&m_Map[(int)Z+1][(int)X]);
	XMVECTOR b = XMLoadFloat4(&m_Map[(int)Z+1][(int)X+1]);
	XMVECTOR c = XMLoadFloat4(&m_Map[(int)Z][(int)X]);
	XMVECTOR d = XMLoadFloat4(&m_Map[(int)Z][(int)X+1]);

	float s = X - (int)X;
	float t = 1 - (Z - (int)Z);

	XMFLOAT4 result;

	//kollar vilken triangel man står i och räknar ut blenddata därefter.
	if (s + t <= 1)					//abc-triangel
	{
		XMVECTOR uy = b - a;
		XMVECTOR vy = c - a;
		XMStoreFloat4(&result, a + s * uy + t * vy);
	}

	else							//bcd-triangel
	{
		XMVECTOR uy = c - d;
		XMVECTOR vy = b - d;
		XMStoreFloat4(&result, d + (1.0f - s) * uy + (1.0f - t) * vy);
	}

	return result;
}
