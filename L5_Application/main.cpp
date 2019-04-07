/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * @brief This is the application entry point.
 * 			FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 * 			@see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */

// MP3 Project
#include "FreeRTOS.h"
#include "task.h"
#include "stdint.h"
#include "stdio.h"
#include "queue.h"
#include "ff.h"
#include "MP3Driver.hpp"
#include "uart3.hpp"

MP3Driver mp3;
Uart3 &lcd = Uart3::getInstance();
QueueHandle_t mp3Queue;
QueueHandle_t lcdQueue;
QueueHandle_t pauseQueue;
QueueHandle_t cursorQueue;
char fileName[10][25] = {0};
char playlist[10][25] = {0};
int totalFileNumber = 0;

void readFileNamesFromSD()
{
    FRESULT status; // File status return structure
    DIR dir; // Directory structure
    static FILINFO finfo; // File info structure

    status = f_opendir(&dir, "1:"); // Open directory ("1:" = SD card)

    if (status == FR_OK) // If directory open successfully
    {
        while (1)
        {
            status = f_readdir(&dir, &finfo); // Read filename and store in file info structure

            if (status != FR_OK || finfo.fname[0] == 0)  // Exit on error or end of directory
            {
                break;
            }

            if (!(finfo.fattrib & AM_DIR)) // If not a sub-directory
            {
                sprintf(fileName[totalFileNumber], "1:%s", finfo.fname); // Store "1:" + filename into song name buffer
                sprintf(playlist[totalFileNumber], "%s", finfo.fname); // Store original filename into play list
                totalFileNumber++;
            }
        }

        f_closedir(&dir); // Close directory
    }
}

void readFileTask(void*)
{
    int fileNumber = 0;
    bool playNow = false;
    bool pause = false;

    while (1)
    {
        FIL fil;
        FRESULT status;
        uint8_t fileBuffer[512];
        UINT bytes_read;

        if (FR_OK != (status = f_open(&fil, fileName[fileNumber], FA_OPEN_EXISTING | FA_READ))) // Open file
        {
            printf("Error %i: ...\n", status);
            vTaskDelay(1000);
            continue;
        }

        while (FR_OK == f_read(&fil, fileBuffer, 512, &bytes_read)) // As long as we can read the file
        {
            // Block forever to put the data into the queue
            // When the player task is behind, the queue will accumulate data
            // and we will just end up sleeping here
            xQueueSend(mp3Queue, &fileBuffer, portMAX_DELAY);

            if (mp3.requestPause() || playNow == false)
            {
                if (pause)
                {
                    xQueueSend(pauseQueue, &fileNumber, 1000); // Send the file number that is paused to lcd task
                }

                vTaskDelay(1000);

                while (!mp3.requestPause())
                {
                    vTaskDelay(100);

                }

                playNow = true;
                pause = true;

                if (playNow)
                {
                    xQueueSend(lcdQueue, &fileNumber, 1000); // Send the file number that is playing to lcd task
                }

                vTaskDelay(500);
            }

            if (mp3.requestBack())
            {
                if (fileNumber == 0) // Loop back to last file if this is first file
                {
                    fileNumber = totalFileNumber - 1;
                }
                else
                {
                    fileNumber--;
                }

                xQueueSend(lcdQueue, &fileNumber, 1000);
                vTaskDelay(1000);
                break;
            }

            if (mp3.requestNext())
            {
                if (fileNumber + 1 == totalFileNumber) // Loop back to first file if this is last file
                {
                    fileNumber = 0;
                }
                else
                {
                    fileNumber++;
                }

                xQueueSend(lcdQueue, &fileNumber, 1000);
                vTaskDelay(1000);
                break;
            }

            if (mp3.requestVolDown())
            {
                mp3.decreaseVol();
                vTaskDelay(100);
            }

            if (mp3.requestVolUp())
            {
                mp3.increaseVol();
                vTaskDelay(100);
            }

            if (mp3.selectUp()) // Select song to play in play list (not yet implemented)
            {
                vTaskDelay(1000);
                break;
            }

            if (mp3.selectdown()) // Select song to play in play list (not yet implemented)
            {
                vTaskDelay(1000);
                break;
            }
        }

        f_close(&fil);
    }
}

void playMP3Task(void*)
{
    uint8_t mp3Buffer[512] = {0};

    while (1)
    {
        if (xQueueReceive(mp3Queue, &mp3Buffer, portMAX_DELAY)) // Receive mp3 file 512 bytes at a time
        {
            for (int i = 0; i < 512; i+=32)
            {
                mp3.waitForDREQ(); // Check DREQ every 32 SDI bytes sent

                mp3.setXDCSLow();
                for (int j = 0; j < 32; j++)
                {
                    mp3.sendSDIByte(mp3Buffer[i+j]); // Send mp3 file 1 byte at a time
                }
                mp3.setXDCSHigh();
            }
        }
    }
}

void resetLCD()
{
    lcd.printf("$CLR_SCR\n");

    for (int i = 0; i < 4; i++)
    {
        lcd.printf("$L:%i:%s\n", i, playlist[i]);
    }
}

void LCDTask(void*)
{
    lcd.init(38400);
    lcd.putChar(0xF0);
    resetLCD();
    int count = 0;

    while(1)
    {
        if (xQueueReceive(lcdQueue, &count, 1000))
        {
            printf("\nReceived data: %i\n", count);
            resetLCD();
            lcd.printf("$L:%i:Now Playing:  %s\n", count, playlist[count]);
        }
        if (xQueueReceive(pauseQueue, &count, 1000))
        {
            resetLCD();
            lcd.printf("$L:%i:Paused:  %s\n", count, playlist[count]);
        }
    }
}

#include "tasks.hpp"
#include "examples/examples.hpp"

/**
 * The main() creates tasks or "threads".  See the documentation of scheduler_task class at scheduler_task.hpp
 * for details.  There is a very simple example towards the beginning of this class's declaration.
 *
 * @warning SPI #1 bus usage notes (interfaced to SD & Flash):
 *      - You can read/write files from multiple tasks because it automatically goes through SPI semaphore.
 *      - If you are going to use the SPI Bus in a FreeRTOS task, you need to use the API at L4_IO/fat/spi_sem.h
 *
 * @warning SPI #0 usage notes (Nordic wireless)
 *      - This bus is more tricky to use because if FreeRTOS is not running, the RIT interrupt may use the bus.
 *      - If FreeRTOS is running, then wireless task may use it.
 *        In either case, you should avoid using this bus or interfacing to external components because
 *        there is no semaphore configured for this bus and it should be used exclusively by nordic wireless.
 */
int main(void)
{
    /**
     * A few basic tasks for this bare-bone system :
     *      1.  Terminal task provides gateway to interact with the board through UART terminal.
     *      2.  Remote task allows you to use remote control to interact with the board.
     *      3.  Wireless task responsible to receive, retry, and handle mesh network.
     *
     * Disable remote task if you are not using it.  Also, it needs SYS_CFG_ENABLE_TLM
     * such that it can save remote control codes to non-volatile memory.  IR remote
     * control codes can be learned by typing the "learn" terminal command.
     */
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    /* Consumes very little CPU, but need highest priority to handle mesh network ACKs */
    scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));

    /* Change "#if 0" to "#if 1" to run period tasks; @see period_callbacks.cpp */
    #if 0
    const bool run_1Khz = false;
    scheduler_add_task(new periodicSchedulerTask(run_1Khz));
    #endif

    /* The task for the IR receiver to "learn" IR codes */
    // scheduler_add_task(new remoteTask  (PRIORITY_LOW));

    /* Your tasks should probably used PRIORITY_MEDIUM or PRIORITY_LOW because you want the terminal
     * task to always be responsive so you can poke around in case something goes wrong.
     */

    /**
     * This is a the board demonstration task that can be used to test the board.
     * This also shows you how to send a wireless packets to other boards.
     */
    #if 0
        scheduler_add_task(new example_io_demo());
    #endif

    /**
     * Change "#if 0" to "#if 1" to enable examples.
     * Try these examples one at a time.
     */
    #if 0
        scheduler_add_task(new example_task());
        scheduler_add_task(new example_alarm());
        scheduler_add_task(new example_logger_qset());
        scheduler_add_task(new example_nv_vars());
    #endif

    /**
	 * Try the rx / tx tasks together to see how they queue data to each other.
	 */
    #if 0
        scheduler_add_task(new queue_tx());
        scheduler_add_task(new queue_rx());
    #endif

    /**
     * Another example of shared handles and producer/consumer using a queue.
     * In this example, producer will produce as fast as the consumer can consume.
     */
    #if 0
        scheduler_add_task(new producer());
        scheduler_add_task(new consumer());
    #endif

    /**
     * If you have RN-XV on your board, you can connect to Wifi using this task.
     * This does two things for us:
     *   1.  The task allows us to perform HTTP web requests (@see wifiTask)
     *   2.  Terminal task can accept commands from TCP/IP through Wifly module.
     *
     * To add terminal command channel, add this at terminal.cpp :: taskEntry() function:
     * @code
     *     // Assuming Wifly is on Uart3
     *     addCommandChannel(Uart3::getInstance(), false);
     * @endcode
     */
    #if 0
        Uart3 &u3 = Uart3::getInstance();
        u3.init(WIFI_BAUD_RATE, WIFI_RXQ_SIZE, WIFI_TXQ_SIZE);
        scheduler_add_task(new wifiTask(Uart3::getInstance(), PRIORITY_LOW));
    #endif

    const uint32_t STACK_SIZE = 4096/4;
    mp3Queue = xQueueCreate(2, 512);
    lcdQueue = xQueueCreate(3, sizeof(int));
    pauseQueue = xQueueCreate(1, sizeof(int));
    cursorQueue = xQueueCreate(4, sizeof(int));

    mp3.init();
    readFileNamesFromSD();

    xTaskCreate(readFileTask, "read", STACK_SIZE, NULL, PRIORITY_HIGH, NULL);
    xTaskCreate(playMP3Task, "play", STACK_SIZE, NULL, PRIORITY_CRITICAL, NULL);
    xTaskCreate(LCDTask, "lcd", STACK_SIZE, NULL, PRIORITY_HIGH, NULL);

    scheduler_start(); ///< This shouldn't return
    return -1;
}
