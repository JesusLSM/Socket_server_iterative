#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <WinSock2.h>
#include <iostream>

using namespace std;
void Err_handling(char *message) {
  fputs(message, stderr); // 메시지 출력
  fputc('\n', stderr);
  exit(1); //종료
}
void main() {
  WSADATA wsaData;
  SOCKET ServSock, ClntSock;
  SOCKADDR_IN servAddr, clntAddr;
  bool opt = TRUE;
  int optlen = sizeof(opt);

  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // WSAStartup함수를 호출하여 소켓버전을 알리고 라이브러리 초기화 작업(성공시 0 실패시 0아닌 에러코드)
    Err_handling("WSAStartup ERR");
  }

  ServSock = socket(PF_INET, SOCK_STREAM, 0); // 서버 소켓생성 ipv4프로토콜, 연결지향형, ??(성공시 소켓핸들, 실패시 INVALID_SOCKET 반환)
  if (ServSock == INVALID_SOCKET) {
    Err_handling("INVALID_SOCKET");
  }

  setsockopt(ServSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, optlen); // 서버가 먼저 FIN하였을 때 TIME-WAIT 상태가된다.
  //디폴트값은 0이며 time-wait상황에서의 소켓할당이 불가능하다. 이를 setsockopt로 true로 바꾸어주면 재할당이 가능하다.

  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET; // ipv4
  servAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);  // long형을 host to network long형으로 바꿔라. INADDR_ANY  = 소켓이동작하는주소  little endian > big endian
  servAddr.sin_port = htons(9000); // host to network  short형임 입력받을땐 atoi로 인티저 형번환
  
  if (SOCKET_ERROR == bind(ServSock, (SOCKADDR*)&servAddr, sizeof(servAddr))) { // 소켓에 ip와 포트번호 할당
    Err_handling("Bind ERR");
  }

  if (SOCKET_ERROR == listen(ServSock, 5)) { // 소켓이 클라이언트 연결을 받아들이기 위한 상태 설정 backlog(최대 연결요청 대기큐의 수)
    Err_handling("Listen ERR");
  }
  int clntAddr_size = sizeof(clntAddr); // accept(Socket, sockaddr*, int *addrlen) addrlen이 포인터형식이므로 bind와는 다르게 변수하나 만들어줘야함
  char message[BUFSIZ];
  for (int i = 0; i < 5; i++) {
    ClntSock = accept(ServSock, (SOCKADDR*)&clntAddr, &clntAddr_size);
    if (ClntSock == -1) {
      Err_handling("Client sock ERR");
    }
    printf("Connected client %d\n", i + 1);

    while (recv(ClntSock, message, sizeof(message), 0) != 0) {
      send(ClntSock, message, sizeof(message), 0);
    }
    closesocket(ClntSock);
  }

  closesocket(ServSock);
  WSACleanup();


  // 전체적 흐름
  // wsastartup으로 소켓버전, 라이브러리 초기화
  // 서버소켓생성
  // 서버소켓 family, 주소, 포트 설정
  // 주소를 소켓에 bind
  // 서버소켓 listen
  // 클라이언트소켓생성 (accept)
  // send, recv 등 작업
  // closesocket, wsacleanup으로 마무리
}