#include <iostream>

using namespace std;

class Matrix
{
private:
	int		size;		// Size of suared-matrix
	double**	element;	// Matrix elements: sizexsize array
	double*		eigenVals;	// vector of size n that contains the eigenvalues
	double**	eigenVectors;	// <size> vectors of size n that contain the eigenvector for each eigenvalue

public:
	Matrix();					// Default constructor
	Matrix(int in_size, double** in_element);	// Parameterized constructor
	Matrix(int in_size, double* in_element);	// Parameterized constructor
	Matrix(const Matrix& orig);			// Copy constructor
	void reset();					// Reset all internal values
	~Matrix();					// Destructor

	void print();					// Print matrix

	int getSize();					// Return the matrix size
	double getElement(int, int);			// Get an element from the matrix
	void setElement(int, int, double);		// Set an element in the matrix

	Matrix copy();					// Create copy of matrix

	void multiplyRowByConstant(int, double);	// Multiply row by constant
	void swapRows(int, int);			// Swap two rows
	void addRowMultiple(int, int, double);		// Add row multiple to another row
	void reducedEchelonForm();			// Transform matrix to REF - Reduced Echelon Form
	void luDecomposition();				// LU decomposition
	void computeEigenValues();			// Find the eigenvalues
	void computeEigenVectors();			// Find the eigenvectors
};

//////////////////////////////////////////////////////////////////////////////
// Default constructor
//////////////////////////////////////////////////////////////////////////////
Matrix::Matrix()
{
	size = 0;
	element = NULL;
	eigenVals = NULL;
	eigenVectors = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Parameterized constructor
//////////////////////////////////////////////////////////////////////////////
Matrix::Matrix(int in_size, double** in_element)
{
	size = in_size;
	element = new double*[size];
	eigenVals = NULL;
	eigenVectors = NULL;
	for (int rr=0; rr<size; rr++) {
		element[rr] = new double[size];
		for (int cc=0; cc<size; cc++) {
			element[rr][cc] = in_element[rr][cc];
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Parameterized constructor
//////////////////////////////////////////////////////////////////////////////
Matrix::Matrix(int in_size, double* in_element)
{
	size = in_size;
	element = new double*[size];
	eigenVals = NULL;
	eigenVectors = NULL;
	for (int rr=0; rr<size; rr++) {
		element[rr] = new double[size];
		for (int cc=0; cc<size; cc++) {
			element[rr][cc] = in_element[rr*size+cc];
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Copy constructor
//////////////////////////////////////////////////////////////////////////////
Matrix::Matrix(const Matrix& orig)
{
	size = orig.size;
	element = new double*[size];
	for (int rr=0; rr<size; rr++) {
		element[rr] = new double[size];
		for (int cc=0; cc<size; cc++) {
			element[rr][cc] = orig.element[rr][cc];
		}
	}

	if (orig.eigenVals != NULL) {
		for (int cc=0; cc<size; cc++) {
			eigenVals[cc] = orig.eigenVals[cc];
		}
	}

	if (orig.eigenVectors != NULL) {
		for (int rr=0; rr<size; rr++) {
			for (int cc=0; cc<size; cc++) {
				eigenVectors[rr][cc] = orig.eigenVectors[rr][cc];
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Reset all internal values
//////////////////////////////////////////////////////////////////////////////
void Matrix::reset()
{
	if (size > 0) {
		for (int rr=0; rr<size; rr++) {
			delete element[rr];
		}
		delete element;
		element = NULL;
	}

	if (eigenVals != NULL) {
		delete eigenVals;
		eigenVals = NULL;
	}

	if (eigenVectors != NULL) {
		for (int rr=0; rr<size; rr++) {
			delete eigenVectors[rr];
		}
		delete eigenVectors;
		eigenVectors = NULL;
	}

	size = 0;
}

//////////////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////////////
Matrix::~Matrix()
{
	reset();
}

//////////////////////////////////////////////////////////////////////////////
// Print matrix
//////////////////////////////////////////////////////////////////////////////
void Matrix::print()
{
	for (int rr=0; rr<size; rr++) {
		cout << element[rr][0];
		for (int cc=1; cc<size; cc++) {
			cout << "	" << element[rr][cc];
		}
		cout << endl;
	}

	cout << endl;

	if (eigenVals != NULL) {
		cout << "Eigenvalues:  ";
		cout << eigenVals[0];
		for (int cc=1; cc<size; cc++) {
			cout << "  ::  " << eigenVals[cc];
		}
		cout << endl;
	}

	cout << endl;

	if (eigenVectors != NULL) {
		for (int rr=0; rr<size; rr++) {
			cout << "Eigenvectors[" << rr << "]:  " << eigenVectors[rr][0];
			for (int cc=1; cc<size; cc++) {
				cout << "  ::  " << eigenVectors[rr][cc];
			}
			cout << endl;
		}
	}

	cout << "--------------------------------------------------" << endl;
}

//////////////////////////////////////////////////////////////////////////////
// Return the matrix size
//////////////////////////////////////////////////////////////////////////////
int Matrix::getSize()
{
	return size;
}

//////////////////////////////////////////////////////////////////////////////
// Get an element from the matrix
//////////////////////////////////////////////////////////////////////////////
double Matrix::getElement(int rowNum, int colNum)
{
	return element[rowNum-1][colNum-1];
}

//////////////////////////////////////////////////////////////////////////////
// Set an element in the matrix
//////////////////////////////////////////////////////////////////////////////
void Matrix::setElement(int rowNum, int colNum, double in_element)
{
	element[rowNum][colNum] = in_element;
}

//////////////////////////////////////////////////////////////////////////////
// Create copy of matrix
//////////////////////////////////////////////////////////////////////////////
Matrix Matrix::copy()
{
	return Matrix(size, element);
}

//////////////////////////////////////////////////////////////////////////////
// Multiply row by constant
//////////////////////////////////////////////////////////////////////////////
void Matrix::multiplyRowByConstant(int rowNum, double constant)
{
	if (rowNum < 1 || rowNum > size) {
		cout << "Invalid row #: " << rowNum << endl;
		return;
	}

	rowNum --;
	for (int cc=0; cc<size; cc++) {
		element[rowNum][cc] *= constant;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Swap two rows
//////////////////////////////////////////////////////////////////////////////
void Matrix::swapRows(int row1, int row2)
{
	if (row1 < 1 || row1 > size) {
		cout << "Invalid row #: " << row1 << endl;
		return;
	}

	if (row2 < 1 || row2 > size) {
		cout << "Invalid row #: " << row2 << endl;
		return;
	}

	row1 --;
	row2 --;
	double tmp;
	for (int cc=0; cc<size; cc++) {
		tmp = element[row1][cc];
		element[row1][cc] = element[row2][cc];
		element[row2][cc] = tmp;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Add row multiple to another row:
//	row1 = row1 + constant*row2
//////////////////////////////////////////////////////////////////////////////
void Matrix::addRowMultiple(int row1, int row2, double constant)
{
	if (row1 < 1 || row1 > size) {
		cout << "Invalid row #: " << row1 << endl;
		return;
	}

	if (row2 < 1 || row2 > size) {
		cout << "Invalid row #: " << row2 << endl;
		return;
	}

	row1 --;
	row2 --;
	for (int cc=0; cc<size; cc++) {
		element[row1][cc] += constant * element[row2][cc];
	}
}

//////////////////////////////////////////////////////////////////////////////
// Transform matrix to REF - Reduced Echelon Form
//////////////////////////////////////////////////////////////////////////////
void Matrix::reducedEchelonForm()
{
	for (int r1=0; r1<size-1; r1++) {
		for (int r2=r1+1; r2<size; r2++) {
			double factor = element[r2][r1] / element[r1][r1];
			addRowMultiple(r2+1, r1+1, -factor);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// LU decomposition
//////////////////////////////////////////////////////////////////////////////
void Matrix::luDecomposition()
{
	// We are not updating the original matrix, we're only printing
	// the L- and U-matrices.

	// // // // // // // / //
	// Create the L-matrix //
	// // // // // // // / //

	double** lVals = new double*[size];
	for (int rr=0; rr<size; rr++) {
		lVals[rr] = new double[size];
		for (int cc=rr; cc<size; cc++) {
			if (cc == rr)
				lVals[rr][cc] = 1;
			else
				lVals[rr][cc] = 0;
		}
	}

	Matrix ll(size, lVals);

	// // // // // // // / //
	// Create the U-matrix //
	// // // // // // // / //

	Matrix uu (size, element);
	for (int r1=0; r1<size-1; r1++) {
		for (int r2=r1+1; r2<size; r2++) {
			double factor = uu.element[r2][r1] / uu.element[r1][r1];
			ll.setElement(r2, r1, factor);
			uu.addRowMultiple(r2+1, r1+1, -factor);
		}
	}

	cout << "L-matrix" << endl << "--------" << endl;
	ll.print();

	cout << "U-matrix" << endl << "--------" << endl;
	uu.print();
}

//////////////////////////////////////////////////////////////////////////////
// Find the eigenvalues
// Note: Works on 2-dimensional arrays only.
//////////////////////////////////////////////////////////////////////////////
void Matrix::computeEigenValues()
{
	if (size != 2) {
		cout << "I don't know how to compute eigenvalues for matrices of size != 2" << endl;
		return;

	}

	if (eigenVals != NULL)
		delete eigenVals;
	eigenVals = new double[size];

	double aa = 1;
	double bb = -(element[0][0] + element[1][1]);
	double cc = (element[0][0]*element[1][1] - element[0][1]*element[1][0]);
	eigenVals[0] = (-bb + sqrt(bb*bb-4*aa*cc)) / (2*aa);
	eigenVals[1] = (-bb - sqrt(bb*bb-4*aa*cc)) / (2*aa);
		// Not dealing w/ complex numbers here
}

//////////////////////////////////////////////////////////////////////////////
// Find the eigenvector
// Note: Works on 2-dimensional arrays only.
//////////////////////////////////////////////////////////////////////////////
void Matrix::computeEigenVectors()
{
	if (size != 2) {
		cout << "I don't know how to compute eigenvectors for matrices of size != 2" << endl;
		return;
	}

	if (eigenVectors != NULL) {
		for (int rr=0; rr<size; rr++) {
			delete eigenVectors[rr];
		}
		delete eigenVectors;
	}
	eigenVectors = new double*[size];

	for (int rr=0; rr<size; rr++) {
		eigenVectors[rr] = new double[size];
		eigenVectors[rr][1] = element[0][1] * element[1][0] / ((element[0][0] - eigenVals[rr]) * (element[1][1] - eigenVals[rr]));
		eigenVectors[rr][0] = element[0][1] * eigenVectors[rr][1] / (eigenVals[rr] - element[0][0]);
	}
}

//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////
int main()
{
	// Test

	double vals1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	Matrix matrix1(3, vals1);
	matrix1.print();

	matrix1.multiplyRowByConstant(2, 3);
	matrix1.print();

	matrix1.swapRows(1, 3);
	matrix1.print();

	matrix1.addRowMultiple(1, 2, 4);
	matrix1.print();

	/////

	double vals2[] = { 25, 5, 1, 64, 8, 1, 144, 12, 1 };
	Matrix matrix2(3, vals2);
	matrix2.print();
	matrix2.luDecomposition();
	matrix2.print();

	/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /

	double vals3[] = { 1, -2, -2, -3, 3, -9, 0, -9, -1, 2, 4, 7, -3, -6, 26, 2 };
		// http://math.stackexchange.com/questions/735388/lu-factorisation-4x4-matrix-most-efficient-method
	Matrix matrix3(4, vals3);
	matrix3.print();
	matrix3.luDecomposition();
	matrix3.print();

	/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /

	double vals4[] = { 1, -2, 3, 2, -5, 12, 0, 2, -10 };
		// https://www.math.ohiou.edu/courses/math3600/lecture12.pdf
	Matrix matrix4(3, vals4);
	matrix4.print();
	matrix4.luDecomposition();
	matrix4.print();

	/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /

	double vals5[] = { 2, -4, -1, -1 };
		// https://www.math.hmc.edu/calculus/tutorials/eigenstuff/
	Matrix matrix5(2, vals5);
	matrix5.print();
	matrix5.computeEigenValues();
	matrix5.computeEigenVectors();
	matrix5.print();

	/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /

	double vals6[] = { 2, 4, -2, 1, -1, 5, 3, 3, 5 };
	Matrix matrix6(3, vals6);
	matrix6.print();
	matrix6.luDecomposition();


	/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /
	// Input a matrix

	cout << "Enter the size of a new square matrix: ";
	int size;
	cin >> size;
	double** vals = new double*[size];
	for (int rr=0; rr<size; rr++) {
		vals[rr] = new double[size];
		for (int cc=0; cc<size; cc++) {
			cout << "Enter the element[" << rr << "][" << cc << "]: ";
			cin >> vals[rr][cc];
		}
	}
	Matrix matrix9 = Matrix(size, vals);
	matrix9.print();

	cout << "Enter 1 for Eigenvalues/Eigenvectors or 2 for LU Decomposition: ";
	int input;
	cin >> input;
	if (input == 1) {
		matrix9.print();
		matrix9.computeEigenValues();
		matrix9.computeEigenVectors();
	}
	else
	if (input == 2) {
		matrix9.print();
		matrix9.luDecomposition();
	}

	matrix9.print();

	return 0;
}