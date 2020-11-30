# RaspiCeilingLightsController

機能説明
---
シーリングライトのリモコンの信号をRaspberryi Piでエミュレートした．接続した赤外線LEDのパルス出力によるデータ送信でシーリングライトの点灯/消灯の切り替えが可能．

実行環境
---
* Raspberry Pi: 3 model B+
* OS : Ubuntu Server 20.04.1 LTS 64-bit
* 赤外線LED : OSI5LA5113A
* シーリングライト : NEC HLDZ06013

赤外線LEDは100Ωの抵抗を介してアノード側をRaspberry Piの25ピンに接続する．カソード側はGNDに接続する．

同じNEC RE0208リモコンを採用している照明であれば操作できるが，その他の製品での動作は保証出来ない．

またRaspberry Piのモデルが2または3ならばプログラムをそのまま使用できるが，モデル4を使用する場合は`myled.c`のxx行目を以下のように書き換え，GPIOの最初のアドレスを変更する必要がある．

```c:myled.c
gpio_base timedatectl list-timezones= ioremap_nocache(0x3f200000, 0xA0); //original:0x3f200000
```

使用方法
---
1. リポジトリをクローンしリポジトリのディレクトリ内へ移動
```
$ git clone https://github.com/karata-sc/RaspiCeilingLightsController.git
$ cd RaspiTvController
```

2. Makefileを実行しカーネルモジュールを作成


```
$ make
```
3. カーネルモジュールをインストール

```
$ sudo insmod myled.ko
```

4. メジャー番号を確認

```
$ tail -n 1 /var/log/kern.log
```
<!--Nov 30 14:07:21 ubuntu kernel: [19904.146763] /home/ubuntu/RaspiCeilingLightsController/myled.c is loaded. major:511-->
5. ファイル`/dev/myled0`を作成

    ※メジャー番号は自分の環境で確認したものを指定
```
$ sudo mknod /dev/myled0 c 511 0
```

6. 誰でも書き込み・読み込みができるようにパーミッションを変更

```
$ sudo chmod 666 /dev/myled0
```

7. 赤外線LEDをシーリングライトに向ける．`/dev/myled`に1を書き込むたびにライトが点灯/消灯する．


```
$ echo 1 > /dev/myled0
```
