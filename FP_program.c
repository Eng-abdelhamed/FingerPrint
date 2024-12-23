
#define F_CPU 8000000UL
#include <xc.h>
#include <util/delay.h>
#include "FP_interface (1).h"
#include "UART_interface.h"
#include "LCD_interface.h"
#include "keys_interface.h"

// Common package headers for communication with the R307 module
const char Header[] = {0xEF, 0x1};
const char Address[] = {0xFF, 0xFF, 0xFF, 0xFF};
const char Command[] = {0x1, 0x00};

// Command packages for R307 module
const char PassVfy[] = {0x7, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B};
const char f_detect[] = {0x3, 0x1, 0x0, 0x5};
const char f_imz2ch1[] = {0x4, 0x2, 0x1, 0x0, 0x8};
const char f_imz2ch2[] = {0x4, 0x2, 0x2, 0x0, 0x9};
const char f_createModel[] = {0x3, 0x5, 0x0, 0x9};
char f_storeModel[] = {0x6, 0x6, 0x1, 0x0, 0x1, 0x0, 0xE};
const char f_search[] = {0x8, 0x4, 0x1, 0x0, 0x0, 0x0, 0xA3, 0x0, 0xb1};
char f_delete[] = {0x7, 0xC, 0x0, 0x0, 0x0, 0x1, 0x0, 0x15};
char fp_empty[] = {0x3, 0xd, 0x0, 0x11};

/**
 * @brief Sends a command to the fingerprint module and handles feedback.
 *
 * @param order The command identifier to be executed.
 * @return char Success or failure status.
 */
char sendcmd2fb(unsigned char order) {
    unsigned char successed = again;

    while (successed == again) {
        // Stage 1: LCD Feedback
        lcdclear();
        switch (order) {
            case FB_connect: lcdprint("Searching for Module"); break;
            case F_find: lcdprint("Place Your Finger"); break;
            case F_im1: case F_im2: lcdprint("Processing..."); break;
            case F_cretModl: lcdprint("Creating Model"); break;
            case FP_empty: lcdprint("Erasing IDs"); break;
            case F_search: lcdprint("Searching..."); break;
            case F_store: lcdprint("Storing Model"); break;
            case F_delete: lcdprint("Deleting Finger"); break;
        }

        // Stage 2: Send command to the module
        UART_Array(Header, 2);
        UART_Array(Address, 4);
        UART_Array(Command, 2);

        int arr_size;
        unsigned char ID;

        switch (order) {
            case FB_connect:
                arr_size = sizeof(PassVfy) / sizeof(PassVfy[0]);
                UART_Array(PassVfy, arr_size);
                break;
            case F_find:
                arr_size = sizeof(f_detect) / sizeof(f_detect[0]);
                UART_Array(f_detect, arr_size);
                break;
            case F_im1:
                arr_size = sizeof(f_imz2ch1) / sizeof(f_imz2ch1[0]);
                UART_Array(f_imz2ch1, arr_size);
                break;
            case F_im2:
                arr_size = sizeof(f_imz2ch2) / sizeof(f_imz2ch2[0]);
                UART_Array(f_imz2ch2, arr_size);
                break;
            case F_cretModl:
                arr_size = sizeof(f_createModel) / sizeof(f_createModel[0]);
                UART_Array(f_createModel, arr_size);
                break;
            case FP_empty:
                arr_size = sizeof(fp_empty) / sizeof(fp_empty[0]);
                UART_Array(fp_empty, arr_size);
                break;
            case F_search:
                arr_size = sizeof(f_search) / sizeof(f_search[0]);
                UART_Array(f_search, arr_size);
                break;
            case F_store:
                ID = GetID();
                f_storeModel[4] = ID;
                f_storeModel[6] = (0xE + ID);
                arr_size = sizeof(f_storeModel) / sizeof(f_storeModel[0]);
                UART_Array(f_storeModel, arr_size);
                break;
            case F_delete:
                ID = GetID();
                f_delete[3] = ID;
                f_delete[7] = (0x15 + ID);
                arr_size = sizeof(f_delete) / sizeof(f_delete[0]);
                UART_Array(f_delete, arr_size);
                break;
        }
        _delay_ms(1000);

        // Stage 3: Handle feedback from the fingerprint module
        if (cont > 1) {
            if (rcvData[6] == 0x07 && (rcvData[8] == 0x03 || rcvData[8] == 0x07)) {
                lcdsecondrow();
                if (rcvData[9] == 0) {
                    successed = success;
                } else {
                    successed = fail;
                    switch (rcvData[9]) {
                        case 0x01: lcdprint("Packet Error"); break;
                        case 0x04: lcdprint("Try Again"); break;
                        case 0x05: lcdprint("Clean Sensor"); break;
                        case 0x06: lcdprint("Model Fail"); break;
                        case 0x07: lcdprint("Small Finger"); break;
                        case 0x09: lcdprint("ID Not Found"); break;
                        case 0x18: lcdprint("Flash Write Err"); break;
                        case 0x13: lcdprint("Password Incorrect"); break;
                        default: lcdprint("Undefined Error"); successed = again; break;
                    }
                }
            } else {
                lcdclear();
                lcdprint("Connection Error");
            }
        } else {
            lcdclear();
            lcdprint("Module Not Found");
            _delay_ms(1000);
        }

        // Stage 4: Provide feedback upon success
        if (successed == success) {
            unsigned char String[20];
            lcdclear();
            switch (order) {
                case FB_connect: lcdprint("Module Connected"); break;
                case F_find: lcdprint("Finger Detected"); break;
                case F_store: lcdprint("Saved Successfully"); break;
                case F_delete:
                    sprintf(String, "ID: %d Deleted", ID);
                    lcdprint(String);
                    break;
                default: lcdprint("Done"); break;
            }
        }
        clearArray(rcvData);
        cont = 0;
        _delay_ms(700);
    }
    return successed;
}

/**
 * @brief Prompts the user to enter an ID using the keypad.
 *
 * @return char The selected ID.
 */
char GetID() {
    lcdclear();
    unsigned char ids = 1;
    char ok = 0;
    lcdprint("Enter ID:");
    while (ok == 0) {
        switch (keys_Feedback()) {
            case keys_up: ids++; break;
            case keys_down: ids--; break;
            case keys_ok: ok = 1; break;
        }
        if (ids >= 128) ids = 1;
        unsigned char String[20];
        lcdgotoxy(1, 10);
        sprintf(String, " %d  ", ids);
        lcdprint(String);
        _delay_ms(200);
    }
    return ids;
}

/**
 * @brief Enrolls a fingerprint into the module.
 */
void enroll() {
    if (sendcmd2fb(F_find)) enroll();
    if (sendcmd2fb(F_im1)) enroll();
    if (sendcmd2fb(F_find)) enroll();
    if (sendcmd2fb(F_im2)) enroll();
    if (sendcmd2fb(F_cretModl)) enroll();
    if (sendcmd2fb(F_store)) enroll();
}

/**
 * @brief Searches for a fingerprint in the module.
 */
void Search() {
    if (sendcmd2fb(F_find)) Search();
    if (sendcmd2fb(F_im1)) Search();
    if (sendcmd2fb(F_search)) Search();
}
