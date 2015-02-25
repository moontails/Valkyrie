#include <iostream>
#include <string.h>
#include "headers/SocketBroker.h"

using namespace std;

int main()
{
  Test example;
  std::cout << example.add(5,6)  << std::endl;
  example.get();
  example.set(5);
  std::cout<<std::endl;
  example.get();
  std::cout<<std::endl;
  return 0;
}
