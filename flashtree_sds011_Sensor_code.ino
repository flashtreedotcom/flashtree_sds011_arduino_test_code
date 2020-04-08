#include <SoftwareSerial.h>


//www.flashtree.com
//E-mail: 6702578@qq.com
//any question E-mail first


SoftwareSerial SoftSerial(3, 2);

unsigned int PM2_5 = 0;
unsigned int PM10 = 0;


void get_Sds011_Data_Sent_To_Serial()
{
    uint8_t RecData = 0; //temp Data
    uint8_t i = 0; //for loop
    uint8_t RecPackage[10] = {0}; //Packet data buffer
    uint8_t RecCheckFlag = 0; //Rec CRC check flag

    while (SoftSerial.available() > 0)
    {
        //Packet Data Format: AA C0 PM2_5_Low PM2_5_High PM10_Low PM10_High 0 0 CRC AB
        RecData = SoftSerial.read();

        delay(2);//wait until packet is received

        if(RecData == 0xAA)//head1 ok
        {
            RecPackage[0] =  RecData;
            RecData = SoftSerial.read();

            if(RecData == 0xc0)//head2 ok
            {
                RecPackage[1] =  RecData;
                RecCheckFlag = 0;

                for(i=0; i < 6; i++) //data recv and crc calc
                {
                    RecPackage[i+2] = SoftSerial.read();
                    delay(2);
                    RecCheckFlag += RecPackage[i+2];
                }
                RecPackage[8] = SoftSerial.read();
                delay(1);
                RecPackage[9] = SoftSerial.read();

                if(RecCheckFlag == RecPackage[8])//crc ok
                {
                    SoftSerial.flush();

                    //Splicing PM2.5 and PM10 data
                    PM2_5 = (uint16_t)RecPackage[2] | (uint16_t)(RecPackage[3] << 8);
                    PM10 = (uint16_t)RecPackage[4] | (uint16_t)(RecPackage[5]<<8);

                    if (PM2_5 > 9999) //Limit the maximum PM2.5
                    {
                        PM2_5 = 9999;
                    }

                    if (PM10 > 9999) //Limit the maximum PM10
                    {
                        PM10 = 9999;
                    }

                    // //send the all data received from sds011 for febug
                    // for (i = 0; i < 10; i++)
                    // {
                    //   Serial.print(RecPackage[i], HEX);
                    // }


                    //send the PM2.5 and PM10 data to Serial
                    Serial.print(" PM2.5: ");
                    Serial.print(PM2_5);
                    Serial.print("     PM10: ");
                    Serial.println(PM10);

                    return;
                }
            }
        }
    }
}




void setup() {

    SoftSerial.begin(9600);

    Serial.begin(9600);
    Serial.println(F(" ======== flashtree SDS011 Sensor Test Code ======== "));
    Serial.println(F(" www.flashtree.com  Email:6702578@qq.com"));
    PM2_5=0;
    PM10=0;

}


void loop() {
    get_Sds011_Data_Sent_To_Serial();


}
