Library : coral - Core Library for Applications and Libraries<br>
Author  : seadog.ahn@gmail.com<br>
Date    : 2021.02.10<br>
Update  : 2023.01.26<br>
~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~

본 문서는 라이브러리에 대한 설명서이며 활용에 필요한 내용이므로 사용 전 숙지하여야 한다. 설치 관련된 내용은 INSTALL 문서를 참조하기 바란다.

### coral과 함께 사용하는 필수 라이브러리 목록
curl : https://curl.se/ <br>
bz : https://bzip2.org <br>
boost : https://www.boost.org/ <br>
occi : https://www.oracle.com/kr/database/technologies/instant-client/downloads.html <br>
actian vector db iiapi : https://www.actian.com/analytic-database/vector-analytic-database/ <br>

### 파일 설명
|File Name|Descriptions|
|---|---|
|coral.h|including coral's headers|
|defines.h|defined MACRO for coral and derived namespaceses and classes|
|defines.cpp| |
|variables.h|coral global variables|
|types.h|type declaration and definition|
|exception.h|process error, exception definition|
|alarm controller.h|alarm signal control|
|config.h|system config value define|
|config.cpp| |
|utility.h|utility functions and classes|
|utility.cpp| |
|thread.h|UNIX POSIX thread|
|thread_lock.h|UNIX POSIX thread mutex management|
|thread_lock.cpp| |
|thread_pool.h|thread pool management using the modern C\+\+|
|file_object.h|file object manager|
|file_trans.h|file trans - using ftp library|
|file_trans.cpp| |
|log_manager.h|output log to the file stream|
|log_manager.cpp| |
|ora_dbm.h|oracle database management class|
|ora_dbm.cpp| |
|vec_dbm.h|actian vector database management class|
|vec_dbm.cpp| |
|net_interface.h|network(socket) program server & client interface class|
|net_client.h|network(socket) program client base class|
|net_client.cpp| |
|net_server.h|network(socket) program server base class|
|net_server.cpp| |
