/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to read and write data blocks on a MIFARE Classic PICC
 * (= card/tag).
 * 
 * BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
 * 
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * 
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
                                    // 可配置的 ,见顶部接线示意图
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance. 创建MFRC522实例

MFRC522::MIFARE_Key keyA;
MFRC522::MIFARE_Key keyB;

    // typedef struct {
    //     byte        keyByte[MF_KEY_SIZE];
    // } MIFARE_Key;

/**
 * Initialize.
 */
void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    keyA.keyByte[0] = 0x01;
//    keyA.keyByte[1] = 0x75;   //UID
//    keyA.keyByte[2] = 0x68;   //前
//    keyA.keyByte[3] = 0x47;   //四
//    keyA.keyByte[4] = 0xc0;   //位
    keyA.keyByte[5] = 0x2b;


    keyB.keyByte[0] = 0x95;
    keyB.keyByte[1] = 0x66;
    keyB.keyByte[2] = 0x05;
    keyB.keyByte[3] = 0x14;
    keyB.keyByte[4] = 0x69;
    keyB.keyByte[5] = 0x02;
    //    95 66 05 14 69 02
    //你可以通过基于闪存的字符串来进行打印输出，将数据放入F()中，再放入Serial.print() Serial.println(F(""));
    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
    Serial.print(F("Using key (for A and B):"));  
    dump_byte_array(keyA.keyByte, MFRC522::MF_KEY_SIZE); //转为16进制
    dump_byte_array(keyB.keyByte, MFRC522::MF_KEY_SIZE); //转为16进制
    Serial.println();
    
    Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));
}

/**
 * Main loop.
 */
void loop() {
    // Look for new cards  
    //寻找新卡
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards  
    //选择一个卡
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)  
    //显示PICC的一些细节（即：标签/卡）
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    keyA.keyByte[1] = mfrc522.uid.uidByte[0];   //UID
    keyA.keyByte[2] = mfrc522.uid.uidByte[1];   //前
    keyA.keyByte[3] = mfrc522.uid.uidByte[2];   //四
    keyA.keyByte[4] = mfrc522.uid.uidByte[3];   //位    
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    //检查兼容性
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        //此示例仅适用于MIFARE经典卡
        return;
    }

    // In this sample we use the second sector, 0-1  = 1
    //在此示例中，我们使用的第二个扇区，就是#1扇区
    // that is: sector #1, covering block #4 up to and including block #7
    //那就是：扇区＃1，覆盖块＃4直至并包括块＃7
    //查看hex文件，扇区#1就是0040-0070  #2就是0080-00B0
    byte sector         = 10;          //扇区
    byte blockAddr      = 40;          //扇区开始块  金额地址1
    byte blockAddr2     = 42;          //金额地址2
    byte dataBlock[]    = {
        0x86, 0x7d, 0x88, 0x13, 
        0xe2, 0x82, 0x00, 0x00, 
        0x00, 0x00, 0x01, 0x00, 
        0x00, 0x01, 0x00, 0x81
    };
    
    //867d8813e28200000000010000010081
    byte trailerBlock   = 43;        //扇区结束块
    MFRC522::StatusCode status;  //状态码
    // STATUS_OK               ,   // Success 成功
    // STATUS_ERROR            ,   // Error in communication 通讯出错
    // STATUS_COLLISION        ,   // Collission detected   碰撞检测
    // STATUS_TIMEOUT          ,   // Timeout in communication. 通信超时
    // STATUS_NO_ROOM          ,   // A buffer is not big enough.   缓冲区不够大
    // STATUS_INTERNAL_ERROR   ,   // Internal error in the code. Should not happen ;-) 内部错误代码。不应该发生
    // STATUS_INVALID          ,   // Invalid argument. 无效的参数
    // STATUS_CRC_WRONG        ,   // The CRC_A does not match  该CRC_A不匹配
    // STATUS_MIFARE_NACK      = 0xff  // A MIFARE PICC responded with NAK. 没有应答。
    byte buffer[18];
    byte size = sizeof(buffer);

    // Authenticate using key A
    //  在trailerBlock块中 使用身份验证密钥A 
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &keyA, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) { //如果状态！=成功
        Serial.print(F("KeyA PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status)); //输出状态
        return;
    }

    // Show the whole sector as it currently is
    //输出指定扇区当前内容
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &keyA, sector);
    Serial.println();

    // Read data from the block
    //从blockAddr块读取数据
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) { //如果读取错误输出错误信息
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    //输出blockAddr块的内容 
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
    Serial.println();

    // Authenticate using key B
    //在trailerBlock块中 使用身份验证密钥B 
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &keyB, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {//如果验证不成功 返回错误状态码
        Serial.print(F("KeyB PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Write data to the block
    //写入数据（dataBlock）到 blockAddr块
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {  //如果写入失败返回错误状态码
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr2, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {  //如果写入失败返回错误状态码
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }    
    Serial.println();

    // Read data from the block (again, should now be what we have written)
    //从blockAddr块读取修改后的数据
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) { //如果读取错误返回错误状态码
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    //输出blockAddr块修改后的内容 
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
        
    // Check that data in block is what we have written
    // by counting the number of bytes that are equal
    //检查写入状态，成功写入16个字节就是正确 否则错误
    Serial.println(F("Checking result..."));
    byte count = 0;
    for (byte i = 0; i < 16; i++) {
        // Compare buffer (= what we've read) with dataBlock (= what we've written)
        if (buffer[i] == dataBlock[i])
            count++;
    }
    Serial.print(F("Number of bytes that match = ")); Serial.println(count);
    if (count == 16) {
        Serial.println(F("Success :-)"));
        digitalWrite(2, HIGH); //点亮小灯  
        delay(2000);
        digitalWrite(2, LOW); //熄灭
    } else {
        Serial.println(F("Failure, no match :-("));
        Serial.println(F("  perhaps the write didn't work properly..."));
    }
    Serial.println();
        
    // Dump the sector data
    //输出修改后扇区内容
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &keyA, sector);
    Serial.println();

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    digitalWrite(2, LOW); //熄灭
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
