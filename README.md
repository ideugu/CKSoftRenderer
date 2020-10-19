# SoftRenderer

교육적 목적으로 고안된 프로젝트입니다. 
브랜치를 사용해 단계별로 코드가 구현되어 있으니 수업에 참고하시기 바랍니다. 

## 요구사항
- 비주얼 스튜디오 2019 커뮤니티 버젼 이상
- CMake 3.1버젼 이상 ( https://cmake.org/download/ )
본 프로젝트는 멀티플랫폼으로 설계되어 있으나, 현재는 윈도우만 지원합니다. 

## 컴파일 방법
- 소스를 다운 받은 후에 CMake-VS-16-2019.bat 배치 파일을 실행합니다. 
- 배치 파일을 실행하면 소스를 푼 폴더에 Project폴더와 솔루션 파일이 생성됩니다. 
- 솔루션을 더블클릭해 비주얼 스튜디오를 열고 컴파일하고 실행합니다. 

## 사용하는 키 
| 키            | 기능           |
| ------------- |:-------------:|
| F1  | 일반 모드 |
| F2  | 와이어 프레임 모드 |
| F3  | 깊이 버퍼 모드 ( 3D 전용 ) |
| F10 | 2D 엔진과 3D 엔진의 변경 |
| 왼쪽,오른쪽 화살표 키 | 캐릭터 좌우 회전 |
| 위,아래 화살표 키 | 캐릭터 전후 이동 |
| 페이지 업,다운 | 카메라 FOV 조절 |

## 일반 모드
텍스쳐를 매핑해 렌더링합니다. 
기즈모는 표시되나 본은 표시되지 않습니다. 

![일반 모드](https://github.com/ideugu/CKSoftRenderer/blob/main/Document/Normal.png "일반 모드")


## 와이어 프레임 모드
선을 사용해 외곽선만 렌더링합니다. 
기즈모와 본을 모두 표시해줍니다. 

![와이어프레임 모드](https://github.com/ideugu/CKSoftRenderer/blob/main/Document/Wireframe1.png "와이어프레임 모드")

가까이서 확대하는 경우 동차좌표계에서 삼각형이 분할되는 과정을 확인할 수 있습니다. 

![와이어프레임 모드 2](https://github.com/ideugu/CKSoftRenderer/blob/main/Document/Wireframe2.png "와이어프레임 모드 2")

## 뎁스 버퍼 모드
투영 변환 후 뎁스 값을 시각화하여 보여줍니다.
뎁스 테스팅은 비선형 값으로 진행하지만, 이를 선형화시켜 보여줍니다. 

![뎁스 버퍼 모드](https://github.com/ideugu/CKSoftRenderer/blob/main/Document/Depth.png "뎁스버퍼 모드")

