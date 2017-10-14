#ifndef EXHEADER_H
#define EXHEADER_H

#include "Filesystem/filesystem.h"

#include <QHash>

const QHash<quint64, QString> modules =
{
    { 0x0004013000001002, "SAFE_MODE sm (Stored in SAFE_MODE NATIVE_FIRM)" },
    { 0x0004013000001102, "fs (Stored in NATIVE_FIRM)" },
    { 0x0004013000001103, "SAFE_MODE fs (Stored in SAFE_MODE NATIVE_FIRM)" },
    { 0x0004013000001202, "pm (Stored in NATIVE_FIRM)" },
    { 0x0004013000001203, "SAFE_MODE pm (Stored in SAFE_MODE NATIVE_FIRM)" },
    { 0x0004013000001302, "loader (Stored in NATIVE_FIRM)" },
    { 0x0004013000001303, "SAFE_MODE loader (Stored in SAFE_MODE NATIVE_FIRM)" },
    { 0x0004013000001402, "pxi (Stored in NATIVE_FIRM)" },
    { 0x0004013000001403, "SAFE_MODE pxi (Stored in SAFE_MODE NATIVE_FIRM)" },
    { 0x0004013000001502, "AM (Application Manager)" },
    { 0x0004013000001503, "SAFE_MODE AM" },
    { 0x0004013020001503, "New_3DS SAFE_MODE AM" },
    { 0x0004013000001602, "Camera" },
    { 0x0004013020001602, "New_3DS Camera" },
    { 0x0004013000001702, "Config (cfg)" },
    { 0x0004013000001703, "SAFE_MODE Config (cfg)" },
    { 0x0004013020001703, "New_3DS SAFE_MODE Config (cfg)" },
    { 0x0004013000001802, "Codec" },
    { 0x0004013000001803, "SAFE_MODE Codec" },
    { 0x0004013020001803, "New_3DS SAFE_MODE Codec" },
    { 0x0004013000001A02, "DSP" },
    { 0x0004013000001A03, "SAFE_MODE DSP" },
    { 0x0004013020001A03, "New_3DS SAFE_MODE DSP" },
    { 0x0004013000001B02, "GPIO" },
    { 0x0004013000001B03, "SAFE_MODE GPIO" },
    { 0x0004013020001B03, "New_3DS SAFE_MODE GPIO" },
    { 0x0004013000001C02, "GSP" },
    { 0x0004013020001C02, "New_3DS GSP" },
    { 0x0004013000001C03, "SAFE_MODE GSP" },
    { 0x0004013020001C03, "New_3DS SAFE_MODE GSP" },
    { 0x0004013000001D02, "HID (Human Interface Devices)" },
    { 0x0004013000001D03, "SAFE_MODE HID" },
    { 0x0004013020001D03, "New_3DS SAFE_MODE HID" },
    { 0x0004013000001E02, "i2c" },
    { 0x0004013020001E02, "New_3DS i2c" },
    { 0x0004013000001E03, "SAFE_MODE i2c" },
    { 0x0004013020001E03, "New_3DS SAFE_MODE i2c" },
    { 0x0004013000001F02, "MCU" },
    { 0x0004013020001F02, "New_3DS MCU" },
    { 0x0004013000001F03, "SAFE_MODE MCU" },
    { 0x0004013020001F03, "New_3DS SAFE_MODE MCU" },
    { 0x0004013000002002, "MIC (Microphone)" },
    { 0x0004013000002102, "PDN" },
    { 0x0004013000002103, "SAFE_MODE PDN" },
    { 0x0004013020002103, "New_3DS SAFE_MODE PDN" },
    { 0x0004013000002202, "PTM (Play time, pedometer, and battery manager)" },
    { 0x0004013020002202, "New_3DS PTM (Play time, pedometer, and battery manager)" },
    { 0x0004013000002203, "SAFE_MODE PTM" },
    { 0x0004013020002203, "New_3DS SAFE_MODE PTM" },
    { 0x0004013000002302, "spi" },
    { 0x0004013020002302, "New_3DS spi" },
    { 0x0004013000002303, "SAFE_MODE spi" },
    { 0x0004013020002303, "New_3DS SAFE_MODE spi" },
    { 0x0004013000002402, "AC (Network manager)" },
    { 0x0004013000002403, "SAFE_MODE AC" },
    { 0x0004013020002403, "New_3DS SAFE_MODE AC" },
    { 0x0004013000002602, "Cecd (StreetPass)" },
    { 0x0004013000002702, "CSND" },
    { 0x0004013000002703, "SAFE_MODE CSND" },
    { 0x0004013020002703, "New_3DS SAFE_MODE CSND" },
    { 0x0004013000002802, "DLP (Download Play)" },
    { 0x0004013000002902, "HTTP" },
    { 0x0004013000002903, "SAFE_MODE HTTP" },
    { 0x0004013020002903, "New_3DS SAFE_MODE HTTP" },
    { 0x0004013000002A02, "MP" },
    { 0x0004013000002A03, "SAFE_MODE MP" },
    { 0x0004013000002B02, "NDM" },
    { 0x0004013000002C02, "NIM" },
    { 0x0004013000002C03, "SAFE_MODE NIM" },
    { 0x0004013020002C03, "New_3DS SAFE_MODE NIM" },
    { 0x0004013000002D02, "NWM (Low-level wifi manager)" },
    { 0x0004013000002D03, "SAFE_MODE NWM" },
    { 0x0004013020002D03, "New_3DS SAFE_MODE NWM" },
    { 0x0004013000002E02, "Sockets" },
    { 0x0004013000002E03, "SAFE_MODE Sockets" },
    { 0x0004013020002E03, "New_3DS SAFE_MODE Sockets" },
    { 0x0004013000002F02, "SSL" },
    { 0x0004013000002F03, "SAFE_MODE SSL" },
    { 0x0004013020002F03, "New_3DS SAFE_MODE SSL" },
    { 0x0004013000003000, "Process9 (in SAFE_MODE and normal NATIVE_FIRM)" },
    { 0x0004013000003102, "PS (Process Manager)" },
    { 0x0004013000003103, "SAFE_MODE PS" },
    { 0x0004013020003103, "New_3DS SAFE_MODE PS" },
    { 0x0004013000003202, "friends (Friends list)" },
    { 0x0004013000003203, "SAFE_MODE friends (Friends list)" },
    { 0x0004013020003203, "New_3DS SAFE_MODE friends (Friends list)" },
    { 0x0004013000003302, "IR (Infrared)" },
    { 0x0004013000003303, "SAFE_MODE IR" },
    { 0x0004013020003303, "New_3DS SAFE_MODE IR" },
    { 0x0004013000003402, "BOSS (SpotPass)" },
    { 0x0004013000003502, "News (Notifications)" },
    { 0x0004013000003702, "RO" },
    { 0x0004013000003802, "act (handles Nintendo Network accounts)" },
    { 0x0004013000004002, "Old3DS nfc" },
    { 0x0004013020004002, "New_3DS nfc" },
    { 0x0004013020004102, "New_3DS mvd" },
    { 0x0004013020004202, "New_3DS qtm" },
    { 0x0004013000008002, "NS (Memory-region: \"SYSTEM\")" },
    { 0x0004013000008003, "SAFE_MODE NS (Memory-region: \"SYSTEM\")" },
    { 0x0004013020008003, "New_3DS SAFE_MODE NS (Memory-region: \"SYSTEM\")" },

};

class Exheader
{
public:
    Exheader(FileBase* file);
    ~Exheader();

    bool save();

    void print();
    void printACI(int index);

    struct CodeSetInfo
    {
        quint32 address;
        quint32 physicalRegionSize;
        quint32 size;
    };

    struct ACI
    {
        struct
        {
            quint8 data[0x170];
        } arm11systemCaps;

        struct
        {
            quint32 desciptors[28];
            quint8 reserverd[0x10];
        } arm11kernelCaps;

        struct
        {
            quint8 data[0x10];
        } arm9accessControl;
    };


    struct
    {
        struct
        {
            char title[8];
            quint8 reserved1[5];

            struct
            {
                unsigned char compressExefsCode : 1;
                unsigned char sdApplication : 1;
                unsigned char : 6;
            } sciFlags;

            quint16 remasterVersion;

            CodeSetInfo textCodeSetInfo;
            quint32 stackSize;
            CodeSetInfo readOnlyCodeSetInfo;
            quint32 reserved2;
            CodeSetInfo dataCodeSetInfo;
            quint32 bssSize;

            quint64 dependencyModules[48];

            struct
            {
                quint64 saveDataSize;
                quint64 jumpId;
                quint8 reserved[0x30];
            } systemInfo;

        } sci;

        ACI aci1;

        quint8 accessDesc[0x100];
        quint8 ncchHdr[0x100];

        ACI aci2;

    } data;

private:
    FileBase* m_file;
};

#endif // EXHEADER_H
