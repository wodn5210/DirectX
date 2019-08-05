#pragma once

#include "define.h"
#include <d3dx9.h>
#define MAX_NAME_LEN 128
using namespace std;
class FIleReadObj
{
public:
	static bool ReadObj(string path, vector<TREE2_VTX>& vt, vector<TRI_IDX>* idx_leaves, vector<TRI_IDX>* idx_trunk)
	{


		
		FILE* file;
		fopen_s(&file, path.c_str(), "r");
		vector<TRI_IDX>* pIdx = idx_leaves;
		D3DXVECTOR3 center = D3DXVECTOR3(0, 0, 0);
		

		while (true) {

			char lineHeader[MAX_NAME_LEN];
			// 줄의 첫 단어 읽음
			int res = fscanf_s(file, "%s", lineHeader, MAX_NAME_LEN);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

			if (strcmp(lineHeader, "v") == 0) 
			{
				TREE2_VTX temp;
				fscanf_s(file, "%f %f %f\n", &temp.p.x, &temp.p.y, &temp.p.z);
				if (temp.p.x == 0 && temp.p.y == 0 && temp.p.z == 0)
				{
					temp.p.x = temp.p.y = temp.p.z = 0.00001f;
				}
				center += temp.p;
				temp.n = temp.p;
				//D3DXVec3Normalize(&temp.n, &temp.n);

				vt.push_back(temp);
			}
			else if (strcmp(lineHeader, "leaves") == 0)
			{
				pIdx = idx_leaves;
			}
			else if (strcmp(lineHeader, "trunk") == 0)
			{
				pIdx = idx_trunk;
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				//한줄 통째로 가져오기
				fgets(lineHeader, MAX_NAME_LEN, file);
				
				
				unsigned int vtIdx[5], uvIdx[5], normIdx[5];
				char* p = lineHeader;
				int i;
				for (i = 0; i < 5; ++i) 
				{
					while (*p == ' ' || *p == '\t') p++;
					if (*p == '\0' || *p == '\r' || *p == '\n') break;

					vtIdx[i] = strtoul(p, &p, 10);
					vtIdx[i]--;
					//vertex말고는 다 무시
					if (*p && *p == '/') 
					{
						p++;
						uvIdx[i] = strtoul(p, &p, 10);
						if (*p && *p == '/') 
						{
							p++;
							normIdx[i] = strtoul(p, &p, 10);
						}
					}


				}
				if (i == 3)
				{
					TRI_IDX temp = { vtIdx[0], vtIdx[1], vtIdx[2] };
					pIdx->push_back(temp);
				}
				else if (i == 4)
				{
					TRI_IDX temp1 = { vtIdx[0], vtIdx[1], vtIdx[2] };
					TRI_IDX temp2 = { vtIdx[0], vtIdx[2], vtIdx[3] };
					pIdx->push_back(temp1);
					pIdx->push_back(temp2);

				}
				
			}
		}
		center /= vt.size();
		printf("%.3f %.3f %.3f\n", center.x, center.y, center.z);

		for (int i = 0; i < vt.size(); i++)
		{

			vt[i].n -= center;
			D3DXVec3Normalize(&vt[i].n, &vt[i].n);
		}
		return true;
	}
};

