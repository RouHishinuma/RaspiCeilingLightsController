# RaspiCeilingLightsController

機能説明
---
遠隔操作でシーリングライトの点灯/消灯を切り替える．

シーリングライトのリモコンが出力する信号と同等の信号をRaspberryi Piに接続した赤外線LEDから出力することで，制御を可能としている．

実行環境
---
* Raspberry Pi: 3 model B+
* OS : Ubuntu Server 20.04.1 LTS 64-bit
* シーリングライト : NEC HLDZ06013

赤外線LED OSI5LA5113Aのアノード側を100Ωの抵抗を介してRaspberry PiのGPIO25ピンに接続する．カソード側はGNDに接続する．

Raspberry Pi 4を使用する場合は`myled.c`の103行目を以下のように書き換える．（Pi 2，3ならば変更しない）
<!--GPIOの最初のアドレスを変更すること．-->

```c:myled.c
gpio_base timedatectl list-timezones= ioremap_nocache(0xfe200000, 0xA0); //original:0x3f200000
```
※本プログラムではNECのリモコンRE0208の動作をエミュレートしているため，それ以外のリモコンを採用している照明では動作が保証出来ない．

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

<!--天井のライトを赤外線LEDのパルス出力によるデータ送信で点灯/消灯させている．-->

動画
---
実行中の動画 : https://youtu.be/0CczAN3XH4g