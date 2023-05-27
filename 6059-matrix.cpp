	#include <bits/stdc++.h>
	#include <iostream>
	#include <fstream>
	using namespace std;
	
	int Arows,Acolumns; //Declaring number of rows and columns of matrix A
	int Brows,Bcolumns; //Declaring number of rows and columns of matrix B


	int** matrixA; //Declaring of matrix A (input of matrix A)
	int** matrixB; //Declaring of matrix B (input of matrix B)
	int** matrixC; //Declaring of matrix C (output of row multiplication)
	int** matrixD; //Declaring of matrix D (output of element by element multiplication)
	
	int currentRow = 0; //used for element by element multiplication to indicate current row
	int currentColumn = 0; //used for element by element multiplication to indicate current column
        
	void* multiplicationByRows(void* arg) //Multiplication by row method multithreaded
	{
		int i = *(int*)arg;
			
		for (int j = 0; j < Bcolumns; j++)
			for (int k = 0; k < Acolumns; k++)
				matrixC[i][j] += matrixA[i][k] * matrixB[k][j]; //Multiplying each row in a thread
		i++;
		
		return 0;
	}
	
	
	void* multiplicationByElements(void* arg) //Multiplication by element method multithreaded
	{
		for(int i=0; i< Acolumns; i++) //Multiplying each element in a thread
			matrixD[currentRow][currentColumn] += matrixA[currentRow][i] * matrixB[i][currentColumn];
			
		return 0;
	}
	
	int main(int argc, const char* argv[])
	{
		string inputFile; //Declaring variable to store input file name
		
		inputFile = argv[1]; //Storing input file name written in terminal
		
		ifstream myFile(inputFile); //Attempt to open file
		
		if (!myFile) //Checking if file doesn't exist
		{
			cout << "File Not Found !!" << endl ;
			return -1;
		}
		
		myFile >> Arows >> Acolumns; //Reading number of rows and columns of Matrix A and inserting them to variables
		
		matrixA = (int**)malloc(Arows*sizeof(int*)); //Allocating Size for Matrix A
		for (int i=0; i<Arows; i++)
			matrixA[i]=(int*)malloc(Acolumns*sizeof(int));
		
		for (int i=0; i<Arows; i++) //Reading values of matrix and inserting them in Matrix A
			for (int j=0; j<Acolumns; j++)
				myFile >> matrixA[i][j];
				
		
		myFile >> Brows >> Bcolumns; //Reading number of rows and columns of Matrix B and inserting them to variables
		
		ofstream writeFile("output.txt"); //Creating a file to write the outputs
		
		if(Brows != Acolumns) //Checking if matrix dimensions are invalid for multiplication
		{
			cout << "Multiplication of Matrix is not Possible !!" << endl; //Printing to console
			writeFile << "Multiplication of Matrix is not Possible !!" << endl; //Writing in file
			exit(0); //Stops the program
		}
		
		
		matrixB = (int**)malloc(Brows*sizeof(int*)); //Allocating Size for Matrix B
		for (int i=0; i<Brows; i++)
			matrixB[i]=(int*)malloc(Bcolumns*sizeof(int));
		
		for (int i=0; i<Brows; i++) //Reading values of matrix and inserting them in Matrix A
			for (int j=0; j<Bcolumns; j++)
				myFile >> matrixB[i][j];
				
		
		matrixC = (int**)malloc(Arows*sizeof(int*)); //Allocating Size for Matrix C
		for (int i=0; i<Arows; i++)
			matrixC[i]=(int*)malloc(Bcolumns*sizeof(int));
		
		matrixD = (int**)malloc(Arows*sizeof(int*)); //Allocating Size for Matrix D
		for (int i=0; i<Arows; i++)
			matrixD[i]=(int*)malloc(Bcolumns*sizeof(int));
		
		myFile.close(); //Closing the file we read from

		clock_t start, finish; //Declaring variables for time measurement
		double timeTaken;
		
		start = clock(); //Clock before we enter threads creation
		
		//Multiplication by Rows
		
		int threadsByRows = Arows; //Maximum number of threads is number of rows in Matrix A
		pthread_t threadsRows[threadsByRows];
                
		for (int i = 0; i < threadsByRows; i++)  //Create Threads to Multiply by Rows in multiplicationByRows method
		{
			int* ptrToPass = new int(i);
			pthread_create(&threadsRows[i], NULL, multiplicationByRows, ptrToPass);
		}


		for (int i = 0; i < threadsByRows; i++) //Allows application to wait for thread to terminate
			pthread_join(threadsRows[i], NULL);
		
		finish = clock(); //Clock after we finish threads creation
		
		writeFile << "By Row" << endl; //Writing this sentence to file
		cout << "By Row" << endl; //Printing this sentence to console
		
		for (int i = 0; i < Arows; i++) //Printing & Writing The Result of Multiplication By Rows
		{
			for (int j = 0; j < Bcolumns; j++)
				{
					cout << matrixC[i][j] << " ";
					writeFile << matrixC[i][j] << " ";	
				}
			cout << endl;
			writeFile << endl;
		}
		
		timeTaken = (double) (finish-start)/CLOCKS_PER_SEC; //Time Taken By Threads
		
		cout << "Time: " << timeTaken << " s" << endl; //Printing Time to console
		writeFile << "Time: " << timeTaken << " s" << endl; //Writing Time to file
		
		//Multiplication ELEMENT BY ELEMENT
		
		int threadsByElement = Arows*Bcolumns; //Maximum number of threads is number of rows in A Times Columns in B
		pthread_t threadsElements[threadsByElement];
		
		int threadCount = 0; //Counter for threads
		
		start = clock(); //Clock before we enter threads creation
		
		for (int i = 0; i < Arows; i++) //Create Threads for element by element multiplication
		{
			for (int j = 0; j < Bcolumns; j++)
			{
				
				pthread_create(&threadsElements[threadCount], NULL, multiplicationByElements, NULL);
				
				pthread_join(threadsElements[threadCount], NULL); //Waiting for threads to terminate
				
				
				currentColumn++; //incrementing the current column of matrix D
				threadCount++; //incrementing threads count
					
			}
			
			currentRow++; //incrementing the current row of matrix D
			currentColumn = 0; //Resetting the column number to zero
		}
		
		finish = clock(); //Clock after we finish threads creation

		cout << endl;
		writeFile << endl;
		
		cout << "By Element" << endl; //Printing sentence to console
		writeFile << "By Element" << endl; //Writing sentence to file
		
		for (int i = 0; i < Arows; i++) //Printing & Writing The Result of Multiplication By Elements
		{
			for (int j = 0; j < Bcolumns; j++)
			{
				cout << matrixD[i][j] << " ";
				writeFile << matrixD[i][j] << " ";
			}
			cout << endl;
			writeFile << endl;
		}
		
		timeTaken = (double) (finish-start)/CLOCKS_PER_SEC; //Time Taken By Threads
		
		cout << "Time: " << timeTaken << " s" << endl; //Printing Time to console
		writeFile << "Time: " << timeTaken << " s" << endl; //Writing Time to file
		
		writeFile.close(); //Closing the file we write the output in
		
		return 0;
	}

