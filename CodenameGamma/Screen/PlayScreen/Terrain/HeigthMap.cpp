#include "HeigthMap.h"

HeigthMap::HeigthMap(void)
{
	m_Width		= 0;
	m_Height	= 0;
}


HeigthMap::~HeigthMap(void)
{
}

//laddar heightmap fr�n en rawfil.
void HeigthMap::Load(std::string filename, int Width, int Height)
{
	m_Width		= Width;
	m_Height	= Height;

	m_Map = new float*[Height];
	for (int z = 0; z < Height; z++)
		m_Map[z] = new float[Width];

	std::vector<unsigned char> vertexHeights(Width * Height);

	//l�ser in data fr�n raw filen.
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

	//filtrerar h�jdmappen 20 g�nger.
	//Filter(20);
}

//J�mnar till marken.
void HeigthMap::Filter(int iterations)
{
	float** temp = new float*[m_Width];
	for (int p = 0; p < m_Width; p++)
		temp[p] = new float[m_Height];
	
	//s�tter v�rdet p� noden till genomsnittv�rdet p� sig sj�lv samt omkringliggande noder.
	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			//adderar sig sj�lv
			int number = 1;
			float sum = m_Map[x][y];

			//adderar h�rn
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

			//r�knar ut genomsnittet.
			temp[y][x] = sum / number;
		}
	}

	//skriver �ver det gamla f�ltet med det nya filtrerade.
	m_Map = temp;

	//k�r filtreringen igen om quality �r st�rre �n 0.
	if (iterations > 0)
		Filter(iterations - 1);
}

//h�mtar h�jddata p� en nod i h�jdmappen.
float HeigthMap::GetY(float x, float z)
{
	if (x < 0 || x >= 1 || z < 0 || z >= 1 ) 
		return 0;

	int X = x * m_Width;
	int Y = z * m_Height;

	return m_Map[Y][X];
}

//h�mtar h�jddata
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

	//h�mtar h�jddata f�r h�rnen i den rektangel man st�r i
	float a = m_Map[(int)Z+1][(int)X];
	float b = m_Map[(int)Z+1][(int)X+1];
	float c = m_Map[(int)Z][(int)X];
	float d = m_Map[(int)Z][(int)X+1];

	float s = X - (int)X;
	float t = 1 - (Z - (int)Z);

	//kollar vilken triangel man st�r i och r�knar ut h�jddata d�refter.
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
