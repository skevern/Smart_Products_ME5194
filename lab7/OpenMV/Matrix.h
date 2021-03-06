/*------------------------------------------------------/
/   Scott Kevern                                        /
/   ME 5194 Smart Products                              /
/   January 27th, 2018                                  /
/------------------------------------------------------*/

#pragma once
#include <numeric>
#include <vector> 
#include <iostream>
#include<math.h>
#include "Matrix.h"
#include <numeric>
#include <vector>
#include <iostream>
using namespace std; 
template <class T> 

class Matrix
{
private:
	//Matrix Members
	std::vector< std::vector<T> > pMa; //Editable data structure
	int nRows; //number of rows
	int nCols; //number of columns


public:
	//--------Constructors and Destructors----------------  
	// constructor for with number of row and columns  
	Matrix(int m, int n) { pMa.resize(m, std::vector<T>(n)); nRows = m; nCols = n; }
	// Default Constructor  
    Matrix() {}
	//Copy Constructor  
	//Matrix(const Matrix<T>&);  
	//Destructor  
	~Matrix() {}

	// --------------Matrix Manipulation--------------  
	//Assignment Operator  
	Matrix<T>& operator= (const Matrix<T>& A)  
	{   
		// Code for assigning matrix values   
		this->nRows = A.nRows;   
		this->nCols = A.nCols;   
		for (int i = 0; i < nRows; i++)   
		{
			for (int j = 0; j < nCols; j++)    
			{     
                pMa[i][j] = A.pMa[i][j];
			}
		}   
		return *this;  
	}  
	//Indexing Operator  
	T& operator() (int i, int j)  
	{
		return pMa[i][j];
	}


	//---------------Lab Assignment----------------
	//Addition operator
	Matrix operator+ (const Matrix<T>&B)
	{
		//Create a new matrix C to populate
		Matrix<T> C(B.nRows, B.nCols);
        
		//Loop through the rows and columns
		for (int i = 0; i < B.nRows; i++) 
		{
			for (int j = 0; j < B.nCols; j++)
			{
				C(i, j) = this->pMa[i][j] + B.pMa[i][j];
			}
		}
		return C;
	}
	//Subtraction operator
	Matrix operator- (const Matrix<T>&B)
	{
		//Create a new matrix C to populate
		Matrix<T> C(B.nRows, B.nCols);
		//Loop through the rows and columns
		for (int i = 0; i < B.nRows; i++)
		{
			for (int j = 0; j < B.nCols; j++)
			{
				C(i, j) = this->pMa[i][j] - B.pMa[i][j];
			}
		}
		return C;
	}
	//Matrix Multplication Operator  
	Matrix operator* (const Matrix<T>& B)  
	{  

		//Create a new matrix C to populate
		Matrix<T> C(this->nRows, B.nCols);
		for (int i = 0; i < this->nRows; i++)
		{
			for (int j = 0; j < B.nCols; j++)
			{
                T temp = 0.0;
                for (int k = 0; k < B.nRows; k++)
                {
                    temp += this->pMa[i][k] * B.pMa[k][j];
                }
                C(i,j) = temp;
			}
		}
		return C;
	}
	//-------------Inverse Matrix----------------------------
    Matrix<T> inverse_matrix()
    {
        //This function returns the inverse of the matrix
        //First check to make sure the matrix isn't singular
        float det = determinant_matrix(*this, this->nRows);
        if (det == 0)
        {
            std::cout << "The matrix is singular and we can't find it's inverse" << endl;
        }
        
        //Temporary matrix to store the inverse
        Matrix<T> inv(this->nRows, this->nCols);
        
        //Find the adjoint matrix
        Matrix<T> adj(this->nRows,this->nCols);
        adj = this->adjoint_matrix();
        
        //Use the formula: Inverse = (1/determinant)*adjoint
        for (int i = 0; i < this->nRows; i++)
        {
            for (int j = 0; j < this->nCols; j++)
            {
                inv(i, j) = adj.pMa[i][j] / det;
            }
        }
        return inv;
    }
    
    //-------------Pseudo Inverse------------------------
    Matrix<T> pseudo_inverse()
    {
        Matrix<T> ps(this->nCols,this->nCols);
        //Function to determine the left or right inverse of a matrix
        if (this->nCols > this->nRows)
        {
            //Temp matrix to store pseudo-inverse
            Matrix<T> ps(this->nCols,this->nCols);
            //Multiply A * A^T
            ps = (*this) * this->transpose_matrix();
            //Find the inverse
            ps = ps.inverse_matrix();
            //Multiply A^T by result
            ps = this->transpose_matrix() * ps;
            return ps;
        }
        else {
            //Temp matrix to store pseudo-inverse
            Matrix<T> ps(this->nRows,this->nRows);
            //Multiply A^T * A
            ps = (this->transpose_matrix() * (*this));
            //Take the inverse
            ps = ps.inverse_matrix();
            //Multiply by A^T
            ps = ps * this->transpose_matrix();
            return ps;
        }
        return ps;
    }
    
    //-------------Cofactor matrix--------------------
    Matrix<T> cofactor_matrix(const Matrix<T> &B, int m, int n, int n2)
    {
        //Create a temporary matrix
        Matrix<T> C(n2-1, n2-1);
        
        //Function that finds the cofactor for the position (m,n) in matrix B
        int x = 0; int y = 0;
        
        //Loop through each element
        for (int i = 0; i < n2; i++)
        {
            for (int j = 0; j < n2; j++)
            {
                //For all elements that aren't in our row or column, copy them
                if (i != m && j != n)
                {
                    C(x,y++) = B.pMa[i][j];
                    //Once we reach the end of column, we move back to the first column and on to the next row
                    if (y == n2 - 1)
                    {
                        y = 0;
                        x++;
                    }
                }
            }
        }

        return C;
    }
    
    //--------------Determinant of Matrix-------------
    int determinant_matrix(const Matrix<T> &B, int n)
    {
        int determinant = 0;    //initialize result
        //If matrix has one element
        if (n==1)
        {
            return B.pMa[0][0];
        }
        if (n == 2)
        {
            return((B.pMa[0][0] * B.pMa[1][1]) - (B.pMa[1][0] * B.pMa[0][1]));
        }
        
        //Matrix to store cofactors
        Matrix<float> F(n, n);
        int sign = 1;   //To flip between addition and subtraction
        
        //Loop through each element of the first row
        for (int i = 0; i < n; i++){
            //Use cofactor function to get the cofactor
            F = cofactor_matrix(B, 0, i, n);
            determinant += sign * B.pMa[0][i] * determinant_matrix(F, n-1);
            //flip the sign
            sign = -sign;
        }
        
        return determinant;
        
    }
    
    //---------------Adjoint of Matrix----------------
    Matrix<T> adjoint_matrix()
    {
        //Create a matrix to store the adjoint
        Matrix<T> D(this->nRows,this->nCols);
        
        //Base where N = 1
        if (this->nRows == 1 & this->nCols == 1)
        {
            D(0,0) = 1;
            return D;
        }
        
        int sign = 1;
        
        //Create a temporary matrix
        Matrix<T> C(this->nRows,this->nCols);
        
        //loop through rows and columns
        for (int i = 0; i < this->nRows; i++)
        {
            for (int j = 0; j < this->nCols; j++)
            {
                //Find the cofactor for this element
                C = cofactor_matrix(*this, i, j, this->nRows);
                
                //If the sum of the row and column indexes is even -> positive
                if ((i + j)%2 == 0){
                    sign = 1;
                }
                else
                {
                    sign = -1;
                }
                
                //flip the rows and columns to get the transpose
                D(j,i) = sign * determinant_matrix(C, this->nRows - 1);
            }
        }
        return D;
    }
    
    
    //--------------Transpose Matrix-------------------
    Matrix<T> transpose_matrix()
    {
        //Create a matrix to compute the transpose
        Matrix<T> C(this->nCols, this->nRows);
        for (int i = 0; i < this->nRows; i++)
        {
            for (int j = 0; j < this->nCols; j++)
            {
                C(j,i) = this->pMa[i][j];
            }
        }
        return C;
    }
    
    //----------------Function to determine the number of operations required for multiplication
    int number_of_operations(const Matrix<T> &B)
    {
        //Calculate the number of operations required for matrix multiplication of k x l * l x m
        int num_mult = this->nCols;
        int num_add = this->nCols-1;
        int final_num = this->nRows * B.nCols * (num_add + num_mult);
        return final_num;
    }
    
    int number_of_operations(int k, int l, int m)
    {
        //Calculate the number of operations required for matrix multiplication of k x l * l x m
        int num_mult = l;
        int num_add = l-1;
        int final_num = k * m * (num_add + num_mult);
        return final_num;
    }
    
    //----------------Ordering Function----------------------
    void ordering_function(Matrix<T> &B, const Matrix<T> &C)
    {
        //Function takes three matricies as an input, and determines the order to multiply them that results in the fewest mathmatical operations
        //We need to check the two different cases: (A1 * A2) * A3 , or A1 * (A2 * A3)
        int first_case = this->number_of_operations(B) + number_of_operations(this->nRows, B.nRows, C.nCols);
        int second_case = B.number_of_operations(C) + number_of_operations(B.nRows, C.nRows, this->nCols);
        std::cout << "First case number of computations = " << first_case << " computations\nSecond case number of computations = " << second_case << " computations" << endl;
        
        if (first_case < second_case)
        {
            std::cout << "[(A1 * A2) * A3] is faster according to the ordering function!\n" << endl;
        }
        else if (first_case == second_case)
        {
            std::cout << "The order for matrix multiplication does not matter in this case!\n" << endl;
        }
        else{
            std::cout << "[A1 * (A2 * A3)] is faster according to the ordering function!\n" << endl;
        }
        return;
    }
    

	//-------------Display Contents--------------------------  
	void show_matrix()  
	{   
		std::cout << "[";   
		for (int i = 0; i < this->nRows; i++)   
		{    
			std::vector<T> row = pMa[i];
			for (int j = 0; j < this->nCols; j++)    
			{     
				if (j != this->nCols - 1) 
				{ 
					std::cout << row[j] << ", "; 
				}     
				else {      
					if (i != this->nRows - 1) 
					{ 
						std::cout << row[j] << ";";
					}      
					else 
					{      
						std::cout << row[j] << "]";
					}
				}    
			}    
			std::cout<<std::endl;   
		}  
	}  
};


/*-----------References---------------------------------------------------------------------------------------------/
/	https://www.geeksforgeeks.org/adjoint-inverse-matrix/                                                           /
/	https://math.stackexchange.com/questions/484661/calculating-the-number-of-operations-in-matrix-multiplication   /
/	https://www.mathsisfun.com/algebra/matrix-multiplying.html                                                      /
/	http://help.matheass.eu/en/Pseudoinverse.html                                                                   /
/------------------------------------------------------------------------------------------------------------------*/


