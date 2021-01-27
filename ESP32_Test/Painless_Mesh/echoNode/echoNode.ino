//************************************************************
// this is a simple example that uses the painlessMesh library and echos any
// messages it receives
//
//************************************************************
#include "painlessMesh.h"

#define   MESH_PREFIX     "MAI_2.4GHz"
#define   MESH_PASSWORD   "0227200846"
#define   MESH_PORT       5555

// Prototypes
void receivedCallback( uint32_t from, String &msg );

painlessMesh  mesh;

void setup() {
  Serial.begin(115200);
    
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(&receivedCallback);
}

void loop() {
  mesh.update();
}

void receivedCallback( uint32_t from, String &msg ) 
{
  static int received_package_counter = 0 ;
  char temp_str[64] ;
  sprintf(temp_str,"#%4d : ACK to node - %u ", received_package_counter++,from) ;
  String response_msg = temp_str ;
  
  Serial.printf("echoNode: Received from %u msg=%s\n", from, msg.c_str());

  mesh.sendSingle(from, response_msg/*msg*/);
}
