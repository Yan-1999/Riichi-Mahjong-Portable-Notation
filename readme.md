# Riichi Mahjong Portable Notation (RMPN) format

RMPN是一种使用类似于PGN(portable game notation)记录方式的立直麻将便携式牌谱记录文件格式。使用纯文本格式书写。使用习惯上与PGN尽量保持类似。

本格式的**约定限制**有：每一种相同牌面值的牌**只有四张**。立直交出的供托**只有一个且为1000点**。使用**没有花牌**的136张麻雀牌。

本格式可以自由实现的地方有：赤牌的个数（小于等于4）以及以何为赤牌。役的采用。点数计算的方式。

*斜体字部分表示暂时还没有纳入规范的实验性部分。请不要按照那些规则记录牌谱。*

## 标签域

标签以`[]`环绕，格式为：`[项目名 "项目值"]`。

必须填写的项目名以及可能的项目值列举如下：

| 项目名  | 项目内容 | 项目值                                                       |
| ------- | -------- | ------------------------------------------------------------ |
| Event   | 赛事名称 | 任意字符串                                                   |
| Site    | 赛事地点 | 任意字符串                                                   |
| Date    | 赛事日期 | `YYYY.MM.DD`格式的日期，不清楚的使用`??`填充。               |
| Round   | 当前局数 | `Wind number`格式表示的局数。`Wind`部分为`East`, `South`, `West`, `North`之一的值。`number`为一个数字，如：`1`。例如南三局表示为`South 3` |
| Honba   | 本场数   | 一个数字`number`，表示`number`本场。如`2`                    |
| Kyotaku | 供托数   | 一个数字`number`，表示`number`个供托。（注意不是供托的分数）：如`1` |
| Result  | 终局结果 | `Agari`（和了）、`RuuKyoku`（流局）                          |
| Style   | 记录方式 | `Horizontal`（横式）、`Vertical`（纵式）、具体见下文。       |

| 项目名 | 项目内容                   | 项目值     |
| ------ | -------------------------- | ---------- |
| East   | 当前局的东家               | 任意字符串 |
| South  | 当前局的南家               | 任意字符串 |
| West   | 当前局的西家               | 任意字符串 |
| North  | 当前局的北家（三麻则不写） | 任意字符串 |

可选填写的项目名以及可能的项目值列举如下：

| 项目名      | 项目内容     | 项目值                                  |
| ----------- | ------------ | --------------------------------------- |
| Annotator   | 记谱人       | 任意字符串                              |
| Termination | 比赛中断情况 | 任意字符串                              |
| Mode        | 比赛模式     | `OTD`（桌上对战），`IMS`（线上对战）    |
| TimeControl | 时间控制     | `1/time0:time1`，如：`1/5:10`表示5+10秒 |

## 牌局开始信息

牌局开始信息以`$项目名 项目值1 项目值2 ...  `的形式给出，具体如下表：

| 项目名 | 项目内容     | 项目值                                                       |
| ------ | ------------ | ------------------------------------------------------------ |
| Start  | 起始点数     | `num1 num2 num3 num4`，分别为东南西北家的本局起始点数（**舍弃末尾的两个零**）。如：`169 452 120 259` |
| Wall   | 牌山（选填） | `tile1tile2tile3...`一串表示牌的无空格字符串，若不填写则用`-`代替。 |

## 牌的表示

数牌以一位数字加上一个表示花色的字母（万子：`m`，筒子`p`，索子`s`）组成。例如：一万`1m`。

字牌以一个大写字母表示：东`E`、南`S`、西`W`、北`N`、白`H` 、发`G`、中`R`。

赤牌（或者其他特殊牌）在牌的表示后附一个`a`来表示。如赤五筒`5pa`。

*可能支持以`0m`、`0p`、`0s`、代替`5ma`、`5pa`、`5sa`的写法。*

## 对局进程的横式表示和纵式表示

### 横式表示

横式表示是以牌局进程位顺序的记录方式，即随着对局进程**横着**记录牌谱。该记录方式适合对战程序记录并读出。

#### 开局翻表Dora指示牌

开局最先翻开表dora**指示牌**以`doraind=tile`表示。如`dora=E`。也可以简记为`d=tile`。

#### 配牌

配牌以`0.`开头，后面跟随着四个以**空格分隔的字串**，分别东南西北家的配牌。如：

`4m8m9m1p2p5p5pa9p1s2s4sES 1m3m4p6p7p8p9p2s5s5sSSS 4m6m8m9m2p4p1s3s5s9sWHG 3m8m2p3p4p6p8p1s2s9sNNH`

*可能支持同花色合并的写法，如`489m1255a9p124sES`*

#### 摸打

摸打以一对牌组成的串表示，前面表示摸入的牌，后面表示打出的牌，如`8sH`表示摸入八索打出白。

如果是摸切，则以`X`代替打出的牌，如`RX`表示摸切中。

每一巡的摸打以`巡目数.`开始，后面跟随着东南西北家的摸切，**以空格分隔**。

如：`1. RX 6s3m 8sH 8sH`

#### 鸣牌

以代表一个鸣牌的串代替摸牌的部分（加杠和暗杠是打牌的部分），具体如下：

| 鸣牌   | 表示方法                                                     |
| ------ | ------------------------------------------------------------ |
| 吃     | `C(tile1tile2)`，其中`tile1tile2`表示从手牌中晒出的牌。吃到的牌由**前一个摸打记录中打牌部分**决定。如：`C(4s6s)`。*可能允许省略花色的简记，如：`C(46)`*。 |
| 碰     | `P(tile1tile2)`，其中`tile1tile2`表示从手牌中晒出的牌。如：`P(5p5pa)`。碰到的牌由**前一个摸打记录中打牌部分**决定。如果碰的三张牌**完全相同不做区分**，则简记为`P`。*可能允许只记录赤牌个数的简记，如：`P(aa)`表示晒出两张赤牌*。 |
| 大明杠 | `K`，杠到的牌由**前一个摸打记录中打牌部分**决定，开杠所用的牌由**手牌决定**。大明杠**不需要写打牌的部分**。 |
| 加杠和 | `K(tile)`，其中`tile`表示杠出的牌。如：`3sK(5pa)`。如果摸牌与杠出的牌相同，则简记为`K`。如：`3sK`。 |
| 暗杠   | `K(tile1tile2tile3tile4)`，其中`tile1tile2tile3tile4`表示从手牌中晒出的牌。如：`K(5p5p5p5pa)`。如果暗杠的四张牌**完全相同不做区分**，则简记为`K(tile)`，如`K(3s)`。在此情况下如果摸牌与杠出的牌相同，则简记为`K`。如：`3sK`。*可能允许只记录赤牌个数的简记，如：`K(aa)`表示杠出两张赤牌*。 |
| 拔北   | `Z`                                                          |

鸣牌会跳过某些家的摸打阶段，这些被跳过的摸打部分使用`*`代替，如：`3. NX * * P8m`表示东家摸切北，北家碰北打八万。`*`可以越过巡目数，如：`3.RX NX 3m4m * 4. * P8m` 。

特别的，开杠/拔北后摸岭上牌视为**其余所有玩家跳过一巡**。如：`8. 7sS K * * 9. * 5maX 1pX 7m9s`。

开杠后的新dora以也由以`doraind=tile`或`d=tile`表示。紧跟在开dora的摸打后。如：`d=E`。

#### 立直宣言

立直宣言在**摸牌部分**后附`+`，如：`9s+4m`。

#### 放铳/自摸

放铳在摸打后后附`#`，如：`3pX#`。若立直宣言时放铳，则在`+`后面后附`#`。如：`9s+4m#`。

自摸在摸牌部分后面后附`#`。如`9m#`。

#### 注释

注释可以包围在`{}`内，以`;`开头至行尾的所有部分也被视为注释



### *纵式表示*

*横式表示是以玩家顺序的记录方式，每名玩家的对局动作分开记录。随着对局进程**竖着**记录牌谱。该记录方式适合人工记录。*

## 牌局结束信息

牌局结束信息分为和牌/流局信息和点数信息。

### 和牌/流局信息

和牌/流局信息分为荣和、自摸、流局。**只能出现一种**。

#### 荣和

荣和的表示方式为`$Ron 项目1 项目2 ...`项目间以空格分隔。

项目依次如下：

| 项目内容               | 项目表示方法                                                 |
| ---------------------- | ------------------------------------------------------------ |
| 荣和者                 | `East`, `South`, `West`, `North`之一的值。紧随`$Ron `后面并与之以空格分隔。或简写为`E`，`S`，`W`，`N`。 |
| 符数                   | `fu=number`，`number`是一个代表符数的数字。或简写为`f=number`。 |
| 里宝牌（若为横式记录） | `ura=tile1tile2...`，`tile1tile2...`是翻开的里宝牌。或简写为`u=tile1tile2...`。 |
| 役（包括宝牌）         | `"yakuname",han`，`yakuname`是役名。`han`是其番数，若为役满则标记`Y`，双倍役满标记`YY`。逗号之后**没有空格**。每个役之间由**空格分开**。 |
| 得分                   | `pt=number,"rank"`，`number`是和了的得分（**不包括供托、本场**）（**舍弃末尾的两个零**）。逗号之后**没有空格**。`"rank"`是得分的等级：如`"満貫"`，没有则不填（也不需要填写`""`）。 |

项目之间的顺序最好按照上表所列的顺序。

有多人荣和则每家的荣和均需要填写一行`$Ron`开头的信息。里宝牌的项目只需填写在第一位荣和者位置即可。

#### 自摸

自摸的表示方式为`$Tsumo 项目1 项目2 ...`项目间以空格分隔。（`$Tsumo`可简写为`$Tsu`）

项目依次如下：

| 项目内容               | 项目表示方法                                                 |
| ---------------------- | ------------------------------------------------------------ |
| 自摸者                 | `East`, `South`, `West`, `North`之一的值。紧随`$Ron `后面并与之以空格分隔。或简写为`E`，`S`，`W`，`N`。 |
| 符数                   | `fu=number`，`number`是一个代表符数的数字。或简写为`f=number`。 |
| 里宝牌（若为横式记录） | `ura=tile1tile2...`，`tile1tile2...`是翻开的里宝牌。或简写为`u=tile1tile2...`。 |
| 役（包括宝牌）         | `"yakuname",han`，`yakuname`是役名。`han`是其番数，若为役满则标记`Y`，双倍役满标记`YY`。逗号之后**没有空格**。每个役之间由**空格分开**。 |
| 得分                   | `pt=number1,number2,"rank"`，`number1`是子家支付得分，`number2`是亲家支付得分（没有则不填，包括后面的逗号）,（**不包括供托、本场**）（**舍弃末尾的两个零**）。逗号之后**没有空格**。`"rank"`是得分的等级：如`"満貫"`，没有则不填（也不需要填写`""`）。 |

项目之间的顺序最好按照上表所列的顺序。

#### 流局

自摸的表示方式为`$Ryuukyoku "流局名称"`或简写成`$Ryuu "流局名称"`。如：`$Ryuu "流し満貫"`。

### 点数信息

绝对值方式以`$End num1 num2 num3 num4`表示。分别为东南西北家的**本局结束点数**（**舍弃末尾的两个零**）。

## 终局信息

终局信息的表示由`[Final "num1,num2,num3,num4`"]。分别为东南西北家的**精算点数**。