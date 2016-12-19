### Welcome to MathMatrix library

This library was created to make it possible do matrix calculations in an easy way. There is many scientific problems that leads to any kind of matrix calculation, that requires boring and repetitive operations across rows and columns. This library intends make this process easier. Sum, subtraction, multiplication, deteminant, transpose, invert matrices are done in a very easy way.

NOTE: This library is not optimized to run with several parallel threads and/or processors simultaneously, YET. Multithreading support will still be added. Be warned.

## Installation

You can choose either of two options below:

* Embbed the code into your application. You can download my source code and import it through Qt Creator or whatever source code editor you are using.

* Compile the code as a library. Just clone my repository using `git clone https://github.com/viniavila/MathMatrix.git`. Enter into the cloned folder and just run these commands:

```
$ qmake MathMatrix.pro
$ make
$ sudo make install
```

The libmathmatrix.so.1.0.0 and its symbolic links, libmathmatrix.so.1.0, libmathmatrix.so.1 and libmathmatrix.so, will be copied to /usr/lib. You can pass PROFILE="new path" to qmake command to change this default path. To use the library, copy all the *.h files to /usr/include or /usr/local/include, or wherever path you want (since you provide this path in your project file, through INCLUDEPATH). To link your project to the .so library, uses the follow:

```
-L"/library_path" -lmathmatrix
```

## Usage

Use this library is pretty simple. Create an instance of a MathMatrix object, and if you want, set its size at the constructor.

```
MathMatrix m0;
MathMatrix m1(3,2); // rows = 3, columns = 2
```

You can set items individually, an entire row, an entire column or the entire matrix, since you provide lists which have enough size.

```
m1.setItem(2,0,4.5) // Set the item in row 2, column 0 to 4.5
m1(1,1) = 2.7; // Set the item in row 1, column 1 to 4.5
m1.setRow(0, {8,-4}); // Change the row 0 items to 8 and -4
m1.setcolumn(1, {3, 2, 5}); // change the column 1 items to 3, 2 and 5
m1.setMatrix({{1,2},{3,4},{5,6}}); // change the entire matrix to the lists of rows provided.
```

You can get the number of rows, columns or the value of an item at some position.

```
m1.rows(); // number of rows
m1.columns(); // number of columns
m1.at(0,1); // value of the item at row 0 and column 1
```

You can make copies of the object with minimal performance penalties, because this class has a _copy-on-write_ system implemented. Both operations below has constant processing time (O(1)).

```
MathMatrix m2(m1);
m0 = m1;
```

In the exaple above, m0, m1 and m2 will share the same data. But if the user modifies any of them, the internal data of this item will be cloned, to not to affect the other objects. More information [here](https://en.wikipedia.org/wiki/Copy-on-write). If you need a real clone of the original matrix, not sharing its internal data with anyone, you can use the `clone()` member.
```
m2.setItem(0,0,2.34); // the system clones the internal data to m2 and then set its item, making now m2 different from m0 and m1
MathMatrix m3 = m0.clone(); // m3 and m0 has the same data, but they are not sharing memory as m0 and m1 do now.
```

*MathMatrix* supports all matrix operations that we usially expect. You can do the follow operations:
* Addiction of two matrices (they must have the same size)
* Subtraction of two matrices (they must have the same size)
* Multiplication of matrix and scalar
* Division of matrix to a scalar (scalar should be different from zero) and vice versa (it cannot have any item equal to zero).
* Addiction assigment of one matrix to another (they must have the same size)
* Subtraction assigment of one matrix to another (they must have the same size)
* Multiplication assignment of one matrix to a scalar
* Division assignment of one matrix to a scalar (different from zero)
* Increment operator of a matrix (all itens incremented by 1)
* Decrement operator of a matrix (all items decremented by 1)
* Matrix multiplication (number of columns of the first must be equal to the number of rows of the second matrix)

Other special operations are also available
* `trace()`. Returns the sum of the main diagonal items (only for square matrices)
* `determinant()`. Returns the determinant of the matrix (only for square matrices)
* `subMatrix(row, column)`. Return a sub-matrix of the original matrix, deleting the `row` and the `column`
* `transposed()`. Returns a matrix that is the transposed matrix of the original matrix
* `transpose()`. In-place transpose operation, the original matrix is modified
* `inverse()`. Returns a matrix that is the inverse matrix of the original matrix (matrix must be square and determinant must be different from zero).
* `invert()`. In-place matrix inversion. The matrix is replaced by its inverse. This method consumes the same amount of memory of `inverted`, and have the same allocations quantity, but it is convenient if you do not need the original matrix and want to save some memory as the original matrix data is destroyed and replaced by the inverse matrix data before returning.

There are some `static` methods to create special matrices:
* `identity(n)`. Creates an identity matrix of size `n` (a square matrix _nxn_ that have all items equal to zero but the main diagonal which items are equal to 1)
* `diagonal(list}`. Creates a diagonal matrix with the size equal to the size of the `list` and the main diagonal values equal to the list values.

For the hack'ish people, a pointer to the vector that holds the internal data are exposed using the member `double* internal_pointer() const`. This pointer is necessary to implement non-member functions that needs direct access to the data (basically the overloaded operators * and /).

This code is not still optimized as I have said before. There is no implementation to multithreading, the there is some dependency to Qt, which I want to remove further, to make it available even to non-Qt users. I do not tested it much so use it at your own risk. Issues or bug reports are appreciated.

## Licensing

My code is licensed in LGPL v3 so you can use it either in open source or closed source applications since it concerns with the terms of this license. If you are using my code into your project and finds it useful, do not forget to mention my name in the credits, and if it is worthy, donation can be accepted.

Many thanks for using this code!

## Authors and Contributors
Vinícius de Ávila Jorge (@viniavila). You can contact me through my GitHub profile, or if you prefer, [email](mailto:vinicius.avila.jorge@gmail.com) me.
