/******************************************************************************
filename    driver.cpp
author      Dimitri Volper
date        11/13/2022

Brief Description: Provided by my professor, this file conatins tests for 
  most every kind of use case of the abstract element array, and associated
  factory/ reference counting functionality.
******************************************************************************/
#include "array.h"
#include "element-factory.h"
#include "element1.h"
#include "element2.h"

void print( DigiPen::Array const& a ) 
{
  a.Print();
#if 1 
  // when working on the first step (that uses deep copy, before reference
  // counting) turn them off, since counts will be wrong
	std::cout << "Element1: live  nodes  " << DigiPen::Element1::GetAlive() << std::endl; 
	std::cout << "Element1: total nodes  " << DigiPen::Element1::GetTotal() << std::endl; 
	std::cout << "Element2: live  nodes  " << DigiPen::Element2::GetAlive() << std::endl; 
	std::cout << "Element2: total nodes  " << DigiPen::Element2::GetTotal() << std::endl; 
#endif
}

void test0(  )
{                               
  // basic test
  // client creates a factory to create elements
  // this way Array doesn't need to know about Element1 and Element2
  // or any future Element-like types
  // this is yet another "programming to interfaces" example
  // or "old code calls new code"
  DigiPen::ElementFactory el_fact;

  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "modify myArray1\n";
  myArray1.Set( 2, 3, 33 );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
}

void test1(  )
{                               // copy
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "make a copy of first array\n";
  DigiPen::Array myArray2( myArray1 );  // copy construct
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
}

void test2(  )
{                               // copy 
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "modify myArray1\n";
  myArray1.Set( 2, 3, 33 );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "Create copy myArray2(myArray1);\n";
  DigiPen::Array myArray2( myArray1 );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
}

void test3(  )
{                               // assignment
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  int array2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  DigiPen::Array myArray2( array2, sizeof ( array2 ) / sizeof ( array2[0] ), &el_fact );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
  std::cout << "Assign myArray2 = myArray1;\n";
  myArray2 = myArray1;
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
}

void test4(  )
{                               // assignment 
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "Set myArray1.Set(2,3,33);\n";
  myArray1.Set( 2, 3, 33 );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  int array2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  DigiPen::Array myArray2( array2, sizeof ( array2 ) / sizeof ( array2[0] ), &el_fact );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
  std::cout << "Assign myArray2 = myArray1;\n";
  myArray2 = myArray1;
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
}

void test5(  )
{                               // copy using Element, then modify the copy
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "Create copy myArray2(myArray1);\n";
  DigiPen::Array myArray2( myArray1 );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
  std::cout << "assign myArray2 = myArray1;\n";
  myArray2 = myArray1;
  std::cout << "Set myArray2.Set(2,3,33);\n";
  myArray2.Set( 2, 3, 33 );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );

}

void test6(  )
{                               // copy using Element, then modify the original
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  DigiPen::Array myArray2( myArray1 );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
  std::cout << "Create copy myArray2(myArray1);\n";
  myArray2 = myArray1;
  std::cout << "Modify myArray1\n";
  myArray1.Set( 2, 3, 33 );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
}

void test7(  )
{                               // assignment using Element2 then modify the copy
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  int array2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  DigiPen::Array myArray2( array2, sizeof ( array2 ) / sizeof ( array2[0] ), &el_fact );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
  std::cout << "Assign myArray2 = myArray1;\n";
  myArray2 = myArray1;
  std::cout << "Modify myArray2\n";
  myArray2.Set( 2, 3, 33 );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
}

void test8(  )
{                               // assignment using Element2 then modify the original
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  int array2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  DigiPen::Array myArray2( array2, sizeof ( array2 ) / sizeof ( array2[0] ), &el_fact );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
  std::cout << "Assign myArray2 = myArray1;\n";
  myArray2 = myArray1;
  std::cout << "Modify myArray1\n";
  myArray1.Set( 2, 3, 33 );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
}

// function accepts by value
void foo( DigiPen::Array a )
{
  std::cout << "in foo a = ";
  print( a   );
  std::cout << std::endl;
}

void test9(  )
{                               // calling function that accepts by value
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  foo( myArray1 );
}

void test10(  )
{                               // copy then delete the original
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array * p_myArray1 = new DigiPen::Array( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( *p_myArray1 );   ";
  print( *p_myArray1 );

  DigiPen::Array myArray2 = *p_myArray1;
  std::cout << "print( *p_myArray1 );   ";
  print( *p_myArray1 );
  std::cout << "print( myArray2 );   ";
  print( myArray2 );
  std::cout << "delete p_myArray1;\n";
  delete p_myArray1;

  std::cout << "print( myArray2 );   ";
  print( myArray2 );
}

void test11(  )
{                               // assign then delete the copy
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );

  {                             // block
    int array2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    DigiPen::Array myArray2( array2, sizeof ( array2 ) / sizeof ( array2[0] ), &el_fact );
    std::cout << "print( myArray2 );   ";
    print( myArray2   );

    myArray2 = myArray1;
    std::cout << "print( myArray1 );   ";
    print( myArray1   );
    std::cout << "print( myArray2 );   ";
    print( myArray2   );
  }                             // myArray2 is deleted

  std::cout << "print( myArray1 );   ";
  print( myArray1   );
}

void test12(  )
{                               // chaining
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  DigiPen::Array * pArray1 = new DigiPen::Array( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  std::cout << "print( *pArray1 );   ";
  print( *pArray1 );

  int a[] = { 1 };
  DigiPen::Array * pArray2 = new DigiPen::Array( a, sizeof ( a ) / sizeof ( a[0] ), &el_fact );
  *pArray2 = *pArray1;
  delete pArray1;

  DigiPen::Array * pArray3 = new DigiPen::Array( a, sizeof ( a ) / sizeof ( a[0] ), &el_fact );
  *pArray3 = *pArray2;
  delete pArray2;

  std::cout << "print( *pArray3 );   ";
  print( *pArray3 );
  delete pArray3;
}

void test13(  )
{
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );

  int array2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
  DigiPen::Array myArray2( array2, sizeof ( array2 ) / sizeof ( array2[0] ), &el_fact );

  int array3[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  DigiPen::Array myArray3( array3, sizeof ( array3 ) / sizeof ( array3[0] ), &el_fact );

  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
  std::cout << "print( myArray3 );   ";
  print( myArray3   );

  std::cout << "Assign myArray3=myArray1;\n";
  myArray3 = myArray1;

  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
  std::cout << "print( myArray3 );   ";
  print( myArray3   );

  std::cout << "Assign myArray3=myArray2;\n";
  myArray3 = myArray2;

  std::cout << "print( myArray1 );   ";
  print( myArray1   );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
  std::cout << "print( myArray3 );   ";
  print( myArray3   );

}

// function returns by value
DigiPen::Array give_my_array( int *p, unsigned int size )
{
  DigiPen::ElementFactory el_fact;
  DigiPen::Array result( p, size, &el_fact );
  for ( unsigned int i = 0; i < size; ++i ) {
    result.Set( 2, i, 2 * result.Get( i ) );  // double them all and change type
  }
  std::cout << "int give_my_array(...) print( result );   ";
  print( result   );
  return result;
}

void test14(  )
{
  int array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  DigiPen::Array myArray1 = give_my_array( array, sizeof ( array ) / sizeof ( array[0] ) );
  std::cout << "in test result from give_my_array     ";
  print( myArray1   );

}

// ///////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////
// //test virtual constructor
// /////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////
void test15(  )
{                               // copy using Element, then modify the copy
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  myArray1.Set( 2, 3, 44 );
  myArray1.Set( 2, 4, 55 );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );

  std::cout << "Create copy myArray2(myArray1);\n";
  DigiPen::Array myArray2( myArray1 );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );

  std::cout << "Modify myArray2\n";
  myArray2.Set( 2, 1, 22 );

  std::cout << "print( myArray1 );   ";
  print( myArray1   );

  std::cout << "print( myArray2 );   ";
  print( myArray2   );

}

void test16(  )
{                               // copy using Element, then modify the original
  DigiPen::ElementFactory el_fact;
  int array[] = { 1, 2, 3, 4, 5, 6 };
  int array2[] = { 1, 2, 3 };
  DigiPen::Array myArray1( array, sizeof ( array ) / sizeof ( array[0] ), &el_fact );
  myArray1.Set( 2, 3, 44 );
  myArray1.Set( 2, 4, 55 );
  std::cout << "print( myArray1 );   ";
  print( myArray1   );

  DigiPen::Array myArray2( array2, sizeof ( array2 ) / sizeof ( array2[0] ), &el_fact );
  std::cout << "print( myArray2 );   ";
  print( myArray2   );
  std::cout << "Assign myArray2 = myArray1;\n";
  myArray2 = myArray1;
  std::cout << "print( myArray2 );   ";
  print( myArray2   );

  std::cout << "Modify myArray2\n";
  myArray2.Set( 2, 1, 22 );

  std::cout << "print( myArray1 );   ";
  print( myArray1   );

  std::cout << "print( myArray2 );   ";
  print( myArray2   );

}

void test17(  );

void ( *pTests[] )(  ) = { test0, test1, test2, test3, test4, test5, test6, test7,
  test8, test9, test10, test11, test12, test13, test14, test15, test16, test17
};

void test17(  )
{
  for ( size_t i = 0; i < sizeof ( pTests ) / sizeof ( pTests[0] ) - 1; ++i )
    pTests[i] (  );
}

#include <cstdio>               // std::sscanf
int main( int argc, char **argv )
{
  if ( argc > 1 ) {
    int test = 0;

    std::sscanf( argv[1], "%i", &test );
    pTests[test] (  );
  }
  return 0;
}
