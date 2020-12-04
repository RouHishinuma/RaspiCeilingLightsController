# RaspiCeilingLightsController

---
機能説明
---
遠隔操作でシーリングライトの点灯/消灯を切り替える．

<!--![](https://i.gyazo.com/828a352fb146bf9b585f50d509d5ee94.png)-->
![](https://i.gyazo.com/49bf35f878a05165b1de8de1678c1f95.gif)

シーリングライトのリモコンが出力する信号と同等の信号をRaspberryi Piに接続した赤外線LEDから出力することで，制御を可能としている．

本プログラムではNECのリモコンRE0208の動作をエミュレートしている．リモコンの点灯/消灯ボタンを押した時に出力されるデータは，Arduinoの赤外線通信ライブラリIRremoteのサンプルプログラムIRrecvDumpV2を使用して解析した．

![](https://i.gyazo.com/0a7ac0286be56f1578b8be70f65a3730.png)


解析結果から，RE0208の赤外線通信はNECフォーマットで，点灯/消灯のコードデータが0x4186D52Aであることがわかった．これより32bitのコードデータをNECフォーマットのパルス信号に変換し出力するプログラムを作成した．NECフォーマットのプロトコルに関しては，以下のサイトを参考にした．
* http://elm-chan.org/docs/ir_format.html

IRrecvDumpV2の使用方法についてはこちらのサイトを参考にした．
* https://asukiaaa.blogspot.com/2018/05/arduino.html


---
実行環境
---
* Raspberry Pi : 3 model B+
* OS : Ubuntu Server 20.04.1 LTS 64-bit
* シーリングライト : NEC HLDZ06013

以下のように，赤外線LED OSI5LA5113Aのアノード側を100Ωの抵抗を介してRaspberry PiのGPIO25ピンに接続する．カソード側はGNDに接続する．

![](https://i.gyazo.com/0fc99ba1afcfed800ac252b211966620.jpg)

もしRaspberry Pi 4を使用する場合は`myled.c`の103行目を以下のように書き換える．（Pi 2，3ならば変更しない）
<!--GPIOの最初のアドレスを変更すること．-->

```c:myled.c
gpio_base timedatectl list-timezones= ioremap_nocache(0xfe200000, 0xA0); //original:0x3f200000
```
また，RE0208で制御されていない機器であっても，同じNECフォーマットであればプログラムを一部書き換えるだけで照明機器に限らず制御可能である．自分で選んだ機器を制御したい場合は，そのリモコンの出力するコードデータを調べて，`myled.c`の9行目にある`TX_DATA`で定義されている32bitのコードデータを書き換える．



---
使用方法
---
1. リポジトリをクローンしリポジトリのディレクトリ内へ移動
```
$ git clone https://github.com/karata-sc/RaspiCeilingLightsController.git
$ cd RaspiCeilingLightsController
```

2. Makefileを実行しカーネルモジュールを作成


```
$ make
```
3. カーネルモジュールのインストール

```
$ sudo insmod myled.ko
```

4. メジャー番号の確認

```
$ tail -n 1 /var/log/kern.log
```
<!--Nov 30 14:07:21 ubuntu kernel: [19904.146763] /home/ubuntu/RaspiCeilingLightsController/myled.c is loaded. major:511-->
5. ファイル`/dev/myled0`の作成

    ※メジャー番号は自分の環境で確認したものを指定
```
$ sudo mknod /dev/myled0 c 511 0
```

6. 誰でも書き込み・読み込みができるようにパーミッションを変更

```
$ sudo chmod 666 /dev/myled0
```

7. `/dev/myled`へ書き込み
    
    赤外線LEDをシーリングライトに向ける．`/dev/myled`に1を書き込むたびにライトが交互に点灯/消灯する．

```
$ echo 1 > /dev/myled0
```



---
アンインストール
---
カーネルモジュールをアンインストールしたい場合は，以下を実行する．

`$ sudo rmmod myled`

ディレクトリ内のコードのファイルも削除したい場合は，以下を実行する．

`$ make clean`




<!--天井のライトを赤外線LEDのパルス出力によるデータ送信で点灯/消灯させている．-->


---
動画
---
* 照明を点灯/消灯している様子 

    https://youtu.be/0CczAN3XH4g

* `TOGGLE_DATA`のコードを書き換え，テレビのチャンネルを切り替えている様子

    https://youtu.be/sqouvlzmQlE
