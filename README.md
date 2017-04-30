# WaterCardFreeRecharge
基于Arduino+RC522制作的，水卡全自动免费充值设备。

前提：得到自动卡的A\B KEY、研究出金额算法（也可以直接复制现有的）。

源码是在Arduino RC522库提供的ReadAndWrite示例上编写的

注释只是我简单看看库文件猜想而来的，不建议用来参考。

以下条件视情况修改：

因为学校水卡的A KEY是通过UID变化而来，所以注释掉了58-61行，写到了100-103行。

124行开始是规定扇区位置。

126行多加的blockAddr2是因为我学校水卡金额需要验证2个扇区块。

190-203行是写入操作，看你要写几个扇区块。

测试视频：https://www.youtube.com/watch?v=t1B8R3-n53g
