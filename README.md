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


2.데이터 시각화(DataVisualization.ipynb)

수집한 rssi 데이터를 시각화 함으로써 데이터가 일관성 있게 수집되었는지 확인해본다.
![image](https://user-images.githubusercontent.com/70446214/98039906-cbd08200-1e62-11eb-962b-74f272de7189.png)

데이터를 수집하는 과정에서 결측값이 발생하지 않도록 유지해주었으므로 결측값을 따로 처리하지 않아도 된다.

![image](https://user-images.githubusercontent.com/70446214/98039349-e5bd9500-1e61-11eb-9ced-5f4cdbb0f3fc.png)

이는 특정 블루투스로부터 멀어질 수록 측정된 RSSI 세기를 확인해보았다. (위에 설명했듯이 RSSI 값에 역수를 취했기 때문에 멀어질 수록 RSSI가 증가하는 추세를 보임에 유의)

3.LinearRegression을 활용한 실내 위치 측위
데이터 전처리는 다양한 방법으로 시도해 보았으며, 이상값의 경우에는 
실제로 센싱된 데이터가 이상치를 포함하고 있을 수 있으므로 전체 데이터셋에 대한 이상치를 처리하는 것이 아닌 Train Data의 이상치를 처리하여 학습모델을 구축하고
이상치를 포함한 TestData를 통해 성능을 확인해 보았다.
![image](https://user-images.githubusercontent.com/70446214/98040025-fae6f380-1e62-11eb-99ac-34e2461ea62c.png)
![image](https://user-images.githubusercontent.com/70446214/98040057-06d2b580-1e63-11eb-9430-393343ee279c.png)
![image](https://user-images.githubusercontent.com/70446214/98040138-1eaa3980-1e63-11eb-996a-18a28965cfe9.png)

