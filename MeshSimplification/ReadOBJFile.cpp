#include "mesh.h"
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>


bool Mesh::ReadOBJFile(char *filename)
{
	ifstream input_file(filename);
	const int LINE_LENGTH = 100;
	n_vertices = 0;
	n_edges = 0;
	n_faces = 0;

	n_texturecoords = 0;

	vector<VertexIter> vertex_iterator;

	while (input_file)
	{
		string type;
		input_file >> type;  // 读取一词，space split

		if (type.length() == 1) {
			switch (type[0]) {
			case '#': {
				char comment[LINE_LENGTH];
				input_file.getline(comment, LINE_LENGTH);
				printf("# %s\n", comment);
				break;
			}
			case 'v': {
				double vertCoord[3];
				input_file >> vertCoord[0] >> vertCoord[1] >> vertCoord[2];
				Vertex vert(vertCoord, n_vertices);
				vertices.push_back(vert);
				vertex_iterator.push_back(--(vertices.end()));
				
				//printf("!vertex %d: %f,%f,%f\n", n_vertices, vertCoord[0], vertCoord[1], vertCoord[2]);

				n_vertices++;
				break;
			}
			case 'f': {
				int v_index[3];
				int vt_index[3];

				string IndexPartStr;
				for (int i = 0; i < 3; i++) {
					input_file >> IndexPartStr;
					int splitIndex = IndexPartStr.find('/');
					istringstream temp(IndexPartStr.substr(0, splitIndex ));
					temp >> v_index[i];
					v_index[i]--;   // obj文件中index是从1开始的

					temp = istringstream( IndexPartStr.substr(splitIndex + 1, IndexPartStr.length()));
					temp >> vt_index[i];
					vt_index[i]--;
				}

				faces.push_back(Face(vertex_iterator[v_index[0]], vertex_iterator[v_index[1]], vertex_iterator[v_index[2]], n_faces, vt_index));

				//printf("!face %d: %d/%d,%d/%d,%d/%d \n", n_faces, v_index[0],vt_index[0], v_index[1],vt_index[1], v_index[2],vt_index[2]);

				n_faces++;
				break;
			}
			}
		}

		if (type.length() == 2)
		{
			if (type == "vt") {
				double vertTexcoord[2];
				input_file >> vertTexcoord[0] >> vertTexcoord[1];

				texturecoords.push_back(TexCoord(vertTexcoord, n_texturecoords));

				//printf("!texture coord %d: %f,%f\n", n_texturecoords, vertTexcoord[0], vertTexcoord[1]);

				n_texturecoords++;
			}
		}
	}

	
	input_file.close();
	
	double range_min[3] = { 1.0e6,  1.0e6,  1.0e6, };
	double range_max[3] = { -1.0e6, -1.0e6, -1.0e6, };
	double center[3];
	// 遍历每个顶点找到坐标x,y,z的最值
	for (VertexIter vi = vertices.begin(); vi != vertices.end(); vi++) {
		for (int i = 0; i < 3; i++) {
			if (vi->coord[i] < range_min[i])	range_min[i] = vi->coord[i];
			if (vi->coord[i] > range_max[i])	range_max[i] = vi->coord[i];
		}
	}

	for (int i = 0; i < 3; i++) center[i] = (range_min[i] + range_max[i])*0.5;

	double largest_range = -1.0;  // 三个方向的最大跨度

	for (int i = 0; i < 3; i++) {
		if (largest_range < range_max[i] - range_min[i]) largest_range = range_max[i] - range_min[i];
	}

	double scale_factor = 2.0 / largest_range;

	for (VertexIter vi = vertices.begin(); vi != vertices.end(); vi++) {
		for (int i = 0; i < 3; i++) {
			vi->coord[i] = (vi->coord[i] - center[i]) * scale_factor;
		}
	}

	printf("Reading OBJ file done ... \n");
	printf("# of vertices %d\n ", n_vertices);
	printf("# of faces %d\n", n_faces);
	return true;
}

void Mesh::OutputOBJ()
{
	ofstream output("./output.obj");
	printf("\nbegin output...");
	// 输出顶点
	for (VertexIter vi = vertices.begin(); vi != vertices.end(); vi++) {
		output << "v " << vi->coord[0] << " " << vi->coord[1] << " " << vi->coord[2] << "\n";
	}

	// 输出纹理坐标
	for (TexCoordIter ti = texturecoords.begin(); ti != texturecoords.end(); ti++)
	{
		output << "vt " << ti->coord[0] << " " << ti->coord[1]<< "\n";
	}
	
	// 输出顶点法线,每个顶点上在各个面上的法线是一致的
	for (VertexIter vi = vertices.begin(); vi != vertices.end(); vi++) {
		output << "vn " << vi->normal[0] << " " << vi->normal[1] << " " << vi->normal[2] << "\n";
	}


	// 输出面
	for (FaceIter fi = faces.begin(); fi != faces.end(); fi++) {
		if (fi->isActive) {
			output << "f ";
			for (int i = 0; i < 3; i++) {
				output << fi->halfedge[i].vertex->id + 1 << "/";  //顶点索引
				output << fi->texCoordIndex[i] + 1 << "/";
				output << fi->halfedge[i].vertex->id + 1 << " ";  //法线索引

			}
			output << "\n";
		}
	}

	printf("output done.");
}