//VUTBR - FIT - PRL - project2
//Jan Kubis / xkubis13

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <mpi.h>

using namespace std;

#define reg_a 0
#define reg_b 1
#define reg_c 2

typedef struct {
	int a;
	int b;
} param_t;

vector<string> split(const string &src, char delim) {
    
    vector<std::string> elems;

    stringstream ss;
    ss.str(src);
    string elem;
    while (getline(ss, elem, delim)) {
        *(back_inserter(elems)++) = elem;
    }

    return elems;
}

vector<vector<int>> parseMatrix(string lines){

	stringstream ss(lines);
	vector<vector<int>> m;
	string line;
	while(getline(ss,line)){
		vector<int> row;
		vector<string> tokens = split(line,' ');
		for(int i=0; i<tokens.size(); i++){
			row.push_back(stoi(tokens[i]));
		}
		m.push_back(row);
	}
	return m;
}

void printMatrix(vector< vector<int> > m){
		for(int r=0;r<m.size();r++){
			vector<int> row = m[r];
			for(int c=0;c<row.size();c++){
				int elem = row[c];
				char delim = c==(row.size()-1) ? '\n' : ' ';
				printf("%d%c",elem,delim );
			}
		}
}

int main(int argc, char* argv[]){

	int numprocs = 0; //pocet procesoru
	int myid = 0; //muj rank

	//MPI INIT
	MPI_Init(&argc,&argv); // inicializace MPI 
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs); // zjistíme, kolik procesů běží 
	MPI_Comm_rank(MPI_COMM_WORLD, &myid); // zjistíme id svého procesu 
	MPI_Status stat; //struct- obsahuje kod- source, tag, error
	
	// "shared" data---------------------
	vector< vector<int> > m1;
	int m1_rcnt=0,m1_ccnt=0;
	vector< vector<int> > m2;
	int m2_rcnt=0,m2_ccnt=0;
	//-----------------------------------
	
	int master_id = numprocs-1;
	if(myid==master_id){

		string line,lines;

		ifstream m1_ifs;
		m1_ifs.open("mat1");
		if(!m1_ifs.is_open()){
			printf("FAIL\n");
			return 1;
		}
		getline(m1_ifs,line); //first line with row count specification
		m1_rcnt = stoi(line);
		lines.assign(
						(std::istreambuf_iterator<char>(m1_ifs) ),
						(std::istreambuf_iterator<char>() ) 
					); //read rest of file into variable
		m1 = parseMatrix(lines);
		m1_ccnt = m1[0].size();

		ifstream m2_ifs;
		m2_ifs.open("mat2");
		if(!m2_ifs.is_open()){
			printf("FAIL\n");
			return 1;
		}
		getline(m2_ifs,line); //first line with row count specification
		m2_ccnt = stoi(line);
		lines.assign(
						(std::istreambuf_iterator<char>(m2_ifs) ),
						(std::istreambuf_iterator<char>() ) 
					); //read rest of file into variable
		m2 = parseMatrix(lines);
		m2_rcnt = m2.size();

		
		m1_ifs.close();
		m2_ifs.close();
		// printf("%dx%d\n", m1_rcnt, m1_ccnt);
		// printMatrix(m1);
		// printf("%dx%d\n", m2_rcnt, m2_ccnt);
		// printMatrix(m2);
		if(m1_ccnt!=m2_rcnt){
			printf("WRONG DIMENSIONS FOR MULT\n");
			return 0;
		}

		//send matrix-----------------------------------
		MPI_Bcast(&m1_rcnt, 1, MPI_INT, master_id, MPI_COMM_WORLD);
		MPI_Bcast(&m1_ccnt, 1, MPI_INT, master_id, MPI_COMM_WORLD);
		MPI_Bcast(&m2_rcnt, 1, MPI_INT, master_id, MPI_COMM_WORLD);
		MPI_Bcast(&m2_ccnt, 1, MPI_INT, master_id, MPI_COMM_WORLD);
		for(int i=0;i<m1_rcnt;i++){
			vector<int> row;
			for(int j=0;j<m1_ccnt;j++){
				MPI_Bcast(&m1[i][j], 1, MPI_INT, master_id, MPI_COMM_WORLD);
			}
		}
		for(int i=0;i<m2_rcnt;i++){
			vector<int> row;
			for(int j=0;j<m2_ccnt;j++){
				MPI_Bcast(&m2[i][j], 1, MPI_INT, master_id, MPI_COMM_WORLD);
			}
		}
		//----------------------------------------------
		vector<vector<int>> m3;
		for(int i=0;i<m1_rcnt;i++){
			vector<int> row;
			int c=0;
			for(int j=0;j<m2_ccnt;j++){
				MPI_Recv(&c, 1, MPI_INT, (i*m2_ccnt)+j, reg_c, MPI_COMM_WORLD, &stat);
				row.push_back(c);
			}
			m3.push_back(row);
		}
		printf("%d:%d\n",m1_rcnt,m2_ccnt);
		printMatrix(m3);
	}


	if(myid!=master_id){
		//get matrix-----------------------------------
		MPI_Bcast(&m1_rcnt, 1, MPI_INT, master_id, MPI_COMM_WORLD);
		MPI_Bcast(&m1_ccnt, 1, MPI_INT, master_id, MPI_COMM_WORLD);
		MPI_Bcast(&m2_rcnt, 1, MPI_INT, master_id, MPI_COMM_WORLD);
		MPI_Bcast(&m2_ccnt, 1, MPI_INT, master_id, MPI_COMM_WORLD);
		int tmp=0;
		for(int i=0;i<m1_rcnt;i++){
			vector<int> row;
			for(int j=0;j<m1_ccnt;j++){
				MPI_Bcast(&tmp, 1, MPI_INT, master_id, MPI_COMM_WORLD);
				row.push_back(tmp);
			}
			m1.push_back(row);
		}
		for(int i=0;i<m2_rcnt;i++){
			vector<int> row;
			for(int j=0;j<m2_ccnt;j++){
				MPI_Bcast(&tmp, 1, MPI_INT, master_id, MPI_COMM_WORLD);
				row.push_back(tmp);
			}
			m2.push_back(row);
		}
		//---------------------------------------------
		int pos_x = myid / m2_ccnt;
		int pos_y = myid % m2_ccnt;

		int c = 0;
		int a=0,b=0;
		for(int i=0;i<m1_ccnt;i++){
			// leftmost and topmost are not receiving
			if(pos_x==0){
				b=m2[i][pos_y];
			}
			else{
				MPI_Recv(&b, 1, MPI_INT, ((pos_x-1)*m2_ccnt)+pos_y, reg_b, MPI_COMM_WORLD, &stat);
			}
			if(pos_y==0){
				a=m1[pos_x][i];
			}
			else{
				MPI_Recv(&a, 1, MPI_INT, ((pos_x)*m2_ccnt)+pos_y-1, reg_a, MPI_COMM_WORLD, &stat);
			}

			c+= a*b;
			// rightmost and bottommost are not sending
			if(pos_x==m1_rcnt-1){

			}
			else{
				// printf("[%d;%d] send\n", pos_x,pos_y);
				MPI_Send(&b,1,MPI_INT,((pos_x+1)*m2_ccnt)+pos_y,reg_b,MPI_COMM_WORLD);
			}
			if(pos_y==m2_ccnt-1){

			}
			else{
				// printf("[%d;%d] send\n", pos_x,pos_y);
				MPI_Send(&a,1,MPI_INT,(pos_x*m2_ccnt)+pos_y+1,reg_a,MPI_COMM_WORLD);
			}

		}
		MPI_Send(&c, 1, MPI_INT, master_id, reg_c, MPI_COMM_WORLD);
	}








	MPI_Finalize();
}