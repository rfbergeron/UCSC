#include <iostream>

using namespace std;

void by_value(int x) {
   
}

//its not obvious that you're passing by reference at
//the call site since you can just hand it the value
//directly. however, no copying is done when passing
//by reference, which is good because copying can be
//very expensive for objects.
void by_reference(int & x) {
   //easier to use than by pointer. allows the
   //callee to change the value without having
   //to worry about dereferencing pointers. also
   //the value can't be null
}

//if passing by value is too expensive due to the
//cost of copying but you don't want the value to
//change, use const reference. Only issue is if this
//function makes another function call; the compiler
//might not know what the other function can change,
//thus the value pointed at by the const might be
//changed, and the compiler is not able to do as much
//optimization
void by_const_reference() {

}

void by_pointer(int * x) {
   //when this is called, the call will look like
   //by_pointer(&var). it will be very obvious that
   //the called method will be able to change the passed
   //value
   //
   //if the caller has a local var and they want to pass it
   //to this function, they must pass the address with &
}

void by_reference_to_pointer(int *& x) {
   //use when you want to change the pointer, meaning
   //what the pointer points to. For example, you could
   //pass a find function an iterator and have it change
   //what the iterator points to. the function would 
   //then return the index of the thing you're trying to
   //find as a memory address, which you can then dereference
   //to get the value you want.
}   
