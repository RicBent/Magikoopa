#include "exheader.h"

#include <QDebug>

Exheader::Exheader(FileBase* file)
    : m_file(file)
{
    if (m_file->size() != 0x800)
        throw new std::runtime_error("Exheader: Invalid file: Wrong file size");

    m_file->setEndianess(Endianess::LittleEndian);

    m_file->open();
    m_file->seek(0);

    m_file->readData((quint8*)&data, sizeof(data));

    m_file->close();
}

Exheader::~Exheader()
{
    if (m_file)
        delete m_file;
}

bool Exheader::save()
{
    m_file->open();
    m_file->resize(sizeof(data));
    m_file->seek(0);
    m_file->writeData((quint8*)&data, sizeof(data));
    m_file->save();
    m_file->close();

    return true;
}

template<typename T>
QString toHex(const T& val, int pad = sizeof(T)*2)
{
    QString ret = QString::number(val, 0x10);
    for (int i = ret.length(); i < pad; i++)
        ret.prepend('0');
    return ret.prepend("0x");
}

void printCodeSetInfo(const Exheader::CodeSetInfo& codeSetInfo)
{
    qDebug() << "      Address:             " << toHex(codeSetInfo.address).toLatin1().data();
    qDebug() << "      Physical region size:" << toHex(codeSetInfo.physicalRegionSize).toLatin1().data();
    qDebug() << "      Address:             " << toHex(codeSetInfo.size).toLatin1().data();
}

void Exheader::printACI(int index)
{
    ACI* aci;

    if (index == 0) aci = &data.aci1;
    else aci = &data.aci2;

    qDebug() << QString("  ACI%1").arg(index).toLatin1().data();
    qDebug() << "  {";

    qDebug() << "    ARM11 Kernel Capabilities\n    {";

    for (int i = 0; i < 28; i++)
    {
        qDebug() << "      Raw: " << toHex(aci->arm11kernelCaps.desciptors[i]).toLatin1().data();

        quint32 descriptor = aci->arm11kernelCaps.desciptors[i];

        // System call mask
        if ((descriptor & 0xF8000000) == 0xF0000000)
        {
            quint32 mask = descriptor & 0x00FFFFFF;
            quint32 tableIndex = (descriptor & 0x03000000) >> 24;

            qDebug() << QString("    - %1 -> %2").arg(mask).arg(tableIndex).toLatin1().data();
        }
    }

    qDebug() << "    }";

    qDebug() << "  }";
}

void Exheader::print()
{
    qDebug() << "NCCH Extended Header\n{";

    qDebug() << "  SCI\n  {";

    qDebug() << "    Title:                 " << data.sci.title;
    qDebug() << "    Compressed Exefs Code: " << data.sci.sciFlags.compressExefsCode;
    qDebug() << "    SD Application:        " << data.sci.sciFlags.sdApplication;
    qDebug() << "    Remaster Version:      " << toHex(data.sci.remasterVersion).toLatin1().data();

    qDebug() << "";
    qDebug() << "    Text code set info:";
    printCodeSetInfo(data.sci.textCodeSetInfo);
    qDebug() << "";


    qDebug() << "    Stack Size:            " << toHex(data.sci.stackSize).toLatin1().data();

    qDebug() << "";
    qDebug() << "    Read-only code set info:";
    printCodeSetInfo(data.sci.readOnlyCodeSetInfo);

    qDebug() << "";
    qDebug() << "    Data code set info:";
    printCodeSetInfo(data.sci.dataCodeSetInfo);
    qDebug() << "";

    qDebug() << "    BSS Size:              " << toHex(data.sci.bssSize).toLatin1().data();

    qDebug() << "";

    qDebug() << "    Dependency module (program ID) list:";
    for (int i = 0; i < 48; i++)
    {
        if (data.sci.dependencyModules[i] == 0) continue;
        qDebug() << "      -" << modules.value(data.sci.dependencyModules[i], "Unknown").toLatin1().data()
                 << toHex(data.sci.dependencyModules[i]).prepend('(').append(')').toLatin1().data();
    }
    qDebug() << "";

    qDebug() << "    Save data size:       " << toHex(data.sci.systemInfo.saveDataSize).toLatin1().data();
    qDebug() << "    Jump ID:              " << toHex(data.sci.systemInfo.jumpId).toLatin1().data();



    qDebug() << "  }";

    qDebug() << "";

    printACI(0);

    qDebug() << "";

    printACI(1);

    qDebug() << "}";
}
