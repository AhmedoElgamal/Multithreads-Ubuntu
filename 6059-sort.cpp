	#include <bits/stdc++.h>
	#include <iostream>
	#include <fstream>
	using namespace std;
	
	int* arr; //Declaring pointer for the array we will use
	int arrSize; //Declaring size of the array
	
	typedef struct //Struct that includes the left and right of an array
	{
		int left;
		int right;
	} position;
	
	void merge(int low, int mid, int high) //The merge function to merge 2 subarrays
	{
		int n1 = mid - low + 1; //Declaring size of left part
		int n2 = high - mid; //Declaring size of right part
		
		int tempLeft[n1]; //Declaring Temp Arrays
		int tempRight[n2];
		
		for (int i=0; i<n1; i++) //Storing values in left temp array
			tempLeft[i] = arr[low+i];
			
		for (int i=0; i<n2; i++) //Storing values in right temp array
			tempRight[i] = arr[mid+1+i];
			
		int leftIndex = 0; //Declaring indicies to merge the temp arrays in our array
		int rightIndex = 0;
		int firstIndex = low;
		
		while (leftIndex < n1 && rightIndex < n2) //Merge left and right temps in our array sorted
		{
			if (tempLeft[leftIndex] <= tempRight[rightIndex])
			{
				arr[firstIndex] = tempLeft[leftIndex];
				leftIndex++;
			}
			else
			{
				arr[firstIndex] = tempRight[rightIndex];
				rightIndex++;
			}
			
			firstIndex++;
		}
		
		while (leftIndex < n1) //Insert any remaining elements of left temp array in our array
		{
			arr[firstIndex] = tempLeft[leftIndex];
			leftIndex++;
			firstIndex++;
		}
		
		while (rightIndex < n2) //Insert any remaining elements of right temp array in our array
		{
			arr[firstIndex] = tempRight[rightIndex];
			rightIndex++;
			firstIndex++;
		}
	}
	
	
	void* mergeSort(void* arg) //MergeSort function Multithreaded
	{
		position* pos = (position*)arg;
		
		if (pos->left < pos->right)
		{
			int middle = (pos->left + pos->right)/2; //calculate middle value
			
			position* posLeft;
			posLeft = (position*) malloc(sizeof(position)); //allocate for left half
			position* posRight;
			posRight = (position*) malloc(sizeof(position)); //allocate for right half
			
			posLeft->left = pos->left; //Insert data of left half
			posLeft->right = middle;
			
			posRight->left = middle + 1; //Insert data of right half
			posRight-> right = pos->right;
			
			pthread_t leftThread, rightThread; //Declaring 2 threads
			
			//Creating threads for the recursive MergeSort function
			
			pthread_create(&leftThread, NULL, mergeSort, (void*)posLeft); 
			pthread_create(&rightThread, NULL, mergeSort, (void*)posRight);
			
			//Allows application to wait for thread to terminate
			
			pthread_join(leftThread, NULL);
			pthread_join(rightThread, NULL);
			
			merge(pos->left, middle, pos->right); //Merging the 2 halves
		}
		
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
		
		myFile >> arrSize; //Reading array size from file and saving it in variable
		
		arr = (int*) malloc(arrSize*sizeof(int)); //Allocating size of array
		
		for (int i=0; i<arrSize; i++) //Reading values from file and inserting them in array
		{
			myFile >> arr[i];
		}
		
		myFile.close(); //Closing the file we read from
		
		pthread_t thread; //Declaring a thread
		
		position* pos = (position*) malloc(sizeof(position)); //Allocating pointer for the struct
		pos->left = 0;
		pos->right = arrSize-1;
		
		pthread_create(&thread, NULL, mergeSort, (void*)pos); //Creating a thread for MergeSort function
		pthread_join(thread, NULL); //Waiting for last thread to terminate
		
		ofstream writeFile("sortedOutput.txt"); //Creating a file to write the output
		
		cout << "Sorted Array: "; //Print this sentence to console
		writeFile << "Sorted Array: "; //Write this sentence to file
		
		for (int i=0; i<arrSize; i++) //Printing and Writing the array
		{
			cout << arr[i] << " ";
			writeFile << arr[i] << " ";
		}
		
		cout << endl;
		writeFile.close(); //Closing the file we write the output in
		
		return 0;
	}
	
	
