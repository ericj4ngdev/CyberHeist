# Cyber Heist
- UNSEEN 2기 프로그램 결과물
- GTA와 배틀그라운드의 특징을 합친 멀티 TPS, FPS(데디케이티드)
- [다운로드 링크](http://naver.me/F0zXHa69)

# 플레이 영상(사진 클릭시 유튜브 이동)

[![image](https://github.com/ericj4ngdev/CyberHeist/assets/108036322/4761d717-a512-4a0d-93af-5578c3831cb7)](https://www.youtube.com/watch?v=ol7ylDDERsw)

# 게임 소개

장르 : FPS, TPS

플랫폼 : PC (Windows)

제작 기간 : 4개월(2024-03-11 ~ 2024-06-25)

사용 도구 : Unreal Engine 5.3.2(C++), Rider 2023

개발 인원 : 1명

게임 소개 : 2~4인이서 협동으로 적들을 물리치고 탈출하는 게임

# 주요 개발 내용

- AI 엄폐시스템
  - NavMesh를 활용한 엄폐 지점 생성 시스템
  - BT 구조
  - CrowdFollowingComponent
- 플레이어 엄폐시스템
- 총기
  - Gun클래스 다이어그램
  - Line Trace, Projectile
  - 벽 앞에서 총구 내리기
  - 총기 기울이기
- 네트워크(데디케이티드 서버)
  - Hit 판정 동기화
  - 1인칭, 3인칭 동기화
  - GameMode MatchState
  - PlayerState 정보 띄우기


# 게임 플레이

## 조작법

|조작키|내용|
|:---:|:---:|
|WASD|이동|
|V|시점 변환|
|P|일시 정지|
|SpaceBar|점프|
|Ctrl|앉기|
|Shift|달리기|
|E|(1인칭) 우로 기울이기|
|Q|(3인칭) 엄폐 <br> (1인칭) 좌로 기울이기|
|마우스 좌클릭|발사|
|마우스 우클릭|조준|
|마우스 휠 위아래|(평상시) 무기 전환 <br> (조준시)확대 조준|





