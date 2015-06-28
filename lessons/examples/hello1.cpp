/* hello1.cpp

   Prints "Hello, World!" to the screen and exits
*/

#include <iostream> // For std::cout
#include <string>   // For std::string
#include <cstdlib>  // For EXIT_SUCCESS

int main() {
  using namespace std::literals; // So we can use ""s syntax
  
  std::string message{"Hello, World!"s};
  std::cout << message << "\n"s;

  return EXIT_SUCCESS;
}
