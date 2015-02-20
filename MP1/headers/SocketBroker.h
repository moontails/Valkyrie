//SocketBroker.h - helps to manage all the socket connections!

#ifndef _SocketBroker
#define _SocketBroker

//code
class SocketBroker
{
  private:
    // Functions to support different types of client connection
    static void* SocketBrokerTCP(void *lp);
    //static void* SocketBrokerUDP(void *lp);
    
  public:
    int add(int x, int y);
};


#endif
