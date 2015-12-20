# RobotUtilityforRaspberryPi
RaspberryPiのためのロボット用ライブラリ
- ScrpMasterは[RasPiMS](https://github.com/Owl8/RasPiMS), Ds3Readは[RasPiDS3](https://github.com/Owl8/RasPiDS3)から移行しました
- makeでビルド sudo make installでインストールできます
- wiringPiを使用しているのでインストールされている必要があります
- c++11以上に対応したコンパイラでビルドしてください
- gccでインストールされたライブラリを使用したプログラムをビルドする際は, "-lRobotUtil -lwiringPi -std=c++11 -pthread"をつけてください
