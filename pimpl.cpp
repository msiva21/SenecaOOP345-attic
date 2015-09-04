// Pimpl - pointer to implementation
// Uses C++14 features:
// clang++ -Wall -std=c++14 pimpl.cpp -o pimpl -pthread
// or  g++ -Wall -std=c++14 pimpl.cpp -o pimpl -pthread

#include <iostream>
#include <memory> // std::unique_ptr

// ------------------------------------------------------------------
// http://herbsutter.com/elements-of-modern-c-style/
// C++11 Pimpl idiom: header file
class widget {
public:
    widget();
    ~widget(); // GB
    // ... (see GotW #100) ...
private:
    class impl;
    std::unique_ptr<impl> pimpl;
};
 
// implementation file
class widget::impl { /*...*/ };
 
widget::widget() : pimpl{ new impl{ /*...*/ } } { std::cout<<"widget\n"; }
widget::~widget() { std::cout<<"~widget\n"; } // GB
// ...
auto pws = std::make_shared<widget>();  // C++11
auto pwu = std::make_unique<widget>();  // C++14

#if 0
// ------------------------------------------------------------------
// GB syntax errors
// http://stackoverflow.com/questions/17554515/hiding-private-members-of-c-library

// PIMPL - Point to IMPLementaation

// Interface.hpp
#include <memory> // unique_ptr

class Implementation;
class Interface {
    std::unique_ptr<Implementation> pimpl;
    // Implementation* pimpl;  // GB see if raw pointer fixes syntax error, no, it didn't
public:
    Interface() : pimpl(new Implementation()) {}
    //  error: allocation of incomplete type 'Implementation'
    // ~Interface() { delete pimpl; }  // GB need destructor with raw pointer

    void publicMethod();
};

// Interface.cpp
class Implementation {
public:
    void PrivateMember();
};

void Interface::publicMethod() { pimpl->PrivateMember(); }
#endif

// ------------------------------------------------------------------
// http://www.gamedev.net/page/resources/_/technical/general-programming/the-c-pimpl-r1794
// MyClass.h

class MyClassImp;                    // forward declaration of Pimpl
class MyClass
{
public:
   MyClass ();
   ~MyClass();
   MyClass( const MyClass &rhs );   // undefined for simplicity
   MyClass& operator=( MyClass );
   void  Public_Method();
private:
   // MyClassImp *pimpl_;              // the Pimpl
   std::unique_ptr<MyClassImp> pimpl_; // GB prefer to use unique_ptr
};

// MyClass.cpp
// #include "MyClass.h"
class MyClassImp
{
public:
   MyClassImp();
   ~MyClassImp();
   void   Private_Method()  {}   // dummy private function
   int    private_var_;          // a private variable
};
 
MyClassImp::MyClassImp()  { std::cout<<"MyClassImp\n"; }
MyClassImp::~MyClassImp()  { std::cout<<"~MyClassImp\n"; }

MyClass::MyClass()  :  pimpl_( new MyClassImp() ) { std::cout<<"MyClass\n"; }
 
MyClass::~MyClass() { std::cout<<"~MyClass\n"; /* delete  pimpl_; */ } // GB not needed with unique_ptr
 
void   MyClass::Public_Method()
{
   std::cout<<"MyClass::Public_Method()\n";
   pimpl_->Private_Method();      // do some private work
   pimpl_->private_var_  = 3;
}

// ------------------------------------------------------------------
// http://www.cppsamples.com/common-tasks/pimpl.html
// GB where is class impl defined?
// foo.h - header file
#include <memory>
class foo
{
public:
  foo();
  ~foo();
  foo(foo&&);
  foo& operator=(foo&&);
private:
  class impl;
  std::unique_ptr<impl> pimpl;
};

// foo.cpp - implementation file
class foo::impl
{
  int internal_data = 0;
public:
  void do_internal_work() { std::cout<<"foo::do_internal_work()\n"; internal_data = 5; }
};

foo::foo()
  : pimpl{std::make_unique<impl>()} 
        // Note: std::make_unique was introduced in C++14. For C++11, you can roll your own implementation.
   { std::cout<<"foo\n"; pimpl->do_internal_work(); }

// foo::~foo() = default;
foo::~foo() { std::cout<<"~foo\n"; }

foo::foo(foo&&) = default;
foo& foo::operator=(foo&&) = default;

// ------------------------------------------------------------------
// C++14
// http://coliru.stacked-crooked.com/a/36ead4897a83c4fc
class bar
{
  class impl;
  struct impl_deleter
  {
      void operator()( impl * ) const;
  };
  std::unique_ptr<impl,impl_deleter> pimpl;
public:
  bar();
  ~bar(); // GB
};
// bar.cpp - implementation file
class bar::impl
{
  int internal_data = 0;
public:
  void do_internal_work()
  {
    std::cout<<"bar::do_internal_work()\n"; 
    internal_data = 5;
  }
};
void bar::impl_deleter::operator()( impl * ptr ) const
{
  delete ptr;
}

bar::bar()
  : pimpl{new impl}
{
  std::cout<<"bar\n";
  pimpl->do_internal_work();
}

bar::~bar() { std::cout<<"~bar\n"; } // GB
// ------------------------------------------------------------------
// http://anthony-arnold.com/2012/04/05/three-fun-cpp-techniques/
// 1. The Pimpl Idiom

// Frankly, if you don’t know about the Pimpl idiom, you’re not a real C++ programmer. Pimpl stands for “Pointer to implementation”, sometimes referred to as the Cheshire Cat. It’s a way to hide the implementation of a class from the user by defining all private members in the class definition (.cpp) file.

// Here is an example:
//car.hpp
class car {
public:
    car();                          //constructor
    ~car();                         //destructor // GB
    car(const car &c);              //copy constructor
    car &operator=(const car &rhs); //copy assignment operator
    car(const car &&c);             //move constructor      
    car &operator=(const car &&rhs);//move assignment operator
    /* Other public methods */
 
private:
    class under_the_hood; //Not defined here
    std::shared_ptr<under_the_hood> pImpl; //POINTER TO IMPLEMENTATION
};
//car.cpp
 
class car::under_the_hood {
public:
    /* Implement car stuff here */
};
 
car::car() : pImpl(std::make_shared<under_the_hood>()) { std::cout<<"car\n"; }
car::~car() { std::cout<<"~car\n"; } // GB
 
car::car(const car &c) : pImpl(std::make_shared<under_the_hood>(*c.pImpl)) {}
 
car &car::operator=(const car &rhs) {
    pImpl = std::make_shared<under_the_hood>(*rhs.pImpl);
    return *this;
}
 
car::car(const car &&c) : pImpl(c.pImpl) { }
 
car &car::operator=(const car &&rhs) {
    pImpl = rhs.pImpl;
    return *this;
}

// This idiom has quite a few advantages, especially when authoring libraries. Firstly, you can maintain secrecy regarding your implementation; the client gets the public interface to your class and can use it freely without being able to see the gritty details of how it works “under the hood”.

// Secondly, the client gets a compile-time performance increase by not having to parse the private declarations of your class; it is all safely hidden in the implementation file.

// Another advantage of the Pimpl idiom is that it allows you to use third-party, header-only libraries in your implementation without requiring that the client has access to these same libraries. A great example is the use of Boost. If you want to use any of the Boost header-only libraries, like Boost.Signals2 or Boost.Geometry, then great! You should use Boost where it’s appropriate. But what if you don’t want your users to require Boost in order to use your library? Easy; use the Pimpl idiom and hide all of your private Signals or Polygons or what have you in the implementation file. Your user need never know. Well, actually they do need to know if you’re using open source libraries, but you get my point.
// ------------------------------------------------------------------

int main()
{
  MyClass mc;
  mc.Public_Method();

  foo fighter;
  // fighter.do_internal_work();

  bar stool;

  car wreck;
}