# hm10 블루투스 모듈의 rssi(측정 신호 세기)에 머신러닝을 활용한 실내 위치 측위

1. 데이터 수집
![image](https://user-images.githubusercontent.com/70446214/98037958-b7d75100-1e5f-11eb-9e1d-57e661704def.png)
feature 데이터의 RSSI1,2,3,4 는 맵 의 가운데 보드에서 각 끝의 블루투스 세기(RSSI)를 스캔하여 순서대로 왼쪽 하단의 블루투스로부터 측정된 rssi값을 시계방향으로 지정한 것이다.
(측정된 rssi 값은 근접할 수록 작은 값이 측정된다.)
*RSSI는 측정 신호 세기의 값으로, 근접할 수록 큰 값을 갖는다. HM-10모듈의 RSSI는 기본적으로 음수 값을 갖지만, 수집한 데이터는 이에 절댓값을 씌었기 때문에 근접할 수록 더 작은 값을 나타낼 것임에 유의하시오.
![image](https://user-images.githubusercontent.com/70446214/98038193-0e448f80-1e60-11eb-96e5-b47a9e1930ee.png)
맵의 크기는 1.5m * 1m 의 크기이다.

위 보드가(movingBluetooth_hm10_MEGA) 주위 블루투스의 rssi값을 측정하면 엔코딩하여 UART 통신을 통해 노트북과 연결된 보드(basicBoard_uno)에게 값을 전달해준다.

![image](https://user-images.githubusercontent.com/70446214/98039018-6203a880-1e61-11eb-9b08-cbf9556dbb0a.png)
이 보드(basicBoard_uno)와 연결된 컴퓨터는 파이썬(3.X) 프로세스를(collecting.py) 통해 보드로부터 지속적으로 데이터를 CSV파일에 적재하여 PORT를 뽑으면 프로세스가 종료되면서 CSV파일을 저장한다.
