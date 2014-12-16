## 1학년 3학기 채팅 client, server project
- make
 make이후 2개의 실행 파일이 생성됩니다
 server
 client

- 사용법

 서버 : ./server {port} {chatName}

 클라이언트 : ./client {IP} {port} {chatName}

- bug report

현재 최초로 접속되는 client인 clientSocketForServer가 5번이 아니라
6번 fd를 가지게 되면서 발생하는 에러 상황

아래 상황들의 문제는
clientSocketForServer가 정상적으로 5번 fd를 차지하게 된다면 
이 상황들을 막을 수 있을텐데
현재 방법을 모르겠다

(현재는 case 3,4가 동작되도록 구현해 둠. 치명적인 에러가 나지 않기 때문)

### case 1,2 
	(myChatting_server.h 상수값 5 && myChatting_server.의 230Line의 부등호 <= 로 둔 경우)

- case 1, 아래의 경우에 정상동작
 ./server ... 실행 이후 '곧장' 'q\n'입력
 clientSocketForServer(서버에서 사용하는 client형 socket)를 닫은 뒤
 다른 클라이언트들의 접속(connected : 5, connected : 6 ... 의 형태로 사용시)

- case 2, 아래의 경우 에러발생(이상한 출력결과 발생)
 ./server ...로 서버 실행 이후
 clientSocketForServer를 닫지 않고
 다른 클라이언트들의 접속(connected : ... 의 형태로 사용시)을 받아 채팅시

### case 3,4
	(myChatting_server.h 상수값 6 && myChatting_server의 230Line의 부등호 < 로 둔 경우)

- case 3, 아래의 경우 5번 클라이언트를 제외한 나머지 클라이언트 정상동작
 ./server ... 실행 이후
 clientSocketForServer를 닫은 뒤에 접속된 클라이언트가 fd를 5번을 가지면서
 해당 clientSocket(fd : 5)에 메시지 전달이 안되는 경우

- case 4, 아래의 경우 정상동작
 ./server ... 실행 이후
 clientSocketForServer를 절대 닫지 않을 경우
