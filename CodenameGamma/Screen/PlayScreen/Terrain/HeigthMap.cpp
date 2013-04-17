#include "HeigthMap.h"

HeigthMap::HeigthMap(void)
{
	m_Width		= 0;
	m_Height	= 0;
}


HeigthMap::~HeigthMap(void)
{
}

//laddar heightmap från en rawfil.
void HeigthMap::Load(std::string filename, int Width, int Height)
{
	m_Width		= Width;
	m_Height	= Height;

	m_Map = new float*[Height];
	for (int z = 0; z < Height; z++)
		m_Map[z] = new float[Width];

	std::vector<unsigned char> vertexHeights(Width * Height);

	//läser in data från raw filen.
	std::ifstream fin;
	fin.open(filename, std::ios_base::binary);

	if (!fin) HRESULT(E_FAIL);

	fin.read((char *)&vertexHeights[0], (std::streamsize)vertexHeights.size());
	fin.close();

	//sparar den inladdade informationen i heigtmappen.
	for (int z = 0; z < Height; z++)
	{
		//Inverterar data i Z axeln.
		int Z = Height - 1 - z;
		for (int x = 0; x < Width; x++)
		{
			m_Map[Z][x] = (float) vertexHeights[z*Width + x];
		}
	}

	//filtrerar höjdmappen 20 gånger.
	//Filter(20);
}

//Jämnar till marken.
void HeigthMap::Filter(int iterations)
{
	float** temp = new float*[m_Width];
	for (int p = 0; p < m_Width; p++)
		temp[p] = new float[m_Height];
	
	//sätter värdet på noden till genomsnittvärdet på sig själv samt omkringliggande noder.
	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			//adderar sig själv
			int number = 1;
			float sum = m_Map[x][y];

			//adderar hörn
			if (y != 0 && x != 0)
			{
				sum		+=	m_Map[y-1][x-1];
				number	+=	1;
			}

			if (y != 0 && x != m_Width - 1)
			{
				sum		+=	m_Map[y-1][x+1];
				number	+=	1;
			}

			if (y != m_Height - 1 && x != m_Width - 1)
			{
				sum		+=	m_Map[x+1][y+1];
				number	+=	1;
			}

			if (y != m_Height - 1 && x != 0)
			{
				sum		+=	m_Map[y+1][x-1];
				number	+=	1;
			}


			//adderar kanter
			if (y != 0)
			{
				sum		+=	m_Map[y-1][x];
				number	+=	1;
			}

			if (y != m_Height - 1)
			{
				sum		+=	m_Map[y+1][x];
				number	+=	1;
			}

			if (x != 0)
			{
				sum		+=	m_Map[y][x-1];
				number	+=	1;
			}

			if (x != m_Width - 1)
			{
				sum		+=	m_Map[y][x+1];
				number	+=	1;
			}

			//räknar ut genomsnittet.
			temp[y][x] = sum / number;
		}
	}

	//skriver över det gamla fältet med det nya filtrerade.
	m_Map = temp;

	//kör filtreringen igen om quality är större än 0.
	if (iterations > 0)
		Filter(iterations - 1);
}

//hämtar höjddata på en nod i höjdmappen.
float HeigthMap::GetY(float x, float z)
{
	if (x < 0 || x >= 1 || z < 0 || z >= 1 ) 
		return 0;

	int X = x * m_Width;
	int Y = z * m_Height;

	return m_Map[Y][X];
}

//hämtar höjddata
//x, z  [0,1]
float HeigthMap::GetHeight(float x, float z)
{
	if (x < 0 || z < 0 ) 
		return 0;

	float X = x * m_Width;
	float Z = z * m_Height;

	if ( X + 1 > m_Width || Z + 1 > m_Height )
		return 0;

	else if ( X + 1 == m_Width || Z + 1 == m_Height )
		return m_Map[(int)Z][(int)X];

	//hämtar höjddata för hörnen i den rektangel man står i
	float a = m_Map[(int)Z+1][(int)X];
	float b = m_Map[(int)Z+1][(int)X+1];
	float c = m_Map[(int)Z][(int)X];
	float d = m_Map[(int)Z][(int)X+1];

	float s = X - (int)X;
	float t = 1 - (Z - (int)Z);

	//kollar vilken triangel man står i och räknar ut höjddata därefter.
	if (s + t <= 1)					//abc-triangel
	{
		float uy = b - a;
		float vy = c - a;
		return a + s * uy + t * vy;
	}

	else							//bcd-triangel
	{
		float uy = c - d;
		float vy = b - d;
		return d + (1.0f - s) * uy + (1.0f - t) * vy;
	}
}
