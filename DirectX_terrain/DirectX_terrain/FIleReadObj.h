#pragma once

#include "define.h"
#include <d3dx9.h>
#define MAX_NAME_LEN 128
using namespace std;
class FIleReadObj
{
public:
	static bool ReadObj(string path)
	{
		vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		vector<D3DXVECTOR3> temp_vt;
		vector<D3DXVECTOR2> temp_uv;
		vector<D3DXVECTOR3> temp_normal;

		
		FILE* file;
		fopen_s(&file, path.c_str(), "r");


		while (true) {

			char lineHeader[MAX_NAME_LEN];
			// read the first word of the line
			int res = fscanf_s(file, "%s", lineHeader, MAX_NAME_LEN);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

			if (strcmp(lineHeader, "v") == 0) 
			{
				D3DXVECTOR3 vt;
				fscanf_s(file, "%f %f %f\n", &vt.x, &vt.y, &vt.z);
				temp_vt.push_back(vt);
			}
			else if (strcmp(lineHeader, "vt") == 0) 
			{
				D3DXVECTOR2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				temp_uv.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				D3DXVECTOR3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, 
					&normal.y, &normal.z);
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				fgets(lineHeader, MAX_NAME_LEN, file);
				
				
				unsigned int vtIdx[5], uvIdx[5], normIdx[5];
				char* p = lineHeader;
				int i;
				for (i = 0; i < 5; ++i) {
					while (*p == ' ' || *p == '\t') p++;
					if (*p == '\0' || *p == '\r' || *p == '\n') break;

					vtIdx[i] = strtoul(p, &p, 10);
					if (*p && *p == '/') {
						p++;
						uvIdx[i] = strtoul(p, &p, 10);
						if (*p && *p == '/') {
							p++;
							normIdx[i] = strtoul(p, &p, 10);
						}
					}
				}
				printf("ÀÏ°Ï¤¶Áö·Õ\n");
				/*int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vtIdx[0], &uvIdx[0], &normIdx[0],
					&vtIdx[1], &uvIdx[1], &normIdx[1],
					&vtIdx[2], &uvIdx[2], &normIdx[2]);


				for (int i = 0; i < 3; i++)
				{
					vertexIndices.push_back(vtIdx[i]);
					uvIndices.push_back(uvIdx[i]);
					normalIndices.push_back(normIdx[i]);
				}*/
			}
		}
		printf("Á¾·á\n");
		return true;
	}
};

