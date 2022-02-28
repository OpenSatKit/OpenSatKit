/*
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
*/

/*
 *  This program calculates the CRC-16/ARC of a given table file.
 *
 *  Algorithm:
 *    - Name: CRC-16/ARC
 *    - Polynomial: 0x8005
 *    - Initialization: 0x0000
 *    - Reflect Input/Output: true
 *    - XorOut: 0x0000
 *
 *  Inputs: One string containing the filename of the table file to CRC.
 *
 *  Outputs: Prints to the terminal the filename, size, and CRC.
 *           Returns 0 if successful.
 *
 *  Author: Mike Blau, GSFC Code 582
 */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "cfe_ts_crc_version.h"

/* These headers are needed for CFE_FS_Header_t and CFE_TBL_File_Hdr_t, respectively.
 * This uses the OSAL definition of fixed-width types, even thought this tool
 * is not using OSAL itself.
 */
#include "common_types.h"
#include "cfe_fs_extern_typedefs.h"
#include "cfe_tbl_extern_typedefs.h"

/*
**             Function Prologue
**
** Function: CalculateCRC  (originated from lro-cfe-4.2.1 delivery - 2/4/09)
**
** Purpose:  Perform a CRC calculation on a range of memory.
**
*/
uint32 CalculateCRC(void *DataPtr, uint32 DataLength, uint32 InputCRC)
{
    int32  i;
    int16  Index;
    int16  Crc = 0;
    uint8 *BufPtr;

    static const uint16 CrcTable[256] = {

        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1,
        0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40,
        0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1,
        0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1,
        0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40,
        0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1,
        0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0,
        0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
        0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0,
        0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1,
        0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140,
        0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0,
        0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0,
        0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
        0x4100, 0x81C1, 0x8081, 0x4040};

    Crc    = (int16)(0xFFFF & InputCRC);
    BufPtr = (uint8 *)DataPtr;

    for (i = 0; i < DataLength; i++, BufPtr++)
    {
        Index = ((Crc ^ *BufPtr) & 0x00FF);
        Crc   = ((Crc >> 8) & 0x00FF) ^ CrcTable[Index];
    }

    return (Crc);
}

int main(int argc, char **argv)
{
    ssize_t readSize;
    off_t   skipSize = 0;
    ssize_t fileSize = 0;
    uint32  fileCRC  = 0;
    int     fd;
    char    buffer[100];
    off_t   offsetReturn = 0;

    /* check for valid input */
    if ((argc != 2) || (strncmp(argv[1], "--help", 100) == 0))
    {
        printf("%s\n", CFE_TS_CRC_VERSION_STRING);
        printf("\nUsage: cfe_ts_crc [filename]\n");
        exit(1);
    }
    /* Set to skip the header (116 bytes) */
    skipSize = sizeof(CFE_FS_Header_t) + sizeof(CFE_TBL_File_Hdr_t);

    /* open the input file if possible */
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        printf("\ncfe_ts_crc error: can't open input file!\n");
        perror(argv[1]);
        exit(1);
    }
    /* seek past the number of bytes requested */
    offsetReturn = lseek(fd, skipSize, SEEK_SET);
    if (offsetReturn != skipSize)
    {
        printf("\ncfe_ts_crc error: lseek failed!\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    /* read the input file 100 bytes at a time */
    do
    {
        readSize = read(fd, buffer, sizeof(buffer));
        if (readSize < 0)
        {
            printf("\ncfe_ts_crc error: file read failed!\n");
            printf("%s\n", strerror(errno));
            exit(1);
        }
        fileCRC = CalculateCRC(buffer, readSize, fileCRC);
        fileSize += readSize;
    } while (readSize > 0);

    /* print the size/CRC results */
    printf("\nTable File Name:            %s\nTable Size:                 %ld Bytes\nExpected TS Validation CRC: "
           "0x%08lX\n\n",
           argv[1], (long)fileSize, (unsigned long)fileCRC);

    /* Close file and check*/
    if (close(fd) != 0)
    {
        printf("\nerror: Cannot close file!\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    return (0);
}
